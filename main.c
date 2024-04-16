// #include "hash_table.h"
#include "tests.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/mman.h>
// #include <pthread.h>
#include <string.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MUTEX_NAME "/inner-mutex"
#define AVAILABLE_COMMANDS "/available-commands"
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
    if (errno != 0)
    {
        printf("Invalid parameter! Parameter must be a positive number\n");
    }
    struct Table *hash_table = initialize_table(arg);
    // test_concurrent(*hash_table);
    communicate_with_clients(*hash_table);
    destroy_lock();
    delete_table(hash_table);
    return EXIT_SUCCESS;
}

void communicate_with_clients(struct Table table)
{

    pthread_t *thread_ids = malloc(sizeof(pthread_t) * MAX_NUMBER_OF_THREADS);
    int current_number_of_threads = 0;
    sem_t *mutex = sem_open(MUTEX_NAME, O_CREAT, 0660, 0);
    if (mutex == SEM_FAILED)
    {
        printf("Couldn't create the mutex\n");
        return;
    }
    sem_t *available_commands = sem_open(AVAILABLE_COMMANDS, O_CREAT, 0660, 0);
    if (available_commands == SEM_FAILED)
    {
        printf("Couldn't create the second mutex\n");
        return;
    }
    int shared_memory = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT | O_EXCL, 0660);
    if (shared_memory == -1)
    {
        printf("Couldn't create shared memory\n");
        return;
    }
    int truncation_result = ftruncate(shared_memory, MAX_SIZE_OF_COMMAND);
    if (truncation_result == -1)
    {
        printf("Couldn't set size of shared memory\n");
        return;
    }
    char *shared_memory_ptr = mmap(NULL, MAX_SIZE_OF_COMMAND, PROT_READ | PROT_WRITE, MAP_SHARED,
                                   shared_memory, 0);
    if (shared_memory_ptr == MAP_FAILED)
    {
        printf("Couldn't map shared memory\n");
        // TODO release shared memory
        return;
    }

    if (sem_post(mutex) == -1)
    {
        printf("Coulnd't release mutex\n");
        return;
    }
    char *command = calloc(1, MAX_SIZE_OF_COMMAND);

    while (strcmp(command, "exit") != 0)
    {

        if (current_number_of_threads == MAX_NUMBER_OF_THREADS)
        {
            // printf("Reached max number of threads\n");
            while (current_number_of_threads > 0)
            {
                // printf("%d\n", pthread_join(thread_ids[current_number_of_threads - 1], NULL));
                //  pthread_join(thread_ids[current_number_of_threads - 1], NULL);
                current_number_of_threads -= 1;
            }
            // printf("Joined all Threads\n");
        }
        if (sem_wait(available_commands) == -1)
        {
            printf("Error while waiting for mutex\n");
            // TODO: Release shared memory
            return;
        }
        strcpy(command, shared_memory_ptr);
        *shared_memory_ptr = 0;
        // printf("start processing\n");
        if (sem_post(mutex) == -1)
        {
            printf("Error while releasing mutex\n");
            // TODO: Release shared memory
            return;
        }
        start_multithreaded_input_processing(table, command, &thread_ids[current_number_of_threads]);
        // printf("end processing\n");
        current_number_of_threads++;
    }
    printf("Exiting!\n");
    if (munmap(shared_memory_ptr, MAX_SIZE_OF_COMMAND) != 0)
    {
        printf("Error unmapping shared memory\n");
    }

    if (shm_unlink(SHARED_MEMORY) != 0)
    {
        printf("Error unlinking shared memory");
    };
    free(thread_ids);
    if (sem_close(mutex) != 0)
    {
        printf("Error closing the mutex");
    }
    return;
}