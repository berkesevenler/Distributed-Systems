#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_STRING_SIZE 128

char buffer[MAX_STRING_SIZE];

void input(int pipe_fd[]) {
    printf("Enter your text here: ");
    fgets(buffer, MAX_STRING_SIZE, stdin);

    write(pipe_fd[1], buffer, strlen(buffer) + 1); // Write to the pipe
}

void output(int pipe_fd[]) {
    read(pipe_fd[0], buffer, MAX_STRING_SIZE); // Read from the pipe
    printf("Your entered text is: %s", buffer);
}

int main() {
    printf("Communication test using Pipes (to exit the program, press CTRL+c)\n");

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // Create a child process

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process
        close(pipe_fd[0]); // Close the read end of the pipe in the parent process
        close(pipe_fd[1]); // Close the write end of the pipe in the parent process
        wait(NULL); // Wait for the child process to finish
    } else {
        // Child process
        close(pipe_fd[1]); // Close the write end of the pipe in the child process
        close(pipe_fd[0]); // Close the read end of the pipe in the child process
    }

    while(1){
        input(pipe_fd);
        output(pipe_fd);
    }

    return 0;
}
