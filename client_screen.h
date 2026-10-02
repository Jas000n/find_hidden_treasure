//
// Created by jason on 2022/1/3.
//

#ifndef CLIENT_FCLIENT_H
#define CLIENT_FCLIENT_H

#include<curses.h>
#include<unistd.h>
#include <signal.h>

#define	BALL	"O"
#define BLANK   " "
#define V_END "You found the hidden treasure! You win the game!\n\t\t\t\tOther players only have one move left!\n\t\t\t\tThis game is finished!\n\t\t\t\tPress any key to exit!"
#define L_END "You miss your last shot! Other client found the hidden treasure before you!\n\t\t\t\tThis game is finished!\n\t\t\t\tPress any key to exit!"

// set by signal handlers, everything else happens in the main loop
volatile sig_atomic_t game_result = 0;//'W' for win, 'L' for lose
volatile sig_atomic_t want_quit = 0;

void on_win(int sig){
    (void) sig;
    game_result = 'W';
}
void on_lose(int sig){
    (void) sig;
    game_result = 'L';
}
void on_quit(int sig){
    (void) sig;
    want_quit = 1;
}
void show_end(const char *msg){
    clear();
    move(20,20);
    addstr(msg);
    refresh();
    flushinp();
    while(!want_quit && getch()==ERR);//wait for any key
}
void notice(){
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
    signal(SIGUSR1,on_win);
    signal(SIGUSR2,on_lose);
    signal(SIGINT,on_quit);
    signal(SIGTERM,on_quit);
    //if the socket process is gone, write fails and we leave the loop instead of dying
    signal(SIGPIPE,SIG_IGN);
    char buf[1];
    int x=20;
    int y=20;
    int new_x;
    int new_y;
    int c;
    initscr();
    crmode();
    noecho();
    notice();
    sleep(10);

    clear();
    move(y,x);
    addstr(BALL);
    refresh();
    timeout(100);//getch returns every 100ms so the flags above are noticed
    while(!want_quit && !game_result){
        c = getch();
        new_x = x;
        new_y = y;
        if(c =='a'){
            new_x--;
        }else if(c =='w'){
            new_y--;
        }else if(c == 's'){
            new_y++;
        }else if(c == 'd'){
            new_x++;
        }else{
            continue;
        }
        //stay inside the console, the move is not sent so the server stays in sync
        if(new_x<0 || new_x>=COLS || new_y<0 || new_y>=LINES){
            continue;
        }
        buf[0] = c;
        if(write(pipe_fd,buf,sizeof(buf))!=1){
            break;
        }
        move(y,x);
        addstr(BLANK);
        x = new_x;
        y = new_y;
        move(y,x);
        addstr(BALL);
        refresh();
    }
    if(!want_quit && game_result){
        show_end(game_result=='W' ? V_END : L_END);
    }
    endwin();
}

#endif //CLIENT_FCLIENT_H
