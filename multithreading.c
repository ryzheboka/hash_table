#include "multithreading.h"
#include <unistd.h>

#define MAX_NUMBER_OF_STRINGS 32
pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

void *process_input(void *process_input)
{
	/*int random = rand() % 10 + 4;
	printf("%d", random);
	sleep(random);*/
	char *answer = calloc(1, 256);
	struct InputProcessingInfo *input = process_input;
	pthread_rwlock_t *p = (pthread_rwlock_t *)&lock_rw;
	if (strncmp(input->input, "read ", 5) == 0)
	{
		struct TableEntry *entry = read_entry(input->table, input->input + 5, p);
		if (entry->error)
		{
			strncpy(answer, entry->error, strlen(entry->error));
			free(entry);
		}
		else
			snprintf(answer, 30 + strlen(entry->key) + strlen(entry->value), "The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);
	}

	if (strncmp(input->input, "delete ", 7) == 0)
	{
		char *error = delete_entry(input->table, input->input + 7, p);
		if (error)
		{
			strncpy(answer, error, strlen(error));
		}
	}

	if (strncmp(input->input, "write ", 6) == 0)
	{
		char *attributes;
		attributes = input->input + 6;
		if (strchr(attributes, ',') == NULL)
		{
			strncpy(answer, "Key and value need to be separated by ,", 40);
		}
		else
		{
			char *key = strsep(&attributes, ",");
			char *value = strsep(&attributes, ",");

			char *error = insert_entry(input->table, key, value, p);

			if (error)
			{
				strncpy(answer, error, strlen(error));
			}
		}
	}
	if (sem_wait(input->sem_free_answers) == -1)
	{
		printf("Error while waiting for sem_free_answers\n");
		free(input);
		// TODO: Release shared memory
		exit(EXIT_FAILURE);
	}
	if (sem_wait(input->mutex) == -1)
	{
		printf("Error while waiting for mutex\n");
		free(input);
		// TODO: Release shared memory
		exit(EXIT_FAILURE);
	}
	strncpy(input->memory_ptr->answers[input->memory_ptr->answer_write_index], input->input_id, 3);
	strncpy(input->memory_ptr->answers[input->memory_ptr->answer_write_index] + 3, answer, strlen(answer));
	free(answer);
	// else
	//	snprintf(input->memory_ptr->answers[input->memory_ptr->answer_write_index], 30 + strlen(entry->key) + strlen(entry->value), "The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);

	input->memory_ptr->answer_write_index++;
	if (input->memory_ptr->answer_write_index == MAX_NUMBER_OF_STRINGS)
	{
		input->memory_ptr->answer_write_index = 0;
	}
	if (sem_post(input->mutex) == -1)
	{
		printf("Error while waiting for mutex\n");
		free(input);
		exit(EXIT_FAILURE);
	}
	if (sem_post(input->sem_answer_count) == -1)
	{
		printf("Error while waiting for mutex\n");
		free(input);
		exit(EXIT_FAILURE);
	}
	free(input);
	return NULL;
}

void start_multithreaded_input_processing(struct Table table, char *input, char *input_id, pthread_t *threadId, struct SharedMemory *memory_ptr, sem_t *mutex, sem_t *sem_free_answers, sem_t *sem_answer_count)
{
	struct InputProcessingInfo *processing_info = malloc(sizeof(struct InputProcessingInfo));
	if (!processing_info)
	{
		printf("malloc() returned NULL while processing input\n");
		return;
	}
	processing_info->table = table;
	processing_info->input = strdup(input);
	processing_info->input_id = strdup(input_id);
	processing_info->memory_ptr = memory_ptr;
	processing_info->mutex = mutex;
	processing_info->sem_free_answers = sem_free_answers;
	processing_info->sem_answer_count = sem_answer_count;

	if (pthread_create(threadId, NULL, process_input, processing_info) != 0)
	{
		printf("Error while creating a new thread\n");
		exit(1);
	};
	return;
}

void destroy_lock()
{
	pthread_rwlock_destroy((pthread_rwlock_t *)&lock_rw);
}