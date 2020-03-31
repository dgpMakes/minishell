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