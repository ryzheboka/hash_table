#include "multithreading.h"

pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

void *process_input(void *process_input)
{
	struct InputProcessingInfo *input = process_input;
	pthread_rwlock_t *p = (pthread_rwlock_t *)&lock_rw;
	if (strncmp(input->input, "read ", 5) == 0)
	{
		if (pthread_rwlock_rdlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			free(input);
			exit(EXIT_FAILURE);
		}
		// printf("Read lock succeeded\n");
		struct TableEntry *entry = read_entry(input->table, input->input + 5);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			free(input);
			exit(EXIT_FAILURE);
		}

		// printf("The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);
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
		if (entry->error)
			strncpy(input->memory_ptr->answers[input->memory_ptr->answer_write_index], entry->error, strlen(entry->error));

		else
			snprintf(input->memory_ptr->answers[input->memory_ptr->answer_write_index], 30 + strlen(entry->key) + strlen(entry->value), "The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);

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
		if (entry->error)
		{
			free(entry->error);
			free(entry);
		}
		return NULL;
	}

	if (strncmp(input->input, "delete ", 7) == 0)
	{
		if (pthread_rwlock_wrlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			free(input);

			exit(EXIT_FAILURE);
		}
		// printf("Read lock succeeded\n");
		char *error = delete_entry(input->table, input->input + 7);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			free(input);
			if (error)
			{
				free(error);
			}
			exit(EXIT_FAILURE);
		}
		if (error)
		{
			if (sem_wait(input->sem_free_answers) == -1)
			{
				printf("Error while waiting for sem_free_answers\n");
				free(input);
				// TODO: Release shared memory
				exit(EXIT_FAILURE);
			}
			if (sem_wait(input->mutex) == -1)
			{
				printf("Error while waiting for PROCESSED_COMMAND mutex\n");
				free(input);
				free(error);
				// TODO: Release shared memory
				return NULL;
			}

			strncpy(input->memory_ptr->answers[input->memory_ptr->answer_write_index], error, strlen(error));
			input->memory_ptr->answer_write_index++;
			if (input->memory_ptr->answer_write_index == MAX_NUMBER_OF_STRINGS)
			{
				input->memory_ptr->answer_write_index = 0;
			}
			if (sem_post(input->mutex) == -1)
			{
				printf("Error while waiting for PROCESSED_COMMAND mutex\n");
				free(input);
				free(error);
				// TODO: Release shared memory
				return NULL;
			}
			if (sem_post(input->sem_answer_count) == -1)
			{
				printf("Error while releasing sem_answer_count\n");
				free(input);
				// TODO: Release shared memory
				exit(EXIT_FAILURE);
			}
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
			free(input);
			if (error)
			{
				free(error);
			}
			return NULL;
		}
	}

	if (strncmp(input->input, "write ", 6) == 0)
	{
		char *attributes;
		attributes = input->input + 6;
		if (strchr(attributes, ',') == NULL)
		{
			strncpy(input->memory_ptr, "Key and value need to be separated by ,", 40);
			return NULL;
		}
		char *key = strsep(&attributes, ",");
		char *value = strsep(&attributes, ",");
		// printf("kkkey: %s, vvvvlue: %s\n", key, value);

		if (pthread_rwlock_wrlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			free(input);
			exit(EXIT_FAILURE);
		}

		// printf("Lock succeded!\n");
		char *error = insert_entry(input->table, key, value);
		if (pthread_rwlock_unlock(p) != 0)
		{

			printf("Unlocking failed. Aborting operation");
			free(input);
			if (error)
			{
				free(error);
			}
			exit(EXIT_FAILURE);
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		}
		if (error)
		{
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
				free(error);
				exit(EXIT_FAILURE);
			}

			strncpy(input->memory_ptr->answers[input->memory_ptr->answer_write_index], error, strlen(error));
			input->memory_ptr->answer_write_index++;
			if (input->memory_ptr->answer_write_index == MAX_NUMBER_OF_STRINGS)
			{
				input->memory_ptr->answer_write_index = 0;
			}
			if (sem_post(input->mutex) == -1)
			{
				printf("Error while waiting for PROCESSED_COMMAND mutex\n");
				free(input);
				free(error);
				exit(EXIT_FAILURE);
			}
			if (sem_post(input->sem_answer_count) == -1)
			{
				printf("Error while waiting for sem_answer_count\n");
				free(input);
				// TODO: Release shared memory
				exit(EXIT_FAILURE);
			}
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
			free(input);
			if (error)
			{
				free(error);
			}
			return NULL;
		}
		free(input);
		return NULL;
	}
	return NULL;
	free(input);
}

void start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId, struct SharedMemory *memory_ptr, sem_t *mutex, sem_t *sem_free_answers, sem_t *sem_answer_count)
{
	struct InputProcessingInfo *processing_info = malloc(sizeof(struct InputProcessingInfo));
	if (!processing_info)
	{
		printf("malloc() returned NULL while processing input\n");
		return;
	}
	processing_info->table = table;
	processing_info->input = strdup(input);
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