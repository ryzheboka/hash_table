#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct TableEntry
{
	char *key;
	char *value;
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

struct TableEntry *read_entry(struct Table table, char *key);

void insert_entry(struct Table table, char *key, char *value);

void delete_entry(struct Table table, char *key);
unsigned long hash(char *key);