#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <signal.h> 
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h> 
#include <fcntl.h>
#define nanosecondsecond 1000000000
#define SNAME "/mysem13543"
#include "queue.h"
#define n 18
#define m 20


long grain;

typedef struct Clock 
{
	int	second;
	long int nanosecond;
	int clockcounts;
} Clock;

struct messagebuffer { 
    long messagetype; 
    char messagetext[10]; 
	int addr;
	int reqType;
	int pid;
} message; 

typedef struct page_table {
int tabel[32];
int reference[32];
} page;

typedef struct frame_table {
int tabel[256];
char status[256];
int reference[256];
} frame;

page pt;
frame fTabel;
FILE *fptr;
int pointer = 0;
int clockid,messageid, s = 18; 
float milli = 0;
int fault = 0,total  = 0;
Clock *logclock;
int lines = 0;
int pids[18] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
struct Queue pQ;
struct Queue aQ;
struct Queue timeQ;
struct Queue reqQ;
void memStatus();
void sigHandler(int sig_num) 
{ 
	int i,j;
	
	memStatus();
	
		

	float k = fault/(float)total;
	
	long t = (logclock -> second * nanosecondsecond) + logclock -> nanosecond;
	int t2 = t/total;
	
	

	fprintf(stderr,"Number of Request generated --> %d\n",total);
	fprintf(stderr,"Number of Faults --> %d\n",fault);
	fprintf(stderr,"memory accesses per second (Total) --> %d\n",(int)total/logclock -> second);
	fprintf(stderr,"page faults per memory access (Total) --> %.4f \n",k);
	fprintf(stderr,"memory access speed (Avearge)--> %ld nanosecond\n",t2);
	fprintf(stderr,"clock time status --> second --> %d --> nanosecond --> %d\n", logclock -> second, logclock -> nanosecond);
	fprintf(stderr,"Number of Childs Created --> %d\n",logclock -> clockcounts);

    if(sig_num == 2)
    	fprintf(stderr,"Thread interrupted\n");
    else
	fprintf(stderr,"Program time exceeds limit\n");


	
i= 0;
while (i<18)

{
        if(pids[i] > 0 )
	kill(pids[i], SIGTERM);
        i++;
}


	fprintf(stderr,"kill all childs which forked\n");
	shmdt(logclock);
	fprintf(stderr,"Release Shared Memory \n");
	fclose(fptr);
	fprintf(stderr,"File Point Closed\n");
	shmctl(clockid,IPC_RMID,NULL);
	fprintf(stderr,"release shared memory\n");
	msgctl(messageid, IPC_RMID, NULL); 
	fprintf(stderr,"cleared Queue\n");
        abort(); 
        fflush(stdout); 



}

void memStatus()
{
	
	int i;
	fprintf(fptr,"\n\n*********************************************\n\n");
	fprintf(fptr,"Current memory Board %d:%ld is:\n",logclock -> second,logclock -> nanosecond);
	
	fprintf(fptr,"Page Tabel memory status : \n");
	fprintf(fptr,"Page Tabel : ");

	
		

i= 0;
while (i<32)
{
        fprintf(fptr,"%d  ",pt.tabel[i]);
        i++;
}

	fprintf(fptr,"\n");
	fprintf(fptr,"reference : ");

i= 0;
while (i<32)

{
        fprintf(fptr,"%d  ",pt.reference[i]);
        i++;
}


	fprintf(fptr,"\n\n");
	fprintf(fptr,"Page Frame Tabel status : \n\n");

i= 0;
while (i<256)

{
        fprintf(fptr,"%c ",fTabel.status[i]); 
        i++;
}

	fprintf(fptr,"\n\n\n");

i= 0;
while (i<256)

{
		if(fTabel.reference[i] != -1)
			fprintf(fptr,"%d ",fTabel.reference[i]);
		else
			fprintf(fptr,". ");
        		i++;
}
        fprintf(fptr,"\n");
	fprintf(fptr,"\n\n-------------------------------------------------\n\n");
	
}
	
int forkChild()
{
	
	int i=0;

while (i<s)
{
			if(pids[i] == -1)
			return i;
                        i++;      
}
			return -1;
}

int getmemFrame(int add)
{
	int i=0;

while (i<32)
{
		        if(pt.tabel[i] == add)
			return i;
                        i++;      
}
			return -1;
}
int mempageFault(int add)
{
	
	
	int temp,idx;
	fault++;
	while(pt.reference[pointer] == 1)
	{
		pt.reference[pointer] = 0;
		fTabel.reference[pt.tabel[pointer]] = 0;
		pointer = (pointer + 1) % 32;
	}
	temp = pt.tabel[pointer];
	if(temp != -1)
	{
	fTabel.tabel[temp] = -1;
	fTabel.reference[temp] = -1;
	fTabel.status[temp] = '.';
	}
	
	pt.tabel[pointer] = add;
	fTabel.tabel[add] = pointer;
	fTabel.reference[add] = 0;
	pt.reference[pointer] = 0;
	pointer = (pointer + 1) % 32;
	idx = pointer - 1;
	if(idx == -1)
		idx = 31;
	fprintf(fptr,"release frame %d and swapp in %d in page %d\n",temp,add,idx);
	return (idx);
}

int getmemloc(int pid)
{
	
		
	int i=0;

while (i<18)
{
		        if(pids[i] == pid)
			return i;
                        i++;      
}

	return -1;
}

void ClockClick()
{
	logclock -> nanosecond += 100000;
	if(logclock -> nanosecond > nanosecondsecond)
		{
			logclock -> second += logclock -> nanosecond/nanosecondsecond;
			logclock -> nanosecond %= nanosecondsecond;
		}	
	
}

void start()
{
	int i=0;

while (i<32)
{
		pt.tabel[i] = -1;
		pt.reference[i] = 0;
                        i++;      
}

i=0;

while (i<256)
{
		fTabel.tabel[i] = -1;
		fTabel.status[i] = '.';
		fTabel.reference[i] = -1;
                i++;      
}

}

void QueueStatus()
{
	
	double t;
	int childPid,addr,pageId,frame,Type;
	
	do
	{
	
	t = frontNode(&timeQ);
	if(t == -1.0)
		return;
	
	long temp = (logclock -> second * nanosecondsecond) + logclock -> nanosecond;
	milli = temp / 1000000.0f;
	if(t < milli)
	{
		childPid  = (int)frontNode(&pQ);
		addr = (int)frontNode(&aQ);
		Type = (int)frontNode(&reqQ);
		
		pop(&pQ);
		pop(&aQ);
		pop(&timeQ);
		pop(&reqQ);
		pageId = mempageFault(addr/1000);	
		
		
				
				frame = pt.tabel[pageId];
				pt.reference[pageId] = 1;
				fTabel.reference[addr/1000] = 1;

				
			
			
				if(Type == 0)
				{
				
					fTabel.status[addr/1000] = 'U';
					fprintf(fptr,"address %d in frame %d, giving data to %d at time %d:%ld\n",addr,frame,childPid,logclock -> second,logclock -> nanosecond);
					
					message.messagetype = childPid;
					msgsnd(messageid, &message, sizeof(message),0); 
			
				}
				else if(Type == 1)
				{
					fTabel.status[addr/1000] = 'D';
					fprintf(fptr,"address %d in frame %d, writing data to frame at time %d:%ld\n",addr,frame,logclock -> second,logclock -> nanosecond);
					fprintf(fptr,"Dirty bit of frame %d set, adding additional time to the clock\n",frame);
					message.messagetype = childPid;
					msgsnd(messageid, &message, sizeof(message),0); 
					fprintf(fptr,"Indicating to %d that write has happened to addr %d\n",childPid,addr);
				}
				logclock -> nanosecond += 10;	
				if(logclock -> nanosecond > nanosecondsecond)
				{
					logclock -> second += logclock -> nanosecond/nanosecondsecond;
					logclock -> nanosecond %= nanosecondsecond;
				}	
	}
	t = frontNode(&timeQ);
	if(t == -1.0)
		return;
	
	temp = (logclock -> second * nanosecondsecond) + logclock -> nanosecond;
	milli = (float)temp / 1000000;
	
	}while(t < milli);
}

int main (int argc,char *argv[]) 
{ 
	
	start();
	grain = time(NULL);
	char arg1[20];
	char arg2[20];
	char arg3[20];
	char *filename = "default";
	int c, i,status,pid,t=2;
	int TimeToGenrate = 0;
	init(&pQ);
	init(&aQ);
	init(&timeQ);
	init(&reqQ);
	signal(SIGALRM, sigHandler);
    signal(SIGINT, sigHandler);
	
	while ((c = getopt (argc, argv, "hl:t:s:")) != -1)
	switch (c)
    {
		case 'h':
			printf("\nOPTIONS :\n");
            printf("-h for HELP \n");
			printf("-l filename  (File where output to be stored)\n");
			printf("-v max number of process");
			printf("-t z (where z is maximum no.of second program allowed to run\n");
			return 1;
		case 'l':
			filename = optarg;
			break;
		case 's':
			s = atoi(optarg);
			if(s > 18)
			{
				fprintf(stderr,"Maximum num of process allowed are 18,value set to 18");
				s = 18;
			}
			break;	
		case 't':
			t = atoi(optarg);
			break;	
		case '?':
			if (optopt == 'z' || optopt == 'l' || optopt == 's')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
			return 1;
		default:
        abort ();
      }
	  
	fptr = fopen(filename, "w");
    if(fptr == NULL)
    {
      fprintf(stderr,"File Open ERROR \n");
      exit(1);
    }
	fprintf(stderr,"Log file : %s \n",filename);

	key_t key = ftok(".",'v');
	key_t key2 = ftok(".",'r');
	
	clockid = shmget(key,sizeof(logclock),0666|IPC_CREAT);
	if ( clockid == -1 )
		fprintf(stderr,"OSS : Error in shmget2");
	
	logclock = (Clock *)shmat(clockid, NULL, 0);
	
	logclock -> second = 0;
	logclock -> nanosecond = 0;
	logclock -> clockcounts  = 0;
	
	messageid = msgget(key2, 0666 | IPC_CREAT); 
	
	snprintf(arg1,20,"%d", clockid);
	snprintf(arg2,20,"%d",messageid);
	
	
	
	alarm(t);	
	int loc = -1;
	int w = 0;
	int frame,addr,childPid,pageId;
	while(1)
	{
		
		if((loc = forkChild()) >= 0/* && logclock -> second  >= TimeToGenrate*/)
		{
			
			if( (pid = fork()) == 0)
			{
				execlp("./user","./user",arg1,arg2,arg3,(char *)NULL);
				fprintf(stderr,"%s failed to execution worker!\n",argv[0]);
				exit(0);
			}
			srand(grain++);
			TimeToGenrate += rand()%2; 
			pids[loc] = pid;
		}
		
		if(msgrcv(messageid, &message, sizeof(message), 1,IPC_NOWAIT) != -1)
		{
			
		if(message.reqType == 2)
		{
					int status;
					waitpid(message.pid, &status,0);
					pids[getmemloc(message.pid)] = -1;
					fprintf(fptr,"Pid : %d Completed execution\n",message.pid);
		}
		else{
			if(total % 100 == 0)
			memStatus();
			total++;
			addr = message.addr;
			childPid = message.pid;
			fprintf(fptr,"%d requesting read of address %d at time %d:%ld\n",childPid,addr,logclock -> second,logclock -> nanosecond);
			if((pageId = getmemFrame(addr/1000)) == -1)
			{
				long temp = (logclock -> second * nanosecondsecond) + logclock -> nanosecond;
				milli = (float)temp / 1000000;
				milli = milli + 15;
				double t1 = (double)childPid;
				double t2 = (double)addr;
				push(&pQ,t1);
				push(&aQ,t2);
				push(&timeQ,milli);
				push(&reqQ,message.reqType);
				fprintf(fptr,"address %d is not in frame, memory pageFault\n",addr);
				fprintf(fptr,"memory pageFault : Blocking Pid : %d\n",childPid);
				
				
			}
			else
			{
				
				frame = pt.tabel[pageId];					//No- Page Fault
				pt.reference[pageId] = 1;
				fTabel.reference[addr/1000] = 1;
				
			
			
				if(message.reqType == 0)
				{
				
					fTabel.status[addr/1000] = 'U';
					fprintf(fptr,"address %d in frame %d, giving data to %d at time %d:%ld\n",addr,frame,childPid,logclock -> second,logclock -> nanosecond);
					
					message.messagetype = childPid;
					msgsnd(messageid, &message, sizeof(message),0); 
			
				}
				else if(message.reqType == 1)
				{
					fTabel.status[addr/1000] = 'D';
					fprintf(fptr,"address %d in frame %d, writing data to frame at time %d:%ld\n",addr,frame,logclock -> second,logclock -> nanosecond);
						fprintf(fptr,"Dirty bit of frame %d set, adding additional time to the clock\n",frame);
					message.messagetype = childPid;
					msgsnd(messageid, &message, sizeof(message),0); 
					fprintf(fptr,"Highlights to %d that write has happened to address %d\n",childPid,addr);
				}
				logclock -> nanosecond += 10;	
				if(logclock -> nanosecond > nanosecondsecond)
				{
					logclock -> second += logclock -> nanosecond/nanosecondsecond;
					logclock -> nanosecond %= nanosecondsecond;
				}	
			}
		}
	}
		else
			ClockClick();
		
		QueueStatus();
		
	} 
	sigHandler(2);
}
	
	
	

