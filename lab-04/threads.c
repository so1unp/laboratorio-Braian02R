#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Para la función sleep()

// Estructura para pasar parámetros al hilo
struct ThreadParams {
    int id;
    int max_sleep_seconds;
};

// Función que se ejecutará en cada hilo
void *ThreadFunction(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *)arg;
    int sleep_seconds = rand() % (params->max_sleep_seconds + 1); // Número aleatorio de segundos
    printf("Hilo %d: dormiré %d segundos\n", params->id, sleep_seconds);
    sleep(sleep_seconds); // Esperar
    pthread_exit((void *)(intptr_t)sleep_seconds); // Retornar el tiempo de espera
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <número de hilos> <segundos máximos de espera>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int max_sleep_seconds = atoi(argv[2]);

    pthread_t threads[num_threads];
    struct ThreadParams params[num_threads];

    // Crear hilos
    int i;
    for (i = 0; i < num_threads; i++) {
        params[i].id = i;
        params[i].max_sleep_seconds = max_sleep_seconds;
        pthread_create(&threads[i], NULL, ThreadFunction, (void *)&params[i]);
    }

    // Esperar a que los hilos terminen y obtener el tiempo de espera
    int j;
    for (j = 0; j < num_threads; j++) {
        void *result;
        pthread_join(threads[j], &result);
        int sleep_time = (int)(intptr_t)result;
        printf("Hilo %d terminó: %d segundos\n", j, sleep_time);
    }

    exit ( EXIT_SUCCESS );
}