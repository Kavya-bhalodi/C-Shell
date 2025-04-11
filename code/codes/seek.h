#ifndef SEEK_H
#define SEEK_H

// Ensuring safe usage of standard and custom headers
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
#include <sys/time.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>

#define MAX_PROCESSES 100

#define MAX_COMMAND_LENGTH 4096
#define MAX_ARGS 100
#define MAX_ALIASES 20
#define ALIAS_NAME_LENGTH 50
#define ALIAS_VALUE_LENGTH 4096
// void colour_print(const char *path, int is_dir);
void colour_print(const char *path, const char *base_path, int is_dir);

int name_matches(const char *filename, const char *target);
void search( char *target, char *base_path, int look_for_files, int look_for_dirs, int *found, char *matched_path, int *is_single_type,char *target_dir);

// void search(const char *target, const char *base_path, int look_for_files, int look_for_dirs, int *found, char *matched_path, int *is_single_type);

void seek(const char *flags, const char *target, const char *target_dir);

#endif 
