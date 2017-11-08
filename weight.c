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
#include "weight.h"
#include "sqlite3.h"

#define UPDATELOCKED do{updatesign = 0;}while(0)
#define UPDATEUNLOCK do{updatesign = 1;}while(0)

wordnode wordnodebase = {0};
wordtrans wordtransbase = {0};
weightnode weightnodebase = {0};
int updatesign = -1;


int select_loadweight(void * data, int col_count, char ** col_values, char ** col_Name)
{
	  wordweight * node = malloc(sizeof(wordweight));
	  memset(node, 0, sizeof(wordweight));
	  sprintf(node->prototype,"%s",col_values[0]);
	  node->weight = atoi(col_values[1]);
	  
	  wordnode * woednode = data;
	  
	  wordweight * seach = woednode->weight;
	  if(seach == NULL){
	      woednode->weight = node;
	      return 0;
	  }
	  while(seach->next != NULL){
	      seach = seach->next;
	  }
	  seach->next = node;
		return 0;
}

int select_loadword(void * data, int col_count, char ** col_values, char ** col_Name)
{
	  sqlite3 * db = data;
	  
	  wordnode * node = malloc(sizeof(wordnode));
	  memset(node, 0, sizeof(wordnode));
	  sprintf(node->wordname,"%s",col_values[0]);
	  sprintf(node->wordtable,"%s",col_values[1]);
    node->position = atoi(col_values[2]);
    char cmd[200]={0};
    char *errmsg = 0;
    sprintf(cmd,"select * from %s;",node->wordtable);
    sqlite3_exec( db, cmd, select_loadweight, node, &errmsg);
    wordnode * seach = NULL;
    seach = &wordnodebase;
    while(seach->next != NULL) {
        seach = seach->next;
    }
    seach->next = node;
		return 0;
}

int select_loadwords(void * data, int col_count, char ** col_values, char ** col_Name)
{
	  //printf("%s %s\n", col_values[0], col_values[1]);
	  
	  char * start = col_values[1];
	  char * end = col_values[1];
	  while(*start != 0) {
	      while((*end != ',')&&(*end != 0)) {end++;}
	      wordtrans * node = malloc(sizeof(wordtrans));
	      memset(node, 0, sizeof(wordtrans));
	      memcpy(node->checkword,col_values[0],strlen(col_values[0]));
	      memcpy(node->prototype,start,end-start);
	      wordtrans * seach = NULL;
        seach = &wordtransbase;
        while(seach->next != NULL) {
            seach = seach->next;
        }
        seach->next = node;
	      start = end + 1;
	      end = start;
	  }
		return 0;
}

int loadWords(char * sqlname)
{
    char cmd[200]={0};
    sqlite3 *db;
    char *errmsg = 0;
    int result = -1;
    result = sqlite3_open(sqlname, &db);
    if(result == 0) {
        //printf("open : %s\n",sqlname);
    }
    else {
        printf("open : %d\n",result);
    }
    sprintf(cmd,"select * from %s;","wordslist");
    //printf("select * from %s;\n","wordslist");
    sqlite3_exec( db, cmd, select_loadword, db, &errmsg);
    result = sqlite3_close(db);
    
    wordnode * seach = NULL;
    wordnode * check = NULL;
    seach = wordnodebase.next;
    while(seach != NULL) {
        wordweight * seach1 = seach->weight;
        while(seach1 != NULL) {
            for(check = wordnodebase.next; check!=NULL; check = check->next) {
                if(strcmp(seach1->prototype,check->wordname) == 0) {
                    seach1->position = check->position;
                    break;
                }
            }
            if(seach1->position == 0) {
                printf("Can not found word!\n");
            }
            seach1 = seach1->next;
        }
        seach = seach->next;
    }
    
    result = sqlite3_open(sqlname, &db);
    if(result == 0) {
        //printf("open : %s\n",sqlname);
    }
    else {
        printf("open : %d\n",result);
    }
    sprintf(cmd,"select * from %s;","wordstrans");
    //printf("select * from %s;\n","wordstrans");
    sqlite3_exec( db, cmd, select_loadwords, db, &errmsg);
    result = sqlite3_close(db);
    
    return 0;
}

int releaseWords()
{
    wordnode * seach = NULL;
    seach = wordnodebase.next;
    wordnode * rlsword = NULL;
    wordweight * rlsweight = NULL;
    while(seach != NULL) {
        rlsword = seach;
        wordweight * seach1 = seach->weight;
        while(seach1 != NULL){
            rlsweight = seach1;
            seach1 = seach1->next;
            free(rlsweight);
        }
        seach = seach->next;
        free(rlsword);
    }
    memset(&wordnodebase,0,sizeof(wordnode));
    
    wordtrans * seach2 = NULL;
    seach2 = wordtransbase.next;
    wordtrans * rlsword2 = NULL;
    while(seach2 != NULL) {
        rlsword2 = seach2;
        seach2 = seach2->next;
        free(rlsword2);
    }
    memset(&wordtransbase,0,sizeof(wordtrans));
    
    return 0;
}

int select_checkcmd(void * data, int col_count, char ** col_values, char ** col_Name)
{
	 /* printf("%s\n", col_values[0]);
	  printf("%s\n", col_values[1]);
	  printf("%s\n", col_values[2]);
	  printf("%s\n", col_values[3]);
	  printf("%s\n", col_values[4]);
	  printf("%s\n", col_values[5]);
	  printf("%s\n", col_values[6]);
	  printf("%s\n", col_values[7]);
	  printf("%s\n", col_values[8]);*/
	  checksave * cmdmax = data;
	  strcpy(cmdmax->localip,col_values[0]);
	  strcpy(cmdmax->cmd[0],col_values[1]);
	  strcpy(cmdmax->cmd[1],col_values[2]);
	  strcpy(cmdmax->cmd[2],col_values[3]);
	  strcpy(cmdmax->cmdinfo,col_values[4]);
	  strcpy(cmdmax->aimip,col_values[5]);
	  strcpy(cmdmax->aimport,col_values[6]);
	  strcpy(cmdmax->cmd1,col_values[7]);
	  strcpy(cmdmax->cmd2,col_values[8]);
	  cmdmax->changesign = 0;
	  return 0;
}

int checkCmd(char * local, char * cmd1, char * cmd2, char * cmd3)
{
    wordnode * cmd[3] = {0};
    wordnode * seach = NULL;
    seach = wordnodebase.next;
    printf("input:%s %s %s %s\n",local, cmd1, cmd2, cmd3);
    while(seach != NULL) {
        if(strcmp(cmd1,seach->wordname) == 0) {
            cmd[0] = seach;
        }
        if(strcmp(cmd2,seach->wordname) == 0) {
            cmd[1] = seach;
        }
        if(strcmp(cmd3,seach->wordname) == 0) {
            cmd[2] = seach;
        }
        seach = seach->next;
    }
    if((cmd[0] == NULL)||(cmd[1] == NULL)||(cmd[2] == NULL)) {
        printf("check no word!\n");
        return -2;
    }
    checksave cmdmax = {0};
    wordweight * cmdww[3] = {0};
	int ti=0,tj=0,tk=0;
    for(cmdww[0] = cmd[0]->weight,ti=0; cmdww[0] != NULL; cmdww[0] = cmdww[0]->next,ti++) {
        for(cmdww[1] = cmd[1]->weight,tj=0; cmdww[1] != NULL; cmdww[1] = cmdww[1]->next,tj++) {
            for(cmdww[2] = cmd[2]->weight,tk=0; cmdww[2] != NULL; cmdww[2] = cmdww[2]->next,tk++) {
                /*if(((cmdww[0]->position & 0x01) == 0)||((cmdww[1]->position & 0x02) == 0)||((cmdww[2]->position & 0x04) == 0)) {
                    continue;
                }*/
                if(cmdmax.weight < cmdww[0]->weight*cmdww[1]->weight*cmdww[2]->weight) {
                    cmdmax.changesign = -1;
                    char cmdtrans[3][20] = {0};
                    int i = 0;
                    wordtrans * seachword = NULL;
                    for(i = 0; i < 3; i++) {
                        for(seachword = &wordtransbase; seachword != NULL; seachword = seachword->next) {
                            if(strcmp(cmdww[i]->prototype, seachword->prototype) == 0) {
                                strcpy(cmdtrans[i], seachword->checkword);
                                break;
                            }
                        }
                    }
                    if((cmdtrans[0][0] == 0)||(cmdtrans[1][0] == 0)||(cmdtrans[2][0] == 0)) {
                        printf("missing words!\n");
                        continue;
                    }
                    
                    char cmd[1000]={0};
                    sqlite3 *db;
                    char *errmsg = 0;
                    int result = -1;
                    result = sqlite3_open("wordweight.db", &db);
                    if(result == 0) {
                        printf("open : %s\n","wordweight.db");
                    }
                    else {
                        printf("open : %d\n",result);
                    }
                    
                    //printf("##%s,%s,%s,%s:||\n",local,cmdtrans[0],cmdtrans[1],cmdtrans[2]);
                    //printf("select * from %s where localip='%s' and word1='%s' and word2='%s' and word3='%s'\n;","wordsnet",local,cmdtrans[0],cmdtrans[1],cmdtrans[2]);
                    sprintf(cmd,"select * from %s where localip='%s' and word1='%s' and word2='%s' and word3='%s';","wordsnet",local,cmdtrans[0],cmdtrans[1],cmdtrans[2]);
                    sqlite3_exec( db, cmd, select_checkcmd, &cmdmax, &errmsg);
                    result = sqlite3_close(db);
                    if(cmdmax.changesign == 0) {
                        cmdmax.weight = cmdww[0]->weight*cmdww[1]->weight*cmdww[2]->weight;
                    }
					printf("%s:##count %d-%d-%d  ##%s,%s,%s:||%d\n",local,ti,tj,tk,cmdtrans[0],cmdtrans[1],cmdtrans[2],cmdww[0]->weight*cmdww[1]->weight*cmdww[2]->weight);
					
					
                }
                //printf("-----------%s,%s,%s,%d\n",cmdww[0]->prototype,cmdww[1]->prototype,cmdww[2]->prototype,cmdww[0]->weight*cmdww[1]->weight*cmdww[2]->weight);
            }
        }
    }
    
    if(cmdmax.weight >= 10) {
        printf("----%s,%s,%s,%s,%s,%s,%s,%s,%s,%d\n", cmdmax.localip, cmdmax.cmd[0], cmdmax.cmd[1], cmdmax.cmd[2], cmdmax.cmdinfo, cmdmax.aimip, cmdmax.aimport, cmdmax.cmd1, cmdmax.cmd2, cmdmax.weight);
        /*printf("----%s\n", cmdmax.localip);
        printf("----%s\n", cmdmax.cmd[0]);
        printf("----%s\n", cmdmax.cmd[1]);
        printf("----%s\n", cmdmax.cmd[2]);
        printf("----%s\n", cmdmax.cmdinfo);
        printf("----%s\n", cmdmax.aimip);
        printf("----%s\n", cmdmax.aimport);
        printf("----%s\n", cmdmax.cmd1);
        printf("----%s\n", cmdmax.cmd2);
        printf("----%d\n", cmdmax.weight);*/
        char cmd1[2000]={0};
        printf(">>>>/mnt/htdocs/zn_shell/zn_php_root %s %s %s\n", cmdmax.aimip, cmdmax.aimport, cmdmax.cmd1);
	      sprintf(cmd1,"/mnt/htdocs/zn_shell/zn_php_root %s %s %s", cmdmax.aimip, cmdmax.aimport, cmdmax.cmd1);
	      printf("%s",cmd);fflush(stdout);
	      system(cmd1);
	      usleep(100);
	      char cmd2[2000]={0};
	      printf(">>>>/mnt/htdocs/zn_shell/zn_php_root %s %s %s\n", cmdmax.aimip, cmdmax.aimport, cmdmax.cmd2);
	      sprintf(cmd2,"/mnt/htdocs/zn_shell/zn_php_root %s %s %s", cmdmax.aimip, cmdmax.aimport, cmdmax.cmd2);
	      system(cmd2);
    }
    else {
        printf("----no cmd reco!\n");
    }
    //printf("-----------%s,%s,%s,%s,%d\n", cmdmax.cmd[0], cmdmax.cmd[1], cmdmax.cmd[2], cmdmax.cmdid, cmdmax.weight);
    
    
    return 0;
}

static void * pthreadUpdate(void *arg)
{
    struct stat statsave = {0};
    int res = stat("wordweight.db", &statsave);
    if(res != 0) {
        printf("Update save error : %d\n",res);
    }
    while(1)
    {
        struct stat statcheck = {0};
        res = stat("wordweight.db", &statcheck);
        if(res != 0) {
            printf("Update check error : %d\n",res);
        }
        if(statcheck.st_mtime > statsave.st_mtime) {
            UPDATELOCKED;
            res = stat("wordweight.db", &statsave);
            printf("Update!\n");
            releaseWords();
            printf("Release!\n");
            loadWords("wordweight.db");
            printf("Load!\n");
            UPDATEUNLOCK;
        }
        sleep(1);
    }
    
		
		pthread_detach(pthread_self());//·ÀÖ¹ÄÚ´æÐ¹Â¶
	  return 0;	
}

static void * pthreadWeight(void *arg)
{
		reconode * seach = &reconodebase;
    reconode * proto = NULL;
    
    while(1)
    {
        while(seach->next != NULL)
        {
		    		proto = seach->next;
		    		seach->next = seach->next->next;
		    		weightnode * node = malloc(sizeof(weightnode));
            memset(node, 0, sizeof(weightnode));
		    		memcpy(node->paline,proto->paline,8);
		    		memcpy(node->pip,proto->pip,16);
		    		memcpy(node->psign,proto->psign,16);
		    		
		    		int i=0;
		    		for(i=0;i<10;i++) {
		    		    if(proto->cmd[i] == NULL) {break;}
		    		    printf("%s\t",proto->cmd[i]);
		    		}
				    printf("\n");
				    //printf("%d %d %d %d\n",strlen(proto->cmd[0]),strlen(proto->cmd[1]),strlen(proto->cmd[2]),strlen(proto->cmd[3]));
				    
		    		if((strlen(proto->cmd[0]) != 0)&&(strlen(proto->cmd[1]) != 0)&&(strlen(proto->cmd[2]) != 0)&&(strlen(proto->cmd[3]) == 0)) {
		    		    printf("%s %s %s %s\n",proto->pip, proto->cmd[0], proto->cmd[1], proto->cmd[2]);
		    		    char cmd[3][20] ={0};
		    		    strcpy(cmd[0],proto->cmd[0]);
		    		    strcpy(cmd[1],proto->cmd[1]);
		    		    strcpy(cmd[2],proto->cmd[2]);
		    		    //checkCmd(proto->pip, proto->cmd[0], proto->cmd[1], proto->cmd[2]);
		    		    checkCmd(proto->pip, cmd[0], cmd[1], cmd[2]);
		    		    //checkCmd(proto->pip, "kai", "dian", "shi");
		    		}
		    		else{
		    		    printf("error cmd catch!\n");
		    		}

				    weightnode * seach1 = NULL;
		        seach1 = &weightnodebase;
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

int createWeightthread()
{
    
		int err;
    loadWords("wordweight.db");
    
    pthread_t lntid = 0;
		err=pthread_create(&lntid,NULL,pthreadUpdate,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
    
    pthread_t wntid = 0;
		err=pthread_create(&wntid,NULL,pthreadWeight,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -2;
	  }
	  
	  
	  
    return 0;
}


int main2(int argc, char * argv[])
{
    loadWords("wordweight.db");
    printf("close : wordweight.db\n");
    wordnode * seach = NULL;
    seach = wordnodebase.next;
    while(seach != NULL) {
        printf("***************************************\n");
        printf("*   %s \t",seach->wordname);
        printf("%s\t",seach->wordtable);
        printf("%d\n",seach->position);
        wordweight * seach1 = seach->weight;
        while(seach1 != NULL){
            printf("*   |- %s, %d, %o\n",seach1->prototype, seach1->weight, seach1->position);
            seach1 = seach1->next;
        }
        seach = seach->next;
    }
    
    int err;
    pthread_t lntid = 0;
		err=pthread_create(&lntid,NULL,pthreadUpdate,NULL);
		if ( 0 != err )
		{
	     	printf("can't create thread:%s\n", strerror(err));
				return -1;
	  }
	  
    
    //checkCmd("kai","dian","shi");
    
    while(1) sleep(1);
    
    releaseWords();
    return 0;
}

