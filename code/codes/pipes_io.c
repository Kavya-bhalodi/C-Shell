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
#define REQUEST_SIZE 1024
#define BUFFER_SIZE 4096
#define MAX_CMD_LEN 1024

extern Alias aliases[MAX_ALIASES];
extern int aliasCount;

extern char commandLog[15][4096];
extern int logCount ;
extern int num_background;
extern char homeDir[4096] ;
extern char prevDir[4096] ;
extern char lastCommand[4096];
extern lastCommandTime ;
extern int found_mk_hop ;
extern int found_hop_seek ;

extern Process processList[100];
extern int processCount;
extern pid_t fg_pid;


void execute_command_redirection(char *command)
{
    char *args[MAX_CMD_LEN];
    char *input_file = NULL, *output_file = NULL;
    int append = 0;
    int i = 0;

 
    char *token = strtok(command, " ");
    while (token != NULL)
    {
        if (strcmp(token, ">") == 0)
        {
       
            token = strtok(NULL, " ");
            output_file = token;
        }
        else if (strcmp(token, ">>") == 0)
        {
          
            token = strtok(NULL, " ");
            output_file = token;
            append = 1; 
        }
        else if (strcmp(token, "<") == 0)
        {
          
            token = strtok(NULL, " ");
            input_file = token;
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[i] = NULL;


    pid_t pid = fork();
    if (pid == 0)
    {
     
        if (input_file)
        {
          
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in < 0)
            {
                perror("No such input file found!");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (output_file)
        {
           
            int fd_out = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (fd_out < 0)
            {
                perror("Output file error");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        int background = 0;
        if (strcmp(args[0], "hop") == 0)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }
            for (int i = 1; i < j; i++)
            {
                hop(newargs[i]);
            }
            if (j == 1)
            {
                hop("~");
            }
        }
        else if (strcmp(args[0], "proclore") == 0)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }
            if (j == 1)
            {
                proclore(NULL);
            }
            else if (j == 2)
            {
                proclore(newargs[1]);
            }
            else
            {
                printf("Usage: proclore [pid]\n");
            }
        }
        else if (strcmp(args[0], "log") == 0)
        {

            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }
            // lastCommandTime = 0;

            if (j == 1)
            {
                for (int i = 0; i < logCount; i++)
                {
                    printf("%d: %s\n", i + 1, commandLog[i]);
                }
            }
            else if (j == 2 && strcmp(args[1], "purge") == 0)
            {
                logCount = 0;
                memset(commandLog, 0, sizeof(commandLog));
                saveCommandLog();
            }
            else if (j == 3 && strcmp(args[1], "execute") == 0)
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
        else if (strcmp(args[0], "iMan") == 0 && i > 1)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }

            fetchManPage(newargs[1]);
        }
        else if (strcmp(args[0], "ping") == 0 && i == 3)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }
            int pid = atoi(newargs[1]);
            int signal_number = atoi(newargs[2]);
            displayActivities();
            handlePingCommand(pid, signal_number);
            // return;
        }
        else if (strcmp(args[0], "seek") == 0)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }

            char cwd1[4096], cwd2[4096];
            getcwd(cwd1, sizeof(cwd1));
            if (j == 2)
            {
                seek("", newargs[1], cwd1);
            }
            else if (newargs[j - 1][0] == '~')
            {
                char cwd2[4096];
                strcpy(cwd2, homeDir);
                // getcwd(cwd1, sizeof(cwd1));
                strcat(cwd2, newargs[j - 1] + 1);
                // printf("%s\n",cwd2);
                chdir(cwd2);
                // strcat(cwd1, args[argCount - 1] + 1);
                int f1 = 0, e1 = 0, d1 = 0;
                for (int ijn = 0; ijn < j; ijn++)
                {
                    if (newargs[ijn][0] == '-')
                    {
                        if (newargs[ijn][1] == 'f')
                        {
                            f1 = 1;
                        }
                        else if (newargs[ijn][1] == 'e')
                        {
                            e1 = 1;
                        }
                        else if (newargs[ijn][1] == 'd')
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
                    seek("-f", newargs[j - 2], cwd2);
                }
                else if (f1 == 1 && e1 == 1)
                {
                    seek("-e-f", newargs[j - 2], cwd2);
                    // seek("-e", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 1)
                {
                    // printf("flagds");
                    seek("-e-d", newargs[j - 2], cwd2);
                }
                else if (d1 == 1 && e1 == 0)
                {
                    // printf("flagds");
                    seek("-d", newargs[j - 2], cwd2);
                }

                else if (e1 == 1)
                {
                    seek("-e", newargs[j - 2], cwd2);
                }
                else if (e1 == 0)
                {
                    seek("", newargs[j - 2], cwd2);
                }
                else
                {
                    printf("Usage: seek <flags> <search> <target_directory>\n");
                }
            }
            else if (newargs[j - 1][0] == '.')
            {
                if (strncmp(newargs[j - 1], "./home", 6) == 0)
                {
                    // printf("1");
                    char cwd1[4096], cwd2[4096];
                    strcpy(cwd1, args[j - 1] + 1);
                    int f1 = 0, e1 = 0, d1 = 0;
                    for (int ijn = 0; ijn < j; ijn++)
                    {
                        if (newargs[ijn][0] == '-')
                        {
                            if (newargs[ijn][1] == 'f')
                            {
                                f1 = 1;
                            }
                            else if (newargs[ijn][1] == 'e')
                            {
                                e1 = 1;
                            }
                            else if (newargs[ijn][1] == 'd')
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
                        seek("-f", newargs[j - 2], cwd1);
                    }
                    else if (f1 == 1 && e1 == 1)
                    {
                        seek("-e-f", newargs[j - 2], cwd1);
                        // seek("-e", args[argCount - 2], cwd1);
                    }
                    else if (d1 == 1 && e1 == 1)
                    {
                        // printf("flagds");
                        seek("-e-d", newargs[j - 2], cwd1);
                    }
                    else if (d1 == 1 && e1 == 0)
                    {
                        // printf("flagds");
                        seek("-d", newargs[j - 2], cwd1);
                    }

                    else if (e1 == 1)
                    {
                        seek("-e", newargs[j - 2], cwd1);
                    }
                    else if (e1 == 0)
                    {
                        seek("", newargs[j - 2], cwd1);
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
                    strcat(cwd1, newargs[j - 1] + 1);
                    int f1 = 0, e1 = 0, d1 = 0;
                    for (int ijn = 0; ijn < j; ijn++)
                    {
                        if (newargs[ijn][0] == '-')
                        {
                            if (newargs[ijn][1] == 'f')
                            {
                                f1 = 1;
                            }
                            else if (newargs[ijn][1] == 'e')
                            {
                                e1 = 1;
                            }
                            else if (newargs[ijn][1] == 'd')
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
                        seek("-f", newargs[j - 2], cwd1);
                    }
                    else if (f1 == 1 && e1 == 1)
                    {
                        seek("-e-f", newargs[j - 2], cwd1);
                        // seek("-e", newargs[j - 2], cwd1);
                    }
                    else if (d1 == 1 && e1 == 1)
                    {
                        // printf("flagds");
                        seek("-e-d", newargs[j - 2], cwd1);
                    }
                    else if (d1 == 1 && e1 == 0)
                    {
                        // printf("flagds");
                        seek("-d", newargs[j - 2], cwd1);
                    }

                    else if (e1 == 1)
                    {
                        seek("-e", newargs[j - 2], cwd1);
                    }
                    else if (e1 == 0)
                    {
                        seek("", newargs[j - 2], cwd1);
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
                for (int ijn = 0; ijn < j; ijn++)
                {
                    if (newargs[ijn][0] == '-')
                    {
                        if (newargs[ijn][1] == 'f')
                        {
                            f1 = 1;
                        }
                        else if (newargs[ijn][1] == 'e')
                        {
                            e1 = 1;
                        }
                        else if (newargs[ijn][1] == 'd')
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
                    seek("-f", newargs[j - 1], cwd1);
                }
                else if (f1 == 1 && e1 == 1)
                {
                    seek("-e-f", newargs[j - 1], cwd1);
                    // seek("-e", args[argCount - 2], cwd1);
                }
                else if (d1 == 1 && e1 == 1)
                {
                    // printf("flagds");
                    seek("-e-d", newargs[j - 1], cwd1);
                }
                else if (d1 == 1 && e1 == 0)
                {
                    // printf("flagds");
                    seek("-d", newargs[j - 1], cwd1);
                }

                else if (e1 == 1)
                {
                    seek("-e", newargs[j - 1], cwd1);
                }
                else if (e1 == 0)
                {
                    seek("", newargs[j - 1], cwd1);
                }
                else
                {
                    printf("Usage: seek <flags> <search> <target_directory>\n");
                }
            }
        }

        else if (strcmp(args[0], "neonate") == 0 && i == 3 && strcmp(args[1], "-n") == 0)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }
            fprintf(stderr, "Usage: %s -n [time_arg]\n", newargs[0]); 
            int interval = atoi(newargs[2]);
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

        else if (strcmp(args[0], "reveal") == 0)
        {
            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }

                newargs[j++] = args[ty];
            }

            if (j == 1)
            {
                reveal(".", 0, 0);
            }
            else if (j == 2 && (newargs[1] == '-'))
            {
                // printf("%d\n", argCount);
                parseRevealCommand(newargs[1], "-");
            }
            else if (j == 2 && newargs[1][0] == '~')
            {
                parseRevealCommand(newargs[1] + 1, "~");
            }
            else if (j == 2 && strcmp(newargs[1], "..") == 0)
            {
                parseRevealCommand(newargs[1], "..");
            }
            else if (j >= 2)
            {
                char combinedFlags[100] = {0};
                int flagsIndex = 0;
                for (int i = 1; i <= j - 1; i++)
                {
                    if (newargs[i][0] == '-')
                    {
                        strcat(combinedFlags, newargs[i] + 1);
                    }
                }
                if (newargs[j - 1][0] != '-')
                {
                    // char combined_paths[4096];

                    parseRevealCommand(combinedFlags, newargs[j - 1]);
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

            // reveal(".",0,0);
            exit(EXIT_SUCCESS);
        }

        else
        {

            char *newargs[1024];
            int j = 0;
            for (int ty = 0; ty < i; ty++)
            {
                if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
                {
                    ty++;
                    continue;
                }
                if (strcmp(args[ty], "&") == 0)
                {
                    background = 1;
                    // printf("1");
                    continue;
                }

                newargs[j++] = args[ty];
            }
            // for (int yui = 0; yui < j; yui++)
            // {
            //     printf("%s\n", newargs[yui]);
            // }
            struct timeval start, end;
            gettimeofday(&start, NULL);

            pid_t pid1 = fork();
            if (pid1 < 0)
            {
                perror("fork failed");
            }
            else if (pid1 == 0)
            {

                setpgid(0, 0);
                execvp(newargs[0], newargs);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
            else
            {

                addProcess(pid1, newargs[0], "Running"); // Add process to list
                fg_pid = pid1;
                if (!background)
                {
                    int status;

                    waitpid(pid1, &status, WUNTRACED);

                    gettimeofday(&end, NULL);

                    int seconds = end.tv_sec - start.tv_sec;

                    if (seconds > 2)
                    {
                        strcpy(lastCommand, newargs[0]);
                        lastCommandTime = seconds;
                    }

                    // remove_update(pid1, status);

                    fg_pid = -1;
                    // tcsetpgrp(STDIN_FILENO, getpgrp());
                }
                else
                {
                    num_background++;
                    printf("[%d] %d\n", num_background, pid1);
                    fg_pid = -1;
                    // foreground_pid = -1;
                }
            }

            // Execute the command
            // setpgid(0, 0);
            // execvp(newargs[0], newargs);
            // perror("Execution failed");
            // exit(EXIT_FAILURE);
        }
    }
    else if (pid > 0)
    {
      
        waitpid(pid, NULL, 0);
    }
    else
    {
       
        perror("Fork failed");
    }
}

void jmp_to_pipes(char *commands)
{

    // printf("1");
    char *cmds[MAX_CMD_LEN];
    int i = 0, jk = 0;

    char asd[4096];
    char *as[4096];
    strcpy(asd, commands);
    // printf("%s\n", asd);
    char *t = strtok(asd, " ");
    while (t != NULL)
    {
        as[jk++] = t;
        // strcat(as, t);
        // printf("%s\n", as);
        t = strtok(NULL, " ");
    }
    as[jk] = NULL;
    if (strcmp(as[0], "|") == 0 || strcmp(as[jk - 1], "|") == 0)
    {
        printf("Invalid command\n");
        return;
    }
    for (int tu = 0; tu < jk - 1; tu++)
    {
        // printf("%s %s\n", as[tu]);
        if (strcmp(as[tu], "&") == 0 && strcmp(as[tu + 1], "|") == 0)
        {
            printf("Invalid command\n");
            return;
        }
        if (strcmp(as[tu], "|") == 0 && strcmp(as[tu + 1], "&") == 0)
        {
            printf("Invalid command\n");
            return;
        }
        if (strcmp(as[tu], "|") == 0 && strcmp(as[tu + 1], "|") == 0)
        {
            printf("Invalid command\n");
            return;
        }
    }
    // printf("%s\n",commands);

    char *token = strtok(commands, "|");
    while (token != NULL)
    {
        cmds[i++] = token;
        token = strtok(NULL, "|");
    }
    cmds[i] = NULL;

    int num_cmds = i;
    int pipefds[2 * (num_cmds - 1)];

    for (i = 0; i < num_cmds - 1; i++)
    {
        if (pipe(pipefds + 2 * i) < 0)
        {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    int cmd_idx = 0;
    while (cmds[cmd_idx] != NULL)
    {
        pid_t pid = fork();
        if (pid == 0)
        {

            if (cmd_idx > 0)
            {

                dup2(pipefds[2 * (cmd_idx - 1)], STDIN_FILENO);
            }
            if (cmds[cmd_idx + 1] != NULL)
            {

                dup2(pipefds[2 * cmd_idx + 1], STDOUT_FILENO);
            }

            for (i = 0; i < 2 * (num_cmds - 1); i++)
            {
                close(pipefds[i]);
            }

            execute_command_redirection(cmds[cmd_idx]);
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {

            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        cmd_idx++;
    }

    for (i = 0; i < 2 * (num_cmds - 1); i++)
    {
        close(pipefds[i]);
    }

    for (i = 0; i < num_cmds; i++)
    {
        wait(NULL);
    }
}

void IO_redirection(char *command)
{
    if (strncmp(command, "reveal", 6) == 0 || strncmp(command, "proclore", 7) == 0 || strncmp(command, "seek", 4) == 0 || strncmp(command, "fg", 2) == 0 || strncmp(command, "bg", 2) == 0 || strncmp(command, "log", 3) == 0 || strncmp(command, "proclore", 7) == 0 || strncmp(command, "iMan", 4) == 0 || strncmp(command, "ping", 4) == 0 || strncmp(command, "neonate", 7) == 0 || strncmp(command, "activities", 10) == 0)
    {
        jmp_to_pipes(command);
        return;
    }

    // printf("3");
    char *args[MAX_CMD_LEN];
    char *input_file = NULL, *output_file = NULL;
    int append = 0; 
    int i = 0;

 
    char *token = strtok(command, " ");
    while (token != NULL)
    {
        if (strcmp(token, ">") == 0)
        {
         
            token = strtok(NULL, " ");
            output_file = token;
        }
        else if (strcmp(token, ">>") == 0)
        {
          
            token = strtok(NULL, " ");
            output_file = token;
            append = 1;
        }
        else if (strcmp(token, "<") == 0)
        {
          
            token = strtok(NULL, " ");
            input_file = token;
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[i] = NULL; 

    if (strcmp(args[0], "hop") == 0)
    {
        char *newargs[MAX_CMD_LEN];
        int j = 0;
        for (int ty = 0; ty < i; ty++)
        {
            if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
            {
                ty++;
                continue;
            }
            newargs[j++] = args[ty];
        }
        newargs[j] = NULL;
 
        // printf("2");


        int fd_out = -1;
        int saved_stdout = dup(STDOUT_FILENO); 
        if (output_file)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
            if (fd_out < 0)
            {
                perror("Output file error");
                exit(EXIT_SUCCESS);
            }
        }

        if (fd_out != -1)
        {
            dup2(fd_out, STDOUT_FILENO); 
            close(fd_out);
        }

  
        if (j == 0)
        {
            hop("~"); 
        }
        else
        {
            for (int k = 0; k < j; k++)
            {
                hop(newargs[k]);
            }
        }

        // printf("1");
        if (fd_out != -1)
        {
            dup2(saved_stdout, STDOUT_FILENO); 
            close(saved_stdout);
        }

        return; 
    }
    else
    {
        char *newargs[MAX_CMD_LEN];
        int j = 0, background = 0;
        for (int ty = 0; ty < i; ty++)
        {
            if (strcmp(args[ty], ">") == 0 || strcmp(args[ty], ">>") == 0 || strcmp(args[ty], "<") == 0)
            {
                ty++;
                continue;
            }
            else if (strcmp(args[ty], "&") == 0)
            {
                background = 1;
                continue;
            }
            newargs[j++] = args[ty];
        }
        newargs[j] = NULL;

       
        struct timeval start, end;
        gettimeofday(&start, NULL);
        pid_t pid = fork();
        if (pid == 0)
        {
            if (input_file)
            {
              
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0)
                {
                    perror("No such input file found!");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            if (output_file)
            {
              
                int fd_out = open(output_file, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0644);
                if (fd_out < 0)
                {
                    perror("Output file error");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            setpgid(0, 0);
            execvp(newargs[0], newargs);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            // wait(NULL);
            addProcess(pid, newargs[0], "Running"); 
            fg_pid = pid;                           
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
    }
}
