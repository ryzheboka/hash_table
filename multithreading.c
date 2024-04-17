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
			return NULL;
		}
		// printf("Read lock succeeded\n");
		struct TableEntry *entry = read_entry(input->table, input->input + 5);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			free(input);
			return NULL;
		}
		if (entry)
		{
			// printf("The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);
			if (sem_wait(input->mutex) == -1)
			{
				printf("Error while waiting for PROCESSED_COMMAND mutex\n");
				// TODO: Release shared memory
				return NULL;
			}
			snprintf(input->memory_ptr, 30 + strlen(entry->key) + strlen(entry->value), "The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);
			if (sem_post(input->mutex) == -1)
			{
				printf("Error while waiting for PROCESSED_COMMAND mutex\n");
				// TODO: Release shared memory
				return NULL;
			}
		}
		return NULL;
	}

	if (strncmp(input->input, "delete ", 7) == 0)
	{
		if (pthread_rwlock_wrlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			free(input);

			return NULL;
		}
		// printf("Read lock succeeded\n");
		delete_entry(input->table, input->input + 7);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			free(input);

			return NULL;
		}
		// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		free(input);

		return NULL;
	}

	if (strncmp(input->input, "write ", 6) == 0)
	{
		char *attributes;
		attributes = input->input + 6;
		char *key = strsep(&attributes, ",");
		char *value = strsep(&attributes, ",");
		// printf("kkkey: %s, vvvvlue: %s\n", key, value);

		if (pthread_rwlock_wrlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			free(input);

			return NULL;
		}

		// printf("Lock succeded!\n");
		insert_entry(input->table, key, value);
		if (pthread_rwlock_unlock(p) != 0)
		{

			printf("Unlocking failed. Aborting operation");
			free(input);

			return NULL;
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		}
		free(input);
		return NULL;
	}
	printf("The command doesn't exist!");
	return NULL;
	free(input);
}

void start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId, char *memory_ptr, sem_t *mutex)
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