
object compliation:
	gcc -Wall -c ls.c cd.c pwd.c echo.c syscmd.c pinfo.c redirect.c setenv.c
	gcc -Wall ls.o cd.o pwd.o echo.o syscmd.o pinfo.o redirect.o setenv.o main.c -o shell

