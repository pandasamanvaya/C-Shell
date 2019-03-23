#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

struct dirent *file;
struct stat filest;

DIR *dir;

void lsp(char direct[])
{

	dir = opendir(direct);
	if(dir == NULL){
		fprintf(stderr, "No such directory\n");
		return;
	}

	file = readdir(dir);
	while(file != NULL){
		if(file -> d_name[0] != '.')
			printf("%s\n", file -> d_name);
		file = readdir(dir);
	}
	closedir(dir);
}

void print_l()
{
	struct passwd *user;
	struct group *grp;

	printf( (S_ISDIR(filest.st_mode)) ? "d" : "-");
	printf( (filest.st_mode & S_IRUSR) ? "r" : "-");
	printf( (filest.st_mode & S_IWUSR) ? "w" : "-");
	printf( (filest.st_mode & S_IXUSR) ? "x" : "-");
	printf( (filest.st_mode & S_IRGRP) ? "r" : "-");
	printf( (filest.st_mode & S_IWGRP) ? "w" : "-");
	printf( (filest.st_mode & S_IXGRP) ? "x" : "-");
	printf( (filest.st_mode & S_IROTH) ? "r" : "-");
	printf( (filest.st_mode & S_IWOTH) ? "w" : "-");
	printf( (filest.st_mode & S_IXOTH) ? "x " : "- ");

	user = getpwuid(filest.st_uid);
	grp = getgrgid(filest.st_gid);
	printf(" %s %s ", user -> pw_name, grp -> gr_name);
	printf("%ld\t", filest.st_size);
	char time[40], *t = NULL;
	t = ctime(&filest.st_ctime);
	strcpy(time, t);
	time[strlen(time) - 1] = '\0';
	printf("%s ", time);
	printf("%s\n", file -> d_name);
}

void ls_l(char optarg[])
{
	dir = opendir(optarg);
	if(dir == NULL){
		fprintf(stderr, "No such directory\n");
		return;
	}
	file = readdir(dir);
	while(file != NULL){
		stat(file -> d_name, &filest);
		if(file -> d_name[0] != '.')
			print_l();
		file = readdir(dir);
	}
	closedir(dir);
}
void ls_a(char optarg[])
{
	dir = opendir(optarg);
	if(dir == NULL){
		fprintf(stderr, "No such directory\n");
		return;
	}

	file = readdir(dir);
	while(file != NULL){
		printf("%s\n", file -> d_name);
		file = readdir(dir);
	}
	closedir(dir);
}

void ls_al(char optarg[])
{
	dir = opendir(optarg);
	if(dir == NULL){
		fprintf(stderr, "No such directory\n");
		return;
	}
	file = readdir(dir);
	while(file != NULL){
		stat(file -> d_name, &filest);
		print_l();
		file = readdir(dir);
	}
	closedir(dir);
}

void ls(int argc, char *argv[20])
{
	
	int a = 0, l = 0, i = 0;
	while(i < argc){
		if(argv[i][1] == 'a'){				
			a = 1;

			if(strlen(argv[i]) > 1 && argv[i][2] == 'l'){
				l = 1;
			}
			if(strlen(argv[i]) > 3 && (argv[i][3] != 'l' || argv[i][3] != 'a')){
				i++;
				a = -1;
				l = -1;
				continue;
			}
		}
		else if(argv[i][1] == 'l'){

			l = 1;
			if(strlen(argv[i]) > 1 && argv[i][2] == 'a')
				a = 1;

			if(strlen(argv[i]) > 3 && (argv[i][3] != 'l' || argv[i][3] != 'a')){
				i++;
				a = -1;
				l = -1;
				continue;
			}
		}
		i++;
	}

	if(a == 1 && l == 1){
		if(argv[i-1][0] == '-')

			ls_al(".");
		else
			ls_al(argv[i-1]);
	}
	else if(a == 1){
		if(argv[i-1][0] == '-')
			ls_a(".");
		else
			ls_al(argv[i-1]);

	}
	else if(l == 1){

		if(argv[i-1][0] == '-')
			ls_l(".");
		else
			ls_l(argv[i - 1]);
	}
	else if(a == -1 || l == -1){
		fprintf(stderr, "No such option in ls -[al]\n");

	}
	else
		lsp(argv[i - 1]);


}
