#include <stdlib.h>
#include <stdio.h>

void set(char *a, char *b, int mode)
{

	if((setenv(a, b, mode)) == -1)
		fprintf(stderr, "Unable to set variable %s\n", a);
	else
		printf("Value of %s is set to %s\n", a, b);

}
void setvar(int argc, char ** argv)
{
	if(argc == 2){
		if(getenv(argv[1]) != NULL)
			set(argv[1], "\0", 1);
		else
			set(argv[1], "\0", 0);
	}
	else{	
		if(getenv(argv[1]) != NULL)
			set(argv[1], argv[2], 1);
		else
			set(argv[1], argv[2], 0);

	}

}
void  unsetvar(char **argv)
{
	if(getenv(argv[1]) != NULL){
		if((unsetenv(argv[1])) == -1)
			fprintf(stderr, "Unable to delete variable %s\n", argv[1]);
		else
			printf("Value of %s is unset\n", argv[1]);
	}
	else
		fprintf(stderr, "Unable to find such variable %s\n", argv[1]);

} 