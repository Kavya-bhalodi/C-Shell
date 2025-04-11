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


void contents_print(struct dirent *entry, struct stat *fileStat, int flag_l)
{
    if (S_ISDIR(fileStat->st_mode))
    {
        printf("\033[1;34m"); 
    }
    else if (fileStat->st_mode & S_IXUSR)
    {
        printf("\033[1;32m"); 
    }
    else
    {
        printf("\033[1;37m");
    }

    if (flag_l)
    {
        printf((S_ISDIR(fileStat->st_mode)) ? "d" : "-");
        printf((fileStat->st_mode & S_IRUSR) ? "r" : "-");
        printf((fileStat->st_mode & S_IWUSR) ? "w" : "-");
        printf((fileStat->st_mode & S_IXUSR) ? "x" : "-");
        printf((fileStat->st_mode & S_IRGRP) ? "r" : "-");
        printf((fileStat->st_mode & S_IWGRP) ? "w" : "-");
        printf((fileStat->st_mode & S_IXGRP) ? "x" : "-");
        printf((fileStat->st_mode & S_IROTH) ? "r" : "-");
        printf((fileStat->st_mode & S_IWOTH) ? "w" : "-");
        printf((fileStat->st_mode & S_IXOTH) ? "x" : "-");
        printf(" %ld", fileStat->st_nlink);

        struct passwd *pw = getpwuid(fileStat->st_uid);
        struct group *gr = getgrgid(fileStat->st_gid);
        printf(" %s %s", pw->pw_name, gr->gr_name);

        printf(" %5ld", fileStat->st_size);

        char timebuf[80];
        struct tm lt;
        localtime_r(&fileStat->st_mtime, &lt);
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", &lt);
        printf(" %s", timebuf);
    }

    printf(" %s\n", entry->d_name);
    printf("\033[0m");
}

int compare(const void *a, const void *b)
{
    struct dirent *entryA = *(struct dirent **)a;
    struct dirent *entryB = *(struct dirent **)b;
    return strcasecmp(entryA->d_name, entryB->d_name); 
}

void reveal(char *path, int flag_a, int flag_l)
{
    char dirPath[4096];
    struct stat fileStat;
    // printf("%s\n",path);
    if (strcmp(path, "~") == 0)
    {
        strcpy(dirPath, homeDir); 
        chdir(homeDir);
    }
    else if (strcmp(path, "-") == 0)
    {
        strcpy(dirPath, prevDir);
    }
    else if (path[0] == '~')
    {
        snprintf(dirPath, sizeof(dirPath), "%s%s", homeDir, path + 1); 
        chdir(dirPath);
    }
    else
    {
        strcpy(dirPath, path); 
    }

    if (stat(dirPath, &fileStat) == -1)
    {
        perror("reveal");
        return;
    }

    if (S_ISDIR(fileStat.st_mode))
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(dirPath)) == NULL)
        {
            perror("reveal");
            return;
        }

        struct dirent *entries[4096];
        int count = 0;
        long totalBlocks = 0; 

        while ((entry = readdir(dir)) != NULL)
        {
            if (!flag_a && entry->d_name[0] == '.')
            {
                continue; 
            }

            char entryPath[4096];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", dirPath, entry->d_name);

            if (stat(entryPath, &fileStat) == -1)
            {
                perror("reveal");
                continue;
            }

            totalBlocks += fileStat.st_blocks; 

            entries[count++] = entry;
        }
        if(flag_l)
        printf("total %ld\n", totalBlocks / 2); // Divide by 2 to convert to 1K blocks

        qsort(entries, count, sizeof(struct dirent *), compare); 

        for (int i = 0; i < count; i++)
        {
            char entryPath[4096];
            snprintf(entryPath, sizeof(entryPath), "%s/%s", dirPath, entries[i]->d_name);

            if (stat(entryPath, &fileStat) == -1)
            {
                perror("reveal");
                continue;
            }

            contents_print(entries[i], &fileStat, flag_l);
        }

        closedir(dir);
    }
    else if (S_ISREG(fileStat.st_mode))
    {
        struct dirent fileEntry;
        strcpy(fileEntry.d_name, path);
        contents_print(&fileEntry, &fileStat, flag_l);
    }
    else
    {
        printf("Invalid file type for path: %s\n", path);
    }
}

void parseRevealCommand(char *flags, char *path)
{
    int flag_a = 0;
    int flag_l = 0;

    for (int i = 0; flags[i] != '\0'; i++)
    {
        if (flags[i] == 'a')
            flag_a = 1;
        if (flags[i] == 'l')
            flag_l = 1;
    }
    // printf("%d %d %s\n",flag_a,flag_l,path);

    reveal(path, flag_a, flag_l);
}
