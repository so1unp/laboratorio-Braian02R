#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define QUEUE_PERMISSIONS 0666
#define MAX_MSG_SIZE 100
#define MAX_MSGS 10
#define USERNAME_MAXSIZE 15

struct msg {
    char sender[USERNAME_MAXSIZE];
    char text[MAX_MSG_SIZE];
};

typedef struct msg msg_t;

void usage(char *argv[]) {
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");//no bloqueante error
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");//bloqueante espera mensaje
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
    fprintf(stderr, "\t-i queue: imprime información de la cola de mensajes queue.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void send_message(char *queue_name, char *message) {
    mqd_t mq = mq_open(queue_name, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    msg_t new_message;
    strncpy(new_message.sender, getlogin(), USERNAME_MAXSIZE - 1);
    strncpy(new_message.text, message, MAX_MSG_SIZE - 1);

    if (mq_send(mq, (char *)&new_message, sizeof(msg_t), 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    mq_close(mq);
}

void receive_message(char *queue_name) {
    mqd_t mq = mq_open(queue_name, O_RDONLY | O_NONBLOCK);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    msg_t received_message;
    if (mq_receive(mq, (char *)&received_message, sizeof(msg_t), NULL) == -1) {
        perror("mq_receive: no hay mensajes en la cola");
        exit(EXIT_FAILURE);
    }

    printf("Mensaje recibido:\n");
    printf("Remitente: %s\n", received_message.sender);
    printf("Mensaje: %s\n", received_message.text);

    mq_close(mq);
}

void receive_all_messages(char *queue_name) {
    mqd_t mq = mq_open(queue_name, O_RDONLY | O_NONBLOCK);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Mensajes en la cola %s:\n", queue_name);
    msg_t received_message;
    while (mq_receive(mq, (char *)&received_message, sizeof(msg_t), NULL) != -1) {
        printf("Remitente: %s\n", received_message.sender);
        printf("Mensaje: %s\n", received_message.text);
    }

    mq_close(mq);
}

void listen_messages(char *queue_name) {
    mqd_t mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Escuchando mensajes en la cola %s...\n", queue_name);
    msg_t received_message;
    while (1) {
        if (mq_receive(mq, (char *)&received_message, sizeof(msg_t), NULL) != -1) {
            printf("Remitente: %s\n", received_message.sender);
            printf("Mensaje: %s\n", received_message.text);
        }
    }

    mq_close(mq);
}

void create_queue(char *queue_name) {
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = sizeof(msg_t);

    mqd_t mq = mq_open(queue_name, O_CREAT | O_EXCL | O_RDWR, QUEUE_PERMISSIONS, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Cola de mensajes %s creada\n", queue_name);
    mq_close(mq);
}

void delete_queue(char *queue_name) {
    if (mq_unlink(queue_name) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }

    printf("Cola de mensajes %s eliminada\n", queue_name);
}

void queue_info(char *queue_name) {
    struct mq_attr attr;

    mqd_t mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    if (mq_getattr(mq, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    printf("Información de la cola %s:\n", queue_name);
    printf("Máximo número de mensajes: %ld\n", attr.mq_maxmsg);
    printf("Tamaño máximo de mensajes: %ld\n", attr.mq_msgsize);
    printf("Número actual de mensajes en la cola: %ld\n", attr.mq_curmsgs);

    mq_close(mq);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    char *queue_name = argv[2];

    switch (option) {
        case 's':
            if (argc < 4) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            send_message(queue_name, argv[3]);
            break;
        case 'r':
            receive_message(queue_name);
            break;
        case 'a':
            receive_all_messages(queue_name);
            break;
        case 'l':
            listen_messages(queue_name);
            break;
        case 'c':
            create_queue(queue_name);
            break;
        case 'd':
            delete_queue(queue_name);
            break;
        case 'i':
            queue_info(queue_name);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}