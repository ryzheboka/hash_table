#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
void insert_entry(struct Table table, char *key, char *value)
{
	struct TableEntry *newEntry = malloc(sizeof(struct TableEntry));
	if (!newEntry)
	{
		printf("malloc() returned NULL while initializing new entry!");
		return;
	}
	newEntry->key = strdup(key);
	if (!newEntry->key)
	{
		printf("malloc() returned NULL while initializing new entry!");
		return;
	}

	newEntry->value = strdup(value);
	if (!newEntry->value)
	{
		printf("malloc() returned NULL while initializing new entry!");
		return;
	}
	newEntry->next = NULL;
	unsigned long index = hash(key) % table.size;

	if (table.entries[index] == 0)
	{
		table.entries[index] = newEntry;
		table.number_of_elements++;
		return;
	}
	else
	{
		printf("Collision!\n");
		struct TableEntry *currentEntry = table.entries[index];
		if (strcmp(currentEntry->key, key) == 0)
		{
			printf("The key already exists and can't be inserted!\n");
			return;
		}
		while (currentEntry->next)
		{
			if (strcmp(currentEntry->key, key) == 0)
			{
				printf("The key already exists and can't be inserted!\n");
				return;
			}
			currentEntry = currentEntry->next;
		}
		currentEntry->next = newEntry;
	}

	return;
}
struct TableEntry *read_entry(struct Table table, char *key)
{
	unsigned long index = hash(key) % table.size;
	struct TableEntry *current_entry = table.entries[index];

	if (current_entry != 0 && strcmp((current_entry->key), key) == 0)
	{
		return current_entry;
	}

	while (current_entry != 0 && current_entry->next != NULL)
	{
		current_entry = current_entry->next;
		if (strcmp(current_entry->key, key) == 0)
		{
			return current_entry;
		}
	}

	printf("The key \"%s\" doesn't exist in the table!\n", key);

	return table.entries[index];
}

void delete_entry_by_key(struct Table table, char *key)
{
	struct TableEntry *entry = read_entry(table, key);
	if (entry)
		delete_entry(table, entry);
}

void delete_entry(struct Table table, struct TableEntry *entry)
{
	table.number_of_elements -= 1;
	table.entries[hash(entry->key) % table.size] = 0;
	free(entry->key);
	free(entry->value);
	free(entry);
}

void delete_table(struct Table *table)
{
	for (int i = 0; i < table->number_of_elements; i++)
	{
		delete_entry(*table, table->entries[i]);
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