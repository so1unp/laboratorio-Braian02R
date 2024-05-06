#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_fork(int count);
void test_thread(int count);

int main(int argc, char *argv[]) 
{
    int modo, count;
    
    // Chequea los  parametros
    if (argc < 3) {
        fprintf(stderr, "Uso: %s [-p | -h] cantidad\n", argv[0]);
        fprintf(stderr, "\t-p           Crea procesos.\n");
        fprintf(stderr, "\t-t           Crea hilos.\n");
        fprintf(stderr, "\tcantidad     Número de procesos o hilos a generar.\n");
        exit(EXIT_FAILURE);
    }
    
    modo = argv[1][1];
    count = atoi(argv[2]);
    
    if (count <= 0) {
        fprintf(stderr, "Error: el contador debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (modo != 'p' && modo != 't') {
        fprintf(stderr, "Error: opción invalida %s.\nUsar -p (procesos) o -t (hilos)\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    if (modo == 'p') {
        printf("Probando fork()...\n");
        test_fork(count);
    } else if (modo == 't') {
        printf("Probando pthread_create()...\n");
        test_thread(count);
    }

    exit(EXIT_SUCCESS);
}

//==========================================================================
// Código para la prueba con fork()
//==========================================================================
void test_fork(int count)
{
    pid_t pid;
    int j, status;

    for (j = 0; j < count; j++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(0);
        }        
        else if (pid == 0) {
            /* Hijo */
            exit(EXIT_SUCCESS);
        }        
        else {
            /* Padre - espera por el hijo */
            waitpid(pid, &status, 0);
        }
    }
}

//==========================================================================
// Código para la prueba con pthread_create()
//==========================================================================
void *thread_exit(void *threadId) {
    //long tid = (long)threadId;
    //printf("Hola desde el hilo %ld\n", tid);
    pthread_exit(NULL);
}

void test_thread(int count) {
    int rc;
    pthread_t threads[count];

    long t;
    for (t = 0; t < count; t++) {
        //printf("En main: creando hilo %ld\n", t);
        rc = pthread_create(&threads[t], NULL, thread_exit, (void *)t);
        if (rc) {
            printf("ERROR; código de retorno de pthread_create() es %d\n", rc);
            exit(-1);
        }
    }

    // Esperar a que los hilos terminen
    long k;
    for (k = 0; k < count; k++) {
        rc = pthread_join(threads[t], NULL);
        if (rc) {
            printf("ERROR; código de retorno de pthread_join() es %d\n", rc);
            exit(-1);
        }
    }
}
