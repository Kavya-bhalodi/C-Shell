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

int process_exists(pid_t pid)
{
    return (kill(pid, 0) == 0);
}
void fg_command(pid_t pid)
{

    if (kill(pid, 0) == -1)
    {
        perror("No such process found");
        return;
    }

  
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);


    if (tcsetpgrp(STDIN_FILENO, pid) == -1)
    {
        perror("Failed to bring process to foreground");

        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        return;
    }


    if (kill(pid, SIGCONT) == -1)
    {
        perror("Failed to continue process");
        tcsetpgrp(STDIN_FILENO, getpid()); 
  
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        return;
    }

    
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1)
    {
        perror("Failed to wait for process");
        tcsetpgrp(STDIN_FILENO, getpid()); 
      
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        return;
    }

 
    if (tcsetpgrp(STDIN_FILENO, getpid()) == -1)
    {
        perror("Failed to restore control to shell");
    }


    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);


    if (WIFSTOPPED(status))
    {
        updateProcessState(pid, "Stopped");

        printf("Process [%d] was stopped by signal %d\n", pid, WSTOPSIG(status));
    }
    else if (WIFEXITED(status))
    {
        removeProcess(pid);

        printf("Process [%d] exited with status %d\n", pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        updateProcessState(pid, "Stopped");
        printf("Process [%d] was killed by signal %d\n", pid, WTERMSIG(status));
    }
    else if (WIFCONTINUED(status))
    {
        removeProcess(pid);
        printf("Process [%d] continued\n", pid);
    }
}
