#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct TableEntry
{
	char *key;
	char *value;
	char *error;
	struct TableEntry *next;
};

struct Table
{
	struct TableEntry **entries;
	unsigned long size;
	unsigned long number_of_elements;
};

struct Table *initialize_table(unsigned int size);

void delete_table(struct Table *table);

struct TableEntry *read_entry(struct Table table, char *key, pthread_rwlock_t *p);

char *insert_entry(struct Table table, char *key, char *value, pthread_rwlock_t *p);

char *delete_entry(struct Table table, char *key, pthread_rwlock_t *p);
unsigned long hash(char *key);