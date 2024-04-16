#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"

struct InputProcessingInfo
{
	struct Table table;
	char *input;
};

struct InputProcessingInfo *start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId);
void *process_input(void *process_input);
void destroy_lock();