//
// Problema del búfer limitado.
//
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

struct buffer {
    int size;
    int* buf;
    sem_t empty; // Semáforo que controla los espacios vacíos en el buffer
    sem_t full;  // Semáforo que controla los espacios ocupados en el buffer
    pthread_mutex_t mutex; // Mutex para proteger el acceso al buffer
};

struct params {
    int wait_prod;
    int wait_cons;
    int items;
    struct buffer* buf;
};

static void* producer(void*);
static void* consumer(void*);

/* Productor */
static void* producer(void *p)
{
    int i = 0;

    struct params *params = (struct params*) p;

    for (i = 0; i < params->items; i++) {
        sem_wait(&params->buf->empty); // Espera hasta que haya espacio vacío en el buffer
        pthread_mutex_lock(&params->buf->mutex); // Bloquea el mutex antes de acceder al buffer
        params->buf->buf[i % params->buf->size] = i;
        pthread_mutex_unlock(&params->buf->mutex); // Desbloquea el mutex después de acceder al buffer
        sem_post(&params->buf->full); // Incrementa el contador de espacios ocupados en el buffer
        usleep(rand() % params->wait_prod);
    }

    pthread_exit(0);
}

/* Consumidor */
static void* consumer(void *p)
{
    int i;

    struct params *params = (struct params*) p;

    // Reserva memoria para guardar lo que lee el consumidor.
    int *reader_results = (int*) malloc(sizeof(int)*params->items);

    for (i = 0; i < params->items; i++) {
        sem_wait(&params->buf->full); // Espera hasta que haya elementos para consumir en el buffer
        pthread_mutex_lock(&params->buf->mutex); // Bloquea el mutex antes de acceder al buffer
        reader_results[i] = params->buf->buf[i % params->buf->size];
        pthread_mutex_unlock(&params->buf->mutex); // Desbloquea el mutex después de acceder al buffer
        sem_post(&params->buf->empty); // Incrementa el contador de espacios vacíos en el buffer
        usleep(rand() % params->wait_cons);
    }

    // Imprime lo que leyo
    for (i = 0; i < params->items; i++) {
        printf("%d ", reader_results[i]);
    }
    printf("\n");

    pthread_exit(0);
}

int main(int argc, char** argv)
{
    pthread_t producer_t, consumer_t;

    // Controla argumentos.
    if (argc != 5) {
        fprintf(stderr, "Uso: %s size items wait-prod wait-cons rand\n", argv[0]);
        fprintf(stderr, "\tsize:\ttamaño del buffer.\n");
        fprintf(stderr, "\titems:\tnúmero de items a producir/consumir.\n");
        fprintf(stderr, "\twaitp:\tnúmero de microsegundos que espera el productor.\n");
        fprintf(stderr, "\twaitc:\tnúmero de microsegundos que espera el consumidor.\n");
        exit(EXIT_FAILURE);
    }

    struct buffer *buf;
    buf = (struct buffer*) malloc(sizeof(struct buffer));
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Tamaño del buffer.
    buf->size = atoi(argv[1]);
    if (buf->size <= 0) {
        fprintf(stderr, "bufsize tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    // Crea el buffer
    buf->buf = (int*) malloc(sizeof(int) * buf->size);
    if (buf->buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Inicializa semáforos y mutex
    sem_init(&buf->empty, 0, buf->size); // Inicialmente, todos los espacios están vacíos
    sem_init(&buf->full, 0, 0); // Inicialmente, no hay elementos para consumir
    pthread_mutex_init(&buf->mutex, NULL);

    // Instancia una estructura de parámetros
    struct params *params;
    params = (struct params*) malloc(sizeof(struct params));
    if (params == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    params->buf = buf;

    // Cantidad de items a producir.
    params->items = atoi(argv[2]);
    if (params->items <= 0) {
        fprintf(stderr, "counter tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    params->wait_prod = atoi(argv[3]);
    if (params->wait_prod <= 0) {
        fprintf(stderr, "wait-prod tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    params->wait_cons = atoi(argv[4]);
    if (params->wait_cons <= 0) {
        fprintf(stderr, "cons-wait tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    // Inicializa semilla para números pseudo-aleatorios.
    srand(getpid());

    // Crea productor y consumidor
    pthread_create(&producer_t, NULL, producer, params);
    pthread_create(&consumer_t, NULL, consumer, params);

    // Espera a que los hilos terminen
    pthread_join(producer_t, NULL);
    pthread_join(consumer_t, NULL);

    // Libera recursos
    free(params);
    free(buf->buf);
    free(buf);
    sem_destroy(&buf->empty);
    sem_destroy(&buf->full);
    pthread_mutex_destroy(&buf->mutex);

    return 0;
}