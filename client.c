#include <semaphore.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define SHARED_MEMORY "/shared-memory"
#define INNER_MUTEX_NAME "/inner-mutex"
#define AVAILABLE_COMMANDS "/available-commands"
#define MAX_SIZE_OF_COMMAND 256

void communicate_with_server();
void exit_programm(sem_t *, sem_t *, char *, int);

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

void communicate_with_server()
{
	printf("Started client\n");
	sem_t *access_mutex = sem_open(INNER_MUTEX_NAME, O_CREAT, 0660, 0);
	// printf("After first inner\n");
	if (access_mutex == SEM_FAILED)
	{
		printf("Couldn't create the mutex\n");
		return;
	}
	sem_t *outer_mutex = sem_open(AVAILABLE_COMMANDS, O_CREAT, 0660, 0);
	if (outer_mutex == SEM_FAILED)
	{
		printf("Couldn't create the mutex\n");
		return;
	}
	int shared_memory = shm_open(SHARED_MEMORY, O_RDWR, 0660);
	if (shared_memory == -1)
	{
		printf("CLIENT: Couldn't create shared memory\n");
		return;
	}
	int truncation_result = ftruncate(shared_memory, MAX_SIZE_OF_COMMAND);
	if (truncation_result == -1)
	{
		printf("Couldn't set size of shared memory\n");
		if (sem_close(outer_mutex) != 0)
		{
			printf("Error closing the mutex");
		}
		if (sem_close(access_mutex) != 0)
		{
			printf("Error closing the mutex");
		}
		return;
	}
	char *shared_memory_ptr = mmap(NULL, MAX_SIZE_OF_COMMAND, PROT_READ | PROT_WRITE, MAP_SHARED,
								   shared_memory, 0);
	if (shared_memory_ptr == MAP_FAILED)
	{
		printf("Couldn't map shared memory\n");
		if (sem_close(outer_mutex) != 0)
		{
			printf("Error closing the mutex");
		}
		if (sem_close(access_mutex) != 0)
		{
			printf("Error closing the mutex");
		}
		return;
	}
	printf("Before outer\n");

	if (sem_post(access_mutex) == -1)
	{
		printf("Coulnd't release mutex\n");
		exit(1);
	}
	printf("Before outer\n");
	if (sem_post(outer_mutex) == -1)
	{
		printf("Coulnd't release mutex\n");
		exit(1);
	}
	char *command = malloc(MAX_SIZE_OF_COMMAND);
	// printf("After outer\n");
	printf("Command: ");

	while (fgets(command, MAX_SIZE_OF_COMMAND, stdin))
	{
		int length = strlen(command);
		if (command[length - 1] == '\n')
			command[length - 1] = '\0';

		if (strcmp(command, "exit") == 0)
		{
			exit_programm(access_mutex, outer_mutex, shared_memory_ptr, 0);
		}
		if (strncmp(command, "read ", 5) == 0 || strncmp(command, "write ", 6) == 0 || strncmp(command, "delete ", 7) == 0)
		{
			printf("Working on command %s\n", command);
			if (sem_wait(access_mutex) == -1)
			{
				printf("Error while waiting for mutex\n");
				exit(1);
			}
			printf("Sending request\n");
			strcpy(shared_memory_ptr, command);

			/*if (sem_post(access_mutex) == -1)
			{
				printf("Coulnd't release mutex\n");
				exit(1);
			}*/
			if (sem_post(outer_mutex) == -1)
			{
				/
					printf("Coulnd't wait for mutex\n");
				exit(1);
			}
		}
		printf("Command: ");
	}
	exit_programm(access_mutex, outer_mutex, shared_memory_ptr, 1);
}

void exit_programm(sem_t *access_mutex, sem_t *outer_mutex, char *shared_memory_ptr, int code)
{
	printf("Exiting client\n");
	if (sem_wait(outer_mutex) == -1)
	{
		printf("Error while waiting for mutes");
		exit(1);
	}
	if (sem_wait(access_mutex) == -1)
	{
		printf("Error while waiting for mutes");
		exit(1);
	}
	printf("Sending request\n");
	strcpy(shared_memory_ptr, "exit");

	if (sem_post(access_mutex) == -1)
	{
		printf("Coulnd't release mutex\n");
		exit(1);
	}
	if (sem_post(outer_mutex) == -1)
	{
		printf("Coulnd't release mutex\n");
		exit(1);
	}
	if (munmap(shared_memory_ptr, MAX_SIZE_OF_COMMAND) != 0)
	{
		printf("Error unmapping shared memory\n");
		exit(1);
	}
	if (sem_close(outer_mutex) != 0)
	{
		printf("Error closing the mutex");
		exit(1);
	}
	if (sem_close(access_mutex) != 0)
	{
		printf("Error closing the mutex");
		exit(1);
	}
	exit(code);
}
