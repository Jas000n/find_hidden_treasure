#include <stdio.h>

//
// Created by jason on 2022/1/3.
//
int main(){
    int m =100;
    int * a = &m;
    int b = 2*(*a);
    printf("%d",b);

    int hello(){
        printf("hello");
        return 1;
    }
    int i =hello();
}
