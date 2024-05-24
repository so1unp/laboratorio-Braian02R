#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra

struct canvas {
    char canvas[HEIGHT*WIDTH];
};

typedef struct canvas canvas_t;

void usage(char *argv[]) {
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
    fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
    fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
    fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void create_canvas(const char *name) {
    int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(canvas_t)) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (canvas == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    memset(canvas->canvas, ' ', HEIGHT * WIDTH); // Inicializa el canvas con espacios
    munmap(canvas, sizeof(canvas_t));
    close(fd);
}

void delete_canvas(const char *name) {
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
}

void print_canvas(const char *name) {
    int fd = shm_open(name, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ, MAP_SHARED, fd, 0);
    if (canvas == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int i,j;
    for ( i = 0; i < HEIGHT; ++i) {
        for ( j = 0; j < WIDTH; ++j) {
            putchar(canvas->canvas[i * WIDTH + j]);
        }
        putchar('\n');
    }

    munmap(canvas, sizeof(canvas_t));
    close(fd);
}

void write_canvas(const char *name, const char *message, int x, int y, char direction) {
    int fd = shm_open(name, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (canvas == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int len = strlen(message);
    if (direction == 'h') {
        int i;
        for ( i = 0; i < len && (x + i) < WIDTH; ++i) {
            canvas->canvas[y * WIDTH + (x + i)] = message[i];
        }
    } else if (direction == 'v') {
        int i;
        for ( i = 0; i < len && (y + i) < HEIGHT; ++i) {
            canvas->canvas[(y + i) * WIDTH + x] = message[i];
        }
    } else {
        fprintf(stderr, "Dirección desconocida '%c'. Use 'h' para horizontal o 'v' para vertical.\n", direction);
        munmap(canvas, sizeof(canvas_t));
        close(fd);
        exit(EXIT_FAILURE);
    }

    munmap(canvas, sizeof(canvas_t));
    close(fd);
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

    switch(option) {
        case 'w':
            if (argc != 7) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            write_canvas(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), argv[6][0]);
            break;
        case 'p':
            if (argc != 3) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            print_canvas(argv[2]);
            break;
        case 'c':
            if (argc != 3) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            create_canvas(argv[2]);
            break;
        case 'd':
            if (argc != 3) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            delete_canvas(argv[2]);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Comando desconocido\n");
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}