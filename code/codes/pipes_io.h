#ifndef PIPES_IO_H
#define PIPES_IO_H

// Ensuring safe usage of standard and custom headers
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>

void execute_command_redirection(char *command);
void jmp_to_pipes(char *commands);
void IO_redirection(char *command);


#endif