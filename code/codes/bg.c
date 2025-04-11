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


int is_process_stopped(pid_t pid)
{
    char proc_path[256];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/status", pid);

    FILE *fp = fopen(proc_path, "r");
    if (fp == NULL)
    {
        return 0; 
    }

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
           
            if (strchr(line, 'T') != NULL)
            {
                fclose(fp);
                return 1; 
            }
            break;
        }
    }

    fclose(fp);
    return 0; 
}
void bg_command(pid_t pid)
{
    if (kill(pid, 0) == -1)
    {
        
        if (errno == ESRCH)
        {
            printf("No such process found\n");
        }
        else
        {
            perror("Error checking process");
        }
        return;
    }

    if (is_process_stopped(pid))
    {
     
        if (kill(pid, SIGCONT) == 0)
        {
            printf("Process [%d] resumed in the background\n", pid);
            updateProcessState(pid, "Running");
        }
        else
        {
            perror("Failed to resume process");
        }
    }
    else
    {
        printf("Process [%d] is not stopped or does not exist\n", pid);
    }
}
