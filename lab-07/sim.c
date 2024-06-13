#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VIRTUAL_MEMORY_SIZE 65536 // 64 KiB
#define PHYSICAL_MEMORY_SIZE 32768 // 32 KiB
#define PAGE_SIZE 4096 // 4 KiB
#define NUM_PAGES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE)
#define NUM_FRAMES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)
#define SECONDARY_STORAGE_SIZE 65536 // 64 KiB
#define NUM_STORAGE_PAGES (SECONDARY_STORAGE_SIZE / PAGE_SIZE)
#define MAX_PROCESSES 256

typedef struct {
    int frame_number;
    int in_memory;
    int in_storage;
} PageTableEntry;

typedef struct {
    PageTableEntry page_table[NUM_PAGES];
} Process;

typedef struct {
    int pid;
    int page_number;
} Frame;

Process processes[MAX_PROCESSES];
Frame physical_memory[NUM_FRAMES];
Frame secondary_storage[NUM_STORAGE_PAGES];

int fifo_queue[NUM_FRAMES];
int fifo_front = 0;
int fifo_rear = 0;

int lru_stack[NUM_FRAMES];
int lru_top = -1;

void initialize() {
    int i;
    int j;
    for ( i = 0; i < NUM_FRAMES; i++) {
        physical_memory[i].pid = -1;
        physical_memory[i].page_number = -1;
        fifo_queue[i] = -1;
        lru_stack[i] = -1;
    }
    for ( i = 0; i < NUM_STORAGE_PAGES; i++) {
        secondary_storage[i].pid = -1;
        secondary_storage[i].page_number = -1;
    }
    for ( i = 0; i < MAX_PROCESSES; i++) {
        for ( j = 0; j < NUM_PAGES; j++) {
            processes[i].page_table[j].frame_number = -1;
            processes[i].page_table[j].in_memory = 0;
            processes[i].page_table[j].in_storage = -1;
        }
    }
}

int find_free_frame() {
    int i;
    for ( i = 0; i < NUM_FRAMES; i++) {
        if (physical_memory[i].pid == -1) return i;
    }
    return -1;
}

int find_free_storage() {
    int i;
    for ( i = 0; i < NUM_STORAGE_PAGES; i++) {
        if (secondary_storage[i].pid == -1) return i;
    }
    return -1;
}

void fifo_replace(int pid, int page) {
    int frame_to_replace = fifo_queue[fifo_front];
    fifo_front = (fifo_front + 1) % NUM_FRAMES;

    int storage_index = find_free_storage();
    if (storage_index == -1) {
        printf("Error: No space in secondary storage\n");
        exit(1);
    }

    int old_pid = physical_memory[frame_to_replace].pid;
    int old_page = physical_memory[frame_to_replace].page_number;

    secondary_storage[storage_index].pid = old_pid;
    secondary_storage[storage_index].page_number = old_page;

    processes[old_pid].page_table[old_page].in_storage = storage_index;
    processes[old_pid].page_table[old_page].in_memory = 0;
    processes[old_pid].page_table[old_page].frame_number = -1;

    physical_memory[frame_to_replace].pid = pid;
    physical_memory[frame_to_replace].page_number = page;

    processes[pid].page_table[page].frame_number = frame_to_replace;
    processes[pid].page_table[page].in_memory = 1;
    processes[pid].page_table[page].in_storage = -1;

    fifo_queue[fifo_rear] = frame_to_replace;
    fifo_rear = (fifo_rear + 1) % NUM_FRAMES;
}

void lru_replace(int pid, int page) {
    int frame_to_replace = lru_stack[0];
    int i;
    for ( i = 1; i <= lru_top; i++) {
        lru_stack[i - 1] = lru_stack[i];
    }
    lru_top--;

    int storage_index = find_free_storage();
    if (storage_index == -1) {
        printf("Error: No space in secondary storage\n");
        exit(1);
    }

    int old_pid = physical_memory[frame_to_replace].pid;
    int old_page = physical_memory[frame_to_replace].page_number;

    secondary_storage[storage_index].pid = old_pid;
    secondary_storage[storage_index].page_number = old_page;

    processes[old_pid].page_table[old_page].in_storage = storage_index;
    processes[old_pid].page_table[old_page].in_memory = 0;
    processes[old_pid].page_table[old_page].frame_number = -1;

    physical_memory[frame_to_replace].pid = pid;
    physical_memory[frame_to_replace].page_number = page;

    processes[pid].page_table[page].frame_number = frame_to_replace;
    processes[pid].page_table[page].in_memory = 1;
    processes[pid].page_table[page].in_storage = -1;

    lru_stack[++lru_top] = frame_to_replace;
}

void allocate_page(int pid, int page, char algorithm) {
    if (processes[pid].page_table[page].in_memory) return; // Page is already in memory

    int free_frame = find_free_frame();
    if (free_frame != -1) {
        physical_memory[free_frame].pid = pid;
        physical_memory[free_frame].page_number = page;
        processes[pid].page_table[page].frame_number = free_frame;
        processes[pid].page_table[page].in_memory = 1;
        processes[pid].page_table[page].in_storage = -1;

        if (algorithm == 'f') {
            fifo_queue[fifo_rear] = free_frame;
            fifo_rear = (fifo_rear + 1) % NUM_FRAMES;
        } else if (algorithm == 'l') {
            lru_stack[++lru_top] = free_frame;
        }
    } else {
        if (algorithm == 'f') {
            fifo_replace(pid, page);
        } else if (algorithm == 'l') {
            lru_replace(pid, page);
        }
    }
}

void print_page_table(int pid) {
    int i;
    printf("Proceso %d: ", pid);
    for ( i = 0; i < NUM_PAGES; i++) {
        if (processes[pid].page_table[i].in_memory) {
            printf("%d ", processes[pid].page_table[i].frame_number);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

void print_physical_memory() {
    int i;
    for ( i = 0; i < NUM_FRAMES; i++) {
        if (physical_memory[i].pid != -1) {
            printf("%d.%d ", physical_memory[i].pid, physical_memory[i].page_number);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

void print_secondary_storage() {
    int i;
    for ( i = 0; i < NUM_STORAGE_PAGES; i++) {
        if (secondary_storage[i].pid != -1) {
            printf("%d.%d ", secondary_storage[i].pid, secondary_storage[i].page_number);
        } else {
            printf("- ");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-l") != 0)) {
        printf("Uso: %s -f|-l\n", argv[0]);
        return 1;
    }

    char algorithm = argv[1][1]; // 'f' para FIFO, 'l' para LRU

    initialize();

    int pid, page;
    while (scanf("%d %d", &pid, &page) != EOF) {
        allocate_page(pid, page, algorithm);
    }
    int i;
    for ( i = 0; i < MAX_PROCESSES; i++) {
        print_page_table(i);
    }
    print_physical_memory();
    print_secondary_storage();

    return 0;
}