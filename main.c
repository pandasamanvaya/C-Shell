#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <pwd.h>
#include "cd.h"
#include "pwd.h"
#include "echo.h"
#include "ls.h"
#include "syscmd.h"
#include "pinfo.h"
#include "redirect.h"
#include "setenv.h"

#define  MAX 256

int dno = 2;
char *user[20];

struct utsname usr[MAX];
int argc, out, in;
bool bg = false, in_red = false, out_red = false, piping = false, app = false;
size_t max = MAX;

ssize_t len;
void (*command[])(char *) = {cd, lsp, pinfo, redirect_in};
void (*command_p[])(char **) = {pwd, syscmd_bg, unsetvar, kjob, fg, bgr};
void (*command_l[])(int, char **) = {ls, syscmd_fg, echo, setvar};
void (*command_j[])() = {jobs, overkill, process_stop};
void (*command_r[])(int, char *) = {redirect_out};
char *input = NULL, id[10];
char **argv;

void print_user()
{
	int j = 2;
	printf("<%s@%s:~", user[0], user[1]);
	while(j < dno)
		printf("/%s", user[j++]);
	printf("> ");

}

/*For parsing shell command*/
int parse_input(char input[], char *argv[MAX])
{
	char *tok;
	int i = 0;

	tok = strtok(input, " \t");
	while(tok != NULL){
		argv[i++] = tok;
		tok = strtok(NULL, " \t");
	}
	return i;
}

void check(char **argv, int argc)
{
	int i;
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "&"))
			bg = true;
		if(!strcmp(argv[i], ">"))
			out_red = true;
		if(!strcmp(argv[i], ">>")){
			out_red = true;
			app = true;
		}

		else if(!strcmp(argv[i], "<"))
			in_red = true;
		else if (!strcmp(argv[i], "|"))
			piping = true;
	}
}

void stop_handler()
{	printf("Stopped Process\n");
	(*command_j[2])();
}

void exit_handler(int sig)
{	printf("\n");
}


void red_out(char *file)
{
	out = dup(1);
	(*command_r[0])(app, file);

}

void red_in(char *file)
{
	in = dup(0);
	(*command[3])(file);

}

void exec_in(int start)
{
	int i, pos;

	i = start;
	while(i < argc){
		if(!strcmp(argv[i], "<")){
			pos = i;
			break;
		}
		else i++;
	}

	red_in(argv[pos + 1]);
	if(out_red){
		i = pos;
		while(i < argc){
			if(!strcmp(argv[i], ">"))
				break;
			else i++;
		}
		red_out(argv[i + 1]);
	}
	(*command_l[1])(pos, argv);

}

void exec_out(int start)
{
	int i;

	i = start;
	while(i < argc){
		if(!strcmp(argv[i], ">"))
			break;
		else i++;
	}
	red_out(argv[i+1]);
	(*command_l[1])(i, argv);

}

void shell_cmd(int argc, char **argv){

	if(argc < 1)
		return;
	else if(!(strcmp(argv[0], "quit"))){
		printf("Exiting\n");
		exit(0);
	}
	else if(!strcmp(argv[0], "jobs")){
		if(argc > 1)
			fprintf(stderr, "Usage: jobs\n");
		else
			(*command_j[0])();
	}
	else if(!strcmp(argv[0], "kjob")){
		if(argc != 3)
			fprintf(stderr,"Usage: kjob [job_no] [signal]\n");
		else
			(*command_p[3])(argv);
	}
	else if(!strcmp(argv[0], "fg")){
		if(argc != 2)
			fprintf(stderr,"Usage: fg [job_no]\n");
		else
			(*command_p[4])(argv);
	}
	else if(!strcmp(argv[0], "bg")){
		if(argc != 2)
			fprintf(stderr,"Usage: bg [job_no]\n");
		else
			(*command_p[5])(argv);
	}
	else if(!strcmp(argv[0], "overkill")){
		if(argc > 1)
			printf("Usage: overkill\n");
		else
			(*command_j[1])();
	}

	else if(!(strcmp(argv[0], "cd"))){

		if(argc > 1){
			if(in_red || out_red)
				(*command[0])("~");
			else
				(*command[0])(argv[1]);
		}
		else
			(*command[0])("\0");
	}

	else if(!(strcmp(argv[0], "pwd"))){
		if(out_red){
			red_out(argv[argc-1]);
			(*command_p[0])(user);
			dup2(out, 1);
		}
		else
			(*command_p[0])(user);
	}

	else if(!(strcmp(argv[0], "echo"))){
		if(out_red){
			red_out(argv[argc-1]);
			argc = argc-2;
		}
		else if(in_red){
			red_in(argv[argc-1]);
			argc = argc-2;
		}
		(*command_l[2])(argc, argv);
		printf("\n");
		if(out_red)
			dup2(out, 1);
		else if(in_red)
			dup2(in, 0);

	}
	else if(!(strcmp(argv[0], "ls"))){
		if(out_red){
			red_out(argv[argc-1]);
			argc = argc-2;
		}
		if(in_red){
			red_in(argv[argc-1]);
			argc = argc-2;
		}
		if(argc > 1){

			if(dno <= 2 && argv[1][1] == '.');
			else{
				(*command_l[0])(argc, argv);
			}
		}
		else
			(*command[1])(".");
			/*if(pipe)
				pipe_func();
*/
		if(out_red)
			dup2(out, 1);
		else if(in_red)
			dup2(in, 0);

	}
	else if(!(strcmp(argv[0], "pinfo"))){
		if(out_red)
			red_out(argv[argc-1]);
		if(in_red)
			red_in(argv[argc-1]);

		if(argc < 2)
			(*command[2])("0");
		else{
			if(!strcmp(argv[1], ">"))
				(*command[2])("0");
			else if(!strcmp(argv[1], "<")){
				scanf("%s", id);
				id[strlen(id)] = '\0';
				(*command[2])(id);
				dup2(in, 0);

			}

			else{
				(*command[2])(argv[1]);
			}			
		}
		if(out_red)
			dup2(out, 1);
		else if(in_red)
			dup2(in, 0);
	}

	else if(!strcmp(argv[0], "setenv"))
	{
		if(argc != 2 && argc != 3)
			printf("Usage: setenv [env_var] [value]\n");
		else
			(*command_l[3])(argc, argv);
	}
	else if(!strcmp(argv[0], "unsetenv"))
	{
		if(argc != 2)
			printf("Usage: unsetenv [env_var]\n");
		else
			(*command_p[2])(argv);
	}
	else{
		if(argc > 1 && bg){
			argv[argc-1] = NULL;
			(*command_p[1])(argv);
		}
		else{
			if(in_red){
				exec_in(0);

			}
			else if(out_red){
				exec_out(0);

			}
			else{
				(*command_l[1])(argc, argv);
			}
		}
		if(in_red)
			dup2(in, 0);
		if(out_red)
			dup2(out, 1);

	}
}

int pipe_occ(char **argv, int argc, int pos)
{
	int i = pos + 1;
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

	for(i = 0; i < pos - j; i++){
		a[i] = (char *)malloc(sizeof(char) * MAX);
		strcpy(a[i], argv[i + j]);

	}
	a[pos] = (char *)malloc(sizeof(char) * MAX);
	a[pos] = NULL;
}

void pipe_func(char ***argv);
void pipe_mul_func(char ***argv, int i);

void pipe_cmd(int argc, char **argv)
{
	char **pipeargs[10];
	int pos = 0, i = 0, lpos = 0;

	while(pos < argc){
		pos = pipe_occ(argv, argc, pos);
		pipeargs[i] = (char **)malloc(sizeof(char) * 10);

		set_arg(lpos, pos, pipeargs[i++], argv);
		lpos = pos + 1;
	}
	if(pipeargs[0][1] == NULL){
		printf("NULL\n");
	}
	if(i == 2)
		pipe_func(pipeargs);
	else
		pipe_mul_func(pipeargs, i);

}
void pipe_func(char ***argv)
{
	int pipfd[2];

	if(pipe(pipfd) != 0){
		fprintf(stderr, "Unable to pipe\n");
		return;
	}

	
	if(!fork()){
		dup2(pipfd[1], 1);
		close(pipfd[0]);
		if((execvp(argv[0][0], argv[0]) == -1)){
			fprintf(stderr, "Unable to run\n");
		}
	}
	if(!fork()){
		dup2(pipfd[0], 0);
		close(pipfd[1]);
		if((execvp(argv[1][0], argv[1]) == -1)){
			fprintf(stderr, "Unable to run\n");
		}
	}
	else{
		close(pipfd[1]);
		close(pipfd[0]);
		wait(NULL);
		wait(NULL);
	}
}

void pipe_mul_func(char ***argv, int count)
{

	int i, j = 0;
	char *file = "temp.txt";

	for(i = 0; i < count; i++){
		for(j = 0; argv[i][j] != NULL; j++){
			printf("%s\n", argv[i][j]);
		}
		if(i != 0){
			(*command[4])(file);

			shell_cmd(j, argv[i]);

		}
		if(i != count){
			(*command[3])(file);
			shell_cmd(j, argv[i]);

		}
	}
}
void loop(){

	print_user();

	len = getline(&input, &max, stdin);
	input[len-1] = '\0';
	len--;
	argv = (char **)malloc(MAX * sizeof(char*));

	argc = parse_input(input, argv);

	check(argv, argc);

	if(piping){
		in = dup(0);
		out = dup(1);
		pipe_cmd(argc, argv);
	}
	else
		shell_cmd(argc, argv);

	bg = false;
	out_red = false;
	in_red = false;
	piping = false;
	app = false;
	free(argv);
}

/*Main Function*/

int main(){
	
	struct passwd *u;
	uid_t uid;

	uname(usr);
	uid = geteuid();
	u = getpwuid(uid);
	signal(SIGINT, exit_handler);
	signal(SIGTSTP, stop_handler);
	user[0] = (char *)malloc(MAX * sizeof(char));
	user[1] = (char *)malloc(MAX * sizeof(char));
	strcpy(user[0], u -> pw_name);
	strcpy(user[1], usr -> nodename);
	while(1){
		loop();
	}
	exit(0);

}
