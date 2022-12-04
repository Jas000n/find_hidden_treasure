//
// Created by jason on 2022/1/3.
//

#ifndef CLIENT_FCLIENT_H
#define CLIENT_FCLIENT_H

#include<curses.h>
#include<unistd.h>
#include <signal.h>

#define	BALL	"O"
#define BLANK   "  "
#define V_END "You found the hidden treasure! You win the game!\n\t\t\t\tOther players only have one move left!\n\t\t\t\tThis game is finished!\n\t\t\t\tPress CTRL + C to exit!"
#define L_END "You miss your last shot! Other client found the hidden treasure before you!\n\t\t\t\tThis game is finished!\n\t\t\t\tPress CTRL + C to exit!"
void v_f(){
    initscr();
    crmode();
    noecho();
    clear();
    move(20,20);
    addstr(V_END);
    refresh();
}
void l_f(){
    initscr();
    crmode();
    noecho();
    clear();
    move(20,20);
    addstr(L_END);
    refresh();
}
void notice(){
    initscr();
    crmode();
    noecho();
    clear();
    move(0,0);
    char notice[]="You need to control you ball with : w,a,s,d\n"
                  "Explore the world (the console) to find a hidden treasure\n"
                  "if other player find it before you, you lose\n"
                  "this notice will exist 10 seconds, and the game will begin!";
    addstr(notice);
    refresh();
}
void screen(int pipe_fd)
{
    signal(2,v_f);
    signal(3,l_f);
    char buf[1];
    int x=20;
    int y=20;
    int c;
    notice();
    sleep(10);

    clear();
    move(y,x);
    addstr(BALL);
    while(1){
        c = getch();

        if(c =='a'){
            x--;
            move(y,x);
            addstr(BLANK);
            move(y,x);
            addstr(BALL);
            buf[0] = c;
            write(pipe_fd,buf,sizeof(buf));
            refresh();
        }
        if(c =='w'){
            x--;
            move(y,x);
            addstr(BLANK);
            x++;
            y--;
            move(y,x);
            addstr(BALL);
            buf[0] = c;
            write(pipe_fd,buf,sizeof(buf));
            refresh();
        }
        if(c == 's'){
            x--;
            move(y,x);
            addstr(BLANK);
            x++;
            y++;
            move(y,x);
            addstr(BALL);
            buf[0] = c;
            write(pipe_fd,buf,sizeof(buf));
            refresh();
        }
        if(c == 'd'){
            x--;
            move(y,x);
            addstr(BLANK);
            x++;
            x++;
            move(y,x);
            addstr(BALL);
            buf[0] = c;
            write(pipe_fd,buf,sizeof(buf));
            refresh();
        }
    }
    endwin();
}

#endif //CLIENT_FCLIENT_H
