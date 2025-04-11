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



extern Process processList[MAX_PROCESSES];
extern int processCount;
extern pid_t fg_pid ;        



void addProcess(pid_t pid, const char *command, const char *state)
{
    if (processCount < MAX_PROCESSES)
    {
        processList[processCount].pid = pid;
        strncpy(processList[processCount].command, command, MAX_COMMAND_LENGTH);
        strncpy(processList[processCount].state, state, sizeof(processList[processCount].state));
        processCount++;
    }
}
void updateProcessState(pid_t pid, const char *state)
{
    for (int i = 0; i < processCount; i++)
    {
        if (processList[i].pid == pid)
        {
            strncpy(processList[i].state, state, sizeof(processList[i].state));
            break;
        }
    }
}
void removeProcess(pid_t pid)
{
    for (int i = 0; i < processCount; i++)
    {
        if (processList[i].pid == pid)
        {
            for (int j = i; j < processCount - 1; j++)
            {
                processList[j] = processList[j + 1];
            }
            processCount--;
            break;
        }
    }
}
void displayActivities()
{
    for (int i = 0; i < processCount - 1; i++)
    {
        for (int j = i + 1; j < processCount; j++)
        {
            if (strcmp(processList[i].command, processList[j].command) > 0)
            {
                Process temp = processList[i];
                processList[i] = processList[j];
                processList[j] = temp;
            }
        }
    }

    for (int i = 0; i < processCount; i++)
    {
        printf("[%d] : %s - %s\n", processList[i].pid, processList[i].command, processList[i].state);
    }
}
