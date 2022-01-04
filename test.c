#include <stdio.h>
#include <unistd.h>
#include <string.h>

//
// Created by jason on 2022/1/3.
//
int main() {
    int pipefd1[2];
    int pipefd2[2];
    pipe(pipefd1);
    pipe(pipefd2);
    int father_pid;
    father_pid = getpid();
    int child1_pid;
    int child2_pid;
    child1_pid = fork();
    if(child1_pid==0){
        //child1
        printf("I am child1,and my pid is %d\n",getpid());
        close(pipefd1[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        char c1buf[1024];
        while(1){
            read(pipefd1[0],c1buf,sizeof(c1buf));
            printf("this is child1 saying, c2 said:%s\n",c1buf);

        }
    }else{
        child2_pid = fork();
        if(child2_pid==0){
            //child2
            printf("I am child2,and my pid is %d\n",getpid());
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            char c2buf[1024];
            while(1){
                scanf("%s",c2buf);
                write(pipefd2[1],c2buf,strlen(c2buf)+1);
                sleep(1);
            }
        }else{
            //father
            printf("this is father, and my pid is %d, my child1 is %d, my child2 is %d\n",getpid(),child1_pid,child2_pid);
            close(pipefd1[0]);
            close(pipefd2[1]);
            while(1){
                int len;
                char buf[1024];
                len = read(pipefd2[0],buf,sizeof (buf));
                printf("this is father saying, c2 said:%s\n",buf);
                write(pipefd1[1],buf,len);
            }
        }
    }
}
