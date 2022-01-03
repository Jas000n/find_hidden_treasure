//
// Created by jason on 2022/1/3.
//

#ifndef CLIENT_FCLIENT_H
#define CLIENT_FCLIENT_H

#include<curses.h>
#include<unistd.h>
#define	MESSAGE	"O"
#define BLANK   "  "

void screen(int pipe_fd)
{
    char buf[1];
    int x=20;
    int y=20;
    int c;
    initscr();
    crmode();
    noecho();
    clear();
    move(y,x);
    addstr(MESSAGE);
    while(1){
        c = getch();

        if(c =='a'){
            x--;
            move(y,x);
            addstr(BLANK);
            move(y,x);
            addstr(MESSAGE);
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
            addstr(MESSAGE);
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
            addstr(MESSAGE);
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
            addstr(MESSAGE);
            buf[0] = c;
            write(pipe_fd,buf,sizeof(buf));
            refresh();
        }
    }
    endwin();
}

#endif //CLIENT_FCLIENT_H
