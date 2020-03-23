#include <stdio.h>	 /* Header file for printf */
#include <sys/types.h> /* Header file for system call open */
#include <sys/stat.h>  /* Defines the structure of data returned by functions fsat(), lstat() and stat() */
#include <fcntl.h>	 /* Manipulate file descriptor */
#include <unistd.h>	/* Header file for system calls read, write y close */
#include <sys/types.h> /* Header file for system calls opendir, readdir and closedir */
#include <dirent.h>
#include <limits.h> /* For PATH_MAX in MacOS */

/* Error handling libraries */
#include <errno.h>
#include <string.h> /* Used for strerror(errno) and strcmp()*/

extern char** environ; //Declaring the environment variable
int main(int argc, char *argv[]) //[1] counts the number of strings introduce [2] store them in an array 
{
int operand1, operand2, result1, result2, resultop1, resultop2;

char** Acc;
Acc=environ;
char operator1 []="add";
char operator2 []="mod";

 /*Checks the comand is properly written */
   if(argc != 3)
   {
       fprintf(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n");
        return -1;
   }

/*Check if the operands are integers*/
if(sizeof(argv[1]==2) && sizeof(argv[3]==2))
{
    operand1 = argv[0];
    operand2 = argv[2];

}else{
    fprint(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n");
    return -1;
}

/*Checks if the operator is correct*/
result1 = strcmp(argv[2], operator1);
result2 = strcmp(argv[2], operator2);

if (result1!=0 || result2!=0)
{
   fprint(stderr, "[ERROR] The structure of the command is <operand1><add/mod><operand2>\n"); 
   return -1;
}else{
    if (result1 == 0){
        Acc= operand1 + operand2;
        printf("Acc =%d",Acc);

    }else{
        resultop1= operand1 % operand2;
        resultop2= operand1 / operand2;
        printf("%d % %d = %d * %d + %d ",operand1, operand2,operand2,result2,result1);

    }
}
return 0;

}