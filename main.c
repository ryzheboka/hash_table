#include "communication_with_clients.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Invalid number of arguments!\n");
        return EXIT_FAILURE;
    }
    unsigned long arg = strtoul(argv[1], NULL, 10);
    if (errno != 0 || arg == 0)
    {
        printf("Invalid parameter! Parameter must be a positive number\n");
        return EXIT_FAILURE;
    }
    struct Table *hash_table = initialize_table(arg);
    communicate_with_clients(*hash_table);
    destroy_lock();
    delete_table(hash_table);
    return EXIT_SUCCESS;
}