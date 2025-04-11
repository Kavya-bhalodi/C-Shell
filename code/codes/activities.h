#ifndef ACTIVITIES_H
#define ACTIVITIES_H
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
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096


typedef struct
{
    pid_t pid;
    char command[100];
    char state[10]; // "Running" or "Stopped"
} Process;


extern Process processList[MAX_PROCESSES];
extern int processCount;
extern pid_t fg_pid ; 


void addProcess(pid_t pid, const char *command, const char *state);
void updateProcessState(pid_t pid, const char *state);
void removeProcess(pid_t pid);
void displayActivities();




#endif
