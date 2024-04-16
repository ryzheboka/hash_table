#include "tests.h"

unsigned int MAX_NUMBER_OF_THREADS = 10;

/*void test(struct Table table)
{
	read_entry(table, "non existent");
	delete_entry(table, "another non existent key");
	insert_entry(table, "key", "value");
	insert_entry(table, "key", "value");
	insert_entry(table, "key2", "value2");
	insert_entry(table, "key3", "");
	insert_entry(table, "key4", "value4");
	insert_entry(table, "key5", "value5");
	insert_entry(table, "key6", "value6");

	struct TableEntry *entry = read_entry(table, "key");
	struct TableEntry *entry2 = read_entry(table, "key2");
	struct TableEntry *entry3 = read_entry(table, "key3");
	struct TableEntry *entry4 = read_entry(table, "key4");
	struct TableEntry *entry5 = read_entry(table, "key5");
	struct TableEntry *entry6 = read_entry(table, "key6");

	//    printf("\nTestMain: %p \n", (&(entry->key)));
	printf("The value for the key \"key\": \"%s\" \n", entry->value);
	printf("The value for the key \"key2\": \"%s\" \n", entry2->value);
	printf("The value for the key \"key3\": \"%s\" \n", entry3->value);
	printf("The value for the key \"key4\": \"%s\" \n", entry4->value);
	printf("The value for the key \"key5\": \"%s\" \n", entry5->value);
	printf("The value for the key \"key6\": \"%s\" \n", entry6->value);

	delete_entry(table, "key2");
	printf("deleted key2 entry\n");
	read_entry(table, "key2");
}*/

void test_concurrent(struct Table table)
{
	printf("starting concurrent test\n");

	pthread_t *thread_ids = malloc(sizeof(pthread_t) * MAX_NUMBER_OF_THREADS);
	struct InputProcessingInfo *processing_info = start_multithreaded_input_processing(table, "read key", &thread_ids[0]);
	struct InputProcessingInfo *processing_info2 = start_multithreaded_input_processing(table, "write concurrent, new_val", &thread_ids[1]);
	struct InputProcessingInfo *processing_info4 = start_multithreaded_input_processing(table, "write concurrent2, new_val2", &thread_ids[2]);
	struct InputProcessingInfo *processing_info3 = start_multithreaded_input_processing(table, "read concurrent", &thread_ids[3]);
	struct InputProcessingInfo *processing_info8 = start_multithreaded_input_processing(table, "read concurrent", &thread_ids[7]);
	struct InputProcessingInfo *processing_info5 = start_multithreaded_input_processing(table, "read concurrent2", &thread_ids[4]);
	struct InputProcessingInfo *processing_info6 = start_multithreaded_input_processing(table, "delete concurrent", &thread_ids[5]);
	struct InputProcessingInfo *processing_info9 = start_multithreaded_input_processing(table, "write concurrent3, new_val3", &thread_ids[8]);
	struct InputProcessingInfo *processing_info7 = start_multithreaded_input_processing(table, "read concurrent", &thread_ids[9]);

	if (!processing_info3)
	{
		printf("malloc() returned NULL while processing input\n");
	}

	pthread_join(thread_ids[1], NULL);
	pthread_join(thread_ids[0], NULL);
	pthread_join(thread_ids[2], NULL);
	pthread_join(thread_ids[3], NULL);
	pthread_join(thread_ids[4], NULL);
	pthread_join(thread_ids[5], NULL);
	pthread_join(thread_ids[6], NULL);
	pthread_join(thread_ids[7], NULL);
	pthread_join(thread_ids[8], NULL);

	// if (strcmp(input->input, "exit") == 0)
	//{
	// printf("Read lock succeeded\n");
	// delete_table((struct Table *)&table);
	free(processing_info);
	free(processing_info2);
	free(processing_info3);
	free(processing_info4);
	free(processing_info5);
	free(processing_info6);
	free(processing_info7);
	free(processing_info8);
	free(processing_info9);

	// printf("The value for the key \"key\": \"%s\" \n", entry->value);
	// free(thread_ids);
	destroy_lock();
}