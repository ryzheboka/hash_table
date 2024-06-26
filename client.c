#include "shared_memory_struct.h"
#include <semaphore.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SHARED_MEMORY "/shared-memory"
#define SEM_ANSWER_COUNT "/sem-answer-count"
#define SEM_COMMAND_COUNT "/sem-command-count-1"
#define SEM_FREE_ANSWERS "/sem-free-answers"
#define SEM_FREE_COMMANDS "/sem-free-commands"
#define MUTEX_SHARED_MEM "/mutex-shared-mem"
#define MAX_SIZE_OF_COMMAND 256
#define MAX_SIZE_OF_ANSWER 256
#define MAX_NUMBER_OF_STRINGS 32

void communicate_with_server();
void exit_programm(sem_t *, sem_t *, struct SharedMemory *, int);

/*struct AnswerListenerArg
{
	struct SharedMemory *memory_ptr;
	sem_t *mutex;
};*/

int main(int argc, char **argv)
{
	if (argc != 1)
	{
		printf("Invalid number of arguments!\n");
		return EXIT_FAILURE;
	}
	communicate_with_server();
	return EXIT_SUCCESS;
}
/*
void *listen_for_answer(void *mutex_and_shared_memory_ptr)
{
	struct AnswerListenerArg *arg = mutex_and_shared_memory_ptr;
	sem_t *mutex_shared_mem = arg->mutex;
	struct SharedMemory *shared_memory_ptr = arg->memory_ptr;
	sem_t *sem_answer_count = sem_open(SEM_ANSWER_COUNT, O_CREAT, 0, 0);
	if (sem_answer_count == SEM_FAILED)
	{
		printf("Couldn't create sem_answer_count\n");
		return NULL;
	}
	sem_t *sem_free_answers = sem_open(SEM_FREE_ANSWERS, 0, 0, MAX_NUMBER_OF_STRINGS);
	if (sem_free_answers == SEM_FAILED)
	{
		printf("Couldn't create sem_answer_count\n");
		return NULL;
	}
	while (1)
	{
		if (sem_wait(sem_answer_count) == -1)
		{
			printf("Error while waiting for sem_answer_cont\n");
			free(arg);
			return NULL;
		}
		if (sem_wait(mutex_shared_mem) == -1)
		{
			printf("Error while waiting for mutex\n");
			free(arg);
			return NULL;
		}

		if (strncmp(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index], "exit", 4) == 0)
		{
			free(arg);
			return NULL;
		}
		if (*(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index]) != '\0')
			printf("Answer: %s\n", shared_memory_ptr->answers[shared_memory_ptr->answer_read_index]);
		*(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index]) = '\0';
		shared_memory_ptr->answer_read_index++;
		if (shared_memory_ptr->answer_read_index == MAX_NUMBER_OF_STRINGS)
		{
			shared_memory_ptr->answer_read_index = 0;
		}
		if (sem_post(mutex_shared_mem) == -1)
		{
			printf("Coulnd't release mutex\n");
			free(arg);
			exit(1);
		}
		if (sem_post(sem_free_answers) == -1)
		{
			printf("Coulnd't post sem_free_answers \n");
			free(arg);
			exit(1);
		}
	}
	free(arg);
	return NULL;
}
*/
void communicate_with_server()
{
	printf("Started client\n");
	sleep(3);
	sem_t *mutex_shared_mem = sem_open(MUTEX_SHARED_MEM, 0, 0, 0);
	// printf("After first inner\n");
	if (mutex_shared_mem == SEM_FAILED)
	{
		printf("Couldn't create the mutex\n");
		return;
	}
	sem_t *sem_command_count = sem_open(SEM_COMMAND_COUNT, 0, 0, 0);
	if (sem_command_count == SEM_FAILED)
	{
		printf("Couldn't create sem_command_count\n");
		return;
	}
	sem_t *sem_free_commands = sem_open(SEM_FREE_COMMANDS, 0, 0, MAX_NUMBER_OF_STRINGS);
	if (sem_free_commands == SEM_FAILED)
	{
		printf("Couldn't create sem_free_commands\n");
		return;
	}
	sleep(3);
	sem_t *sem_answer_count = sem_open(SEM_ANSWER_COUNT, O_CREAT, 0, 0);
	if (sem_answer_count == SEM_FAILED)
	{
		printf("Couldn't create sem_answer_count\n");
		return;
	}
	sem_t *sem_free_answers = sem_open(SEM_FREE_ANSWERS, 0, 0, MAX_NUMBER_OF_STRINGS);
	if (sem_free_answers == SEM_FAILED)
	{
		printf("Couldn't create sem_answer_count\n");
		return;
	}
	int shared_memory = shm_open(SHARED_MEMORY, O_RDWR, 0660);
	if (shared_memory == -1)
	{
		printf("CLIENT: Couldn't create shared memory\n");
		return;
	}
	int truncation_result = ftruncate(shared_memory, sizeof(struct SharedMemory));
	if (truncation_result == -1)
	{
		printf("Couldn't set size of shared memory\n");
		if (sem_close(sem_command_count) != 0)
		{
			printf("Error closing the mutex");
		}
		if (sem_close(mutex_shared_mem) != 0)
		{
			printf("Error closing the mutex");
		}
		return;
	}
	struct SharedMemory *shared_memory_ptr = mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED,
												  shared_memory, 0);
	if (shared_memory_ptr == MAP_FAILED)
	{
		printf("Couldn't map shared memory\n");
		if (sem_close(sem_command_count) != 0)
		{
			printf("Error closing the mutex");
		}
		if (sem_close(mutex_shared_mem) != 0)
		{
			printf("Error closing the mutex");
		}
		return;
	}

	/*struct AnswerListenerArg *listener_arg = malloc(sizeof(struct AnswerListenerArg));
	if (!listener_arg)
	{
		printf("malloc() returned NULL\n");
		exit(1);
	}
	listener_arg->memory_ptr = shared_memory_ptr;
	listener_arg->mutex = mutex_shared_mem;
	// printf("After outer\n");
	pthread_t *thread_id = malloc(sizeof(pthread_t));
	if (pthread_create(thread_id, NULL, listen_for_answer, listener_arg) != 0)
	{
		printf("Error while creating a new thread\n");
		exit(1);
	};*/
	char *command = malloc(MAX_SIZE_OF_COMMAND);
	char *command_id = malloc(3);
	printf("Command: ");
	while (fgets(command, MAX_SIZE_OF_COMMAND, stdin))
	{
		int length = strlen(command);
		if (command[length - 1] == '\n')
			command[length - 1] = '\0';

		if (strcmp(command, "exit") == 0)
		{
			exit_programm(mutex_shared_mem, sem_command_count, shared_memory_ptr, 0);
		}
		if (strncmp(command, "read ", 5) == 0 || strncmp(command, "write ", 6) == 0 || strncmp(command, "delete ", 7) == 0)
		{
			// printf("Working on command %s\n", command);

			if (sem_wait(sem_free_commands) == -1)
			{
				printf("Error while waiting for sem_free_requests\n");
				exit(1);
			}
			if (sem_wait(mutex_shared_mem) == -1)
			{
				printf("Error while waiting for mutex\n");
				exit(1);
			}
			// printf("Sending request\n");

			// Append the index to the request to use as an id when recognizing the answer
			snprintf(command_id, 3, "%02d", shared_memory_ptr->request_write_index);
			snprintf(shared_memory_ptr->requests[shared_memory_ptr->request_write_index], 3, "%02d", shared_memory_ptr->request_write_index);
			strncpy((shared_memory_ptr->requests[shared_memory_ptr->request_write_index]) + 3, command, MAX_SIZE_OF_COMMAND);
			// printf("%s", shared_memory_ptr->requests[shared_memory_ptr->request_write_index] + 3);
			shared_memory_ptr->request_write_index++;
			if (shared_memory_ptr->request_write_index == MAX_NUMBER_OF_STRINGS)
			{
				shared_memory_ptr->request_write_index = 0;
			}
			if (sem_post(mutex_shared_mem) == -1)
			{
				printf("Error while releasing mutex\n");
				exit(1);
			}
			if (sem_post(sem_command_count) == -1)
			{
				printf("Coulnd't post sem_command_count\n");
				exit(1);
			}
			if (sem_wait(sem_answer_count) == -1)
			{
				printf("Error while waiting for sem_answer_cont\n");
				exit(1);
			}
			if (sem_wait(mutex_shared_mem) == -1)
			{
				printf("Error while waiting for mutex\n");
				exit(1);
			}

			if (strncmp(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index] + 3, "exit", 4) == 0)
			{
				exit(1);
			}
			// if (*(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index]) != '\0')
			int i = 0;
			while (i < 32 && strncmp(shared_memory_ptr->answers[i], command_id, 3) != 0)
				i++;
			if (i != 32)
				printf("Answer: %s\n", shared_memory_ptr->answers[shared_memory_ptr->answer_read_index] + 3);
			else
				printf("Answer: SOmething went wrong \n");

			//*(shared_memory_ptr->answers[shared_memory_ptr->answer_read_index]) = '\0';
			shared_memory_ptr->answer_read_index++;
			if (shared_memory_ptr->answer_read_index == MAX_NUMBER_OF_STRINGS)
			{
				shared_memory_ptr->answer_read_index = 0;
			}
			if (sem_post(mutex_shared_mem) == -1)
			{
				printf("Coulnd't release mutex\n");
				exit(1);
			}
			if (sem_post(sem_free_answers) == -1)
			{
				printf("Coulnd't post sem_free_answers \n");
				exit(1);
			}
		}

		printf("Command: ");
	}
	exit_programm(mutex_shared_mem, sem_command_count, shared_memory_ptr, 1);
}

void exit_programm(sem_t *mutex_shared_mem, sem_t *sem_command_count, struct SharedMemory *shared_memory_ptr, int code)
{
	printf("Exiting client\n");
	if (munmap(shared_memory_ptr, sizeof(struct SharedMemory)) != 0)
	{
		printf("Error unmapping shared memory\n");
		exit(1);
	}
	if (sem_close(sem_command_count) != 0)
	{
		printf("Error closing the mutex\n");
		exit(1);
	}
	if (sem_close(mutex_shared_mem) != 0)
	{
		printf("Error closing the mutex\n");
		exit(1);
	}
	exit(code);
}
