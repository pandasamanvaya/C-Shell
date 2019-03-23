#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define size 1000

void itoa(int pid, char no[])
{
	int i = 0, len = 0, temp;

	while(pid > 0){
		no[len++] = pid%10 + '0';
		pid = pid/10;

	}

	no[len] = '\0';
	while(i < len/2){
		temp = no[i];
		no[i] = no[len-i-1];
		no[len-i-1] = temp;
		i++;
	}
}
void pinfo(char pid[])
{
	char command[size] = "/proc/";
	char no[8], *tok = NULL, info[size];
	char *val[50];
	int fin, i = 0, p;

	if(!strcmp(pid, "0")){
		p = getpid();
		itoa(p, no);
	}
	else
		strcpy(no, pid);

	strcat(command, no);
	strcat(command, "/stat");

	fin = open(command, O_RDONLY);
	if(fin == -1)
		fprintf(stderr, "Unable to open status file\n");
	else{
		read(fin, info, size);
		tok = strtok(info, " ");
		while(tok != NULL){
			val[i] = (char *)malloc(20 * sizeof(char));
			strcpy(val[i++], tok);
			
			tok = strtok(NULL, " ");
		}

		printf("pid  - %s\n", val[0]);
		printf("Process name - %s\n", val[1]);
		printf("Process state - %s\n", val[2]);
		printf("Memory - %s\n", val[23]);
	}
	close(fin);
}