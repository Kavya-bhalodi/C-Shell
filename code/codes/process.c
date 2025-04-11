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

// #include "main1.c"
#define MAX_LOG_SIZE 15
#define LOG_FILE "/home/kavya-bhalodi/osn/history.txt"
#define MAX_PATH_LEN 4096

#define MAX_PROCESSES 100

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096
extern char commandLog[15][4096];
extern int logCount;
extern int num_background;
extern char homeDir[4096];
extern char prevDir[4096];
extern char lastCommand[4096];
extern int lastCommandTime;

extern int processCount; 
extern pid_t fg_pid;    
// extern pid_t foreground_pid;

void handleBackgroundCompletion(int sig)
{
    int status;
    pid_t pid;

   
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        if (WIFEXITED(status))
        {
            removeProcess(pid);
            printf("Process with PID %d exited normally\n", pid);
        }
        else if (WIFSIGNALED(status))
        {
            updateProcessState(pid, "Stopped");
            printf("Process with PID %d was terminated by signal %d\n", pid, WTERMSIG(status));
            // removeProcess(pid);
        }
        else if (WIFSTOPPED(status))
        {
            updateProcessState(pid, "Stopped");
            printf("Process with PID %d was stopped by signal %d\n", pid, WSTOPSIG(status));
        }
        else
        {

            removeProcess(pid);
        }
    }
}