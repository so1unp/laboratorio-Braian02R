#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    //	printf("hola mundo\n");

	int i=1;
	for(i; i<argc ; i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
