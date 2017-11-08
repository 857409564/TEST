#ifndef __YX_AUDIOCUT_H_
#define __YX_AUDIOCUT_H_

#include "process.h"

typedef struct audioword{
    char pcmname[30];
    char signame[30];
    char wavname[30];
    char mfccname[30];
		struct audioword * next;
}audioword;

typedef struct audionode{
    char paline[20];
    char pip[20];
		char psign[20];
		audioword * words;
		int lenth;
		struct audionode * next;
}audionode;

extern audionode audionodebase;
int createAudiothread();

#endif