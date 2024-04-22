#include "communication_with_clients.h"
#include "shared_memory_struct.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SHARED_MEMORY "/shared-memory"
#define SEM_ANSWER_COUNT "/sem-answer-count"
#define SEM_COMMAND_COUNT "/sem-command-count"
#define SEM_FREE_ANSWERS "/sem-free-answers"
#define SEM_FREE_COMMANDS "/sem-free-commands"
#define MUTEX_SHARED_MEM "/mutex-shared-mem"
#define MAX_SIZE_OF_COMMAND 256
#define MAX_SIZE_OF_ANSWER 256
#define MAX_NUMBER_OF_STRINGS 32
#define MAX_NUMBER_OF_THREADS 5

void communicate_with_clients(struct Table table)
{

	pthread_t *thread_ids = malloc(sizeof(pthread_t) * MAX_NUMBER_OF_THREADS);
	int current_number_of_threads = 0;
	sem_t *mutex_shared_mem = sem_open(MUTEX_SHARED_MEM, O_CREAT, 0660, 0);
	if (mutex_shared_mem == SEM_FAILED)
	{
		printf("Couldn't create the mutex\n");
		exit(EXIT_FAILURE);
	}
	sem_t *sem_command_count = sem_open(SEM_COMMAND_COUNT, O_CREAT, 0660, 0);
	if (sem_command_count == SEM_FAILED)
	{
		printf("Couldn't create sem_command_count\n");
		exit(EXIT_FAILURE);
	}
	sem_t *sem_free_commands = sem_open(SEM_FREE_COMMANDS, O_CREAT, 0660, MAX_NUMBER_OF_STRINGS);
	if (sem_free_commands == SEM_FAILED)
	{
		printf("Couldn't create sem_free_commands\n");
		exit(EXIT_FAILURE);
	}
	sem_t *sem_answer_count = sem_open(SEM_ANSWER_COUNT, O_CREAT, 0660, 0);
	if (sem_command_count == SEM_FAILED)
	{
		printf("Couldn't create sem_command_count\n");
		exit(EXIT_FAILURE);
	}
	sem_t *sem_free_answers = sem_open(SEM_FREE_ANSWERS, O_CREAT, 0660, MAX_NUMBER_OF_STRINGS);
	if (sem_free_commands == SEM_FAILED)
	{
		printf("Couldn't create sem_free_commands\n");
		exit(EXIT_FAILURE);
	}
	int shared_memory = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT | O_EXCL, 0660);
	if (shared_memory == -1)
	{
		printf("Couldn't create shared memory\n");
		exit(EXIT_FAILURE);
	}
	int truncation_result = ftruncate(shared_memory, sizeof(struct SharedMemory));
	if (truncation_result == -1)
	{
		printf("Couldn't set size of shared memory\n");
		exit(EXIT_FAILURE);
	}
	struct SharedMemory *shared_memory_ptr = mmap(NULL, MAX_SIZE_OF_COMMAND, PROT_READ | PROT_WRITE, MAP_SHARED,
												  shared_memory, 0);
	if (shared_memory_ptr == MAP_FAILED)
	{
		printf("Couldn't map shared memory\n");
		// TODO release shared memory
		exit(EXIT_FAILURE);
	}
	shared_memory_ptr->answer_read_index = 0;
	shared_memory_ptr->answer_write_index = 0;
	shared_memory_ptr->request_read_index = 0;
	shared_memory_ptr->answer_read_index = 0;

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
		if (sem_wait(sem_command_count) == -1)
		{
			printf("Error while waiting for sem_commands_count\n");
			exit(EXIT_FAILURE);
		}
		if (sem_wait(mutex_shared_mem) == -1)
		{
			printf("Error while waiting for mutex\n");
			exit(EXIT_FAILURE);
		}
		strncpy(command, shared_memory_ptr->commands[shared_memory_ptr->request_read_index], MAX_SIZE_OF_COMMAND);
		if (strncmp(command, "exit", 4) == 0)
		{

			exiting = true;
		}

		*shared_memory_ptr->commands[shared_memory_ptr->request_read_index] = '\0';
		request_read_index++;
		if (request_read_index == MAX_NUMBER_OF_STRINGS)
		{
			request_read_index = 0;
		}
		// printf("start processing\n");
		if (sem_post(mutex_shared_mem) == -1)
		{
			printf("Error while releasing mutex\n");
			// TODO: Release shared memory
			exit(EXIT_FAILURE);
		}
		if (sem_post(sem_free_commands) == -1)
		{
			printf("Error while releasing sem_free_commands\n");
			// TODO: Release shared memory
			exit(EXIT_FAILURE);
		}
		start_multithreaded_input_processing(table, command, &thread_ids[current_number_of_threads], shared_memory_ptr, mutex_shared_mem, sem_free_answers, sem_answer_count);
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
	if (sem_close(mutex_shared_mem) != 0)
	{
		printf("Error closing the PROCESSED_COMMAND mutex");
	}
	return;
}