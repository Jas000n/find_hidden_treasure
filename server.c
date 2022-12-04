////
//// Created by Jas0n on 2021/12/8.
////
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include "server_socket.h"


int main(int ac, char *av[]) {
    int tcp_socket;
    struct sockaddr_in addr;
    int fd;
    int shm=shmget(99, 1, IPC_CREAT|0777);

    char*state;
    state=shmat(shm, NULL, 0);
    strcpy(state, "N");//whether the spot is reached by some client
    int f_x = 24;
    int f_y = 25;
    int c_x = 20;
    int c_y = 20;
    int rv_fork;


    int serve_time=0;
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(av[1]));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("cannot bind");
        exit(1);
    }

    listen(tcp_socket, 1);




    for(int i=0;i<5;i++){
        rv_fork=fork();
        if(0==rv_fork){
            //child
            break;
        }
    }
    if(rv_fork!=0){
        printf("The game is on, the treasure is at (%d,%d), has preforked 5 times!\n",f_x,f_y);

    }
    while (1) {
        //printf("%d  is serving!\n",getpid());
        fd = accept(tcp_socket, NULL, NULL);
        handlefd(fd,c_x,c_y,f_x,f_y,state);
        close(fd);
        serve_time++;
        if(serve_time>=10){
            rv_fork=fork();
            if(0==rv_fork){
                //child
                serve_time=0;
            }else{
                //parent
                exit(0);
            }
        }
    }

}