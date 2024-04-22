#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash_table.h"
#include "shared_memory_struct.h"
#include <semaphore.h>

struct InputProcessingInfo
{
	struct Table table;
	char *input;
	struct SharedMemory *memory_ptr;
	sem_t *mutex;
};

void start_multithreaded_input_processing(struct Table table, char *input, pthread_t *threadId, char *memory_ptr, sem_t *mutex, sem_t *sem_free_answers.sem_t *sem_answer_count);
void *process_input(void *process_input);
void destroy_lock();