#ifndef IMAN_H
#define IMAN_H

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

void fetchManPage(const char *command);

#endif