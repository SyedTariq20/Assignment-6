#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define nanosecondsecond 1000000000

int i,pid,temporary,t,requestclockcount = 0,second = 0;
long nanosecond = 0;


typedef struct Clock 
{
	int	second;
	long int nanosecond;
	int clockcount;
}Clock;


struct message_buffer { 
        long message_type; 
        char message_text[10];
	int address;
	int requestType;
	int pid;
}message; 

static long grain;
Clock *logclock;
int r;


int genrateRandomNumber()
{
	srand(grain++ * getpid());
	r =rand()%100;
	if(r <= 50)
		return 1;			
	else 
		return 2;			


}



int main(int argc,char *argv[])
 {
	 grain = time(NULL);
	 int clockid = atoi(argv[1]);
	 int messageid = atoi(argv[2]);
	 pid = getpid();
 
	
	 logclock = shmat(clockid, NULL, 0);
	 logclock -> clockcount = logclock -> clockcount + 1;
	

	 do{
		 
		 

		
		srand(grain++ * pid);
		temporary = rand()%256000;
		
		t = genrateRandomNumber();
		if(t ==1)
		{
				
				
				message.address = temporary;
				message.requestType = 0;    				
				message.message_type = 1; 
				message.pid = pid;
				msgsnd(messageid, &message, sizeof(message), 0); 
				msgrcv(messageid, &message, sizeof(message), pid, 0);
				requestclockcount++;
		}
		else
		{		
				
			
				message.address = temporary;
				message.requestType = 1;    				
				message.message_type = 1; 
				message.pid = pid;
				msgsnd(messageid, &message, sizeof(message), 0); 
				msgrcv(messageid, &message, sizeof(message), pid, 0);
				requestclockcount++;
				
				
		}

		
		if(requestclockcount > 100)
			break;
		 
	 }while(i);

	 message.requestType = 2;    				
	 message.message_type = 1; 
	 message.pid = pid;
	 msgsnd(messageid, &message, sizeof(message), 0); 
	 shmdt(logclock);




	 return 0;
}



 
