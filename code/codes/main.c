#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>
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
#include "iMan.h"
#include "pipes_io.h"

#define MAX_PROCESSES 100

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096
#define REQUEST_SIZE 1024
#define BUFFER_SIZE 4096
#define MAX_CMD_LEN 1024

Alias aliases[MAX_ALIASES];
int aliasCount = 0;

char commandLog[15][4096];
int logCount = 0;
int num_background = 0;
char homeDir[4096] = {0};
char prevDir[4096] = {0};
char lastCommand[4096] = {0};
int lastCommandTime = 0;
int found_mk_hop = 0;
int found_hop_seek = 0;

Process processList[100];
int processCount = 0;
pid_t fg_pid = -1;

void remove_update(int pid, int status)
{

    if (WIFSIGNALED(status))
    {
        updateProcessState(pid, "Stopped");
        printf("Process with PID %d was terminated by signal %d\n", pid, WTERMSIG(status));
        // removeProcess(pid);
    }
    else if (WIFSTOPPED(status))
    {
        updateProcessState(pid, "Stopped");
        printf("Process with PID %d was stopped by signal %d\n", pid, WSTOPSIG(status));
    }
    else if (WIFEXITED(status))
    {
        printf("Process with PID %d exited normally\n", pid);
        removeProcess(pid);
    }
    else
    {
        removeProcess(pid);
    }
    return;
}

void parseAndExecute(char *input)
{
    char *token;
    char *saveptr;

    token = strtok_r(input, ";", &saveptr);
    while (token != NULL)
    {
        int fg = 0, redirection_pipes = 0, io_red = 0;
        for (int i = 0; i < strlen(token); i++)
        {
            if (token[i] == '|')
            {
                // printf("%s\n", input);
                redirection_pipes = 1;
                break;
            }
            else if (token[i] == '>' || token[i] == '<')
            {

                io_red = 1;
                // break;
            }
        }
        if (redirection_pipes)
        {
            jmp_to_pipes(token);
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }
        else if (io_red)
        {
            IO_redirection(token);
            // execute_command_redirection(token);
            token = strtok_r(NULL, ";", &saveptr);
            continue;
        }

        // int yi = 0;
        for (int i = 0; i < strlen(token); i++)
        {
            if (token[i] == '&')
            {
                fg++;
            }
        }
        // printf("%s %d\n", token, fg);
        char *subtoken;
        char *subsaveptr;
        if (fg)
        {
            subtoken = strtok_r(token, "&", &subsaveptr);
            while (subtoken != NULL)
            {
                if (fg--)
                {
                    int len = strlen(subtoken);
                    while (len > 0 && (subtoken[len - 1] == ' ' || subtoken[len - 1] == '\t'))
                    {
                        len--;
                        subtoken[len] = '\0';
                    }
                    int background = 0;
                    if (subsaveptr != NULL)
                    {
                        background = 1;
                    }
                    // int background = (subsaveptr != NULL);
                    executeCommand(subtoken, background);
                    subtoken = strtok_r(NULL, "&", &subsaveptr);
                }
                else
                {
                    int len = strlen(subtoken);
                    while (len > 0 && (subtoken[len - 1] == ' ' || subtoken[len - 1] == '\t'))
                    {
                        len--;
                        subtoken[len] = '\0';
                    }

                    executeCommand(subtoken, 0);
                    subtoken = strtok_r(NULL, "&", &subsaveptr);
                }
            }
        }
        else
        {

            size_t len = strlen(token);
            while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\t'))
            {
                len--;
                token[len] = '\0';
            }
            // printf("%s\n",token);

            executeCommand(token, 0);
        }

        token = strtok_r(NULL, ";", &saveptr);
    }
}

void executeCommand(char *command, int background)
{
    char *args[100];
    int argCount = 0;

    for (int i = 0; i < aliasCount; i++)
    {
        if (strncmp(command, aliases[i].name, strlen(aliases[i].name)) == 0 &&
            (command[strlen(aliases[i].name)] == ' ' || command[strlen(aliases[i].name)] == '\0'))
        {
            // Replace alias name with its value
            char newCommand[ALIAS_VALUE_LENGTH + MAX_COMMAND_LENGTH];
            snprintf(newCommand, sizeof(newCommand), "%s%s",
                     aliases[i].value,
                     command + strlen(aliases[i].name));
            command = newCommand;
            break;
        }
    }

    char *arg = strtok(command, " ");
    while (arg != NULL)
    {
        args[argCount++] = arg;
        arg = strtok(NULL, " ");
    }
    args[argCount] = NULL;

    if (argCount == 0)
    {

        return;
    }
    if (strcmp(args[0], "iMan") == 0 && argCount > 1 && argCount < 3)
    {

        fetchManPage(args[1]);
    }
    else if (strcmp(args[0], "iMan") == 0 && argCount > 1 && argCount <= 3 && strcmp(args[2], "extra") == 0)
    {

        fetchManPage(args[1]);
    }
    else if (strcmp(args[0], "iMan") == 0)
    {
        printf("Usage: iMan <command> [extra]\n");
    }

    else if (strcmp(args[0], "mk_hop") == 0 && argCount == 2 && found_mk_hop == 1)
    {
        mkdir(args[1], 0755);
        chdir(args[1]);
    }
    else if (strcmp(args[0], "hop_seek") == 0 && argCount == 2 && found_hop_seek == 1)
    {
        chdir(args[1]);
        char cwd1[4096], cwd2[4096];
        getcwd(cwd1, sizeof(cwd1));
        seek("", args[1], cwd1);
    }
    else if (strcmp(args[0], "bg") == 0 && argCount == 2)
    {
        int pid = atoi(args[1]);
        bg_command(pid);
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        printf("Usage: bg <pid>\n");
    }
    else if (strcmp(args[0], "fg") == 0 && argCount == 2)
    {
        int pid = atoi(args[1]);
        fg_command(pid);
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        printf("Usage: fg <pid>\n");
    }

    else if (strcmp(args[0], "ping") == 0 && argCount == 3)
    {
        int pid = atoi(args[1]);
        int signal_number = atoi(args[2]);
        displayActivities();
        handlePingCommand(pid, signal_number);
    }
    else if (strcmp(args[0], "ping") == 0)
    {
        printf("Usage: ping <pid> <signal_number>\n");
    }

    else if (argCount > 0 && strcmp(args[0], "neonate") == 0 && argCount == 3 && strcmp(args[1], "-n") == 0)
    {
        fprintf(stderr, "Usage: %s -n [time_arg]\n", args[0]);
        int interval = atoi(args[2]);
        if (interval <= 0)
        {
            fprintf(stderr, "Please provide a positive integer for the time argument.\n");
            return;
        }

        printRecentPIDs(interval);
    }
    
    else if (strcmp(args[0], "activities") == 0)
    {
        displayActivities();
    }

    else if (strcmp(args[0], "seek") == 0)
    {

        char cwd1[4096], cwd2[4096];
        getcwd(cwd1, sizeof(cwd1));
        if (argCount == 2)
        {
            seek("", args[1], cwd1);
        }
        else if (args[argCount - 1][0] == '~')
        {
            char cwd2[4096];
            strcpy(cwd2, homeDir);
            // getcwd(cwd1, sizeof(cwd1));
            strcat(cwd2, args[argCount - 1] + 1);
            // printf("%s\n",cwd2);
            chdir(cwd2);
            // strcat(cwd1, args[argCount - 1] + 1);
            int f1 = 0, e1 = 0, d1 = 0;
            for (int ijn = 0; ijn < argCount; ijn++)
            {
                if (args[ijn][0] == '-')
                {
                    if (args[ijn][1] == 'f')
                    {
                        f1 = 1;
                    }
                    else if (args[ijn][1] == 'e')
                    {
                        e1 = 1;
                    }
                    else if (args[ijn][1] == 'd')
                    {
                        d1 = 1;
                    }
                }
            }
            if (f1 && d1)
            {
                printf("Invalid flags\n");
                // return;
            }
            else if (f1 == 1 && e1 == 0)
            {
                seek("-f", args[argCount - 2], cwd2);
            }
            else if (f1 == 1 && e1 == 1)
            {
                seek("-e-f", args[argCount - 2], cwd2);
                // seek("-e", args[argCount - 2], cwd1);
            }
            else if (d1 == 1 && e1 == 1)
            {
                // printf("flagds");
                seek("-e-d", args[argCount - 2], cwd2);
            }
            else if (d1 == 1 && e1 == 0)
            {
                // printf("flagds");
                seek("-d", args[argCount - 2], cwd2);
            }

            else if (e1 == 1)
            {
                seek("-e", args[argCount - 2], cwd2);
            }
            else if (e1 == 0)
            {
                seek("", args[argCount - 2], cwd2);
            }
            else
            {
                printf("Usage: seek <flags> <search> <target_directory>\n");
            }
        }
        else if (args[argCount - 1][0] == '.')
        {
            // printf("a");
            if (strncmp(args[argCount - 1], "./home", 6) == 0)
            {
                // printf("1");
                char cwd1[4096], cwd2[4096];
                strcpy(cwd1, args[argCount - 1] + 1);
                int f1 = 0, e1 = 0, d1 = 0;
                for (int ijn = 0; ijn < argCount; ijn++)
                {
                    if (args[ijn][0] == '-')
                    {
                        if (args[ijn][1] == 'f')
                        {
                            f1 = 1;
                        }
                        else if (args[ijn][1] == 'e')
                        {
                            e1 = 1;
                        }
                        else if (args[ijn][1] == 'd')
                        {
                            d1 = 1;
                        }
                    }
                }
                if (f1 && d1)
                {
                    printf("Invalid flags\n");
                    // return;
                }
                else if (f1 == 1 && e1 == 0)
                {
                    seek("-f", args[argCount - 2], cwd1);
                }
                else if (f1 == 1 && e1 == 1)
                {
                    seek("-e-f", args[argCount - 2], cwd1);
                    // seek("-e", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 1)
                {
                    // printf("flagds");
                    seek("-e-d", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 0)
                {
                    // printf("flagds");
                    seek("-d", args[argCount - 2], cwd1);
                }

                else if (e1 == 1)
                {
                    seek("-e", args[argCount - 2], cwd1);
                }
                else if (e1 == 0)
                {
                    seek("", args[argCount - 2], cwd1);
                }
                else
                {
                    printf("Usage: seek <flags> <search> <target_directory>\n");
                }
            }
            else
            {
                // printf("b");
                char cwd1[4096], cwd2[4096];
                getcwd(cwd1, sizeof(cwd1));
                strcat(cwd1, args[argCount - 1] + 1);
                int f1 = 0, e1 = 0, d1 = 0;
                for (int ijn = 0; ijn < argCount; ijn++)
                {
                    if (args[ijn][0] == '-')
                    {
                        if (args[ijn][1] == 'f')
                        {
                            f1 = 1;
                        }
                        else if (args[ijn][1] == 'e')
                        {
                            e1 = 1;
                        }
                        else if (args[ijn][1] == 'd')
                        {
                            d1 = 1;
                        }
                    }
                }
                if (f1 && d1)
                {
                    printf("Invalid flags\n");
                    // return;
                }
                else if (f1 == 1 && e1 == 0)
                {
                    seek("-f", args[argCount - 2], cwd1);
                }
                else if (f1 == 1 && e1 == 1)
                {
                    seek("-e-f", args[argCount - 2], cwd1);
                    // seek("-e", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 1)
                {
                    // printf("flagds");
                    seek("-e-d", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 0)
                {
                    // printf("flagds");
                    seek("-d", args[argCount - 2], cwd1);
                }

                else if (e1 == 1)
                {
                    seek("-e", args[argCount - 2], cwd1);
                }
                else if (e1 == 0)
                {
                    seek("", args[argCount - 2], cwd1);
                }
                else
                {
                    printf("Usage: seek <flags> <search> <target_directory>\n");
                }
            }
        }
        else
        {
            char cwd1[4096], cwd2[4096];
            getcwd(cwd1, sizeof(cwd1));
            int f1 = 0, e1 = 0, d1 = 0;
            for (int ijn = 0; ijn < argCount; ijn++)
            {
                if (args[ijn][0] == '-')
                {
                    if (args[ijn][1] == 'f')
                    {
                        f1 = 1;
                    }
                    else if (args[ijn][1] == 'e')
                    {
                        e1 = 1;
                    }
                    else if (args[ijn][1] == 'd')
                    {
                        d1 = 1;
                    }
                }
            }
            if (f1 && d1)
            {
                printf("Invalid flags\n");
            }
            else if (f1 == 1 && e1 == 0)
            {
                seek("-f", args[argCount - 1], cwd1);
            }
            else if (f1 == 1 && e1 == 1)
            {
                seek("-e-f", args[argCount - 1], cwd1);
                // seek("-e", args[argCount - 2], cwd1);
            }
            else if (d1 == 1 && e1 == 1)
            {
                // printf("flagds");
                seek("-e-d", args[argCount - 1], cwd1);
            }
            else if (d1 == 1 && e1 == 0)
            {
                // printf("flagds");
                seek("-d", args[argCount - 1], cwd1);
            }

            else if (e1 == 1)
            {
                seek("-e", args[argCount - 1], cwd1);
            }
            else if (e1 == 0)
            {
                seek("", args[argCount - 1], cwd1);
            }
            else
            {
                printf("Usage: seek <flags> <search> <target_directory>\n");
            }
        }
    }
    else if (strcmp(args[0], "proclore") == 0)
    {
        if (argCount == 1)
        {
            proclore(NULL);
        }
        else if (argCount == 2)
        {
            proclore(args[1]);
        }
        else
        {
            printf("Usage: proclore [pid]\n");
        }
    }

    else if (strcmp(args[0], "hop") == 0)
    {
        // lastCommandTime = 0;

        for (int i = 1; i < argCount; i++)
        {
            hop(args[i]);
        }
        if (argCount == 1)
        {
            hop("~");
        }
    }
    else if (strcmp(args[0], "reveal") == 0)
    {
        // lastCommandTime = 0;
        if (argCount == 1)
        {
            reveal(".", 0, 0);
        }
        else if (argCount == 2 && (args[1] == '-'))
        {
            // printf("%d\n", argCount);
            parseRevealCommand(args[1], "-");
        }
        else if (argCount == 2 && args[1][0] == '~')
        {
            parseRevealCommand(args[1] + 1, "~");
        }
        else if (argCount == 2 && strcmp(args[1], "..") == 0)
        {
            parseRevealCommand(args[1], "..");
        }
        else if (argCount >= 2)
        {
            char combinedFlags[100] = {0};
            int flagsIndex = 0;
            for (int i = 1; i <= argCount - 1; i++)
            {
                if (args[i][0] == '-')
                {
                    strcat(combinedFlags, args[i] + 1);
                }
            }
            if (args[argCount - 1][0] != '-')
            {
                // char combined_paths[4096];

                parseRevealCommand(combinedFlags, args[argCount - 1]);
            }
            else
            {
                parseRevealCommand(combinedFlags, ".");
            }
        }
        else
        {
            printf("Usage: reveal <flags> <path>\n");
        }
    }
    else if (strcmp(args[0], "log") == 0)
    {
        // lastCommandTime = 0;

        if (argCount == 1)
        {
            for (int i = 0; i < logCount; i++)
            {
                printf("%d: %s\n", i + 1, commandLog[i]);
            }
        }
        else if (argCount == 2 && strcmp(args[1], "purge") == 0)
        {
            logCount = 0;
            memset(commandLog, 0, sizeof(commandLog));
            saveCommandLog();
        }
        else if (argCount == 3 && strcmp(args[1], "execute") == 0)
        {
            int n = atoi(args[2]);
            int index = logCount - n;
            if (n <= 0 || index < 0 || index >= logCount)
            {
                printf("Invalid log index.\n");
            }
            else
            {

                char commandToExecute[4096];
                strcpy(commandToExecute, commandLog[index]);

                addCommandToLog(commandToExecute);
                parseAndExecute(commandToExecute);
            }
        }

        else
        {
            printf("Invalid log command.\n");
        }
    }
    else
    {

        struct timeval start, end;
        gettimeofday(&start, NULL);
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
        }
        else if (pid == 0)
        {

            setpgid(0, 0);
            execvp(args[0], args);
            perror("execvp failed");
            exit(1);
        }
        else
        {
            // printf("1");
            addProcess(pid, args[0], "Running"); // Add process to list
            fg_pid = pid;                        // Track the foreground process ID
            if (!background)
            {

                // printf("fbvxfgdfbvc");
                int status;
                // setpgid(pid, pid); // Set the child process group
                // tcsetpgrp(STDIN_FILENO, pid);
                waitpid(pid, &status, WUNTRACED);
                // printf("rgdfbvcx");
                // waitpid(pid, NULL, 0);

                gettimeofday(&end, NULL);

                int seconds = end.tv_sec - start.tv_sec;

                if (seconds > 2)
                {
                    strcpy(lastCommand, args[0]);
                    lastCommandTime = seconds;
                }

                // tcsetpgrp(STDIN_FILENO, getpgrp());

                remove_update(pid, status);

                fg_pid = -1;
                // tcsetpgrp(STDIN_FILENO, getpgrp());
            }
            else
            {
                num_background++;
                printf("[%d] %d\n", num_background, pid);
                fg_pid = -1;
                // foreground_pid = -1;
            }
        }
        // add_Process(pid, command, "Running");
    }
}

int main()
{

    // setup_terminal();
    signal(SIGCHLD, handleBackgroundCompletion);
    signal(SIGTSTP, handle_ctrl_z);
    signal(SIGINT, handle_ctrl_c);
    signal(SIGQUIT, handle_ctrl_d);
    getcwd(homeDir, sizeof(homeDir));
    // set_signal_action();
    getcwd(prevDir, sizeof(prevDir));
    loadMyshrc();

    loadCommandLog();
    char input[4096];
    char myshrcPath[4096];
    snprintf(myshrcPath, sizeof(myshrcPath), "%s/.myshrc", homeDir);
    FILE *file = fopen(myshrcPath, "r");
    if (!file)
    {
        perror("Failed to open .myshrc");
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = 0; // Remove newline character

        // Skip empty lines
        if (strlen(line) == 0)
            continue;

        else if (strncmp(line, "func ", 5) == 0)
        {
            // Process function definitions (this part can be expanded based on your needs)
            char *funcName = line + 5; // Skip "func "
                                       // Additional logic to handle function parsing goes here...
                                       // printf("Function found: %s\n", funcName);
            if (strcpy(funcName, "mk_hop") == 0)
                found_mk_hop = 1;
            else if (strcpy(funcName, "hop_seek") == 0)
                found_hop_seek = 1;
        }
    }

    fclose(file);

    while (1)
    {
        displayPrompt();

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            if (feof(stdin))
            {
                handle_ctrl_d(0);
            }
            continue;

            // break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0)

            continue;

        if (strcmp(input, "exit") == 0)
            return;

        int ghj = 0;
        char new_input[4096];
        strcpy(new_input, input);
        for (int i = 0; i < strlen(input); i++)
        {
            if (input[i] == 'l' && input[i + 1] == 'o' && input[i + 2] == 'g')
            {
                ghj = 1;
            }
        }
        if (ghj == 0)
            addCommandToLog(input);

        lastCommandTime = 0;
        parseAndExecute(input);
    }

    return 0;
}
