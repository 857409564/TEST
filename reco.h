#ifndef __YX_RECO_H_
#define __YX_RECO_H_

#include "audiocut.h"

typedef struct reconode{
    char paline[20];
    char pip[20];
		char psign[20];
		char cmd[10][10];
		struct reconode * next;
}reconode;

extern reconode reconodebase;
int createRecothread();

#endif