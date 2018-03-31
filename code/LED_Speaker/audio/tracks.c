#include "tracks.h"

const double TRACKS_SEC_PER_FRAME = 0.085;

void TracksReadFiles(track_t* tracks) {
  tracks_count = 0;
   
  DIR *dir;
  struct dirent *ent;
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
}
  
void TracksAnalysis(void* buffer, uint32_t size, int index) {
  
}
