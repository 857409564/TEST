#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fcntl.h"
#include "sys/stat.h"
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <sys/time.h> /* timeval{} for select() */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /* inet(3) functions */
#include <errno.h>
#include <netdb.h>
#include <sys/time.h>
#include <limits.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <limits.h>
#include <pthread.h>
#include "process.h"

transformnode transformnodebase = {0};

int audioTransform(prototypenode ** prototype, transformnode * transform)
{
    prototypenode * proto = (*prototype);
    (*prototype) = (*prototype)->next;
		transformnode * node = malloc(sizeof(transformnode));
		memset(node,0,sizeof(transformnode));
    
    node->base   = proto->base;
    node->paline = proto->paline;
    node->pip    = proto->pip;
		node->psign  = proto->psign;
		node->ppcm   = proto->ppcm;
		node->lenth  = proto->lenth;
    
    transformnode * seach = NULL;
    seach = &transformnodebase;
    while(seach->next != NULL) {
        seach = seach->next;
    }
		seach->next = node;
    return 0;
}


static void * pthreadTransform(void *arg)
{
		prototypenode * seach = &prototypenodebase;
    prototypenode * proto = NULL;
    while(1)
    {
        while(seach->next != NULL)
        {
		    		proto = seach->next;
		    		transformnode * node = malloc(sizeof(transformnode));
		    		memset(node,0,sizeof(transformnode));
						seach->next = seach->next->next;
		        node->base   = proto->base;
		        node->paline = proto->paline;
		        node->pip    = proto->pip;
			    	node->psign  = proto->psign;
			    	node->ppcm   = proto->ppcm;
				    node->lenth  = proto->lenth;
				    
				    transformnode * seach1 = NULL;
		        seach1 = &transformnodebase;
		        while(seach1->next != NULL) {
                seach1 = seach1->next;
            }
		        seach1->next = node;
		        free(proto);
        }
        usleep(100);
    }
    
		
		pthread_detach(pthread_self());//·ÀÖ¹ÄÚ´æÐ¹Â¶
	  return 0;	
}

int createTransformthread()
{
		int err;
    pthread_t lntid = 0;
		err=pthread_create(&lntid,NULL,pthreadTransform,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
    return 0;
}