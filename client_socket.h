//
// Created by jason on 2022/1/3.
//

#ifndef FINAL_CLIENT_SOCKET_H
#define FINAL_CLIENT_SOCKET_H

#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

void forward(int server_fd, int pipe_fd, int child_pid)
{
    char buf[1];
    char reply;
    int over = 0;
    //Ctrl+C is handled by the game process, we leave once it has closed the pipe
    signal(SIGINT, SIG_IGN);
    //a dead server must show up as a failed write, not kill us silently
    signal(SIGPIPE, SIG_IGN);
    while(read(pipe_fd,buf,sizeof (buf))==1){
        if(over){
            continue;//game finished, drop keys until the game process exits
        }
        if(write(server_fd, buf, 1)!=1 || read(server_fd, &reply, 1)!=1){
            kill(child_pid, SIGTERM);
            waitpid(child_pid, NULL, 0);
            fprintf(stderr, "lost connection to the server\n");
            exit(1);
        }
        if(reply=='W'){
            kill(child_pid, SIGUSR1);
            over = 1;
        }else if(reply=='L'){
            kill(child_pid, SIGUSR2);
            over = 1;
        }
    }
    close(server_fd);
    waitpid(child_pid, NULL, 0);
}



int connect_server(char *av[])//return fd
{
    int tcp_socket;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(av[2]));
    if(inet_pton(AF_INET, av[1], &addr.sin_addr)!=1){
        fprintf(stderr, "invalid server address: %s\n", av[1]);
        exit(1);
    }

    tcp_socket  =  socket(AF_INET,  SOCK_STREAM, 0);
    if(tcp_socket==-1){
        perror("cannot create socket");
        exit(1);
    }

    if(connect(tcp_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in))==-1){
        perror("cannot connect");
        exit(1);
    }
    return tcp_socket;
}
#endif //FINAL_CLIENT_SOCKET_H
