#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX 256

int pipe_occ(char **argv, int argc)
{
	int i = 0;
	while (i < argc){
		if(!strcmp(argv[i], "|"))
			return i;
		else i++;

	}
	return i;
}

void set_arg(int j, int pos, char **a, char **argv)
{
	int i;

	for(i = j; i < pos; i++){
		a[i] = (char *)malloc(sizeof(char) * MAX);
		strcpy(a[i], argv[i]);
	}
	a[pos] = (char *)malloc(sizeof(char) * MAX);
	a[pos] = NULL;

}
void pipe_func(int argc, char **argv)
{
	int pipfd[2], pid, in, out, pos;
	char *child[argc], *parent[argc];

	if(pipe(pipfd) != 0){
		fprintf(stderr, "Unable to pipe\n");
		return;
	}
	in = dup(0);
	out = dup(1);

	pos = pipe_occ(argv, argc);

	set_arg(0, pos, parent, argv);
	set_arg(pos + 1, argc, child, argv);

	pid = fork();
	if(!pid){
		dup2(pipfd[0], in);
		close(pipfd[1]);
		execvp(child[0], child);
	}
	else{
		dup2(pipfd[1], out);
		close(pipfd[0]);
		execvp(parent[0], parent);
	}
	
	dup2(out, 1);	
	dup2(in, 0);
}

