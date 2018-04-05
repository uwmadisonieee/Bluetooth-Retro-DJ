#include "tracks.h"

void TracksReadFiles(track_t* tracks) {
  DIR *dir;
  struct dirent *ent;
  tracks_count = 0;
  samples_count = 0;
  
  // TRACKS
  if ((dir = opendir (TRACKS_PATH)) != NULL) {

    while ((ent = readdir (dir)) != NULL) {

      // ignore ./ and ../ inode
      if (!strcmp(ent->d_name, ".") || !strcmp (ent->d_name, "..")) { continue; }

      // needs to be under name size length
      if (strlen(ent->d_name) > TRACKS_NAME_SIZE) { continue; }

      strcpy(tracks[tracks_count].name, ent->d_name);
      printf("Track [%d]: %s\n", tracks_count, ent->d_name);
      tracks_count++;

      if (tracks_count >= TRACKS_MAX_COUNT) { break; }
    }
    closedir(dir);
  } else {
    fprintf(stderr, "ERROR: Can't oppen directory!");
    exit(-2);
  }

  // SAMPLES
  if ((dir = opendir (SAMPLES_PATH)) != NULL) {

    while ((ent = readdir (dir)) != NULL) {

      // ignore ./ and ../ inode
      if (!strcmp(ent->d_name, ".") || !strcmp (ent->d_name, "..")) { continue; }

      // needs to be under name size length
      if (strlen(ent->d_name) > TRACKS_NAME_SIZE) { continue; }

      strcpy(samples_x[samples_count].name, ent->d_name);
      printf("Sample [%d]: %s\n", samples_count, ent->d_name);
      samples_count++;

      if (samples_count >= SAMPLE_MAX_COUNT) { break; }
    }
    closedir(dir);
  } else {
    fprintf(stderr, "ERROR: Can't oppen directory!");
    exit(-2);
  }
}

int TrackLoad(char* file, track_t* track) {
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
  fseek(fp, WAV_HEADER_SIZE, SEEK_SET);
  
  alloc_size = f_len + (buffer_size - (f_len % buffer_size));
  track->buffer = malloc(alloc_size);
  if (track->buffer == NULL) {
    fprintf(stderr,"ERROR: malloc %s\n", file);
    fclose(fp);
    return -1;
  }

  track->size = alloc_size / buffer_size;
  track->file_size = f_len;
  track->time = (f_len - 4) / ((TRACKS_RATE / 1000)* 2); // in ms

  fprintf(stderr, "\t%s \n\t\tFile: [%d]\tbuffer_size: [%d]\ttrack_buf_size: [%u]\talloc_size: [%d]\t time: [%u]ms\n", file, f_len, buffer_size, track->size, alloc_size, track->time); 

  fread(track->buffer, f_len, 1, fp);
  fclose(fp);

  return 0;
}

int SampleLoad(char* file, sample_t* sample) {
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
  fseek(fp, WAV_HEADER_SIZE, SEEK_SET);

  alloc_size = f_len + (buffer_size - (f_len % buffer_size));
  sample->buffer = malloc(alloc_size);
  if (sample->buffer == NULL) {
    fprintf(stderr,"ERROR: malloc %s\n", file);
    fclose(fp);
    return -1;
  }
 
  sample->size = alloc_size / buffer_size;
  fprintf(stderr, "\t%s \n\t\tFile: [%d]\tbuffer_size: [%d]\ttrack_buf_size: [%u]\talloc_size: [%d]\n", file, f_len, buffer_size, sample->size, alloc_size); 

  fread(sample->buffer, f_len, 1, fp);
  fclose(fp);

  return 0;
}


void TracksAsString(char* buffer) {
  int i;
  char *p;
  char temp[TRACKS_NAME_SIZE];
    
  for(i = 0; i < tracks_count; i++) {
    memset(temp, 0, sizeof(temp));
    strncpy(temp, tracks[i].name, strlen(tracks[i].name) - 4); // remove .wav
    for (p = temp; (p = strchr(p, '_')); ++p) { // replace _ for spaces
      *p = ' ';
    }
    strcat(buffer, ";");
    strcat(buffer, temp);
  }

  strcat(buffer, "=");
  
  for(i = 0; i < samples_count; i++) {
    memset(temp, 0, sizeof(temp));
    strncpy(temp, samples_x[i].name, strlen(samples_x[i].name) - 4); // remove .wav
    for (p = temp; (p = strchr(p, '_')); ++p) { // replace _ for spaces
      *p = ' ';
    }
    strcat(buffer, ";");
    strcat(buffer, temp);
  }

}

int TracksInfo(char* buffer, int index) {
  char temp[16];
  sprintf(temp, "%d", tracks[index].time);
  strcat(buffer, temp);
  strcat(buffer, ";");
  memcpy(buffer + strlen(temp) + 1, tracks[index].analysis, TRACKS_ANALYSIS_SIZE);
  return strlen(temp) + 1;
}
  
void TracksAnalysis(track_t* track) {
  int i, j;
  int32_t max, cur;
  // divide by 2 since its int16_t from a void*
  uint32_t stride = (track->file_size / TRACKS_ANALYSIS_SIZE) / 2;
  
  for (i = 0; i < TRACKS_ANALYSIS_SIZE; i++) {

    max = 0;
    for(j = 0; j < stride; j++) {
      cur = *((int16_t*)track->buffer + (i * stride) + j);
      if (cur > max) {
	max = cur;
      }
    }
    // 128 nerfs it down to 0-255
    track->analysis[i] = (uint8_t)(max / 128);
  }
}

int TracksPackAnalysis(void* buffer) {
  int i = 0;
  for (i = 0; i < tracks_count; i++) {
    memcpy(buffer + (i*TRACKS_ANALYSIS_SIZE), &(tracks[i].analysis), TRACKS_ANALYSIS_SIZE);
  }

  return tracks_count * TRACKS_ANALYSIS_SIZE;
}
