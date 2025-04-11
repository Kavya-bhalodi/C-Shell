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



void setNonBlockingMode()
{
    struct termios ttystate;

    if (tcgetattr(STDIN_FILENO, &ttystate) == -1)
    {
        perror("tcgetattr");
        return;
    }


    ttystate.c_lflag &= ~(ICANON | ECHO);

 
    ttystate.c_cc[VMIN] = 1;

 
    ttystate.c_cc[VTIME] = 0;


    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttystate) == -1)
    {
        perror("tcsetattr");
        return;
    }

 
    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
    }
}
void resetBlockingMode()
{
    struct termios ttystate;


    if (tcgetattr(STDIN_FILENO, &ttystate) == -1)
    {
        perror("tcgetattr");
        return;
    }

 
    ttystate.c_lflag |= ICANON | ECHO;


    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttystate) == -1)
    {
        perror("tcsetattr");
        return;
    }


    if (fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK) == -1)
    {
        perror("fcntl");
    }
}
pid_t getMostRecentPID()
{
    DIR *dir;
    struct dirent *entry;
    pid_t recent_pid = -1;


    if ((dir = opendir("/proc")) == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        
        if (isdigit(entry->d_name[0]))
        {
            pid_t pid = atoi(entry->d_name);
            if (pid > recent_pid)
            {
                recent_pid = pid;
            }
        }
    }

    closedir(dir);
    return recent_pid;
}
void printRecentPIDs(int interval)
{
    pid_t last_pid = -1;

    setNonBlockingMode();

    while (1)
    {
        pid_t recent_pid = getMostRecentPID();

        if (recent_pid != last_pid)
        {
            printf("%d\n", recent_pid);
            last_pid = recent_pid;
        }

    
        sleep(interval);

        char c;
        if (read(STDIN_FILENO, &c, 1) == 1 && c == 'x')
        {
            break;
        }
    }

    resetBlockingMode();
}

