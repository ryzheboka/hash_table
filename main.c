// #include "hash_table.h"
#include "tests.h"
#include <stdlib.h>
#include <stdio.h>
// #include <pthread.h>
#include <string.h>

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
    // test(*hash_table);
    test_concurrent(*hash_table);
    // 4. Print your result
    delete_table(hash_table);
    return EXIT_SUCCESS;
}