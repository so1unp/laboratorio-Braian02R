#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

#define ITEMS 15
#define MAX_WORD 50

struct wordstack {
    int free;
    int items;
    int max_word;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
    char heap[ITEMS][MAX_WORD];
};

typedef struct wordstack wordstack_t;

void usage(char *argv[]) {
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w pila palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r pila: elimina y recupera la palabra del tope de la pila\n");
    fprintf(stderr, "\t-p pila: imprime la pila de palabras\n");
    fprintf(stderr, "\t-c pila: crea una zona de memoria compartida con el nombre indicado donde almacena la pila\n");
    fprintf(stderr, "\t-d pila: elimina la pila indicada\n");
    fprintf(stderr, "\t-h: imprime este mensaje\n");
}

void agregar_palabra(char *pila, char *palabra) {
    int shm_fd = shm_open(pila, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(wordstack_t));
    wordstack_t *shared_memory = mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(shared_memory->mutex));
    sem_wait(&(shared_memory->full));
    strcpy(shared_memory->heap[shared_memory->free], palabra);
    shared_memory->free = (shared_memory->free + 1) % ITEMS;
    shared_memory->items++;
    pthread_mutex_unlock(&(shared_memory->mutex));
    sem_post(&(shared_memory->empty));
    munmap(shared_memory, sizeof(wordstack_t));
    close(shm_fd);
}

void recuperar_palabra(char *pila) {
    int shm_fd = shm_open(pila, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(wordstack_t));
    wordstack_t *shared_memory = mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(shared_memory->mutex));
    sem_wait(&(shared_memory->empty));
    printf("Palabra recuperada: %s\n", shared_memory->heap[(shared_memory->free + ITEMS - shared_memory->items) % ITEMS]);
    shared_memory->items--;
    pthread_mutex_unlock(&(shared_memory->mutex));
    sem_post(&(shared_memory->full));
    munmap(shared_memory, sizeof(wordstack_t));
    close(shm_fd);
}

void imprimir_pila(char *pila) {
    int shm_fd = shm_open(pila, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(wordstack_t));
    wordstack_t *shared_memory = mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(shared_memory->mutex));
    printf("Pila de palabras:\n");
    int i;
    for (i = 0; i < shared_memory->items; i++) {
        printf("%s\n", shared_memory->heap[(shared_memory->free + ITEMS - shared_memory->items + i) % ITEMS]);
    }
    pthread_mutex_unlock(&(shared_memory->mutex));
    munmap(shared_memory, sizeof(wordstack_t));
    close(shm_fd);
}

void crear_pila(char *pila) {
    int shm_fd = shm_open(pila, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(wordstack_t));
    wordstack_t *shared_memory = mmap(NULL, sizeof(wordstack_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&(shared_memory->mutex), NULL);
    sem_init(&(shared_memory->full), 1, ITEMS);
    sem_init(&(shared_memory->empty), 1, 0);
    munmap(shared_memory, sizeof(wordstack_t));
    close(shm_fd);
}

void eliminar_pila(char *pila) {
    shm_unlink(pila);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    switch (option) {
        case 'w': {
            if (argc != 4) {
                fprintf(stderr, "Uso incorrecto. Uso correcto: %s -w pila palabra\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            char *pila = argv[2];
            char *palabra = argv[3];
            agregar_palabra(pila, palabra);
            break;
        }
        case 'r': {
            if (argc != 3) {
                fprintf(stderr, "Uso incorrecto. Uso correcto: %s -r pila\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            char *pila = argv[2];
            recuperar_palabra(pila);
            break;
        }
        case 'p': {
            if (argc != 3) {
                fprintf(stderr, "Uso incorrecto. Uso correcto: %s -p pila\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            char *pila = argv[2];
            imprimir_pila(pila);
            break;
        }
        case 'c': {
            if (argc != 3) {
                fprintf(stderr, "Uso incorrecto. Uso correcto: %s -c pila\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            char *pila = argv[2];
            crear_pila(pila);
            break;
        }
        case 'd': {
            if (argc != 3) {
                fprintf(stderr, "Uso incorrecto. Uso correcto: %s -d pila\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            char *pila = argv[2];
            eliminar_pila(pila);
            break;
            }
        case 'h':{
            usage(argv);
            break;
        }
        default:
            fprintf(stderr, "-%c: opción desconocida.\n", option);
            exit(EXIT_FAILURE);
        
    }

    exit(EXIT_SUCCESS);
}
