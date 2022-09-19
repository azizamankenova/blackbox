/**
 * 
 * @author Aziza Mankenova
 *
 * @brief Part A of the Project 1.
 * The purpose of part A is to run the blackbox with the given inputs, 
 * get the output from the blackbox and redirect it as requested 
 *
 * In this code, the parent process captures the input and output of the child process.
 * The parent creates two pipes (one for stdin and the other for stdout and stderr of the child process).
 * The child attaches its stdin, stdout, and stderr to these pipes.
 * Everythıng that child writes on stdout and stderr goes to the parent through pipe.
 * Conversely, what it reads comes from the forward pipe rather than stdin.
 *
 * Therefore, this rogram is quite useful when you want to execute an external program and capture its output.
 * PS codes were utilized to wrıte the code. 
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

#define RUN_EXEC

#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_APPEND)

#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv) {
    int     p2c[2], c2p[2];
    pid_t   pid;
    char    w_buf[1000000], r_buf[1000000];
    int fd;

    //open the file with the name given as 3rd argment in terminal with two flags
    fd = open(argv[2], CREATE_FLAGS, CREATE_MODE);
    
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
        /**
         * This is the code for the parent process.
         */
        int nbytes;

        /**
         * The parent closes the ends of the pipes that it will not use.
         */
        close(p2c[0]);    // Parent will not read from p2c
        close(c2p[1]);    // Parent will not write to c2p

        //Prompt user to enter 2 integer numbers
        int num1, num2;
        scanf("%d %d", &num1, &num2);
        // write two numbers as a string for the child to read with scanf
        sprintf(w_buf, "%d %d\n", num1, num2);
        // Send a message to the child process through the forward pipe (p2c).
        write(p2c[1], w_buf, strlen(w_buf));

        
        // Read the child's output through the reverse pipe (c2p) into r_buf
        nbytes = read(c2p[0], r_buf, sizeof(r_buf));
        //check if the file was successfully created
        if (fd == -1) {
            perror("Failed to open the file");
            return 1;
        }
        //check if stdout was associated with the file
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Failed to redirect standard output");
            return 1;
        }
        //check if the file was closed 
        if (close(fd) == -1) {
            perror("Failed to close the file");
            return 1;
        }
        int length = strlen(r_buf);
        //Check if the child's output was success or fail. If it is a sucess then it mush only consist of integers.
        //If there was an error, some sort of string is captured. 
        for (int i=0;i<length; i++)
            if (!isdigit(r_buf[i]) && (r_buf[i] != ' ') && (r_buf[i] != '\n'))
            {   
                //It is written to the file instead of stdout
                printf("FAIL:\n%s", r_buf);
                return 0;
            }
        //It is written to the file instead of stdout
        printf("SUCCESS:\n%s", r_buf);



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
        execl(argv[1], argv[1],  NULL);
        scanf("%d %d", &a, &b);
        

    }

    return(0);
}

