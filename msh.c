//  MSH main file
// Write your msh source code here

#include <parser.h>
#include <stddef.h>	
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


int mycp(FILE *source_file, FILE *destination_file)  // [1] original archive, [2] to paste in file (destination)
{
    struct dirent *destination_read;    /*to read the destination path*/
    char count;  /*to read entire path*/

	while( ( count = fgetc(source_file) ) != EOF )
      fputc(count, destination_file);
 
   printf("File copied successfully.\n");
 
   fclose(source_file);
   fclose(destination_file);
	return 0;
}

void mycalc(int operand1, char *operator, int operand2, int *Acc){

    int result;
    if (strcmp(operator, "mod") == 0){
        result = operand1 % operand2;
        int aux = operand1/operand2;
        printf("[OK] %d mod %d = %d * %d + %d\n",operand1,operand2,operand2,aux,result);
    } else if(strcmp(operator,"add")==0){
        result = operand1 + operand2;
        *Acc += result;
        printf("[OK] %d + %d = %d; Acc %d\n", operand1, operand2, result, *Acc);
    }else{
        fprintf(stderr, "[ERROR] Error opening the copied file");
		
    }

}

int count_elements(char **argvv){//[asdf],[asdf]
    int i = 0;
    for (;argvv[i] != NULL; i++){
        printf("%s\n",argvv[i]);
    }
    printf("%i\n", i);
    return i;
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
                }else if( end != 0 && executed_cmd_lines == end){
                    return 0;
                }else{
                    command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
                }
		            

                //************************************************************************************************


              /************************ STUDENTS CODE ********************************/
	            if (command_counter > 0) {
                    if (command_counter > MAX_COMMANDS)
                        printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
                    else {
                        // Print command
                        

                        if(command_counter == 1){//[[mycalc],[3],[+],[4]]

                            if(strcmp(**argvv, "mycalc") == 0){
                                //Call mycalc
                                mycalc(*argvv[1][0], *argvv[2], *argvv[3][0], 0);

                            } else if (strcmp(**argvv, "mycp") == 0){//[[mycp],[hola],[adios], NULL]
                                //Call mycp

                                    struct stat prove_structure; /*to prove the structure of the given arguments*/

                                	/*checks the command is properly written*/
                                    if (count_elements(*argvv) != 3) //RAUL
                                    {
                                        fprintf(stderr,  "as[ERROR] The structure of the command is mycp <original file> <copied file>\n");
                                        continue;
                                    }
                                write(STDOUT_FILENO, "1", strlen("1"));

                                    if(stat(*argvv[1], &prove_structure) == 0)
                                    {
                                        if(prove_structure.st_mode & S_IFDIR)
                                        {
                                            fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
                                            continue;
                                        }
                                    }

                                write(STDOUT_FILENO, "2", strlen("1"));

	
                                    if(stat(*argvv[2], &prove_structure) == 0)
                                    {
                                        if( prove_structure.st_mode & S_IFDIR)
                                        {
                                            fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
                                            continue;
                                        }
                                    }

                                write(STDOUT_FILENO, "3.14", strlen("1123"));

                                    
                                    /*to identify a file that already exists*/
	                                int source_descriptor;

                                    /*opens file and addresses it a descriptor*/
                                    source_descriptor = open(*argvv[1], O_RDONLY);

                                    /*if f_descriptor is -1, the file cannot be opened*/
                                    if (source_descriptor == -1)
                                    { 
                                        fprintf(stderr, "[ERROR] Error opening original file");
                                        continue;
                                    }
                                write(STDOUT_FILENO, "3", strlen("1"));


                                    /*opens the requested file*/
                                    FILE* source_file = fopen(*argvv[1], "r"); //we open in mode reading and check it is correct
                                    if(source_file == NULL){
                                        fprintf(stderr, "[ERROR] Error opening original file");
                                        continue;
                                    }

                                write(STDOUT_FILENO, "4", strlen("1"));

                                    /*creates the new file*/
                                    FILE* destination_file = fopen(*argvv[2], "w"); //we create the new file and check it is correct
                                    if(destination_file == NULL){
                                        fprintf(stderr, "[ERROR] Error opening the copied file");
                                        continue;
                                    }

                                write(STDOUT_FILENO, "5", strlen("1"));


                                    int result = mycp(source_file, destination_file);
                                    if(result == 0){
                                        write(STDOUT_FILENO, "copied", strlen("copied"));
                                    }



                                write(STDOUT_FILENO, "coronavirus", strlen("coronavirus"));

                            } else {
                                //Fork and exec
                                print_command(argvv, filev, in_background);

                                //Create a pipe for the child

                                int pid = fork();
                                if(pid == 0){
                                    //The child
                                    execvp(**argvv, *argvv);

                                } else {
                                    wait(&pid);
                                    write(STDOUT_FILENO, "done\n", strlen("done"));
                                }

                            }

                        }



                }
              }
        }
	return 0;
}
