/**
 * 
 * @author Aziza Mankenova, 2018400387
 *
 * @brief Part B of the Project 1.
 * PS codes and codes from Part A are utilized. 
 * The purpose of part B is to upgrade the wrapper designed in Part A to be a Remote Procedural Call
 * (RPC) server that executes the blackbox for any client that requests.
 * The client program calls RPC function in the Server, passing three parameters: path, number1, and number2 using an of input struct.
 * The server program is ready for clients to run the RPC function.
 * This function creates a child process that executes the given blackbox and return the response to the Client.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "part_b.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>	
#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_APPEND)
#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)



void
part_b_prog_1(char *host, char *executable, char *output, int num1,int num2)
{
	CLIENT *clnt;
	char * *result_1;
	input  blackbox_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, PART_B_PROG, PART_B_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	

	// assign the numbers and path of the executable to blackbox arguments
	blackbox_1_arg.a = num1;
	blackbox_1_arg.b = num2;
	blackbox_1_arg.exec = executable;
	result_1 = blackbox_1(&blackbox_1_arg, clnt);
	//check if the call was failed
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else{
		int fd;
		//open the file with the name given as 3rd argment in terminal with two flags
    	fd = open(output, CREATE_FLAGS, CREATE_MODE);
    	//check if the file was successfully created
    	if (fd == -1) {
            perror("Failed to open the file");
            return;
        }
        //check if stdout was associated with the file
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Failed to redirect standard output");
            return;
        }
        //check if the file was closed 
        if (close(fd) == -1) {
            perror("Failed to close the file");
            return;
        }

        //Check if the child's output was success or fail. If it is a sucess then it must only consist of integers.
        //If there was an error, some sort of string is captured. 
        char * result_2 = *result_1;
         while(*result_2 != '\0')
	    {
	        if((*result_2 < '0' || *result_2 > '9') && *result_2 != '\n' && *result_2 != ' '){
	            printf("FAIL:\n%s", *result_1);
                exit (0);
             }
	        result_2++;
	    }
	    printf("SUCCESS:\n%s", *result_1);
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
	char *output;
	char *executable;
	// check for correct number of arguments
	if (argc < 4) {
		printf ("usage: %s blackbox output.txt server_host\n", argv[0]);
		exit (1);
	}
	//read path of executable, host and output file name from the arguments
	executable = argv[1];
	host = argv[3];
	output = argv[2];
	int num1, num2;
	//Prompt user to enter 2 integer numbers
    scanf("%d %d", &num1, &num2);
    //call the function part_b_prog_1 with 5 parameters
	part_b_prog_1 (host, executable, output, num1, num2);
	
	exit (0);
}
