#ifndef __YX_PROCESS_H_
#define __YX_PROCESS_H_

#include "receive.h"


typedef struct transformnode{
    char * base;
    char * paline;
    char * pip;
		char * psign;
		char * ppcm;
		int lenth;
		struct transformnode * next;
}transformnode;

extern transformnode transformnodebase;
int createTransformthread();

#endif