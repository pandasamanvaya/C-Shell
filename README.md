# This is basic shell for Linux-based Os(all commands may not work on MacOs) which supports the following commands :- 
* ls [-a, -l, -al, -la] - To list files in a directory.
* cd - Change directory.
* pwd - Present working directory.
* echo - Print message in shell.
* pinfo <pid> - Prints information about a given process.
* setenv <var> <value> - To set enviroment variables.
* unsetenv <var>- To delete enviroment variables.
* jobs - Displays status of all jobs running(both foreground and background).
* fg <job_no> - To bring a background process to foreground.
* bg <job_no> - To take a foreground process to background.
* kjob <job_no> <signal> - To send a signal to a particular job.
* overkill - To kill all background processes. 
* quit - To exit the shell

# Some other features that are also implemented are :-  
* Foreground and Background process `using &`.
* I/O redirection using `>` and `<`.
* Commmand piping using `|`.
* Ctrl+z puts current running process in background.
* Ctrl+c closes the current running process in shell.
