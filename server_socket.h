//
// Created by jason on 2022/1/3.
//

#ifndef FINAL_SERVER_SOCKET_H
#define FINAL_SERVER_SOCKET_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>

void handlefd(int fd){
    char buf[2];
    int n;
    while((n=read(fd, buf, sizeof(buf)))>0){
        printf("this time read %d byte\n",n);
        printf("%s\n",buf);
//        if(strcmp(buf,"a")==0){
//            printf("this is a!!!!\n");
//        }


    }
}
#endif //FINAL_SERVER_SOCKET_H
