#include <parser.h>
#include <stddef.h>	
#include <sys/types.h>/* Header file for system call open */
#include <sys/stat.h> /* Defines the structure of data returned by functions fsat(), lstat() and stat() */
#include <fcntl.h>
#include <wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h>

/* Error handling libraries */
#include <errno.h>
#include <string.h>

#define MAX_COMMANDS 8

//Singal handlers
void sigchld_handler(int param);
void sigint_handler(int param);

//Built-in shell functions
int mycp(char *source_string, char *destination_string);
void mycalc(int operand1, char *operator, int operand2);

//Functions to organise code
int io_redirect(char filev[3][64]);
int create_simple_process(char ***argvv, char filev[3][64], int in_background);
int create_pipes(char ***argvv, char filev[3][64], int in_background, int command_counter);
void recursive_pipe(char ***argvv, int currentPipe, int totalPipes,  int nextPipe);

//Helper function
int count_command_arguments(char **argvv);


// Redirection files
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];


/*
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
    
    setenv("Acc", "0", 1);
    int num_commands;
    char ***argvv = NULL; 

    
    //Main while
	while (1) 
	{
		int status = 0;
	    int command_counter = 0;
        int in_background = 0;

        //Set signal handler functions
		signal(SIGINT, sigint_handler);
        signal(SIGCHLD, sigchld_handler);

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
            printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
            continue;
        }
       
        // Mycalc function
        if(strcmp(**argvv, "mycalc") == 0 && command_counter == 1) {
            mycalc(atoi(argvv[0][1]), argvv[0][2], atoi(argvv[0][3]));
            continue;
        }

        //Mycp function
        if (strcmp(*argvv[0], "mycp") == 0 && command_counter == 1) {//[[mycp],[hola],[adios], NULL]]

            /*checks the command is properly written*/
            if (count_command_arguments(*argvv) != 3) {
                fprintf(stdout, "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
                continue;
            }

            mycp(argvv[0][1], argvv[0][2]);
            continue;
        }

        //Simple process
        if(command_counter == 1){
            create_simple_process(argvv, filev, in_background);
            continue;
        }

        //Several processess with pipes
        if(command_counter > 1){
            create_pipes(argvv, filev, in_background, command_counter);
        }
        
    }

	return 0;

}


//Functions

int create_pipes(char ***argvv, char filev[3][64], int in_background, int command_counter){

    //Create a copy of the file descriptors to later raet
    int stdi = dup(0);
    int stdo = dup(1);
    int stde = dup(2);

    //File redirection in pipes
    io_redirect(filev);
 
    //Start the recursive pipe function
    recursive_pipe(argvv, 0, command_counter, 0);

    //If not in background, wait for every process in the pipe to finish
    if(in_background == 0){
        while(wait(NULL) > 0);
    }

    //Restore state to attach to tty before creating pipes
    close(0);
    dup(stdi);
    close(1);
    dup(stdo);
    close(2);
    dup(stde);
    close(stdi);
    close(stdo);
    close(stde);

    return 1;  
}

void recursive_pipe(char ***argvv, int currentPipe, int totalPipes, int nextPipe){

    //The current call creates a pipe for the next one
    int pip[2];
    pipe(pip);

    //Base case in recursion
    if(currentPipe == totalPipes){
        close(nextPipe);
        close(pip[0]);
        close(pip[1]);
        return;
    }

    int pid = fork();
    if(pid == 0){//Child
            
        //The first process
        if(currentPipe == 0){

            close(STDOUT_FILENO);
            dup(pip[1]);
            close(pip[1]);
            close(pip[0]);
            
        }
        //The last process
        else if(currentPipe == totalPipes - 1){
            close(STDIN_FILENO);
            dup(nextPipe);

            //No need to create more pipes
            close(pip[0]);
            close(pip[1]);

        }
        //The general case
        else {

            //Stablish output using the created pipe
            close(STDOUT_FILENO);
            dup(pip[1]);
            close(pip[1]);
            close(pip[0]);

            //Stablish the input using the passed pipe
            close(STDIN_FILENO);
            dup(nextPipe);
            close(nextPipe);

        }
        
        //Launch the process
        int result = execvp(argvv[currentPipe][0], argvv[currentPipe]);
        if(result == -1){
            fprintf(stderr, "[Error executing the pipe]: %s\n", strerror(errno));
        }

    } else {
        //Parent does no longer need the pipe
        //Discard the first interation
        if (currentPipe != 0)
            close(nextPipe);

        close(pip[1]);
        recursive_pipe(argvv, currentPipe + 1, totalPipes, pip[0]);
    }
}


int create_simple_process(char ***argvv, char filev[3][64], int in_background) {

    //Create a pipe for the child
    int pid = fork();
    if(pid == 0) {

        //The child
        //File redirection
        io_redirect(filev);
        //Replace child image
        if(execvp(**argvv, *argvv) == -1)
            perror("Error executing fork");

    }
    else if(pid == -1) {
        
        //Error case
        perror("Error creating fork");

    } 
    else {

        //The parent
        if(in_background) printf("[%i]\n",pid);
        if(!in_background) wait(&pid);

    }


}


int io_redirect(char filev[3][64]) {

    /*For stdin*/
    if(strcmp(filev[0], "0") != 0) {

        //Open the file
        int fd = open(filev[0], O_RDONLY, 0600);
        if(fd == -1) printf("error: %s\n",strerror(errno));
        
        /*Safely attach the file to stdin*/
        close(STDIN_FILENO);
        if(dup(fd) == -1) perror("Error duplicating");
        close(fd);
    }

    /*For stdout*/
    if(strcmp(filev[1], "0") != 0) {

        //Open file to write to it
        int fd = open(filev[1], O_RDWR|O_TRUNC|O_CREAT, 0600);
        if(fd == -1) printf("Error: %s\n",strerror(errno));

        /*Safely attach the stdout to the file to stdin*/
        close(STDOUT_FILENO);
        if(dup(fd) == -1) perror("Error duplicating");
        close(fd);

    }
    
    /*For stderr*/
    if(strcmp(filev[2], "0") != 0) {
        int fd = open(filev[2], O_RDWR|O_TRUNC|O_CREAT, 0600);
        if(fd == -1) printf("error: %s\n",strerror(errno));
        
        /*Safely attach the stdout to the file to stdin*/
        close(STDERR_FILENO);
        if(dup(fd) == -1) perror("Error duplicating");
        close(fd);

    }

}





//Self implemented functions

void mycalc(int operand1, char *operator, int operand2) {

    int value = atoi(getenv("Acc"));
    int result;
    char acc_value[100];

    if (strcmp(operator, "mod") == 0) {

        result = operand1 % operand2;
        int aux = operand1 / operand2;
        fprintf(stderr, "[OK] %d %% %d = %d * %d + %d\n", operand1, operand2, operand2, aux, result);

    } else if (strcmp(operator, "add") == 0) {

        //Increase environment variable
        result = operand1 + operand2;
        value += result;
        sprintf(acc_value, "%d", value);
        setenv("Acc", acc_value, 1);

        //Print the result
        fprintf(stderr, "[OK] %d + %d = %d; Acc %d\n", operand1, operand2, result, value);
        
    } else {
        printf("[ERROR] The structure of the command is  <operand 1> <add/mod> <operand 2>\n");
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
            fprintf(stdout, "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
            return -1;
        }
    }

   if(stat(destination_string, &prove_structure) == 0)
   {
        if(prove_structure.st_mode & S_IFDIR)
        {
            fprintf(stdout, "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
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
        fprintf(stdout, "[ERROR] Error opening original file: %s\n", strerror(errno));
        return -1;
    }

    /*creates the new file*/
    FILE* destination_file = fopen(destination_string, "w"); //we create the new file and check it is correct
    if(destination_file == NULL){
        fprintf(stdout, "[ERROR] Error opening copied file: %s\n", strerror(errno));
        return -1;
    }

	while( ( count = fgetc(source_file) ) != EOF )
    fputc(count, destination_file);
 
    printf("[OK] Copy has been successful between %s and %s\n", source_string, destination_string);
    fclose(source_file);
    fclose(destination_file);
    close(source_descriptor);
	return 0;
}


//Helper function
int count_command_arguments(char **argvv){//[asdf],[asdf]
    int i = 0;
    for (;argvv[i] != NULL; i++){
    }
    return i;
}


//Signal handler functions
void sigint_handler(int param) {

	printf("****  Exiting MSH ****\n");
    exit(0);
    
}

void sigchld_handler(int param) {
    /*Wait for any child that has finished 
    and prevent them from turning into zombies*/
    wait(NULL);
}


//Unused function
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
