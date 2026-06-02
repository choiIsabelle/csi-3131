/* ------------------------------------------------ ------------
File: cpr.c

Last name: Casey, Choi
Student number: 300291133, 3003282805

Description: This program contains the code for creation
 of a child process and attach a pipe to it.
	 The child will send messages through the pipe
	 which will then be sent to standard output.

Explanation of the zombie process
(point 5 of "To be completed" in the assignment):

	(please complete this part);

------------------------------------------------------------- */
#include <stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>

/* Prototype */
void createChildAndRead (int);

char **args;

/* -------------------------------------------------------------
Function: main
Arguments: 
	int ac	- number of command arguments
	char **av - array of pointers to command arguments
Description:
	Extract the number of processes to be created from the
	Command line. If an error occurs, the process ends.
	Call createChildAndRead to create a child, and read
	the child's data.
-------------------------------------------------- ----------- */

int main (int ac, char **av)
{
 int processNumber; 

 if (ac == 2)
 {
 if (sscanf (av [1], "%d", &processNumber)== 1)
 {
	args = av;
    createChildAndRead(processNumber);
 }
    else fprintf(stderr, "Cannot translate argument\n");
 }
    else fprintf(stderr, "Invalid arguments\n");
    return (0);
}


/* ------------------------------------------------ -------------
Function: createChildAndRead
Arguments: 
	int prcNum - the process number
Description:
	Create the child, passing prcNum-1 to it. Use prcNum
	as the identifier of this process. Also, read the
	messages from the reading end of the pipe and sends it to 
	the standard output (df 1). Finish when no data can
	be read from the pipe.
-------------------------------------------------- ----------- */

void createChildAndRead(int prcNum)
{
	/*
	STEP 5 observations:
		What we observed when we ran the program with sleep(10) added was that after a process
		declared it had ended, its child would be labelled "defunct" until the parent's parent process declared 
		its end 10 seconds later, which make the parent process become a zombie, although the original zombie
		process would be removed from the table after its parent became zombified. This is because the child 
		process have become zombies, which are processes that have finished executing but their parents haven't 
		read their exit status. When the parent process of a zombie process finishes executing, the responsibility
		of reading the zombie process's exit status falls to the init process, which will read the exit status upon 
		receiving it and remove the zombie process from the process table. This is why the original zombie process
		disappears when its parent becomes a zombie, because the init process has finally read its exit status.
	
	
	
	
	code plan: 
	-make variables
	-make pipe check pipe works
	-case 1: print 1 started. sleep for five. print 1 ended
	-case !1: print process number started, fork child
	-in child: switch end of pipe, then rerun program with new parameters
	-in parent: loop (read child's message, print child's message) until no more messages, 
		print process number ended, close write pipe
	*/

	int fd[2], PID, p, nbytes;


	char buff[25];

	if (prcNum == 1){
		printf("Process %d begins\n", prcNum);
		fflush(stdout); //flushing stdout ensures that the messages are pushed through the stream as soon as they enter, preserving message order
		sleep(5);
		printf("Process %d ends\n", prcNum);
		fflush(stdout); //printing then flushing, its possible to write to stdoutput instead, and use sprintf() to format the message
	
		sleep(10); //step 5
	}

	else {
		p = pipe(fd);

		if (p == -1) { //pipe failed
			fprintf(stderr, "Pipe failed\n");
			exit(-1);
		}

		printf("Process %d begins\n", prcNum);
		fflush(stdout);

		PID = fork();

		if (PID < 0) {//fork failed
			fprintf(stderr, "Fork failed\n");
			exit(-1);	
		}

		else if (PID == 0){ //child
			close(fd[0]);
			dup2(fd[1], 1); //change stdoutput to write end of pipe with parent
			close(fd[1]);
			 
			char output[8];
			sprintf(output, "%d", prcNum - 1);
			execlp(args[0], args[0], output, NULL); //replace process image and rerun
		}

		else {//parent
			close(fd[1]); //close write end of pipe to child

			while((nbytes = read(fd[0], buff, sizeof(buff))) > 0 ){ 
				//uses sizeof() rather than strlen(), because sizeof() returns the memory space and strlen returns 0 if the string isn't initialized
				//which at this point it isn't
				write(1, buff, nbytes); //writes to the stdoutput rather than printing to preserve the size of messages (not possible with printf)
				fflush(stdout);
			}
			
			close(fd[0]);
			printf("Process %d ends with a PID of %d\n", prcNum, PID);
			fflush(stdout);

			sleep(10); //step 5			
		}
	}
}



