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
#include <signal.h>

/* Error handling libraries */
#include <errno.h>
#include <string.h> /* Used for strerror(errno) and strcmp()*/

#define MAX_COMMANDS 8

//Singal handlers
void sigchild_handler(int param);
void siginthandler(int param);

//Built-in shell functions
int mycp(char *source_string, char *destination_string);
void mycalc(int operand1, char *operator, int operand2);

//Self made functions
int ioRedirect(char filev[3][64]);
int createSimpleProcess(char ***argvv, char filev[3][64], int in_background);
int nicePipe(char ***argvv, char filev[3][64], int in_background);

//Helper function
int count_elements(char **argvv);

// Redirection files
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];


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
    const char storeAcc [100];
    setenv(const char *Acc, storeAcc,1);

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

    int num_commands;
    char ***argvv = NULL; 

    //Main while
	while (1) 
	{
		int status = 0;
	    int command_counter = 0;
        int in_background = 0;


		signal(SIGINT, siginthandler);
        signal(SIGCHLD, sigchild_handler);


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
        if(command_counter <= 0){
            perror("Error");
            continue;
        } 
        if (command_counter > MAX_COMMANDS){
            printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
            continue;
        }
       
        // Mycalc function
        if(strcmp(**argvv, "mycalc") == 0 && command_counter == 1) {
            mycalc(atoi(argvv[0][1]), argvv[0][2], atoi(argvv[0][3]), 0);
            continue;
        }

        //Mycp function
        if (strcmp(*argvv[0], "mycp") == 0 && command_counter == 1) {//[[mycp],[hola],[adios], NULL]]

            /*checks the command is properly written*/
            if (count_elements(*argvv) != 3) {
                fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
                continue;
            }

            mycp(argvv[0][1], argvv[0][2]);
            continue;
        }

        //Simple process
        if(command_counter == 1){
            createSimpleProcess(argvv, filev, in_background);
            continue;
        }

        //Several processess with pipes
        if(command_counter > 1){
            nicePipe(argvv, filev, in_background);
        }
        
    }

	return 0;

}





int nicePipe(char ***argvv, char filev[3][64], int in_background){
    return 1;
}


int createSimpleProcess(char ***argvv, char filev[3][64], int in_background){

    //Create a pipe for the child
    int pid;
    switch(pid = fork()){
        case 0:
            //The child
            ioRedirect(filev);
            if(execvp(**argvv, *argvv) == -1)
            {
                perror("Error executing fork");
            }
            break;
        case -1:
            //Error
            perror("Error creating fork");
            break;
        default:
            //Parent
            if(in_background) printf("[%i]\n",pid);
            if(!in_background) wait(&pid);
    }

}


int ioRedirect(char filev[3][64]){

    /*For stdin*/
    if(strcmp(filev[0], "0") != 0){
        int fd = open(filev[0], O_RDONLY, 0600);
        if(fd==-1){
            printf("error: %s\n",strerror(errno));
        }
        close(STDIN_FILENO);
        /*Safely open*/

        
        int new_fd = dup(fd);
        if(new_fd == -1){
            perror("Error duplicating");
        }
        close(fd);
    }

    /*For stdout*/
    if(strcmp(filev[1], "0") != 0){
        int fd = open(filev[1], O_RDWR|O_TRUNC|O_CREAT, 0600);
        if(fd==-1){
            printf("error: %s\n",strerror(errno));
        }
        close(STDOUT_FILENO);

        /*Safely open*/
        int new_fd = dup(fd);
        if(new_fd == -1){
            perror("Error duplicating");
        }

        close(fd);

    }
    
    /*For stderr*/
    if(strcmp(filev[2], "0") != 0){
        int fd = open(filev[2], O_RDWR|O_TRUNC|O_CREAT, 0600);
        if(fd==-1){
            printf("error: %s\n",strerror(errno));
        }
        close(STDERR_FILENO);
        /*Safely open*/
        int new_fd = dup(fd);
        if(new_fd == -1){
            perror("Error duplicating");
        }
        close(fd);

    }

}





//Self implemented functions

void mycalc(int operand1, char *operator, int operand2){
    int value=atoi(getenv(Acc));
    int result;
    if (strcmp(operator, "mod") == 0){
        result = operand1 % operand2;
        int aux = operand1/operand2;
        printf("[OK] %d mod %d = %d * %d + %d\n",operand1,operand2,operand2,aux,result);
    } else if(strcmp(operator,"add")==0){
        result = operand1 + operand2;
        value += result;
        sprintf(Acc,"[OK] %d + %d = %d; Acc %d\n", operand1, operand2, result, value);


    }else{
        fprintf(stderr, "[ERROR] Error opening the copied file");

    }

}


int mycp(char *source_string, char *destination_string)  // [1] original archive, [2] to paste in file (destination)
{
    char count;  /*to read entire path*/
    struct stat prove_structure; /*to prove the structure of the given arguments*/
	int source_descriptor;/*to identify a file that already exists*/

    if(stat(source_string, &prove_structure) == 0)
    {
        if(prove_structure.st_mode & S_IFDIR)
        {
            fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
            return -1;
        }
    }

   if(stat(destination_string, &prove_structure) == 0)
   {
        if(prove_structure.st_mode & S_IFDIR)
        {
            fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
            return -1;
        }
    }

    source_descriptor = open(source_string, O_RDONLY); /*opens file and addresses it a descriptor*/
                
    /*if f_descriptor is -1, the file cannot be opened*/
    if (source_descriptor == -1)
    { 
        fprintf(stderr, "[ERROR] Error opening original file: %s\n", strerror(errno));
        return -1;
    }

    /*opens the requested file*/
    FILE* source_file = fopen(source_string, "r"); //we open in mode reading and check it is correct
    if(source_file == NULL){
        fprintf(stderr, "[ERROR] Error opening original file: %s\n", strerror(errno));
        return -1;
    }

    /*creates the new file*/
    FILE* destination_file = fopen(destination_string, "w"); //we create the new file and check it is correct
    if(destination_file == NULL){
        fprintf(stderr, "[ERROR] Error opening copied file: %s\n", strerror(errno));
        return -1;
    }

	while( ( count = fgetc(source_file) ) != EOF )
    fputc(count, destination_file);
 
    printf("[OK] Copy has been successful between %s and %s.\n", source_string, destination_string);
 
    fclose(source_file);
    fclose(destination_file);
	return 0;
}


//Helper function
int count_elements(char **argvv){//[asdf],[asdf]
    int i = 0;
    for (;argvv[i] != NULL; i++){
    }
    return i;
}


//Signal handler functions
void siginthandler(int param)
{
	printf("****  Saliendo del MSH **** \n");
	//signal(SIGINT, siginthandler);
    exit(0);
}

void sigchild_handler(int param)
{
    //Wait for any child
    int pid = wait(NULL);
    //printf("Process in background with pid %i just finished \n", pid);
}


//Shitty Unused function

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