#include "playback.h"

static uint32_t rate = TRACKS_RATE; // hard assumption
static uint8_t channels = 1; // mono only

static uint32_t pcm;
static snd_pcm_t* pcm_handle;
static snd_pcm_hw_params_t *params;
static snd_pcm_uframes_t frames;
static snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

static void* buffer_frame;
static void* buffer_mix;
static double* buffer_d;

static time_t t_rand;

double playback_gain;

static void WriteBufS16(double *in, void *out, int s)
{
  int16_t *outn = (int16_t *) out;
  int p = -1;
  while (++p < s) {
	  outn[p] = SAMPLE_TO_S16(in[p]);
  }
}

static void ReadBufS16(int16_t* in, double *out, int s)
{
  int16_t *inn = in;
  while (s-- > 0) {
    out[s] = S16_TO_SAMPLE(inn[s]);
  }
}

static void TrackMix(void* a_buf, void* b_buf, void* m_buf, int length) {
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
}

static uint8_t snake6_count = 0;
static uint8_t snake6_color = 0;
static uint8_t wall_c = 0;
static led_frame a_frame = {0xff, 0x00, 0x00, 0x00};
static void AnalyzeLive(void* buffer_live, int len) {
  int32_t i;
  int32_t max = 0;
  int32_t cur = 0;
  switch(led_effect) {
  case 0: // Red Amp
  case 1: // Green amp
  case 2: // Blue Amp
    for (i = 0; i < len; i++) {
      cur = *((int16_t*)buffer_live + i);
      if (cur > max) {
	max = cur;
      }
    }

    // only on up amplitude
    if (max > 1058) {
      i = max / 1059; // 2^15 / 31 == 1058
      LEDSetAllBrightness((uint8_t) i);
      spi_lcd_or_led = 1;
    }
    break;
  case 3: // Snake Amp
    for (i = 0; i < len; i++) {
      cur = *((int16_t*)buffer_live + i);
      if (cur > max) {
	max = cur;
      }
    }
    if (max < PLAY_COL_1_8) { LEDSnake(255,0,0); }
    else if (max < PLAY_COL_2_8) { LEDSnake(200,50,0); }
    else if (max < PLAY_COL_3_8) { LEDSnake(150,50,50); }
    else if (max < PLAY_COL_4_8) { LEDSnake(120,120,50); }
    else if (max < PLAY_COL_5_8) { LEDSnake(50, 150,150); }
    else if (max < PLAY_COL_6_8) { LEDSnake(70,130,220); }
    else if (max < PLAY_COL_7_8) { LEDSnake(120,120,255); }
    else { LEDSnake(200,200,200); }
      spi_lcd_or_led = 1;
    break;
  case 4: // 4 Walls
    for (i = 0; i < len; i++) {
      cur = *((int16_t*)buffer_live + i);
      if (cur > max) {
	max = cur;
      }
    }
    if (max < PLAY_COL_1_8) { LEDSetSide(wall_c,255,0,0); }
    else if (max < PLAY_COL_2_8) { LEDSetSide(wall_c,0,255,0); }
    else if (max < PLAY_COL_3_8) { LEDSetSide(wall_c,0,0,255); }
    else if (max < PLAY_COL_4_8) { LEDSetSide(wall_c,200,200,0); }
    else if (max < PLAY_COL_5_8) { LEDSetSide(wall_c,0, 200,200); }
    else if (max < PLAY_COL_6_8) { LEDSetSide(wall_c,200,0,200); }
    else { LEDSetSide(wall_c,200,200,200); }
    wall_c++;
    if (wall_c > 3) { wall_c = 0; }
    spi_lcd_or_led = 1;
    break;
  case 5: // random
    for (i = 0; i < len; i++) {
      cur = *((int16_t*)buffer_live + i);
      if (cur > max) {
	max = cur;
      }
    }

    // only on up amplitude
    if (max > 1058) {
      i = max / 1059; // 2^15 / 31 == 1058
      cur = rand() % 3;
      if (cur == 0) {a_frame.red = 255; a_frame.green = 0;a_frame.blue = 0;}
      if (cur == 1) {a_frame.red = 0; a_frame.green = 255;a_frame.blue = 0;}
      if (cur == 2) {a_frame.red = 0; a_frame.green = 0;a_frame.blue = 255;}
      a_frame.head =  0xE0 | (i & 0x1F);
      LEDSetFrame(rand() % 20, a_frame);
      LEDSetFrame(rand() % 20, a_frame);
      LEDSetFrame(rand() % 20, a_frame);
      spi_lcd_or_led = 1;
    }
    
    break;
  case 6: // snake but with amp
    for (i = 0; i < len; i++) {
      cur = *((int16_t*)buffer_live + i);
      if (cur > max) {
	max = cur;
      }
    }

    // only on up amplitude
    if (max > 1058) {
      i = max / 1059; // 2^15 / 31 == 1058

      if (snake6_color == 0) {a_frame.red = 255; a_frame.green = 0;a_frame.blue = 0;}
      if (snake6_color == 1) {a_frame.red = 255; a_frame.green = 255;a_frame.blue = 0;}
      if (snake6_color == 2) {a_frame.red = 0; a_frame.green =255;a_frame.blue = 0;}
      if (snake6_color == 3) {a_frame.red = 0; a_frame.green = 255;a_frame.blue = 255;}
      if (snake6_color == 4) {a_frame.red = 0; a_frame.green = 0;a_frame.blue = 255;}
      a_frame.head = 0xE0 | (i & 0x1F);

      LEDSetFrame(snake6_count, a_frame);

      snake6_count++;
      if (snake6_count >= 20) {
	snake6_count = 0;
	snake6_color++;
	if (snake6_color >= 5) {
	  snake6_color = 0;
	}
      }
       spi_lcd_or_led = 1;
    }
      break;
  default:
    break;
  }

}

static int current_frame = 0;

static void FramePlay(void* track_buf) {
  int j;

  // gets frame to a double buffer
  ReadBufS16(track_buf, buffer_d, frames);

  // apply effects to buffer
  for (j = 0; j < frames; j++) {
    *(buffer_d + j) *= playback_gain;
  }

  // wrtie back to a int16_t buffer
  WriteBufS16(buffer_d, buffer_frame, frames);

  AnalyzeLive(buffer_frame, frames);
  
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
  int s_frame = 0;
  // idk what the fuck, but can't play audio files < 2 seconds
  int s_offset = (TRACKS_RATE * 4) / buffer_size;
  
  // endless loop on track playing thread
  while (1) {

    for(current_frame = 0; current_frame < ( tracks[playback_cur_track].size - 1); current_frame++) {

      if (playback_pause == 1) {
	current_frame--; // dirty fix
	usleep(100000);	
      } else {

	if (playback_play_sample == 1) {
	  // sound fx
	  TrackMix(tracks[playback_cur_track].buffer + (buffer_size * current_frame), samples_x[playback_cur_sample].buffer + (buffer_size * s_frame), buffer_mix, buffer_size);
	  FramePlay(buffer_mix);

	  s_frame++;
	  if (s_frame == samples_x[playback_cur_sample].size - s_offset) {
	    s_frame = 0;
	    playback_play_sample = 0;
	  }
	} else {
	  // normal
	  FramePlay(tracks[playback_cur_track].buffer + (buffer_size * current_frame));
	}

      }

      if (playback_new_track == 1) {
	current_frame = 0;
	playback_new_track = 0;
      }
    }

    s_frame = 0;
    playback_play_sample = 0;
    // song ended - default is to start over and pause
    // NOT THREAD SAFE atm if change play_cur_track in this thread
    // TODO -  need way to tell DJ controller
    playback_pause = 1;
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
  int i;
  char full_path[256];
  char path[128] = TRACKS_PATH;
  char path_s[128] = SAMPLES_PATH;  

  if (tracks_count >= TRACKS_MAX_COUNT) {
    fprintf(stderr, "Too many tracks loaded already");
    return -1;
  }

  for (i = 0; i < tracks_count; i++) {
    memset(full_path, 0, sizeof(full_path));

    memcpy(full_path, path, strlen(path));
    strcat(full_path, tracks[i].name);
   
    TrackLoad(full_path, &(tracks[i]) );
    TracksAnalysis(&tracks[i]);
  }

  // SAMPLE SETUP
  for (i = 0; i < samples_count; i++) {
    memset(full_path, 0, sizeof(full_path));

    memcpy(full_path, path_s, strlen(path_s));
    strcat(full_path, samples_x[i].name);
   
    SampleLoad(full_path, &(samples_x[i]));
  }

  return 0;
}

void PlaybackSetup() {
  clock_t start_t, end_t;
  
  PCMSetup();
  fprintf(stdout, "\tPCMSetup\n");

  /* Intializes random number generator */
  srand((unsigned) time(&t_rand));
  
  tracks = malloc(TRACKS_MAX_COUNT * sizeof(track_t));
  samples_x = malloc(SAMPLE_MAX_COUNT * sizeof(sample_t));
  if (tracks == NULL || samples_x == NULL) {
    fprintf(stderr,"ERROR: malloc tracks and samples_x\n");
  }
  
  buffer_size = frames * channels * 2 /* 2 -> sample size */;
  //printf("Frames: %d \tChannels: %d \tbuffer_size: %d\n", frames, channels, buffer_size);

  buffer_frame = (void*) malloc(buffer_size);
  buffer_mix = (void*) malloc(buffer_size);
  buffer_d = (double*) malloc(buffer_size);
  if (buffer_frame  == NULL){fprintf(stderr,"ERROR: malloc buffer\n");}
  if (buffer_mix == NULL){fprintf(stderr,"ERROR: malloc buffer\n");}
  if (buffer_d == NULL){fprintf(stderr,"ERROR: malloc buffer_d\n");}

  playback_new_track = 0;
  playback_pause = 0; // start paused by setting as 1
  playback_cur_track = 0;
  playback_cur_sample = 0;
  playback_play_sample = 0;
  playback_gain = 1.0f;

  TracksReadFiles(tracks);
  fprintf(stdout, "\tTrack files read in\n");
  
  start_t = clock();
  TrackSetup();
  end_t = clock();
  fprintf(stdout, "Track Setup: %f seconds\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);

}
 
