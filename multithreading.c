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
			return NULL;
		}
		// printf("Read lock succeeded\n");
		struct TableEntry *entry = read_entry(input->table, input->input + 5);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			return NULL;
		}
		if (entry)
			printf("The value for the key \"%s\": \"%s\" \n", entry->key, entry->value);
		return NULL;
	}

	if (strncmp(input->input, "delete ", 7) == 0)
	{
		if (pthread_rwlock_wrlock(p) != 0)
		{
			printf("Locking failed. Aborting operation");
			return NULL;
		}
		// printf("Read lock succeeded\n");
		delete_entry(input->table, input->input + 7);
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			return NULL;
		}
		// printf("The value for the key \"key\": \"%s\" \n", entry->value);
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
			return NULL;
		}

		// printf("Lock succeded!\n");
		insert_entry(input->table, key, value);
		if (pthread_rwlock_unlock(p) != 0)
		{

			printf("Unlocking failed. Aborting operation");
			return NULL;
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		}
		return NULL;
	}
	return NULL;
}

struct InputProcessingInfo *start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId)
{
	struct InputProcessingInfo *processing_info = malloc(sizeof(struct InputProcessingInfo));
	if (!processing_info)
	{
		printf("malloc() returned NULL while processing input\n");
		return NULL;
	}
	processing_info->table = table;
	processing_info->input = strdup(input);
	pthread_create(threadId, NULL, process_input, processing_info);
	return processing_info;
}

void destroy_lock()
{
	pthread_rwlock_destroy((pthread_rwlock_t *)&lock_rw);
}