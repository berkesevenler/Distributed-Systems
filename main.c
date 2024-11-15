#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_STRING_SIZE 128 //the maximum size for the input text buffer.
#define BLOCK_SIZE 2048 //the size of the shared memory block.
#define IPC_RESULT_ERROR (-1) //a constant representing an error in IPC
#define FILENAME "main.c"

char buffer[MAX_STRING_SIZE]; //character array buffer to store user input.
int shared_memory; //to store the identifier of the shared memory block.

void input() {
    while(1) {
        printf("Enter text: "); //prompts the user to enter text.
        memset(buffer, 0, MAX_STRING_SIZE); //initialize the buffer array to zeros
        // before reading user input with fgets.
        fgets(buffer, MAX_STRING_SIZE, stdin); // reads the text using fgets from standard input.
        char *block = shmat(shared_memory, NULL, 0); //attaches the shared memory block using shmat.
        block[0] = '1'; // Set flag to indicate new text has been entered.
        strncpy(block + 1, buffer, BLOCK_SIZE - 1); //copies the text into the shared memory block.
        shmdt(block); //detaches the shared memory block using shmdt.
        sleep(1);
    }
}

void output() {
    while(1) {
        char *block = shmat(shared_memory, NULL, 0); //attaches the shared memory block using shmat.
        if (block[0] == '1') { // Check if new text has been entered.
            printf("Entered text: %s\n", block + 1);
            block[0] = '0'; // Reset flag.
        }
        shmdt(block); //detaches the shared memory block using shmdt.
        sleep(1); // To prevent it from continuously printing
    }
}

int main(int argc, char **argv) {
    printf("Communication test (exit with CTRL+c)\n");

    key_t key = ftok(FILENAME, 0); //generates a System V IPC key
    shared_memory = shmget(key, BLOCK_SIZE, 0644 | IPC_CREAT);
    //shmget, is used to create or access a shared memory segment
    //key: The key obtained from ftok that identifies the shared memory segment.
    //BLOCK_SIZE: The size (in bytes) of the shared memory segment.
    //0644: The permissions for the shared memory segment. 0644 is an octal representation of read and write permissions for the owner and read permissions for others (rw-r--r--).
    //IPC_CREAT indicates that the segment should be created if it does not already exist.

    pid_t pid = fork(); //creates a process fork

    if (pid == 0) {//child process
        output();
        waitpid(pid, NULL, 0);
    } else {
        input();
      //The parent process waits for the child to finish using waitpid.
    }

    shmctl(shared_memory, IPC_RMID, NULL);
    //using the shmctl function to perform control operations on the shared memory
    //IPC_RMID stands for "remove identifier" and is used to mark the shared memory segment for removal.

    return 0;
}
