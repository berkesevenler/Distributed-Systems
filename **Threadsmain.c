#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define MAX_STRING_SIZE 128

char *shared_memory;

void input() {
    printf("Enter your text here: ");
    fgets(shared_memory, MAX_STRING_SIZE, stdin);
}

void output() {
    printf("Your entered text is: %s\n", shared_memory);
}

int main() {
    printf("Communication test using Shared-Memory (to exit the program, press CTRL+c)\n");
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, MAX_STRING_SIZE, IPC_CREAT | 0666);

    shared_memory = (char *)shmat(shmid, NULL, 0);

    while (1) {
        input();
        output();
    }

    shmdt(shared_memory);       //detach shared memory segment 
    shmctl(shmget(key, 'A', 0666 | IPC_CREAT), IPC_RMID, NULL);       //remove memory segment

    return 0;
}


