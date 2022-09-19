/**
 * 
 * @author Aziza Mankenova, 2018400387
 *
 * @brief Part B of the Project 1.
 * PS codes and codes from Part A are utilized. 
 * The purpose of part B ıs to upgrade the wrapper designed in Part A to be a Remote Procedural Call
 * (RPC) server that executes the blackbox for any client that requests.
 *
 */

#include "part_b.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


char **
blackbox_1_svc(input *argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */
	int num1 = argp->a;
	int num2 = argp->b;
	char* exec = argp->exec;
    char var;
    result = &var;

	// char *output = argp->output;	
	int     p2c[2], c2p[2];
    pid_t   pid;
    char    w_buf[1000000];
    char	r_buf[1000000];

    /**
     * Two pipes are created, from parent to child and from child to parent.
     * The parent creates the pipes, which will be attached to stdin, stdout, and stderr or the child.
     */
    pipe(p2c);
    pipe(c2p);

    if((pid=fork()) == -1) {
        fprintf(stderr, "fork() failed.\n");
        exit(-1);
    }

    //.......................................................................................................
    if(pid > 0) {
       
        int nbytes;

        /**
         * The parent should close the ends of the pipes that it will not use.
         */
        close(p2c[0]);    // Parent will not read from p2c
        close(c2p[1]);    // Parent will not write to c2p

        // write two numbers as a string for the child to read with scanf
        sprintf(w_buf, "%d %d\n", num1, num2);
        // Send a message to the child process through the forward pipe (p2c).
        write(p2c[1], w_buf, strlen(w_buf));

        memset ( r_buf, 0, sizeof r_buf );

        // Read the child's output through the reverse pipe (c2p) into r_buf
        nbytes = read(c2p[0], r_buf, sizeof(r_buf));
        result = r_buf;
        
        return &result;


    }
    //.......................................................................................................
    else {
        /**
         * This is the code for the child process.
         */

        /**
         * The child associates the read end of forward pipe (p2c) with its own stdin.
         */
        dup2(p2c[0], STDIN_FILENO);

        /**
         * The child associates the write end of reverse pipe (c2p) with its own stdout and stderr.
         */
        dup2(c2p[1], STDOUT_FILENO);
        dup2(c2p[1], STDERR_FILENO);

        /**
         * The child can close all pipes since they are not needed anymore. Nothing will happen to stdin, stdout, stderr.
         */
        close(c2p[0]);
        close(c2p[1]);
        close(p2c[0]);
        close(p2c[1]);

        // Child reads from the console (stdin) using scanf(). In fact, the input comes from the parent 
        // through the forward pipe p2c.
        // Execute a program the path of which was given as a second argument in the terminal
        // And the output that is written to the console(stdout) by the blackbox goes to the parent 
        // through the reverse pipe c2p.
        int a, b;
        execl(exec, exec, NULL);
        scanf("%d %d", &a, &b);
        
    }
    return &result;
}
