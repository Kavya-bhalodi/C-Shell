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


void handlePingCommand(int pid, int signal_number)
{
    signal_number %= 32;


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
    // printf("a");
 
    if (kill(pid, signal_number) == -1)
    {
        perror("Error sending signal");
        // printf("b");
    }
    else
    {
        updateProcessState(pid, "Stopped");
        printf("Sent signal %d to process with pid %d\n", signal_number, pid);
        // printf("c");
    }
    // printf("d");
    if (signal_number == 9 )
    {
        removeProcess(pid);

    }
    // printf("e");
    return ;
}
