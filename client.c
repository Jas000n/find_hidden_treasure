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
#include <pthread.h>
#include "client_screen.h"
#include "client_socket.h"

int main(int ac, char *av[]){
    int pipefds[2];
    pipe(pipefds);
    int rv = fork();
    if(-1==rv){ perror("cannot fork!");}
    if(rv==0){
        //child
        close(pipefds[1]);
        int fd = connect_server(av);
        forward(fd,pipefds[0]);

    }else{
        //parent
        close(pipefds[0]);
        screen(pipefds[1]);
    }

}