#ifndef CONTROLS_H
#define CONTROLS_H

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

#define MAX_PROCESSES 100

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096

extern int processCount;     // Number of processes in the list
// extern pid_t fg_pid;        // Global variable to track the foreground process

void handle_ctrl_c(int sig);
void handle_ctrl_d(int sig);
void handle_ctrl_z(int signal, char *command);



#endif
