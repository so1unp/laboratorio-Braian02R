#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void print_file_type(mode_t mode) {
    if (S_ISREG(mode)) {
        printf("Tipo de archivo: regular file\n");
    } else if (S_ISDIR(mode)) {
        printf("Tipo de archivo: directory\n");
    } else if (S_ISCHR(mode)) {
        printf("Tipo de archivo: character device\n");
    } else if (S_ISBLK(mode)) {
        printf("Tipo de archivo: block device\n");
    } else if (S_ISFIFO(mode)) {
        printf("Tipo de archivo: FIFO/pipe\n");
    } else if (S_ISLNK(mode)) {
        printf("Tipo de archivo: symlink\n");
    } else if (S_ISSOCK(mode)) {
        printf("Tipo de archivo: socket\n");
    } else {
        printf("Tipo de archivo: unknown\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_path = argv[1];
    struct stat file_stat;

    if (stat(file_path, &file_stat) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("ID del usuario propietario: %d\n", file_stat.st_uid);
    print_file_type(file_stat.st_mode);
    printf("Número de i-nodo: %ld\n", file_stat.st_ino);
    printf("Tamaño en bytes: %ld\n", file_stat.st_size);
    printf("Número de bloques: %ld\n", file_stat.st_blocks);
    printf("Tiempo de última modificación: %s", ctime(&file_stat.st_mtime));
    printf("Tiempo de último acceso: %s", ctime(&file_stat.st_atime));

    return 0;
}
