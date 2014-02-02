/*-------------------------------------------------------
 File: persons.c
 CSI 3131 WINTER 2014
 Assignment 1
 
 Name: Collin Glass
 Student Number: 6006546
 
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
    int persBrfd;  // read end of pipe
    int persAwfd;  // write end of pipe
    
    printf("Simulation starting\n");
    
    persBrfd = doPersonB(); // setup Person B and its pipe
    
    // Here we will create the 2 pipes that will be used by the talk agents.
    
    int pipefd1[2]; // File descriptors
    int pipefd2[2];
    
    if (pipe(pipefd1) == -1) { // Create pipe and handle error
        printf(stderr, "Talk Agent: Failed to create pipe 1.");
        exit(-1);
    }
    
    if (pipe(pipefd2) == -1) { // Create pipe and handle error
        printf(stderr, "Talk Agent: Failed to create pipe 2.");
        exit(-1);
    }
    
    // Create talkagent for Person B
    setupTalkAgentB(persBrfd, pipefd1, pipefd2);
    
    // Create talkagent for Person A
    persAwfd = setupTalkAgentA(pipefd1, pipefd2);
    
    // Lets do person A now
    doPersonA(persAwfd);
    
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
    // Create pipe between Person B and Talk Agent B   
    int pipefd[2]; // File descriptor for the pipe.
    
    if (pipe(pipefd) == -1) { // Create pipe and error handling
        printf(stderr, "Person B: Failed to create pipe.");
        exit(-1);
    }
    
    // Fork process for Person B
    int pid;
    pid = fork();
    
    if (pid < 0) {
    // Error Handling
        printf(stderr, "Person B: Failed to fork.");
        exit(-1);
        
    } else if (pid == 0) { // CHILD
        close(pipefd[0]); // Close read end of pipe
        dup2(pipefd[1], 1); // Duplicate pipe to given end and
        close(pipefd[1]); // Close original
       
        
        int i;
        
        /* now we can start the conversation */
        sleep(2);  /* wait for first message from Person A */
        for(i=0 ; persBdialog[i] != NULL ; i++)
        {
            printf("%s (%d)\n",persBdialog[i], getpid());
            fflush(stdout);
            sleep(2);  /* wait for response */
        }
        exit(0);
        
    } else { // PARENT
        close(pipefd[1]); // Close write end of pipe
        return(pipefd[0]); // Return the read end of the pipe
    }
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
void setupTalkAgentB(int persBrfd, int pipefd1[], int pipefd2[])
{
    // Fork process for Talk Agent B
    int pid;
    pid = fork();
    
    if (pid < 0) {
    // Error Handling
        printf(stderr, "Talk Agent B: Failed to fork.");
        exit(-1);
        
    } else if (pid == 0) { // CHILD
        
        dup2(persBrfd, 0); // Duplicate the pipe to given end and
        close(persBrfd); // close original 
        
        char pipeFeed1[2]; // char array to represent pipefd1
        char pipeFeed2[2]; // char array to represent pipefd2
        sprintf(pipeFeed1, "%d", pipefd1[1]);
        sprintf(pipeFeed2, "%d", pipefd2[0]);
        
        close(pipefd1[0]); // Close read end of pipe 1 
        close(pipefd2[1]); // Close write end of pipe 2
        

        // Now we can execute talkagent
        
        execlp("./talkagent", "./talkagent", pipeFeed2, pipeFeed1, NULL);
        exit(-1); // In case the execution fails.
    } else { // PARENT 
        
        close(persBrfd); // We don't need persBrfd anymore. 
        
        close(pipefd2[0]); // Close read end of pipefd2.
        close(pipefd1[1]); // Close write end of pipefd1.
        
    }
        
    
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
int setupTalkAgentA(int pipefd1[], int pipefd2[])
{
    
    /* First I will create a pipe, to be used between Person A and Talk Agent A. */
    
    int pipefd[2]; // File descriptor for the pipe.
    
    if (pipe(pipefd) == -1) { // This creates the pipe, and checks for error.
        printf(stderr, "Talk Agent A: Failed to create pipe.");
        exit(-1);
    }
    
    int pid;
    pid = fork();
    
    if (pid < 0) { // Check for error
        printf(stderr, "Talk Agent A: Failed to fork.");
        exit(-1);
        
    } else if (pid == 0) { // CHILD
        
        dup2(pipefd[0], 0); // Change the input source to pipefd 
        close(pipefd[0]); // Close duplicate 
        close(pipefd[1]); // Close write end of the pipe created 
        
        char pipeFeed1[2]; // char array to represent pipefd1
        char pipeFeed2[2]; // char array to represent pipefd2
        sprintf(pipeFeed1, "%d", pipefd1[0]);
        sprintf(pipeFeed2, "%d", pipefd2[1]);
        
        // Close unused ends
        close(pipefd1[1]); // Close write end of pipe 1
        close(pipefd2[0]); // Close read end of pipe 2

        // Now we can execute talkagent
        
        execlp("./talkagent", "./talkagent", pipeFeed1, pipeFeed2, NULL);
        exit(-1); // In case the execution fails.
    
    } else { // PARENT
        
        close(pipefd[0]); // Close read end of pipefd
        close(pipefd1[0]); // Close read end of pipefd1
        close(pipefd2[1]); // Close write end of pipefd2
        
        return(pipefd[1]); // Return the file descriptor of write end
        
    }
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
void doPersonA(int persAwfd)
{
    dup2(persAwfd, 1); // Duplicate pipe with end at specified location
    close(persAwfd); // Close original end.
    
    int i;
    
    /* now we can start the conversation */
    sleep(1);  /* wait before sending first message to Person B */
    for(i=0 ; persAdialog[i] != NULL ; i++)
    {
        printf("%s (%d)\n",persAdialog[i], getpid());
        fflush(stdout);
        sleep(2);  /* wait for response */
    }
    /* conversation done */
}
