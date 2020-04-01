//  MSH main file
// Write your msh source code here

#include <parser.h>
#include <stddef.h>		/* NULL */
#include <sys/types.h>/* Header file for system call open */
#include <sys/stat.h> /* Defines the structure of data returned by functions fsat(), lstat() and stat() */
#include <fcntl.h> /* Manipulate file descriptor */
#include <wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* Header file for system calls read, write y close */
#include <wait.h>
#include <signal.h>

/* Error handling libraries */
#include <errno.h>
#include <string.h> /* Used for strerror(errno) and strcmp()*/

#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Saliendo del MSH **** \n");
	//signal(SIGINT, siginthandler);
        exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
    //reset first
    for(int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for ( i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}

void mycalc(int operand1, int operand2, int *Acc, char *operator){
    int result;
    if (strcmp(operator,"mod")==0){
        result=operand1%operand2;
        int aux=operand1/operand2;
        printf("[OK] %d mod %d = %d * %d + %d\n",operand1,operand2,operand2,aux,result);
    } else if(strcmp(operator,"add")==0){
        result= operand1 + operand2;
        *Acc += result;
        printf("[OK] %d + %d = %d; Acc %d\n",operand1,operand2,result,*Acc);
    }else{
        fprintf(stderr, "[ERROR] Error opening the copied file");
		
    }

}

/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
    /**** Do not delete this code.****/
    int end = 0; 
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char*)malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF){
            if(strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush (stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;


	while (1) 
	{
		int status = 0;
	        int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
                //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
                executed_cmd_lines++;
                if( end != 0 && executed_cmd_lines < end) {
                    command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
                }else if( end != 0 && executed_cmd_lines == end)
                    return 0;
                else
                    command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
                //************************************************************************************************


              /************************ STUDENTS CODE ********************************/
	      if (command_counter > 0) {
                if (command_counter > MAX_COMMANDS)
                      printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
                else {
            	   // Print command
		   print_command(argvv, filev, in_background);
                }
              }
        }
	return 0;
}
