/*
 * Invoca la llamada al sistema pscnt();
 */
#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  // Descomentar una vez implementada la llamada al sistema.

  int pid = fork();

    if (pid < 0)
    {
        printf(1, "Error al crear el proceso hijo.\n");
    }
    else if (pid == 0)
    {
        // Código para el proceso hijo
        printf(1, "¡Hola desde el proceso hijo! PID: %d\n", getpid());
    }
    else
    {
        // Código para el proceso padre
        printf(1, "¡Hola desde el proceso padre! PID del hijo: %d\n", pid);

        // Llamada al sistema pscnt() para obtener el número de procesos en el sistema
        int numProcesos = pscnt();
        printf(1, "Número total de procesos en el sistema: %d\n", numProcesos);
        wait();
    }

    exit();
}