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
#include "reco.h"
#include <unistd.h>
#include "myHvite.h"

reconode reconodebase = {0};

int releaseword(audionode * proto) {
    audioword * word = NULL;
    audioword * release = NULL;
    word = proto->words;
    while(word != NULL){
        release = word;
        word = word->next;
        free(release);
    }
    free(proto);
    return 0;
}


static void * pthreadReco(void *arg)
{
		audionode * seach = &audionodebase;
    audionode * proto = NULL;
    
    while(1)
    {
        while(seach->next != NULL)
        {
		    		printf("reco:\n");
		    		proto = seach->next;
		    		seach->next = seach->next->next;
		    		reconode * node = malloc(sizeof(reconode));
            memset(node, 0, sizeof(reconode));
		    		memcpy(node->paline,proto->paline,8);
		    		memcpy(node->pip,proto->pip,16);
		    		memcpy(node->psign,proto->psign,16);
		    		
		    		audioword * word = NULL;
		    		word = proto->words;
		    		int cmdcount = 0;
		    		while(1)
		    		{
		    		    if((word == NULL)||(cmdcount > 9)){
		    		        break;
		    		    }
		    		    int save_fd = dup(STDOUT_FILENO);
								int fd = open("reco.log",(O_RDWR | O_CREAT | O_APPEND), 0644);  
								dup2(fd,STDOUT_FILENO);
						    hcopymain(word->wavname, word->mfccname);
						    hvitemain(word->mfccname, node->cmd[cmdcount]);
							remove("reco.mlf");
						    dup2(save_fd,STDOUT_FILENO);
							close(fd);
							close(save_fd);
						    word = word->next;
						    cmdcount++;
		    		}
		    		
		    		releaseword(proto);
		    		
		    		/*int i=0;
		    		for(i=0;i<10;i++) {
		    		    if(node->cmd[i] == NULL) {break;}
		    		    printf("%s\t",node->cmd[i]);
		    		}
				    printf("\n");*/
				    reconode * seach1 = NULL;
		        seach1 = &reconodebase;
		        while(seach1->next != NULL) {
                seach1 = seach1->next;
            }
		        seach1->next = node;
        }
        usleep(100);
    }
    
		
		pthread_detach(pthread_self());//·ÀÖ¹ÄÚ´æÐ¹Â¶
	  return 0;	
}

int createRecothread()
{
		int err;
    pthread_t lntid = 0;
		err=pthread_create(&lntid,NULL,pthreadReco,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
    return 0;
}