#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
 
int main(int  argc , char  * argv [])
{   
    if ( argc < 3 ) {
        fprintf ( stderr , "No hay suficientes argumentos\n" );
        exit ( EXIT_FAILURE );
    }
    kill ( atoi ( argv [ 1 ]), atoi ( argv [ 2 ]));
    exit ( EXIT_SUCCESS );
}