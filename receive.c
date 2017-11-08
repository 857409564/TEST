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
#include "receive.h"

#define BACKLOG 10

#define THREADLIMIT 256
#define BUFFER_SIZE 1024

prototypenode prototypenodebase = {0};
int yserverfd = 0;
int yclientfd[THREADLIMIT] = {0};
pthread_t lntid = 0;
pthread_t hntid = 0;
pthread_t cntid[THREADLIMIT] = {0};
int threadcount = 0;
int (*ysocketClientHandle)(int client_fd, char * buffer, int buflen) = NULL;

int server_start(char *port)
{
    struct sockaddr_in serv_addr;
    int sockfd;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;//地址族
    serv_addr.sin_port=htons(atoi(port));//端口号
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//表示机器上的所用网络地址
     
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        return -1;
    }
     
    int on=1;  
    if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {
        perror("setsockopt failed");
        return -2;
    }
    /* 绑定 */
    if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){
        perror("bind");
        return -3;
    }
    /* 侦听连接 */
    if(listen(sockfd,BACKLOG)==-1){
        perror("listen");
        return -4;
    }
    return sockfd;
}

int get_connect(int sockfd)
{
    fd_set readfd;
    struct sockaddr_in client_sockaddr;

    int sin_size;
    int client_fd;
    sin_size=sizeof(struct sockaddr_in);
    while(1)
		{
        if((client_fd=accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size))==-1){
            perror("accept");
            return -1;
        }
        return client_fd;
    }
    /*while*/
}
int server_connect(char *server,char * port)
{
    struct hostent *host;
    struct sockaddr_in serv_addr;
    int sockfd;

    serv_addr.sin_family=AF_INET;//地址族
    serv_addr.sin_port=htons(atoi(port));//端口号
    serv_addr.sin_addr.s_addr = inet_addr(server);

    bzero(&(serv_addr.sin_zero),8);//填充0，保证与struct sockaddr同样大小
 
    /* 建立socket连接 */
     
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket create");
        return -1;
    }
     
    /* 尝试连接远程服务器 */
    if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){
        perror("connect");
        close(sockfd);
        return -1;
    }
    return sockfd;
}



int closeClient(int client_fd)
{
		int i = 0;
		for(i=0;i<THREADLIMIT;i++)
		{
				if(yclientfd[i] == client_fd)
				{
						yclientfd[i] = 0;
						cntid[i] = 0;
						printf("removeClientInfo client_fd: %d     %d\n", i, client_fd);
						close(client_fd);
						return 0;
				}
		}
		printf("not find: %d     %d\n", i, client_fd);
		return -1;
}

static void * pthreadHeart(void *arg)
{
		int i = 0;
		for(i=0;i<THREADLIMIT;i++)
		{
				sleep(60);
				if(yclientfd[i] != 0)
				{
						send(yclientfd[i], "FE0B#", 5, 0);
						return 0;
				}
		}
		
		pthread_detach(pthread_self());//防止内存泄露
	  return 0;	
}

static void * pthreadClient(void *arg)
{
		int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};
    pthread_testcancel();
		
		prototypenode * node = malloc(sizeof(prototypenode));
		memset(node,0,sizeof(prototypenode));
		node->base = malloc(sizeof(char)*1024*1024);
		memset(node->base, 0, sizeof(char)*1024*1024);
		node->paline = node->base;
		node->pip = node->base+8;
		node->psign = node->base+24;
		node->ppcm = node->base+40;
		int str_len = recv(client_fd, node->base, 1024*1024, MSG_WAITALL);
		node->lenth = str_len - 40;
		prototypenode * seach = NULL;
    seach = &prototypenodebase;
    while(seach->next != NULL) {
        seach = seach->next;
    }
		seach->next = node;
		
		
		pthread_testcancel();
		
    usleep(100);
	
		closeClient(client_fd);
		
		pthread_detach(pthread_self());//防止内存泄露
	  return 0;	
}

int insertClientInfo(pthread_t ntid, int client_fd)
{
		int i = 0;
		for(i=0;i<THREADLIMIT;i++)
		{
				if(yclientfd[i] == 0)
				{
						yclientfd[i] = client_fd;
						cntid[i] = ntid;
						return 0;
				}
		}
		return -1;
}

int removeClientInfo(pthread_t ntid, int client_fd)
{
		int i = 0;
		for(i=0;i<THREADLIMIT;i++)
		{
				if(yclientfd[i] == client_fd)
				{
						yclientfd[i] = 0;
						pthread_cancel(cntid[i]); 
						cntid[i] = 0;
						return 0;
				}
		}
		return -1;
}

static void * pthreadListen(void *arg)
{
		int err = 0;
		while(1)
		{
				pthread_testcancel();
				int client_fd = get_connect(yserverfd);
				if(client_fd <= 0)
				{
						printf("connect error: %d\n",client_fd);
						sleep(5);
						continue;
				}
				printf("connect: %d\n",client_fd);
				pthread_t ntid;
				err = pthread_create(&ntid,NULL,pthreadClient,&client_fd);
			  if ( 0 != err )
		    {
		      	printf("can't create thread:%s\n", strerror(err));
				    continue;
				}
				err = insertClientInfo(ntid, client_fd);
				if ( 0 != err )
		    {
		      	printf("Full channel:%s\n", strerror(err));
				    continue;
				}
		}
		pthread_detach(pthread_self());//防止内存泄露
	  return 0;	
}

int createMultithread(char * port)
{
		int err;
		struct sockaddr_in serv_addr;

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;//地址族
    serv_addr.sin_port=htons(atoi(port));//端口号
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//表示机器上的所用网络地址
     
    if((yserverfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        return -1;
    }
     
    int on=1;  
    if((setsockopt(yserverfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {
        perror("setsockopt failed");
        return -2;
    }
    /* 绑定 */
    if(bind(yserverfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){
        perror("bind");
        return -3;
    }
    /* 侦听连接 */
    if(listen(yserverfd,BACKLOG)==-1){
        perror("listen");
        return -4;
    }
    
		err=pthread_create(&lntid,NULL,pthreadListen,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
    
}

int closeMultithread()
{
		int i = 0;
		pthread_cancel(hntid);
		pthread_cancel(lntid);
		for(i=0;i<THREADLIMIT;i++)
		{
				if(yclientfd[i] != 0)
				{
						yclientfd[i] = 0;
						pthread_cancel(cntid[i]); 
						cntid[i] = 0;
						return 0;
				}
		}
		return 0;
}
