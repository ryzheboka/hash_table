#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"
#include <semaphore.h>

struct InputProcessingInfo
{
	struct Table table;
	char *input;
	char *memory_ptr;
	sem_t *mutex;
};

void start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId, char *memory_ptr, sem_t *mutex);
void *process_input(void *process_input);
void destroy_lock();