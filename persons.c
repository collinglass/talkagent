/*-------------------------------------------------------
File: persons.c

Name:
Student Number:

Description: This program is designed to test the talk
             agent program using pipes.  It simulates
	     two persons by spawning a child to act
	     as person B, while the parent acts as Person
	     A.  To enable the dialog, two talkagent
	     processes are spawned and connected using
	     two pipes.  An additional 2 pipes are used
	     to connect the Person A process to one 
	     talkagent standard input and the Person B
	     process to the other talk agent standard
	     input.
--------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

char *persAdialog[] = /* Person A messages */
{
   "Hello Person B.",
   "I am doing great - want to meet for lunch?",
   "How about tomorrow?",
   "Ok - let's make it on Friday.",
   "Good luck with your assignment. Bye for now.",
   "exit",
   NULL
};

char *persBdialog[] = /* Person B messages */
{
   "How are you Person A?",
   "Good idea - what day are you free this week?",
   "Sorry - can't make it tomorrow - I have an assignment due the next day.",
   "I am free on Friday - see you then.",
   "exit",
   NULL
};
  
/* function prototypes */
int doPersonB(void);;
void setupTalkAgentB(int, int [], int []);
int setupTalkAgentA(int [], int []);
void doPersonA(int);

/*---------------------------------------------------------------
Function: main

Description:  The main function calls the following four functions:
                 doPersonB: spawns a child to act as Person B.
		 setupTalkAgentB: spawns talkagent process for
		                  servicing Person B
		 setupTalkAgentA: spawns talkagent process for
		                  servicing Person A
		 doPersonA: acts as Person A (no process is 
		            spawned).

Assignment: Complete this function to ensure proper calls to 
            the other functions.
---------------------------------------------------------------*/
int main(int argc, char *argv[])
{
   int persBrfd;  /* read end of pipe */
   int persAwfd;  /* write end of pipe */

   printf("Simulation starting\n");

   persBrfd = doPersonB(); /* setup Person B and its pipe */

   /* Create talkagent for Person B*/
   /* note that stdin connected to persB pipe */
   setupTalkAgentB(/*define arguments*/);
   /* Create talkagent for Person A*/
   /* note that stdin connected to persB pipe */
   persAwfd = setupTalkAgentA(/*define arguments*/);

   /* Lets do person A now */
   doPersonA(/*define the arguments*/);

   fprintf(stderr,"Simulation all done\n");
}

/*---------------------------------------------------------------
Function: doPersonB

Description: This function spawns a child process to act as
             Person B.  It must create a pipe and attach the
	     write end of the pipe to the standard output of the 
             Person B process. It returns the file descriptor of 
             the read end so that it may be attached to the 
	     standard input of a talkagent process.

Assignment: Complete this function.  The code for generating
            messages has been provided.
---------------------------------------------------------------*/
int doPersonB(void)
{
   
   int i;
   pipe(i);

   pid_t pid = fork();

   if (pid == 0)                // child
     {
        // Code only executed by child process

        /* now we can start the conversation */
        sleep(2);  /* wait for first message from Person A */
        for(i=0 ; persBdialog[i] != NULL ; i++)
          {
            printf("%s (%d)\n",persBdialog[i], getpid());
            fflush(stdout);
            sleep(2);  /* wait for response */
          }
        
      }
      else if (pid < 0)            // failed to fork
      {
          cerr << "Failed to fork" << endl;
          exit(1);
          // Throw exception
      }
      else                                   // parent
      {
        // Code only executed by parent process
      }
      // Return file descriptor for talk agent input
    return i;
}

/*---------------------------------------------------------------
Function: setupTalkAgentB

Description: This function spawns a talkagent process for 
             Person B.  Two arguments are provided to the called 
	     talkagent program, the file descriptors for reading 
	     and writing to the talk agent servicing person A.
	     It must also attach read end of a pipe attached
	     to Person B process to its standard input.  

Assignment: Complete this function.
---------------------------------------------------------------*/
void setupTalkAgentB(/*define the parameters*/)
{
    /* Complete the function */
}

/*---------------------------------------------------------------
Function: setupTalkAgentA

Description: This function spawns a talkagent process for 
             Person A.  Two arguments are provided to the called 
	     talkagent program, the file descriptors for reading 
	     and writing to the talk agent servicing person B.
	     It must also create a new pipe and attach the read 
	     end of the new pipe to the standard input of the
	     talk agent process. It shall return the write
	     end of the pipe to be attached to the Person A
	     process.

Assignment: Complete this function.
---------------------------------------------------------------*/
int setupTalkAgentA(/*define the parameters*/)
{
    /* Complete the function */
}

/*---------------------------------------------------------------
Function: doPersonA

Description: This function acts as Person A by sending the
             person A messages to the standard output.  The
	     standard output is attached to the pipe
	     attached to the talkagent standard input servicing
	     Person A. 

Assignment: Complete this function.  The code for generating
            messages has been provided.
---------------------------------------------------------------*/
void doPersonA(/*define the parameters*/)
{
    int i;

    pipe(i);

    /* now we can start the conversation */
    sleep(1);  /* wait before sending first message to Person B */
    for(i=0 ; persAdialog[i] != NULL ; i++)
    {
       printf("%s (%d)\n",persAdialog[i], getpid());
       fflush(stdout);
       sleep(2);  /* wait for response */
    }
    /* conversation done */

    // return the file descriptor to use for talk agent A
    return i;
}
