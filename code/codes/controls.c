#include "activities.h"
#include "alias.h"
#include "bg.h"
#include "command_log.h"
#include "controls.h"
#include "fg.h"
#include "hop.h"
#include "neonate.h"
#include "ping.h"
#include "process.h"
#include "proclore.h"
#include "reveal.h"
#include "prompt.h"
#include "seek.h"
#include "pipes_io.h"


#define MAX_PROCESSES 100
#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096

extern int processCount; 
extern pid_t fg_pid;    

void handle_ctrl_c(int sig)
{
    // printf("%d\n", fg_pid);
    if (fg_pid > 0)
    {
        kill(fg_pid, SIGINT); 
        fg_pid = -1;       
    }
}
void handle_ctrl_d(int sig)
{
    // printf("bfvc xdfgb");
    // if (fg_pid > 0)
    // {
    for (int i = 0; i < processCount; i++)
    {
        kill(processList[i].pid, SIGQUIT);
    }
    printf("Exiting shell\n");
    exit(0);
    // }
}
void handle_ctrl_z(int signal, char *command)
{
    if (fg_pid > 0 && signal == SIGTSTP)
    {
        printf("\nIntercepted SIGINT!\n");

        printf("%d\n", fg_pid);
        kill(fg_pid, SIGTSTP);

        fg_pid = -1;
    }
}
