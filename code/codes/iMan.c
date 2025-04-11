#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include"iMan.h"

#define BUFFER_SIZE 4096
#define PORT 80


void fetchManPage(const char *command_name)
{
    int sock;
    struct sockaddr_in server;
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    char *host = "man.he.net";
    struct hostent *he;

   
    he = gethostbyname(host);
    if (he == NULL)
    {
        perror("gethostbyname failed");
        return;
    }


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Could not create socket");
        return;
    }

  
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    memcpy(&server.sin_addr, he->h_addr, he->h_length);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        close(sock);
        return;
    }

    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Mozilla/5.0 (compatible; iMan/1.0)\r\n"
             "Accept: text/html\r\n"
             "Connection: close\r\n\r\n",
             command_name, host);


    if (send(sock, request, strlen(request), 0) < 0)
    {
        perror("Send failed");
        close(sock);
        return;
    }


    int received;
    int header_end = 0;
    int is_in_tag = 0;


    int df = 0;
    while ((received = recv(sock, response, sizeof(response) - 1, 0)) > 0)
    {
        response[received] = '\0';

 
        for (int i = 0; i < received; i++)
        {
            if (df < 200)
            {
                df++;
                continue;
            }
            if (response[i] == '<')
            {
                is_in_tag = 1;
            }
            else if (response[i] == '>')
            {
                is_in_tag = 0;
                continue;
            }

            if (!is_in_tag)
            {
                putchar(response[i]);
            }
        }
    }

    if (received < 0)
    {
        perror("Receive failed");
    }
    else if (received == 0)
    {
        printf("Connection closed by the server.\n");
    }

    close(sock);
}
