#include <stdio.h>
#include <string.h>

void echo(int argc, char **mssg)
{
	int i, j;
	for(i = 1; i < argc; i++){
		for(j = 0; j < strlen(mssg[i]); j++){
			if(mssg[i][j] != '"' && mssg[i][j] != '\'')
				printf("%c", mssg[i][j]);

		}
		printf(" ");
	}
}
