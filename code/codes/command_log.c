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
#define MAX_LOG_SIZE 15
#define LOG_FILE "./history.txt"
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

void loadCommandLog()
{
    FILE *file = fopen(LOG_FILE, "r");
    if (file)
    {
        while (fgets(commandLog[logCount], sizeof(commandLog[logCount]), file) != EOF)
        {
            int x = strlen(commandLog[logCount]);
            commandLog[logCount][x - 1] = '\0';
           
            logCount++;
            if (logCount >= MAX_LOG_SIZE)
                break;
        }
        fclose(file);
    }
}
    // for (int i = 0; i < logCount; i++)
    // {
    //     printf("%s\n",commandLog[i]);
    // }
    


void saveCommandLog()
{
    FILE *file = fopen(LOG_FILE, "w");
    if (file)
    {
        for (int i = 0; i < logCount; i++)
        {
            fprintf(file, "%s\n", commandLog[i]);
        }
        fclose(file);
    }
}

void addCommandToLog(char *input)
{
    // printf("%s\n%s\n%d\n", input, commandLog[logCount - 1], logCount);
    if (logCount == 0 || strcmp(commandLog[logCount - 1], input) != 0)
    {
        if (logCount < MAX_LOG_SIZE)
        {
            strcpy(commandLog[logCount++], input);
        }
        else
        {
          
            for (int i = 1; i < MAX_LOG_SIZE; i++)
            {
                strcpy(commandLog[i - 1], commandLog[i]);
            }

            strcpy(commandLog[MAX_LOG_SIZE - 1], input);
        }

        saveCommandLog();
    }
    return ;
}