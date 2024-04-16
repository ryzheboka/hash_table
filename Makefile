server : main.c hash_table.c tests.c multithreading.c; gcc -Werror -Wall tests.c main.c hash_table.c multithreading.c -o main
client: client.c; gcc -Werror -Wall client.c -o client