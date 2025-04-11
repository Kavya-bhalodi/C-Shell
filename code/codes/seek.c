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

void colour_print(const char *path, const char *base_path, int is_dir)
{
    const char *relative_path = path + strlen(base_path) + 1; // +1 to skip the '/'
    if (is_dir)
    {
        printf(".\033[1;34m%s\033[0m\n", relative_path);
    }
    else
    {
        printf(".\033[1;32m%s\033[0m\n", relative_path);
    }
}

int name_matches(const char *filename, const char *target)
{
    return strncmp(filename, target, strlen(target)) == 0;
}

void search( char *target, char *base_path, int look_for_files, int look_for_dirs, int *found, char *matched_path, int *is_single_type,char *target_dir)
{
    DIR *dir = opendir(base_path);
    if (!dir)
    {
        return;
    }

    struct dirent *entry;
    char path[MAX_PATH_LEN];

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
        struct stat st;
        if (stat(path, &st) == -1)
        {
            continue;
        }

        int is_dir = S_ISDIR(st.st_mode);
        int is_file = S_ISREG(st.st_mode);

        if ((is_file && look_for_files && name_matches(entry->d_name, target)) ||
            (is_dir && look_for_dirs && name_matches(entry->d_name, target)))
        {
            *found += 1;
            if (*found == 1)
            {
                strcpy(matched_path, path);
                *is_single_type = is_dir ? 1 : 2;
            }
            else if (*found > 1)
            {
                *is_single_type = 0;
            }
            colour_print(path, target_dir, is_dir); // Correctly print paths relative to target_dir
        }

        if (is_dir)
        {
            search(target, path, look_for_files, look_for_dirs, found, matched_path, is_single_type, target_dir);
        }
    }
    closedir(dir);
}

void seek(const char *flags, const char *target, const char *target_dir)
{
    int look_for_files = 1, look_for_dirs = 1, execute_effect = 0;

    for (int i = 0; flags[i] != '\0'; i++)
    {
        if (flags[i] == '-')
        {
            if (flags[i + 1] == 'd')
            {
                look_for_files = 0;
            }
            else if (flags[i + 1] == 'f')
            {
                look_for_dirs = 0;
            }
            else if (flags[i + 1] == 'e')
            {
                execute_effect = 1;
            }
            else
            {
                printf("Invalid flags!\n");
                return;
            }
        }
    }

    if (look_for_files == 0 && look_for_dirs == 0)
    {
        printf("Invalid flags!\n");
        return;
    }

    char abs_path[MAX_PATH_LEN];
    realpath(target_dir, abs_path); // Resolve the absolute path of the target directory

    int found = 0;
    char matched_path[MAX_PATH_LEN] = "";
    int is_single_type = 0;

    search(target, abs_path, look_for_files, look_for_dirs, &found, matched_path, &is_single_type, abs_path);

    if (found == 0)
    {
        printf("No match found!\n");
    }
    else if (execute_effect && found == 1 && is_single_type != 0)
    {
        struct stat st;
        if (stat(matched_path, &st) == -1)
        {
            printf("Missing permissions for task!\n");
            return;
        }

        if (is_single_type == 1)
        {
            if (access(matched_path, X_OK) == 0)
            {
                chdir(matched_path);
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
        else if (is_single_type == 2)
        {
            if (access(matched_path, R_OK) == 0)
            {
                FILE *file = fopen(matched_path, "r");
                if (file)
                {
                    char line[MAX_PATH_LEN];
                    while (fgets(line, sizeof(line), file))
                    {
                        printf("%s", line);
                    }
                    fclose(file);
                }
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
    }
}
