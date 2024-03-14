#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	int suma = 0;
	int i = 1;
    // Comenzamos desde el segundo argumento (índice 1) porque el primer argumento (índice 0) es el nombre del programa
        for ( i; i < argc; i++) {
                suma += atoi(argv[i]); // Convertimos la cadena a entero y sumamos
              }

    
        printf("%i\n", suma);

    // Termina la ejecución del programa.
        return(0);
    // exit(EXIT_SUCCESS);
}
