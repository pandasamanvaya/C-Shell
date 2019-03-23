#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#define  MAX 256

pid_t pid, bgpid = 0, stopid = 0;
int job[20], id1 = 1;

void job_queue(int pid)
{
	int i = 1;
	while (i < id1){
		if(job[i] == pid){
			while(i < id1 - 1){
				job[i] = job[i+1];
				i++;
			}
			break;
		}
		else
			i++;
	}
	id1--;
}


void child_handler(int sig)
{
	/*pid_t chpid;

	chpid = wait(NULL); */
	if(pid == bgpid)
		fprintf(stderr, "\nProcess exited with pid %d normally\n", pid);
	job_queue(pid);
}
void syscmd_bg(char **argv)
{

	signal(SIGCHLD, child_handler);

	pid = fork();

	if(!pid){
		if((execvp(argv[0], argv)) == -1){
			fprintf(stderr, "Unable to find such command\n");
			return;
		}
		exit(3);

	}
	else{
		bgpid = pid;
		job[id1] = pid;
		printf("[%d]\t%d\n", id1++, pid);

	}
}


void syscmd_fg(int argc, char **argv)
{
	int stat;

	argv[argc] = (char *)malloc(MAX * sizeof(char));
	argv[argc] = NULL;

	pid = fork();
	if(!pid){
		stat = execvp(argv[0], argv);
		if(stat == -1){
			fprintf(stderr, "Unable to find such command\n");
			return;
		}
		exit(2);
	}
	else{
		stopid = pid;
		waitpid(pid, &stat, WUNTRACED);

		//printf("%d %d %d\n",WIFSIGNALED(stat), WIFEXITED(stat), WEXITSTATUS(stat));
	}
}

void jobs()
{
	int i, fin, j;
	char *val[50], buf[MAX], file[50], *tok = NULL;

	for(i = 1; i < id1; i++){
		sprintf(file, "/proc/%d/stat", job[i]);
		fin = open(file, O_RDONLY);
		if(fin == -1)
			fprintf(stderr, "Unable to open status file\n");
		else{
			read(fin, buf, MAX);
			tok = strtok(buf, " ");
			j = 0;
			while(tok != NULL){
				val[j] = (char *)malloc(20 * sizeof(char));
				strcpy(val[j++], tok);

				tok = strtok(NULL, " ");
			}
		}
		printf("[%d]\t", i);
		if(!strcmp(val[2], "S"))
			printf("Running \t");
		else if(!strcmp(val[2], "T"))
			printf("Stopped\t");
		for(j = 0; j < strlen(val[1]) - 1; j++)
			file[j] = val[1][j+1];
		file[j-1] = '\0';
		printf("%s[%d]\n", file, job[i]);

		close(fin);

	}
}

void overkill()
{
	int i;

	for(i = 0; i < id1; i++){
		kill(job[i], SIGKILL);
	}
	id1 = 0;
}

void process_stop()
{
	printf("pid = %d\n", stopid);
	if(stopid){
		job[id1++] = stopid;
		kill(stopid, SIGTSTP);
		stopid = 0;
	}
	
}

void kjob(char ** argv)
{
	int i, found = 0, job_no, sig;

	job_no = atoi(argv[1]);
	sig = atoi(argv[2]);

	for(i = 0; i < id1; i++){
		if(i+1 == job_no){
			found = 1;
			break;
		}

	}
	if(found){
		job_queue(job[job_no]);
		kill(job[job_no], sig);
	}
	else
		fprintf(stderr, "No such job is found\n");
}

void fg(char **argv)
{
	int job_no, found = 0, i;

	job_no = atoi(argv[1]);

	for(i = 0; i < id1; i++){
		if(i+1 == job_no){
			found = 1;
			break;
		}

	}
	if(found){
		kill(job[job_no], SIGCONT);
		waitpid(job[job_no], NULL, WUNTRACED);
	}
	else
		fprintf(stderr, "No such job is found\n");
}

void bgr(char **argv)
{
	int job_no, found = 0, i;

	job_no = atoi(argv[1]);

	for(i = 0; i < id1; i++){
		if(i+1 == job_no){
			found = 1;
			break;
		}

	}
	if(found){
		kill(job[job_no], SIGTTIN);
		kill(job[job_no], SIGCONT);
	}
	else
		fprintf(stderr, "No such job is found\n");
}	
