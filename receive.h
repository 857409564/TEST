#ifndef __YX_RECEIVE_H_
#define __YX_RECEIVE_H_


typedef struct prototypenode{
    char * base;
    char * paline;
    char * pip;
		char * psign;
		char * ppcm;
		int lenth;
		struct prototypenode * next;
}prototypenode;

extern prototypenode prototypenodebase;
int createMultithread(char * port);



#endif