//
// Created by jason on 2022/1/3.
//

#ifndef FINAL_CLIENT_SOCKET_H
#define FINAL_CLIENT_SOCKET_H

#include <signal.h>
#include <string.h>
#include <fcntl.h>

void forward(int server_fd, int pipe_fd, int child_pid)
{
    char buf[2];
    int n;
    int success;
//    int pid =getpid();
    int hand_shake[2];
    hand_shake[0] = child_pid;

    write(server_fd,hand_shake,4);
    while(1){
        n = read(pipe_fd,buf,sizeof (buf));
        success = write(server_fd, buf, n);
        }
    }



int connect_server(char *av[])//return fd
{
    int tcp_socket;
    struct sockaddr_in addr;
    int fd;
    char buf[512];
    int n;

    tcp_socket  =  socket(AF_INET,  SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(av[2]));
    addr.sin_addr.s_addr = inet_addr(av[1]);

    if(connect(tcp_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in))==-1){
        perror("cannot connect");
        exit(1);
    }
    return tcp_socket;
}
#endif //FINAL_CLIENT_SOCKET_H
