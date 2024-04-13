#include "hash_table.h"
#include <stdlib.h>
#include <stdio.h>

void test(struct Table table);

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
    read_entry(table, "key2");
}
