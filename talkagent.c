/*-------------------------------------------------------------------
File: talkagent.c

Name:
Student Number:

Description: This is a communications agent designed to exchange
             with another agent text entered received via the
	     standard input (typically attached to a terminal).
	     Any data received from the remote agent is sent
	     to the standard output.  The process is expected to
	     have the communications channel with the 
	     remote agent (another talkagent process) already setup 
	     by its parent process.  The command line contains the
	     file descriptors to the communications channel - the
	     first giving the read end (infd) of the channel to 
	     receive text from the remote agent, and the second
	     (outfd) the write end of the channel to send text
	     to the remote agent.  Note that the channel between
	     agents can be any communications channel such as
	     pipes or sockets.

	     The program uses two threads, each responsible for
	     a direction in the communications.  One thread reads
	     from the standard input and writes to the comms 
	     channel while the other reads from the comms channel
	     and writes to the standard output.
-------------------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>

#define OK 0
#define ERR1 -1

void *talk(void *);
int createThreads(int []);

/*-------------------------------------------------------------------
Function: main

Description: The main function processes the command line to get the
             comms channel file descriptors which it stores in an
	     integer array.  It then calls the create_thread 
	     function that will create and monitor the program
	     threads.

	     Assignment: You do not need to change this function.
-------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
   int fds[2];  /* fds[0] remote input, fds[1], remote output */
   int retcd;

   if(argc != 3)
   {
      printf("Usage: talkagent <infd> <outfd>\n");
      retcd = 1;
   }
   else if(sscanf(argv[1],"%d",&fds[0]) != 1)
   {
      printf("Invalid <infd> %s\n",argv[1]);
      retcd = 1;
   }
   else if(sscanf(argv[2],"%d",&fds[1]) != 1)
   {
      printf("Invalid <outfd> %s\n",argv[2]);
      retcd = 1;
   }
   else 
   {
      if(createThreads(fds) == OK) retcd = 0;
      else retcd = 1;
   }
   printf("All done! (%s)\n", getpid());
   return(retcd);
}

/*-------------------------------------------------------------------
Function: createThreads

Description: This function is responsible for creating two threads
             using the pthread library API. Both threads execute the
	     talk function.  The difference is in the setup of the
	     file descriptors passed to the thread which define the
	     read and write fds. Once threads are created, the
	     function waits until both threads have terminated
	     before returning.
	     
	     Assignment: You need to complete this function.
-------------------------------------------------------------------*/
int createThreads(int fds[])
{
   int retcd = OK;  /* return code */

   /* lets print a welcome message */
   printf("Welcome to talk agent (%d)\n", getpid());

   return(retcd);
}

/*-------------------------------------------------------------------
Function: talk

Description: This is the function executed by a thread for 
             communications in a single direction.  The paramter
             received points to an integer array that gives the
	     read and write file descriptors.
	     
	     Assignment: You do not need to change this function.
	                 Use as is.
-------------------------------------------------------------------*/
void *talk(void *fdPtr)
{
   int *fds = (int *) fdPtr;  /* cast parameter into an integer pointer */
   int num;                   /* for storing number of characters read */
   char buffer[BUFSIZ];       /* buffer for reading characters and 
                                 building other output messages */
  
   while(1)  /* setup up loop */
   {
     num = read(fds[0],buffer,BUFSIZ);
     if(num == -1) /* Error in the read */
     {
        sprintf(buffer,"Fatal read error on %d (%d)\n",fds[0],getpid());
        write(fds[1],buffer,strlen(buffer));
	perror(buffer);  /* this writes to the standard error */
	break;  /* break out of loop */
     }
     else if(num == 0) /* link severed by the remote */
     {
	sprintf(buffer,"Link severed (%d)\n",getpid());
        write(fds[1],buffer,strlen(buffer));
	break;  /* break out of loop */
     }
     else if(strncmp("exit",buffer,strlen("exit")) == 0) 
     {  /* request from local to terminate */
	break;  /* break out of loop */
     }
     else write(fds[1],buffer,num); /* send the data received */
   }
   /* close channels */
   close(fds[0]);
   close(fds[1]); 
   pthread_exit(0);  /*terminate the thread*/
}
