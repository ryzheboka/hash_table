#include "multithreading.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PROCESSED_COMMAND "/processed-command"
#define AVAILABLE_COMMAND "/available-command"
#define SHARED_MEMORY "/shared-memory"
#define MAX_SIZE_OF_COMMAND 256
#define MAX_NUMBER_OF_THREADS 5

void communicate_with_clients(struct Table);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Invalid number of arguments!\n");
        return EXIT_FAILURE;
    }
    unsigned long arg = strtoul(argv[1], NULL, 10);
    if (errno != 0 || arg == 0)
    {
        printf("Invalid parameter! Parameter must be a positive number\n");
        return EXIT_FAILURE;
    }
    struct Table *hash_table = initialize_table(arg);
    communicate_with_clients(*hash_table);
    destroy_lock();
    delete_table(hash_table);
    return EXIT_SUCCESS;
}

void communicate_with_clients(struct Table table)
{

    pthread_t *thread_ids = malloc(sizeof(pthread_t) * MAX_NUMBER_OF_THREADS);
    int current_number_of_threads = 0;
    sem_t *processed_command_mutex = sem_open(PROCESSED_COMMAND, O_CREAT, 0660, 0);
    if (processed_command_mutex == SEM_FAILED)
    {
        printf("Couldn't create the PROCESS_COMMAND mutex\n");
        exit(EXIT_FAILURE);
    }
    sem_t *available_command_mutex = sem_open(AVAILABLE_COMMAND, O_CREAT, 0660, 0);
    if (available_command_mutex == SEM_FAILED)
    {
        printf("Couldn't create the AVAILABLE_COMMAND mutex\n");
        exit(EXIT_FAILURE);
    }
    int shared_memory = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT | O_EXCL, 0660);
    if (shared_memory == -1)
    {
        printf("Couldn't create shared memory\n");
        exit(EXIT_FAILURE);
    }
    int truncation_result = ftruncate(shared_memory, MAX_SIZE_OF_COMMAND);
    if (truncation_result == -1)
    {
        printf("Couldn't set size of shared memory\n");
        exit(EXIT_FAILURE);
    }
    char *shared_memory_ptr = mmap(NULL, MAX_SIZE_OF_COMMAND, PROT_READ | PROT_WRITE, MAP_SHARED,
                                   shared_memory, 0);
    if (shared_memory_ptr == MAP_FAILED)
    {
        printf("Couldn't map shared memory\n");
        // TODO release shared memory
        exit(EXIT_FAILURE);
    }

    char *command = calloc(1, MAX_SIZE_OF_COMMAND);
    bool exiting = false;

    printf("Server initialized successfully!\n");
    while (!exiting)
    {

        if (current_number_of_threads == MAX_NUMBER_OF_THREADS)
        {
            while (current_number_of_threads > 0)
            {
                if (pthread_join(thread_ids[current_number_of_threads - 1], NULL) != 0)
                {
                    printf("Error while joining threads");
                    exit(EXIT_FAILURE);
                }
                current_number_of_threads -= 1;
            }
            // printf("Joined all Threads\n");
        }
        if (sem_wait(available_command_mutex) == -1)
        {
            printf("Error while waiting for mutex\n");
            exit(EXIT_FAILURE);
        }
        strncpy(command, shared_memory_ptr, MAX_SIZE_OF_COMMAND);
        if (strncmp(command, "exit", 4) == 0)
        {

            exiting = true;
        }

        *shared_memory_ptr = '\0';
        // printf("start processing\n");
        if (sem_post(processed_command_mutex) == -1)
        {
            printf("Error while releasing PROCESSED_COMMAND mutex\n");
            // TODO: Release shared memory
            exit(EXIT_FAILURE);
        }
        start_multithreaded_input_processing(table, command, &thread_ids[current_number_of_threads], shared_memory_ptr, processed_command_mutex);
        // printf("end processing\n");
        current_number_of_threads++;
    }
    printf("Exit server!\n");
    if (munmap(shared_memory_ptr, MAX_SIZE_OF_COMMAND) != 0)
    {
        printf("Error unmapping shared memory\n");
    }

    if (shm_unlink(SHARED_MEMORY) != 0)
    {
        printf("Error unlinking shared memory");
    };
    free(thread_ids);
    if (sem_close(processed_command_mutex) != 0)
    {
        printf("Error closing the PROCESSED_COMMAND mutex");
    }
    return;
}