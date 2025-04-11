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

void displayPrompt()
{
    char cwd[4096];
    uid_t uid = getuid();

  
    struct passwd *pw = getpwuid(uid);
    char *username;
    if (pw != NULL)
    {

        username = pw->pw_name;
    
    }
    else
    {

        perror("Failed to get username");
    }
    struct utsname sysInfo;
    uname(&sysInfo);

    getcwd(cwd, sizeof(cwd));

    if (lastCommandTime > 2)
    {
        printf("<%s@%s:~ %s : %ds>", username, sysInfo.nodename, lastCommand, lastCommandTime);
    }
    else if (strncmp(cwd, homeDir, strlen(homeDir)) == 0)
    {
        // printf("fbdgfnvhb");
        // printf("%d\n",lastCommandTime);
        printf("<%s@%s:~%s>", username, sysInfo.nodename, cwd + strlen(homeDir));
    }
    else
    {
        printf("<%s@%s:~%s>", username, sysInfo.nodename, cwd);
    }


}
