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

void proclore(char *pid_str)
{
    pid_t pid = (pid_str == NULL) ? getpid() : atoi(pid_str);
    char path[4096], buffer[4096];
    char status;
    int vm_size;
    pid_t pgid;
    ssize_t read_size;

  
    pgid = getpgid(pid);
    if (pgid == -1)
    {
        perror("Failed to get process group ID");
        return;
    }

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(path, "r");
    if (!stat_file)
    {
        perror("Failed to open stat file");
        return;
    }
    fscanf(stat_file, "%*d %*s %c", &status);
    fclose(stat_file);

    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    FILE *statm_file = fopen(path, "r");
    if (!statm_file)
    {
        perror("Failed to open statm file");
        return;
    }
    fscanf(statm_file, "%d", &vm_size);
    fclose(statm_file);


    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    read_size = readlink(path, buffer, sizeof(buffer) - 1);
    if (read_size == -1)
    {
        if (errno == EACCES)
        {
            printf("Permission denied to access executable path for pid %d\n", pid);
            printf("Try running this program with 'sudo' to get access.\n");
        }
        else
        {
            perror("Failed to get executable path");
        }
        return;
    }
    buffer[read_size] = '\0';

 
    char status_plus = (tcgetpgrp(STDIN_FILENO) == pgid) ? '+' : ' ';

    printf("pid: %d\n", pid);
    printf("process status: %c%c\n", status, status_plus);
    printf("Process group: %d\n", pgid);
    printf("Virtual memory: %d KB\n", vm_size * (getpagesize() / 1024)); 
    printf("Executable path: %s\n", buffer);
}
