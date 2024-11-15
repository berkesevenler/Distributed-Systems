#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_STRING_SIZE 128

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_input = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_output = PTHREAD_COND_INITIALIZER;

char buffer[MAX_STRING_SIZE];
int input_done = 0;

// Input Thread Function
void* input(void* arg) {
    pthread_mutex_lock(&mutex); // Locks the mutex to ensure exclusive access to shared resources like the buffer

    printf("Enter your text here: ");
    memset(buffer, 0, MAX_STRING_SIZE);

    // Reads user input from the console / compiler
    if (fgets(buffer, MAX_STRING_SIZE, stdin) == NULL) {
        perror("Error reading input");
        exit(EXIT_FAILURE);
    }

    input_done = 1; // Signals that input is complete
    pthread_cond_signal(&cond_output); // Signal the output thread that input is done
    pthread_mutex_unlock(&mutex); // Unlocks the mutex to allow other threads to access shared resources
    pthread_exit(NULL); // Exits the thread
}

// Output Thread Function
void* output(void* arg) {
    pthread_mutex_lock(&mutex); // Locks the mutex to ensure exclusive access to shared resources like the buffer

    // Waits for input to be done (otherwise could be conflict sometimes when running our program)
    while (!input_done) {
        pthread_cond_wait(&cond_output, &mutex);
    }

    // Prints the entered text
    printf("Your entered text is: %s\n", buffer);

    input_done = 0; // Signals that output is complete
    pthread_cond_signal(&cond_input); // Signal the input thread to start a new round
    pthread_mutex_unlock(&mutex); // Unlocks the mutex to allow other threads to access shared resources
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    printf("Communication test using Pthreads (to exit the program, press CTRL+c)\n");

    // Initializes the mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    // Initializes the input and output condition variables
    if (pthread_cond_init(&cond_input, NULL) != 0 || pthread_cond_init(&cond_output, NULL) != 0) {
        perror("Condition variable initialization failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pthread_t thread1, thread2;

        // Creates the input thread
        if (pthread_create(&thread1, NULL, input, NULL) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex); // Locks the mutex to ensure exclusive access to shared resources like the buffer

        // Waits for the signal from the output thread to start a new round of communication
        while (input_done) {
            if (pthread_cond_wait(&cond_input, &mutex) != 0) {
                perror("Condition wait failed");
                exit(EXIT_FAILURE);
            }
        }

        pthread_mutex_unlock(&mutex); // Unlocks the mutex

        // Creates the output thread
        if (pthread_create(&thread2, NULL, output, NULL) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        // Waits for both threads to finish before starting a new round
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }

    // Destroys the mutex and condition variables before exiting the program
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_input);
    pthread_cond_destroy(&cond_output);

    return 0;
}
