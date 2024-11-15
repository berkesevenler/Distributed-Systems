#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_STRING_SIZE 128
#define BLOCK_SIZE 2048
#define IPC_RESULT_ERROR (-1)
#define FILENAME "ShMem.c"

char buffer[MAX_STRING_SIZE];
void input() {
    printf("Enter your text here: ");
    memset(buffer, 0, MAX_STRING_SIZE);
    fgets(buffer, MAX_STRING_SIZE, stdin);

    //Link key to filename for other process
    key_t key;
    key = ftok(FILENAME, 0);

    //create shared memory block 
    int shared_block_id = shmget(key, BLOCK_SIZE, 0644 | IPC_CREAT);

    //attach shared memory segment
    char *block = shmat(shared_block_id, NULL, 0); 

    strncpy(block, buffer, BLOCK_SIZE);   
}

void output() {
    pid_t pid = fork();
    if (pid == 0) {
        //child process

        //read input from shared memory
        key_t key;
        key = ftok(FILENAME, 0);
        int shared_block_id = shmget(key, BLOCK_SIZE, 0644 | IPC_CREAT);
        char *block = shmat(shared_block_id, NULL, 0); 

        printf("Your entered text is: %s\n", block);

        //detach shared memory segment 
        shmdt(block);

        //terminate child process
        exit(0);
    } else {
        //parent process
        waitpid (pid, NULL, 0);
    }     
}

int main(int argc, char** argv) {
    printf("Communication test using Shared-Memory (to exit the program, press CTRL+c)\n");
    while(1) {
        input();
        output();
    }

    //remove memory segment
    key_t key = ftok(FILENAME, 0);
    shmctl(shmget(key, 0, 0644 | IPC_CREAT), IPC_RMID, NULL);

    return 0;
}
