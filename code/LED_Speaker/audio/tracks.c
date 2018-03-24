#include "tracks.h"

static char track_list[TRACKS_MAX_COUNT][TRACKS_NAME_SIZE];

void TracksReadFiles() {
  tracks_count = 0;
   
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (TRACKS_PATH)) != NULL) {

    while ((ent = readdir (dir)) != NULL) {

      // ignore ./ and ../ inode
      if (!strcmp(ent->d_name, ".") || !strcmp (ent->d_name, "..")) { continue; }

      // needs to be under name size length
      if (strlen(ent->d_name) > TRACKS_NAME_SIZE) { continue; }

      strcpy(track_list[tracks_count], ent->d_name);
      
      tracks_count++;

      if (tracks_count >= TRACKS_MAX_COUNT) { break; }
    }
    closedir(dir);
  } else {
    fprintf(stderr, "ERROR: Can't oppen directory!");
    exit(-2);
  }
}

char* TracksAt(int index) {
  if (index >= TRACKS_MAX_COUNT || index < 0) {
    return "DEADBEEF";
  } else {
    return track_list[index];
  }
}

void TracksAsString(char* buffer) {
  int i;
  char *p;
  char temp[TRACKS_NAME_SIZE];
    
  for(i = 0; i < tracks_count; i++) {
    memset(temp, 0, sizeof(temp));
    strncpy(temp, track_list[i], strlen(track_list[i]) - 4); // remove .wav
    for (p = temp; (p = strchr(p, '_')); ++p) { // replace _ for spaces
      *p = ' ';
    }
    strcat(buffer, ";");
    strcat(buffer, temp);
  }
}
  
