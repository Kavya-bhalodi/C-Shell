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
# define PATH_MAX 4096


extern Alias aliases[MAX_ALIASES];
extern int aliasCount;

void loadMyshrc()
{



    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcat(cwd, "/.myshrc");
    }
    else
    {
        perror("getcwd() error");
        return;
    }

    FILE *file = fopen(cwd, "r");
    if (!file)
    {
        perror("Failed to open .myshrc");
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0)
            continue;

        if (strncmp(line, "alias ", 6) == 0 || strchr(line, '=') != NULL)
        {
            char *aliasLine = (strncmp(line, "alias ", 6) == 0) ? line + 6 : line;
            char *equalSign = strchr(aliasLine, '=');

            if (equalSign)
            {
                *equalSign = '\0';
                char *name = aliasLine;
                char *value = equalSign + 1;

                while (isspace((unsigned char)*name))
                    name++;
                char *end = name + strlen(name) - 1;
                while (end > name && isspace((unsigned char)*end))
                    end--;
                *(end + 1) = '\0';

                while (isspace((unsigned char)*value))
                    value++;
                end = value + strlen(value) - 1;
                while (end > value && isspace((unsigned char)*end))
                    end--;
                *(end + 1) = '\0';

                addAlias(name, value);
            }
        }

        else if (strncmp(line, "func ", 5) == 0)
        {

            char *funcName = line + 5;

            // printf("Function found: %s\n", funcName);
        }
    }

    fclose(file);
}

void addAlias(const char *name, const char *value)
{
    if (aliasCount < MAX_ALIASES)
    {
        strncpy(aliases[aliasCount].name, name, MAX_COMMAND_LENGTH);
        strncpy(aliases[aliasCount].value, value, MAX_COMMAND_LENGTH);
        aliases[aliasCount].name[MAX_COMMAND_LENGTH - 1] = '\0';
        aliases[aliasCount].value[MAX_COMMAND_LENGTH - 1] = '\0';
        aliasCount++;
    }
    else
    {
        printf("Alias limit reached, cannot add more aliases.\n");
    }
}