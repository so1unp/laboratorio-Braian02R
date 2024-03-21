#include <stdio.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 20

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    int word_lengths[MAX_WORD_LENGTH + 1] = {0}; // inicializa un array para guardar la longitud de las palabras

    printf("Ingrese el texto (Ctrl+D para finalizar):\n");

    int c;
    int longitud_actual = 0;
    while ((c = getchar()) != EOF) {
        if (isalpha(c)) {
            // si el caracter es una letra, incrementa la longitud de la palabra actual
            longitud_actual++;
        } else if (longitud_actual > 0) {
            // Si el carácter no es una letra y tenemos una longitud de palabra distinta de cero,
            // actualiza la entrada del histograma correspondiente
            if (longitud_actual <= MAX_WORD_LENGTH) {
                word_lengths[longitud_actual]++;
            }
            longitud_actual = 0; // Restablece la longitud de la palabra
        }
    }

    // Print el histograma

    printf("\nHistograma:\n");
    int i = 1;
    for (i ; i <= MAX_WORD_LENGTH; i++) {
        printf("%d ", i);
        int j = 0;
        for (j ; j < word_lengths[i]; j++) {
            printf("*");
        }
        printf("\n");
    }	

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
