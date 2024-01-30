#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#define PORT 8000
#define MAXCONN 5
#define MAX_FDS 3
#define TIMEOUT 1000

int main(void)
{
    //create a non blocking socket
    int sockFd;
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sockFd, F_SETFL, O_NONBLOCK);
    int optVal = 1;
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) == -1)
    {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    //bind and listen
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT); //get PORT from config file
    addr.sin_addr.s_addr = INADDR_ANY; //get IP from config file
    if (bind(sockFd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }
    if (listen(sockFd, MAXCONN) == -1)
    {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    //handle incoming connections
    struct pollfd fds[MAX_FDS];
    memset(fds, -1, sizeof(fds));
    fds[0].fd = sockFd;
    fds[0].events = POLLIN;

    while(1)
    {
        int result = poll(fds, MAX_FDS, TIMEOUT);
        printf("%d\n", result);
        if (result == 0)
            continue; //timeout occurred.
        else if (result == -1)
        {
            perror("Poll error");
            break;
        }
        //check for events on each socket in the fds
        for (int i = 0; i < MAX_FDS; i++)
        {
            printf("fd %d: revents=%d\n", i, fds[i].revents);
            if(fds[i].revents & POLLIN)
            {
                if (i == 0)
                {
                    int newSockFd = accept(sockFd, NULL, NULL);
                    if (newSockFd == -1)
                        perror("accept() error");
                    else
                    {
                        //add the new socket to the fds to poll
                        for (int j = 1; j < MAX_FDS; j++)
                        {
                            if (fds[j].fd == -1)
                            {
                                fds[j].fd = newSockFd;
                                fds[j].events = POLLIN;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    //event on other sockets means data is ready to be read
                    char buf[1024];
                    int readBytes = read(fds[i].fd, buf, sizeof(buf));
                    if (readBytes == -1)
                        perror("read() error");
                    else if(readBytes == 0 || (fds[i].revents & POLLHUP) || (fds[i].revents & POLLERR))
                    {
                        printf("here\n");
                        if (close(fds[i].fd) == -1)
                            perror("close() error");
                        fds[i].fd = -1;
                    }
                    else
                    {
                        //here can handle cgi scripts and other http requests
                        printf("Received data from client no %d: %d bytes\n", i, readBytes);
                        write(1, buf, 10);
                        printf("\n");
                    }
                    // else
                    // {
                    //     ssize_t totalWritten = 0;
                    //     while (totalWritten < readBytes)
                    //     {
                    //         ssize_t written = write(1, buf + totalWritten, readBytes);
                    //         if (written == -1)
                    //         {
                    //             perror("write() error");
                    //             break;
                    //         }
                    //         totalWritten += written;
                    //     }
                    // }
                }
            }
        }
    }
    return 0;
}