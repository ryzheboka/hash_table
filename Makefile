server : main.c hash_table.c multithreading.c; gcc -Werror -Wall main.c hash_table.c multithreading.c -o main
client: client.c; gcc -Werror -Wall client.c -o client