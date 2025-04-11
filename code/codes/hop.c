
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
#define LOG_FILE "/home/kavya-bhalodi/osn/history.txt"
#define MAX_PATH_LEN 4096

#define MAX_PROCESSES 100
#define MAX_COMMAND_LENGTH 256
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

void hop(char *path)
{
    char newDir[4096];
    char currentDir[4096];

  
    getcwd(currentDir, sizeof(currentDir));

    if (strcmp(path, "~") == 0)
    {
        strcpy(newDir, homeDir);
    }
    else if (strcmp(path, "-") == 0)
    {
        strcpy(newDir, prevDir);
    }
    else
    {
        if (path[0] == '~')
        {
            snprintf(newDir, sizeof(newDir), "%s%s", homeDir, path + 1);
        }
        else
        {
            strcpy(newDir, path);
        }
    }


    if (chdir(newDir) == 0)
    {

        strcpy(prevDir, currentDir);
        getcwd(newDir, sizeof(newDir));
        printf("%s\n", newDir);
    }
    else
    {
    
        perror("hop");
    }
}