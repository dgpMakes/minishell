
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
	int source_descriptor;		  /*to identify a file that already exists*/
	FILE *source_file, *destination_file;
    struct dirent *destination_read;    /*to read the destination path*/
    char count;  /*to read entire path*/
	struct stat prove_structure; /*to prove the structure of the given arguments*/

	/*checks the command is properly written*/
	if (argc != 3)
	{
		fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
		return -1;
	}
	
	if(stat(argv[1],&prove_structure) == 0)
	{
    	if(prove_structure.st_mode & S_IFDIR)
    	{
        	fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
			return -1;
   		}
	}
	if( stat(argv[2],&prove_structure) == 0)
	{
    	if( prove_structure.st_mode & S_IFDIR)
    	{
        	fprintf(stderr,  "[ERROR] The structure of the command is mycp <original file> <copied file>\n");
			return -1;
   		}
	}

	/*opens file and addresses it a descriptor*/
	source_descriptor = open(argv[1], O_RDONLY);

	/*if f_descriptor is -1, the file cannot be opened*/
	if (source_descriptor == -1)
	{ 
		fprintf(stderr, "[ERROR] Error opening original file");
		return -1;
	}

	/*opens the requested file*/
	source_file = fopen(argv[1], "r"); //we open in mode reading and check it is correct
	if(source_file == NULL){
		fprintf(stderr, "[ERROR] Error opening original file");
		return -1;
	}

	/*creates the new file*/
	destination_file= fopen(argv[2], "w"); //we create the new file and check it is correct
	if(destination_file == NULL){
		fprintf(stderr, "[ERROR] Error opening the copied file");
		return -1;
	}

	while( ( count = fgetc(source_file) ) != EOF )
      fputc(count, destination_file);
 
   printf("[OK] Copy has been successful between <original> and <copied>\n");
 
   fclose(source_file);
   fclose(destination_file);
	return 0;
}
