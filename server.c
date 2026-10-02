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
#include <errno.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "server_socket.h"

#define OPERATORS 5//number of preforked operators
#define MAX_SERVE 10//an operator retires after serving this many clients
#define START_X 20
#define START_Y 20
#define TREASURE_X 24
#define TREASURE_Y 25

void operate(int tcp_socket, struct game *game) {
    int fd;
    int serve_time = 0;
    while (serve_time < MAX_SERVE) {
        //printf("%d  is serving!\n",getpid());
        fd = accept(tcp_socket, NULL, NULL);
        if (fd == -1) {
            perror("cannot accept");
            continue;
        }
        handlefd(fd, START_X, START_Y, TREASURE_X, TREASURE_Y, game);
        close(fd);
        serve_time++;
    }
    exit(0);
}

void spawn_operator(int tcp_socket, struct game *game) {
    int rv_fork;
    while ((rv_fork = fork()) == -1) {
        perror("cannot fork an operator, retry in 1 second");
        sleep(1);
    }
    if (0 == rv_fork) {
        //child
        operate(tcp_socket, game);
    }
}

int main(int ac, char *av[]) {
    int tcp_socket;
    struct sockaddr_in addr;
    int port;
    int on = 1;
    int shm;
    struct game *game;

    if (ac != 2) {
        fprintf(stderr, "usage: %s <port>\n", av[0]);
        exit(1);
    }
    port = atoi(av[1]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "invalid port: %s\n", av[1]);
        exit(1);
    }
    //operators inherit stdout, flush every line so no half-filled buffer gets copied by fork
    setvbuf(stdout, NULL, _IOLBF, 0);
    //a client hanging up must not kill its operator
    signal(SIGPIPE, SIG_IGN);

    shm = shmget(IPC_PRIVATE, sizeof(struct game), IPC_CREAT | 0600);
    if (shm == -1) {
        perror("cannot create shared memory");
        exit(1);
    }
    game = shmat(shm, NULL, 0);
    if (game == (void *) -1) {
        perror("cannot attach shared memory");
        exit(1);
    }
    //operators inherit the attachment, the segment is freed once all of them have exited
    shmctl(shm, IPC_RMID, NULL);
    if (sem_init(&game->mutex, 1, 1) == -1) {
        perror("cannot init semaphore");
        exit(1);
    }
    game->round = 0;

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        perror("cannot create socket");
        exit(1);
    }
    setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_socket, (const struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("cannot bind");
        exit(1);
    }

    if (listen(tcp_socket, 16) == -1) {
        perror("cannot listen");
        exit(1);
    }

    for (int i = 0; i < OPERATORS; i++) {
        spawn_operator(tcp_socket, game);
    }
    printf("The game is on, the treasure is at (%d,%d), has preforked %d operators!\n", TREASURE_X, TREASURE_Y,
           OPERATORS);

    //reap each retired operator and fork a new one to keep the number of operators
    while (1) {
        if (wait(NULL) == -1) {
            if (errno != EINTR) {
                perror("wait");
                sleep(1);
            }
            continue;
        }
        spawn_operator(tcp_socket, game);
    }

}
