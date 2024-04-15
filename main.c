#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

struct InputProcessingInfo
{
    struct Table table;
    char *input;
};

pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;

void test(struct Table table);
void test_concurrent(struct Table table);

int main(int argc, char **argv)
{
    // 1. Validate the number of arguments
    if (argc != 2)
    {
        printf("Invalid number of arguments!\n");
        return EXIT_FAILURE;
    }
    // 2. Use strtoul(...) to convert a string to an unsigned long
    unsigned long arg = strtoul(argv[1], NULL, 10);
    // 3. Call fib(n)
    struct Table *hash_table = initialize_table(arg);
    test(*hash_table);
    test_concurrent(*hash_table);
    // 4. Print your result
    delete_table(hash_table);
    return EXIT_SUCCESS;
}

void test(struct Table table)
{
    read_entry(table, "non existent");
    delete_entry_by_key(table, "another non existent key");
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

    delete_entry_by_key(table, "key2");
    printf("deleted key2 entry\n");
    read_entry(table, "key2");
}

void *process_input(void *process_input)
{
    struct InputProcessingInfo *input = process_input;
    pthread_rwlock_t *p = (pthread_rwlock_t *)&lock_rw;
    // printf("Printting from a thread!\n");
    // printf(input->input);
    // printf("%d\n", strncmp(input->input, "read ", 5));
    if (strncmp(input->input, "read ", 5) == 0)
    {
        if (pthread_rwlock_rdlock(p) != 0)
        {
            printf("Locking failed. Aborting operation");
            return NULL;
        }
        printf("Read lock succeeded\n");
        struct TableEntry *entry = read_entry(input->table, input->input + 5);
        if (pthread_rwlock_unlock(p) != 0)
        {
            printf("Unlocking failed. Aborting operation");
            return NULL;
        }
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
        delete_entry_by_key(input->table, input->input + 5);
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
        printf("kkkey: %s, vvvvlue: %s\n", key, value);

        if (pthread_rwlock_wrlock(p) != 0)
        {
            printf("Locking failed. Aborting operation");
            return NULL;
        }

        printf("Lock succeded!\n");
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
void test_concurrent(struct Table table)
{
    printf("starting concurrent test\n");
    struct InputProcessingInfo *processing_info = malloc(sizeof(struct InputProcessingInfo));
    if (!processing_info)
    {
        printf("malloc() returned NULL while processing input\n");
    }
    processing_info->table = table;

    /*test 1: read "key"*/
    processing_info->input = strdup("read key");
    pthread_t *thread_ids = malloc(sizeof(pthread_t) * 4);
    printf("Before Thread\n");
    pthread_create(&thread_ids[0], NULL, process_input, processing_info);
    printf("After Thread\n");
    struct InputProcessingInfo *processing_info2 = malloc(sizeof(struct InputProcessingInfo));
    if (!processing_info2)
    {
        printf("malloc() returned NULL while processing input\n");
    }
    processing_info2->table = table;
    /*test 2: write "concurrent"*/
    processing_info2->input = strdup("write concurrent, new_val");
    pthread_create(&thread_ids[1], NULL, process_input, processing_info2);
    struct InputProcessingInfo *processing_info3 = malloc(sizeof(struct InputProcessingInfo));
    if (!processing_info3)
    {
        printf("malloc() returned NULL while processing input\n");
    }
    processing_info3->table = table;
    /*test 2: write "concurrent"*/
    processing_info3->input = strdup("read concurrent");
    pthread_create(&thread_ids[2], NULL, process_input, processing_info3);
    pthread_join(thread_ids[1], NULL);
    pthread_join(thread_ids[0], NULL);
    pthread_join(thread_ids[2], NULL);

    // if (strcmp(input->input, "exit") == 0)
    //{
    if (pthread_rwlock_wrlock((pthread_rwlock_t *)&lock_rw) != 0)
    {
        printf("Locking failed. Aborting operation");
        return;
    }
    // printf("Read lock succeeded\n");
    // delete_table((struct Table *)&table);
    free(processing_info);
    free(processing_info2);
    free(processing_info3);
    if (pthread_rwlock_unlock((pthread_rwlock_t *)&lock_rw) != 0)
    {
        printf("Unlocking failed. Aborting operation");
        return;
    }
    // printf("The value for the key \"key\": \"%s\" \n", entry->value);
    // free(thread_ids);
    pthread_rwlock_destroy((pthread_rwlock_t *)&lock_rw);
}