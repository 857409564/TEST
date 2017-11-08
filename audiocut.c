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
#include "audiocut.h"


audionode audionodebase = {0};

static void * pthreadAudiocut(void *arg)
{
		transformnode * seach = &transformnodebase;
    transformnode * proto = NULL;
    
    while(1)
    {
        while(seach->next != NULL)
        {
		    		proto = seach->next;
		    		seach->next = seach->next->next;
            audionode * wordsnode = malloc(sizeof(audionode));
            memset(wordsnode, 0, sizeof(audionode));
		    		
		    		memcpy(wordsnode->paline,proto->paline,8);
		    		memcpy(wordsnode->pip,proto->pip,16);
		    		memcpy(wordsnode->psign,proto->psign,16);
		    		
						long max = 0;
						short * pstart = NULL;
						int s = 0;
						int nframes = 0;
						long * ener = malloc(sizeof(long)*((proto->lenth)/(2*80)));
						memset(ener, 0, sizeof(long)*((proto->lenth)/(2*80)));
						short * pcheck = (short *)proto->ppcm;
						int j = 0;
						for(;pcheck < (short *)(proto->ppcm + (proto->lenth));pcheck = pcheck+80)
						{
						    long sum = 0;
								int i =0;
								for(i=0; i < 80; i++)
								{
										sum = sum + (long)pcheck[i]*(long)pcheck[i];
								}
								if(sum>max) max = sum;
								ener[j]=sum;
								j++;
						}
						for(j=0;j<((proto->lenth)/(2*80));j++)
						{
						  	if((ener[j]>(max/100))&&(max > 7000000))
								{
										/*printf("#\t");*/
										if((pstart == NULL)&&(s == 0))
										{
												pstart =(short *)(proto->ppcm + (80 * 2 * j));
												s = j;
										}
										nframes++;
								}
								else
								{
										/*printf("|\t");*/
										if(nframes >= 20)
										{
												/*44å­—èŠ‚wavå¤? */
												void *wav = malloc(44);
												/*æ·»åŠ wavæ–‡ä»¶å¤? */
												memcpy(wav, "RIFF", 4);
												*(int *)((char*)wav + 4) = (j - s)*80*2 + 36;
												memcpy(((char*)wav + 8), "WAVEfmt ", 8);
												*(int *)((char*)wav + 16) = 16;
												*(short *)((char*)wav + 20) = 1;
												*(short *)((char*)wav + 22) = 1;
												*(int *)((char*)wav + 24) = 16000;
												*(int *)((char*)wav + 28) = 32000;
												*(short *)((char*)wav + 32) = 16 / 8;
												*(short *)((char*)wav + 34) = 16;
												strcpy((char*)((char*)wav + 36), "data");
												*(int *)((char*)wav + 40) = (j - s)*80*2;
												/*free(wav);*/
												
												audioword * savefile = malloc(sizeof(audioword));
												memset(savefile, 0, sizeof(audioword));
												sprintf(savefile->wavname,"./train/wav/word%d.wav",j);
												sprintf(savefile->mfccname,"./train/mfcc/word%d.mfcc",j);
												sprintf(savefile->signame,"./train/sig/word%d.sig",j);
												sprintf(savefile->pcmname,"./train/pcm/word%d.pcm",j);

												FILE * fp1 = fopen(savefile->wavname,"wb+");
												if(fp1==NULL) printf("open filefd error");
												int res = fwrite (wav, 1, 44, fp1 );
												if(res==0) printf("write file error");
												res = fwrite(pstart,1,(j - s)*80*2,fp1);
												
												if(res==0) printf("write file error");
												fclose(fp1);
												free(wav);
												fp1 = fopen(savefile->pcmname,"wb+");
												if(fp1==NULL) printf("open filefd error");
												res = fwrite(pstart,1,(j - s)*80*2,fp1);
												if(res==0) printf("write file error");
												fclose(fp1);
												if(wordsnode->words == NULL){
												    wordsnode->words = savefile;
												    
												}
												else{
												    audioword * seach2 = wordsnode->words;
												    while(seach2->next != NULL)
												    {
												        seach2 = seach2->next;
												    }
												    seach2->next = savefile;
												}
										}
										
										pstart = NULL;
										s = 0;
										nframes =0;
								}
						}
				    free(ener);
				    free(proto->base);
				    audionode * seach1 = NULL;
		        seach1 = &audionodebase;
		        while(seach1->next != NULL) {
                seach1 = seach1->next;
            }
		        seach1->next = wordsnode;
        }
        usleep(100);
    }
    
		
		pthread_detach(pthread_self());//·ÀÖ¹ÄÚ´æÐ¹Â¶
	  return 0;	
}

int createAudiothread()
{
		int err;
    pthread_t lntid = 0;
		err=pthread_create(&lntid,NULL,pthreadAudiocut,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
    return 0;
}