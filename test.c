#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

main(){
    int state=shmget(99, 1, 0777);
    int rv = fork();
    if(rv==0){
        //child
        char*p;
        p=shmat(state, NULL, 0);
        strcpy(p, "Y");
    }else{
        //parent
        char *p;
        p=shmat(state, NULL, 0);
        printf("%s", p);
    }

}