#include <stdio.h>

extern int dno;

void pwd(char *path[20])
{
	int j = 2;
	printf("~");
	while(j < dno)
		printf("/%s", path[j++]);
	printf("\n");
}
