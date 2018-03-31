#include "playback.h"

static uint32_t rate = 44000; // hard assumption
static uint8_t channels = 1; // mono only

static uint32_t pcm;
static snd_pcm_t* pcm_handle;
static snd_pcm_hw_params_t *params;
static snd_pcm_uframes_t frames;
static snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

static void* buffer_frame;
static void* buffer_mix;
static double* buffer_d;
static uint32_t buffer_size;

//static void**    track_buffer;
//static uint32_t* track_size;

double playback_gain;

static void WriteBufS16(double *in, void *out, int s)
{
  int16_t *outn = (int16_t *) out;
  int p = -1;
  while (++p < s) {
	  outn[p] = SAMPLE_TO_S16(in[p]);
  }
}

static void ReadBufS16(void *in, double *out, int s)
{
  int16_t *inn = (int16_t *) in;
  while (s-- > 0) {
    out[s] = S16_TO_SAMPLE(inn[s]);
  }
}

/*static void TrackMix(void* a_buf, void* b_buf, void* m_buf, int length) {
  int i;
  int32_t a, b;
  int32_t m;

  for (i = 0; i < length; i++) {
    a = (int32_t) *(((int16_t*)a_buf) + i);
    b = (int32_t) *(((int16_t*)b_buf) + i);
    // Make both samples unsigned (0..65535)
    a += 32768;
    b += 32768;

    // Pick the equation
    if ((a < 32768) || (b < 32768)) {
      // Viktor's first equation when both sources are "quiet"
      // (i.e. less than middle of the dynamic range)
      m = a * b / 32768;
    } else {
      // Viktor's second equation when one or both sources are loud
      m = 2 * (a + b) - (a * b) / 32768 - 65536;
    }

    // Output is unsigned (0..65536) so convert back to signed (-32768..32767)
    if (m == 65536) m = 65535;
    m -= 32768;

    *(((int16_t*)m_buf) + i) = m;
  }
  }*/

static int TrackLoad(char* file, track_t* track) {
  FILE* fp;
  uint32_t alloc_size;
  uint32_t f_len;

  fp = fopen(file, "rb");
  if (fp == NULL) {
    fprintf(stderr,"ERROR: opening %s\n", file);
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  f_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  alloc_size = f_len + (buffer_size - (f_len % buffer_size));
  track->buffer = malloc(alloc_size);
  if (track->buffer == NULL) {
    fprintf(stderr,"ERROR: malloc %s\n", file);
    fclose(fp);
    return -1;
  }

  track->size = alloc_size / buffer_size;
  track->time = (f_len - 4) / (TRACKS_RATE * 2);
  fprintf(stderr, "\t%s \n\t\tFile: [%d]\tbuffer_size: [%d]\ttrack_buf_size: [%u]\talloc_size: [%d]\t time: [%u]\n", file, f_len, buffer_size, track->size, alloc_size, track->time); 
  fread(track->buffer, f_len, 1, fp);
  fclose(fp);

  return 0;
}

static int current_frame = 0;

static void FramePlay(void* track_buf) {
  int j;

  // gets frame to a double buffer
  ReadBufS16(track_buf, buffer_d, buffer_size);

  // apply effects to buffer
  for (j = 0; j < frames; j++) {
    *(buffer_d + j) *= playback_gain;
  }

  // wrtie back to a int16_t buffer
  WriteBufS16(buffer_d, buffer_frame, buffer_size);
  
  // play audio
  if ((pcm = snd_pcm_writei(pcm_handle, buffer_frame, frames)) == -EPIPE) {
    fprintf(stderr,"XRUN.\n");
    snd_pcm_prepare(pcm_handle);
  } else if (pcm < 0) {
    fprintf(stderr, "ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
  }
}

void TrackChange(int index) {
  if (index >= tracks_count) {
    playback_cur_track = tracks_count - 1;
  } else if (index < 0) {
    playback_cur_track = 0;
  } else {
    playback_cur_track = index;
  }

  playback_new_track = 1;
}

void TrackNext(int direction) {
  if (direction >= 0) { // forward
    if (playback_cur_track + 1 >= tracks_count) {
      playback_cur_track = 0;
    } else {
      playback_cur_track++;
    }
  } else { // backward
    if (playback_cur_track == 0) {
      playback_cur_track = tracks_count - 1;
    } else {
      playback_cur_track--;
    }
  }

  playback_new_track = 1;
}

void TrackSeek(int amount) {
  // can read current_frame here since for min check the value of current_frame is always rising
  // for max check the for loop in the play thread break loop and pause
  if (current_frame + amount < 0) {
    current_frame = 0;
  } else {
    current_frame += amount;
  }
}

void* TrackPlay(void* na) {
  play_test = 0;
  int mixb = 0;
  // endless loop on track playing thread
  while (1) {

    for(current_frame = 0; current_frame < ( tracks[playback_cur_track].size - 1); current_frame++, mixb++) {
      if (playback_pause == 1) {
	current_frame--; mixb--; // dirty fix
	usleep(100000);
      } else {
	if (play_test == 1) {
	  play_test = 0;
	  mixb =0;
	}
	/*	TrackMix(
		 *(track_buffer + 2) + (buffer_size * current_frame),
		 *(track_buffer + 3) + (buffer_size * mixb),
		 buffer_mix,
		 buffer_size);
		 FramePlay(buffer_mix);*/
	FramePlay(tracks[playback_cur_track].buffer + (buffer_size * current_frame));
      }

      if (playback_new_track == 1) {
	current_frame = 0;
	playback_new_track = 0;
      }
    }

    // song ended - default is to start over and pause
    // NOT THREAD SAFE atm if change play_cur_track in this thread
    // TODO -  need way to tell DJ controller
    playback_pause = 1;

    //    if (snd_pcm_state(state->dev) != SND_PCM_STATE_PAUSED)
    //      snd_pcm_delay(state->dev, &state->delay);
    //    snd_pcm_pause(state->dev, p);
  }

  return NULL;

}

void PlaybackCleanup() {
  int i;
  free(buffer_frame);
  free(buffer_mix);
  free(buffer_d);

  for(i = 0; i < tracks_count; i++) {
    free(tracks[i].buffer);
  }
}

static void PCMSetup() {
  uint32_t tmp;
  char empty_buffer[10000];
  memset(empty_buffer, 0, sizeof(empty_buffer));

  /* Open the PCM device in playback mode */
  if ((pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    fprintf(stderr,"ERROR: Can't open \"%s\" PCM device. %s\n", PCM_DEVICE, snd_strerror(pcm));
  }

  /* Allocate parameters object and fill it with default values*/
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(pcm_handle, params);

  /* Set parameters */
  if ((pcm = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf(stderr,"ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_format(pcm_handle, params, format)) < 0) {
    fprintf(stderr,"ERROR: Can't set format. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels)) < 0) {
    fprintf(stderr,"ERROR: Can't set channels number. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0)) < 0) {
    fprintf(stderr,"ERROR: Can't set rate. %s\n", snd_strerror(pcm));
  }

  if ((pcm = snd_pcm_format_size(format, channels * rate)) < 0) {
    fprintf(stderr,"ERROR: Can't set count %s\n", snd_strerror(pcm));
  }

  /* Write parameters */
  if ((pcm = snd_pcm_hw_params(pcm_handle, params)) < 0) {
    fprintf(stderr,"ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));
  }

  //  snd_pcm_start(pcm_handle);
  snd_pcm_prepare(pcm_handle);
  //  snd_pcm_drain(pcm_handle);

  /* Allocate buffer to hold single period */
  snd_pcm_hw_params_get_period_size(params, &frames, 0);

  snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

  // easily most dirty way of clearing out ring buffer, snd_pcm_drain not fucken working
  snd_pcm_writei(pcm_handle, empty_buffer, frames);
  snd_pcm_writei(pcm_handle, empty_buffer, frames);
  snd_pcm_writei(pcm_handle, empty_buffer, frames);
  snd_pcm_writei(pcm_handle, empty_buffer, frames);
  snd_pcm_writei(pcm_handle, empty_buffer, frames);
  snd_pcm_writei(pcm_handle, empty_buffer, frames);

}

static int TrackSetup() {
  int status;
  int i;
  char full_path[256];
  char path[128] = TRACKS_PATH;
  if (tracks_count >= TRACKS_MAX_COUNT) {
    fprintf(stderr, "Too many tracks loaded already");
    return -1;
  }

  for (i = 0; i < tracks_count; i++) {
    memset(full_path, 0, sizeof(full_path));

    memcpy(full_path, path, strlen(path));
    strcat(full_path, tracks[i].name);

    status = TrackLoad(full_path, &(tracks[i]) );

    if (status < 0) { // error occured
      return -1;
    }
  }

  return 0;
}

void PlaybackSetup() {

  PCMSetup();
  fprintf(stdout, "\tPCMSetup\n");

  tracks = malloc(TRACKS_MAX_COUNT * sizeof(track_t));
  if (tracks == NULL) {
    fprintf(stderr,"ERROR: malloc tracks\n");
  }

  buffer_size = frames * channels * 2 /* 2 -> sample size */;
  //printf("Frames: %d \tChannels: %d \tbuffer_size: %d\n", frames, channels, buffer_size);

  buffer_frame = (void*) malloc(buffer_size);
  buffer_mix = (void*) malloc(buffer_size);
  buffer_d = (double*) malloc(buffer_size);
  if (buffer_mix == NULL){fprintf(stderr,"ERROR: malloc buffer\n");}
  if (buffer_frame  == NULL){fprintf(stderr,"ERROR: malloc buffer\n");}
  if (buffer_d == NULL){fprintf(stderr,"ERROR: malloc buffer_d\n");}

  playback_new_track = 0;
  playback_pause = 0; // start paused by setting as 1
  playback_cur_track = 0;
  playback_gain = 1.0f;

  TracksReadFiles(tracks);
  fprintf(stdout, "\tTrack files read in\n");

  TrackSetup();
  fprintf(stdout, "\tTrack setup\n");

}
 
