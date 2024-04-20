#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
 
void my_handler(int sig){
    printf("señal recibida: %d (%s)\n", sig, strsignal(sig));
}


int main(void)
{
    int i = 1;
    for(i; i < 65; i++){
        signal(i, my_handler);
    }
    //signal(15, my_handler);

    printf("%d\n",getpid());

    printf("esperando señales...");

    while (1)
    {
        pause();
    }
    

    exit(EXIT_SUCCESS);
}
