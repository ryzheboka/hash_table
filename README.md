# Interactive hash table in C

### Getting Started
To try the hash table out, you first need to clone the repository and go to the root of the repository on your machine. Then you can use the provided Dockerfile  as following:
```
docker build -t hash_table .
docker run -it --rm --name my-running-app hash_table sh
```
Then, a shell should be open in the container. You can start the server here by running

```
./server 10
```

The only argument of the server specifies the size of the table. The table in the example below will be able to hold maximum 10 entries without collisions. The table is not resizable, so the size specified here is final. However, collisions are resolved by maintaining a list for each entry.

To use one or multiple clients, you will need more shells. First, you need to look up the id of the container that was started by the docker run command. Then, execute the following for each additional client:
```
docker exec -it <your docker container id> sh
```
Then 
```
./client
```

Then, you should wait for the following output:
```
Started client
Command:
```
Then, you can type a command for the hash table. The answer to this commands will be preceeded with "Answer:" . Here is an example containing possible input and corresponding output.
```
Command: read key
Answer: The key "key" doesn't exist in the table!


```
Another example:

```
Command: write key1,value
Command: read key1
Answer: The value for the key "key1": "value" 
```
After using the hash_table, you need to exit the clients and the server. To exit a client, type "exit" as command. Right now, exiting the server correctly isn't possible. Press ctrl+c to exit the server
```
Command: exit
Exiting client
```

### All provided commands for the client
- read key
- write key, value
- delete key
- exit

### Error Handling
The commands are checked for correctness. If you do one of the following, the client will print a message. No actions on the table will be performed:

- use "write" without ","
- try to write with an existing key
- try to delete with a non existing key
- try to read with a non existing key

If you type a non existing command, you will get the "Command:" output in the newline. No other action will be preformed. 

It might happen, that one of the system related calls e. g. "malloc()" will fail inside the project. In this case, the programm should automatically exit with an error. However, the handling of such errors in this project needs to be improved.

All in all, the error handling needs to be done more consistently. It's one of the aspects that need to be improved about this project.


### Other limitations
- The possiblity to exit the server correctly is really needed!
- The code needs to be cleaned up and refactored :)
- The project doesn't have any automatic tests anymore. It was only tested manually. Parts of the project were automatically tested during implementation. You can find these tests in the project history. However, they were deleted because they didn't comply with the new interface. The project needs automatic tests.
- The table cannot be resized.
- The "#include"s need to be less chaotic. 
- The Makefile needs to be improved by adding a "clean" option. Also, .h should be included in the Makefile so that the Makefile knows to execute goals after changed .h files.


