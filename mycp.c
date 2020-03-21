
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
#include <string.h> /* Only used for strerror(errno) */


int main(int argc, char *argv[])  // [1] original archive, [2] to paste in file (destination)
{
	int source_descriptor, destination_descriptor;		  /*to identify a file that already exists*/
	DIR *dir;                 /*the destination path*/

	FILE *source_file, *destination_file;
    struct dirent *destination_read;    /*to read the destination path*/
    char count;  /*to read entire path*/

	/*checks the command is properly written*/
	if (argc < 2)
	{
		fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
		return -1;
	}
	
	/*opens file and addresses it a descriptor*/
	source_descriptor = open(argv[1], O_RDONLY);
	/*if f_descriptor is -1, the file cannot be opened*/
	if (source_descriptor == -1)
	{ 
		fprintf(stderr, "[ERROR] Error opening original file");
		return -1;
	}

	/*we check the destination using its descriptor*/
	destination_descriptor = open(argv[2], O_RDONLY);
	/*if f_descriptor is -1, the file cannot be opened*/
	if (destination_descriptor == -1)
	{ 
		fprintf(stderr, "[ERROR] Error opening the copied file");
		return -1;
	}

	/*opens the requested file*/
	source_file = fopen(argv[1], "r"); //we open in mode reading and check it is correct

	/*we create the new file and check it is correct*/
	destination_file= fopen(argv[2], "w");

	while( ( count = fgetc(source_file) ) != EOF )
      fputc(count, destination_file);
 
   printf("File copied successfully.\n");
 
   fclose(source_file);
   fclose(destination_file);
	return 0;
}
