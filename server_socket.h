//
// Created by jason on 2022/1/3.
//


#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>

// replies sent to the client after each move
#define GOING_ON 'C'
#define WIN      'W'
#define LOSE     'L'

struct game //lives in shared memory, seen by every operator
{
    sem_t mutex;
    int round;//bumped each time someone finds the treasure, which starts a new game
};

int arrive(int c_x, int c_y,int f_x,int f_y){
    int end = 0;
    if((c_x==f_x)&&(c_y==f_y)){
        end = 1;
    }
    printf("c_x =%d,c_y=%d,f_x=%d,f_y=%d\n",c_x,c_y,f_x,f_y);
    return end;
}
char calculate_result(int myend,struct game *game,int my_round)
//after each step, compare my current result and other client
{
    char result;
    //check and update under one lock, so only the first player to reach the spot wins
    sem_wait(&game->mutex);
    if(game->round!=my_round){
        result = LOSE;
    }else if(myend==1){
        game->round++;
        result = WIN;
    }else{
        result = GOING_ON;
    }
    sem_post(&game->mutex);
    return result;
}
void handlefd(int fd,int c_x,int c_y,int f_x,int f_y,struct game *game){
    char c;
    char result;
    int my_round;
    sem_wait(&game->mutex);
    my_round = game->round;
    sem_post(&game->mutex);
    //one byte per move, so keys that arrive together are never lost
    while(read(fd, &c, 1)==1){
        printf("server %d is serving a client in round %d\n",getpid(),my_round);
        printf("%c\n",c);
        switch(c){
            case 'a':
                printf("move left\n");
                c_x--;
                break;
            case 'w':
                printf("move up!\n");
                c_y--;
                break;
            case 's':
                printf("move down!\n");
                c_y++;
                break;
            case 'd':
                printf("move right\n");
                c_x++;
                break;
            default:
                continue;//not a move
        }
        result = calculate_result(arrive(c_x,c_y,f_x,f_y),game,my_round);
        if(result==WIN){
            printf("%d's client has reached the spot!\n",getpid());
        }else if(result==LOSE){
            printf("Other player find it, you lose!\n");
        }else{
            printf("It is not the spot,keep going!\n");
        }
        printf("=========================================\n");
        if(write(fd,&result,1)!=1){
            perror("cannot reply to client");
            return;
        }
        if(result!=GOING_ON){
            return;//this client's game is over
        }
    }
}
