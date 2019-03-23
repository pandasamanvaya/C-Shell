#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "main.h"

#define MAX 256
extern int dno;
extern char *user[20];

void parse_dir(char directory[], char *delim)
{
	char *tok, path[MAX];
	int dirstat;

	tok = strtok(directory, delim);
	while(tok != NULL){
		strcpy(path, tok);
		if(!strcmp(path,"..")){
			if(dno > 2){
				chdir(path);
				free(user[--dno]);
			}
		}
		else if(!strcmp(path, "."));
		else if(!strcmp(path, "-") || !strcmp(path, "~")){
			while(dno > 2){
				chdir("..");
				free(user[--dno]);
			}
		}

		else{
			dirstat = chdir(path);
			if(dirstat == -1)
				fprintf(stderr, "No such directory\n");
			else{
				user[dno] = (char *)malloc(MAX * sizeof(char));
				strcpy(user[dno++], path);
			}
		}
		tok = strtok(NULL, delim);
	}
}

void cd(char directory[])
{
	int j;

	j = 0;
	while(directory[j] != '\0'){
		if(directory[j++] == '/'){
			parse_dir(directory, "/");
			break;
		}
	}
	if(j == strlen(directory))
		parse_dir(directory, "\n");
}

