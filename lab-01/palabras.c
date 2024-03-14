#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	char  c;
	//c = getchar();
	while((c = getchar()) != EOF){
		if(c == '\t' || c == ' ' || c =='\n'){
                	printf("\n");
      		}else{
			putchar(c);
		}	
	
	}
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
