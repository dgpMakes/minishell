#include <stdio.h>	 /* Header file for printf */
#include <sys/types.h> /* Header file for system call open */
#include <sys/stat.h>  /* Defines the structure of data returned by functions fsat(), lstat() and stat() */
#include <fcntl.h>	 /* Manipulate file descriptor */
#include <unistd.h>	/* Header file for system calls read, write y close */
#include <sys/types.h> /* Header file for system calls opendir, readdir and closedir */
#include <dirent.h>
#include <limits.h> /* For PATH_MAX in MacOS */
#include <stdlib.h>


/* Error handling libraries */
#include <errno.h>
#include <string.h> /* Used for strerror(errno) and strcmp()*/

//extern char** environ; //Declaring the environment variable
int main(int argc, char *argv[]) //[1] counts the number of strings introduce [2] store them in an array 
{
int operand1, operand2, result1, result2, resultop1, resultop2;
int Acc=0;
char environmentVariable [3]="Acc";
char value[100];

char operator1 [3]="add";
char operator2 [3]="mod";

 /*Checks the comand is properly written */
   if(argc != 3)
   {
       fprintf(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n");
        return -1;
   }

/*Check if the operands are integers*/
if(sizeof(argv[1])==2 && sizeof(argv[3])==2)
{
    operand1 =(int) argv[1];
    operand2 =(int) argv[3];

}else{
    fprintf(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n");
    return -1;
}

/*Checks if the operator is correct*/
result1 = strcmp(argv[1], operator1);
result2 = strcmp(argv[1], operator2);

if (result1!=0 || result2!=0)
{
   fprintf(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n"); 
   return -1;
}else{
    if (result1 == 0){
        Acc= operand1 + operand2;
        sprintf(value,"%d",Acc);
        setenv(environmentVariable, value ,1);
        

    }else{
        resultop1= operand1 % operand2;
        resultop2= operand1 / operand2;
        printf("%d % %d = %d * %d + %d ",operand1, operand2,operand2,result2,result1);

    }
}
return 0;

}