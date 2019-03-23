#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define max 100000

void redirect_out(int app, char *file)
{
	int fd;

	if(!app)
		fd = open(file, O_CREAT | O_TRUNC | O_WRONLY , 00644);
	else
		fd = open(file, O_CREAT | O_APPEND | O_WRONLY , 00644);

	if(fd == -1){
		fprintf(stderr, "Error in opening file %s\n", file);
		return;
	}

	if(dup2(fd, 1) == -1)
		fprintf(stderr, "Error in dup\n");
	close(fd);

}

void redirect_in(char *file)
{
	int fd;
	fd = open(file, O_RDONLY , 00644);

	if(fd == -1){
		fprintf(stderr, "Error in opening file %s\n", file);
		return;
	}

	if(dup2(fd, 0) == -1)
		fprintf(stderr, "Error in dup\n");
	close(fd);

}
