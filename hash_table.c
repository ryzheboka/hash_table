#include "hash_table.h"

struct Table *initialize_table(unsigned int size)
{
	struct Table *hash_table = malloc(sizeof(struct Table));
	if (!hash_table)
	{
		printf("malloc() returned NULL while initializing table\n");
		exit(EXIT_FAILURE);
	}
	hash_table->size = size;
	hash_table->entries = calloc(size, sizeof(struct TableEntry *));
	if (!hash_table->entries)
	{
		printf("malloc() returned NULL while initializing table\n");
		exit(EXIT_FAILURE);
	}
	hash_table->number_of_elements = 0;
	return hash_table;
}
char *insert_entry(struct Table table, char *key, char *value, pthread_rwlock_t *p)
{
	struct TableEntry *newEntry = malloc(sizeof(struct TableEntry));
	if (!newEntry)
	{
		printf("malloc() returned NULL while initializing new entry!");
		exit(EXIT_FAILURE);
	}
	newEntry->key = strdup(key);
	if (!newEntry->key)
	{
		printf("malloc() returned NULL while initializing new entry!");
		exit(EXIT_FAILURE);
	}

	newEntry->value = strdup(value);
	if (!newEntry->value)
	{
		printf("malloc() returned NULL while initializing new entry!");
		exit(EXIT_FAILURE);
	}
	newEntry->next = NULL;
	newEntry->error = NULL;
	unsigned long index = hash(key) % table.size;
	if (pthread_rwlock_wrlock(p) != 0)
	{
		printf("Locking failed. Aborting operation");
		// free(input);
		exit(EXIT_FAILURE);
	}

	if (table.entries[index] == 0)
	{
		table.entries[index] = newEntry;
		table.number_of_elements++;

		if (pthread_rwlock_unlock(p) != 0)
		{

			printf("Unlocking failed. Aborting operation");
			exit(EXIT_FAILURE);
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		}
		char *error = malloc(3);
		snprintf(error, 3, "Ok");
		return error;
	}
	else
	{
		struct TableEntry *currentEntry = table.entries[index];
		if (strcmp(currentEntry->key, key) == 0)
		{
			if (pthread_rwlock_unlock(p) != 0)
			{

				printf("Unlocking failed. Aborting operation");
				exit(EXIT_FAILURE);
				// printf("The value for the key \"key\": \"%s\" \n", entry->value);
			}
			char *error = malloc(38 + sizeof(key));
			snprintf(error, 50 + sizeof(key), "The key \"%s\" already exists and can't be inserted!", key);
			return error;
		}
		while (currentEntry->next != 0)
		{
			if (strcmp(currentEntry->key, key) == 0)
			{
				if (pthread_rwlock_unlock(p) != 0)
				{

					printf("Unlocking failed. Aborting operation");
					exit(EXIT_FAILURE);
					// printf("The value for the key \"key\": \"%s\" \n", entry->value);
				}
				char *error = malloc(38 + sizeof(key));
				snprintf(error, 50 + sizeof(key), "The key \"%s\" already exists and can't be inserted!", key);
				return error;
			}
			currentEntry = currentEntry->next;
		}
		currentEntry->next = newEntry;
	}

	if (pthread_rwlock_unlock(p) != 0)
	{

		printf("Unlocking failed. Aborting operation");

		exit(EXIT_FAILURE);
		// printf("The value for the key \"key\": \"%s\" \n", entry->value);
	}
	char *error = malloc(3);
	snprintf(error, 3, "Ok");
	return error;
}
struct TableEntry *read_entry(struct Table table, char *key, pthread_rwlock_t *p)
{
	unsigned long index = hash(key) % table.size;
	if (pthread_rwlock_rdlock(p) != 0)
	{
		printf("Locking failed. Aborting operation");
		exit(EXIT_FAILURE);
	}

	struct TableEntry *current_entry = table.entries[index];

	if (current_entry != 0 && strcmp((current_entry->key), key) == 0)
	{
		if (pthread_rwlock_unlock(p) != 0)
		{
			printf("Unlocking failed. Aborting operation");
			exit(EXIT_FAILURE);
		}
		return current_entry;
	}

	while (current_entry != 0 && current_entry->next != 0)
	{
		current_entry = current_entry->next;
		if (strcmp(current_entry->key, key) == 0)
		{
			if (pthread_rwlock_unlock(p) != 0)
			{
				printf("Unlocking failed. Aborting operation");
				exit(EXIT_FAILURE);
			}
			return current_entry;
		}
	}
	if (pthread_rwlock_unlock(p) != 0)
	{
		printf("Unlocking failed. Aborting operation");
		exit(EXIT_FAILURE);
	}
	struct TableEntry *error_entry = calloc(1, sizeof(struct TableEntry));
	error_entry->error = malloc(38 + sizeof(key));
	snprintf(error_entry->error, 38 + sizeof(key), "The key \"%s\" doesn't exist in the table!\n", key);

	return error_entry;
}

char *delete_entry(struct Table table, char *key, pthread_rwlock_t *p)
{
	unsigned long index = hash(key) % table.size;
	if (p != NULL && pthread_rwlock_wrlock(p) != 0)
	{
		printf("Locking failed. Aborting operation");
		// free(input);
		exit(EXIT_FAILURE);
	}

	struct TableEntry *current_entry = table.entries[index];

	if (current_entry != 0 && strcmp((current_entry->key), key) == 0)
	{
		table.number_of_elements -= 1;
		table.entries[index] = 0;
		if (p != NULL && pthread_rwlock_unlock(p) != 0)
		{

			printf("Unlocking failed. Aborting operation");

			exit(EXIT_FAILURE);
			// printf("The value for the key \"key\": \"%s\" \n", entry->value);
		}
		free(current_entry->key);
		free(current_entry->value);
		free(current_entry);
		char *error = malloc(3);
		snprintf(error, 3, "Ok");
		return error;
	}

	while (current_entry != 0 && current_entry->next != 0)
	{
		if (strcmp(current_entry->next->key, key) == 0)
		{
			table.number_of_elements -= 1;
			current_entry->next = current_entry->next->next;
			if (p != NULL && pthread_rwlock_unlock(p) != 0)
			{

				printf("Unlocking failed. Aborting operation");

				exit(EXIT_FAILURE);
				// printf("The value for the key \"key\": \"%s\" \n", entry->value);
			}
			free(current_entry->next->key);
			free(current_entry->next->value);
			free(current_entry->next);
			char *error = malloc(3);
			snprintf(error, 3, "Ok");
			return error;
		}
		current_entry = current_entry->next;
	}
	if (p != NULL && pthread_rwlock_unlock(p) != 0)
	{

		printf("Unlocking failed. Aborting operation");

		exit(EXIT_FAILURE);
		// printf("The value for the key \"key\": \"%s\" \n", entry->value);
	}
	char *error = malloc(38 + sizeof(key));
	snprintf(error, 38 + sizeof(key), "The key \"%s\" doesn't exist in the table!\n", key);

	return error;
}

void delete_table(struct Table *table)
{
	for (int i = 0; i < table->number_of_elements; i++)
	{
		delete_entry(*table, table->entries[i]->key, NULL);
	}
	free(table->entries);
	free(table);
}

// https://stackoverflow.com/questions/7666509/hash-function-for-string
unsigned long hash(char *key)
{
	unsigned long hash = 5381;
	unsigned char c;
	while ((c = *key++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}