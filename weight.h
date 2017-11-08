#ifndef __YX_WEIGHT_H_
#define __YX_WEIGHT_H_

#include "reco.h"

typedef struct weightnode{
    char paline[20];
    char pip[20];
		char psign[20];
		char * cmd[10];
		int lenth;
		struct weightnode * next;
}weightnode;

typedef struct wordweight{
    char prototype[20];
    int weight;
    int position;
    struct wordweight * next;
}wordweight;

typedef struct word{
    char wordname[20];
    char wordtable[20];
    int position;
    struct wordweight * weight;
    struct word * next;
}wordnode;

typedef struct wordtrans{
    char prototype[20];
    char checkword[20];
    struct wordtrans * next;
}wordtrans;

typedef struct checksave{
    char localip[20];
    char cmd[3][20];
    char cmdinfo[20];
    char aimip[20];
    char aimport[20];
    char cmd1[200];
    char cmd2[200];
    int weight;
    char cmdid[20];
    int changesign;
    struct checksave * next;
}checksave;

extern wordnode wordnodebase;
extern weightnode weightnodebase;
int createWeightthread();
#endif