//
// Created by jason on 2022/1/3.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "client_screen.h"
#include "client_socket.h"

int main(int ac, char *av[]){

    if(ac!=3){
        fprintf(stderr, "usage: %s <server_ip> <port>\n", av[0]);
        exit(1);
    }
    //connect before fork, so a failed connection does not leave the game screen behind
    int fd = connect_server(av);
    int pipefds[2];
    if(pipe(pipefds)==-1){
        perror("cannot create pipe");
        exit(1);
    }
    int rv = fork();
    if(-1==rv){
        perror("cannot fork!");
        exit(1);
    }
    if(rv==0){
        //child
        close(pipefds[0]);
        close(fd);
        screen(pipefds[1]);

    }else{
        //parent
        close(pipefds[1]);

        forward(fd,pipefds[0],rv);
    }
    return 0;
}
