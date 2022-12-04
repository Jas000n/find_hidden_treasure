//
// Created by jason on 2022/1/3.
//


#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

int arrive(int c_x, int c_y,int f_x,int f_y){
    int end = 0;
    if((c_x==f_x)&&(c_y==f_y)){
        end = 1;
    }
    printf("c_x =%d,c_y=%d,f_x=%d,f_y=%d\n",c_x,c_y,f_x,f_y);
    return end;
}
void calculate_result(int myend,char * state,int client_pid)
//after each step, compare my current result and other client
{
    sem_t *mutex;
    if((mutex = sem_open("mutexsem", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("unable to create semaphore");
        sem_unlink("mutexsem");
        exit(1);
    }
    if(myend==1){
        kill(client_pid,2);
        sem_wait(mutex);
        strcpy(state, "Y");
        sem_post(mutex);
        printf("%d client has reached the spot!\n",client_pid);
    }else{

        if(strcmp(state,"Y")==0){
            printf("Other player find it, you lose!");
            kill(client_pid,3);
        }else{
            printf("It is not the spot,keep going!\n");
        }
    }
}
void handlefd(int fd,int c_x,int c_y,int f_x,int f_y,char * state){
    char buf[2];
    int n;
    int end;//state of current game
    int client[2];//pid of client
    int s = read(fd,client,sizeof (client));
    if(s==-1){
        perror("something went wrong!");
    }
    while((n=read(fd, buf, sizeof(buf)))>0){
        printf("server %d is serve client %d\n",getpid(),client[0]);
        printf("%s\n",buf);
        if(strcmp(buf,"a")==0){
            printf("move left\n");
            c_x--;
            end = arrive(c_x,c_y,f_x,f_y);
            calculate_result(end,state,client[0]);
        }
        if(strcmp(buf,"w")==0){
            printf("move up!\n");
            c_y--;
            end = arrive(c_x,c_y,f_x,f_y);
            calculate_result(end,state,client[0]);

        }
        if(strcmp(buf,"s")==0){
            printf("move down!\n");
            c_y++;
            end = arrive(c_x,c_y,f_x,f_y);
            calculate_result(end,state,client[0]);

        }
        if(strcmp(buf,"d")==0){
            printf("move right\n");
            c_x++;
            end = arrive(c_x,c_y,f_x,f_y);
            calculate_result(end,state,client[0]);

        }
        printf("=========================================\n");

    }
}

