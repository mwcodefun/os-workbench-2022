#include <co.h>
#include <stdio.h>

void work(void *arg){
    for(int i =0;i< 10;i++){
        printf("%s%d \n",(char *)arg,i);
    }
}


int main(){
    struct co *p = co_start("work",work,"X");
    co_wait(p);
    printf("hello finish\n");

}