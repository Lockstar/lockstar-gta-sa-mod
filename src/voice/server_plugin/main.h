#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#include <winsock.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#include <unistd.h> // sleep
#include <cstring> // memcpy
#include <sys/mman.h> // mprotect
#endif
#include <math.h>
#include <float.h>
#include <malloc.h>

#ifdef WIN32
#define PLUGIN_FILE "mod_sa_voice.dll"
#else
#define PLUGIN_FILE "mod_sa_voice.so"
#endif

#include "RakVoice/RakPeerInterface.h"
#include "RakVoice/BitStream.h"
#include "RakVoice/PacketEnumerations.h"

typedef void (*logprintf_t)(char* format, ...);
extern logprintf_t logprintf;

extern RakPeerInterface *rakPeer;
