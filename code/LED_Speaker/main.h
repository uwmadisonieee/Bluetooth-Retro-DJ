#ifndef SPEAKER_MAIN_H
#define SPEAKER_MAIN_H

#include "audio/playback.h"
#include "server/blueServer.h"

void BlueDataCallback( int type, char* value);

void HardwareSetup( void );


#endif
