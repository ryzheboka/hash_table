# Interactive hash table in C

### Getting Started
To try the hash table out, you first need to clone the repository and go to the root of the repository on your machine. Then you can use the provided Dockerfile  as following:
```
docker build -t hash_table .
docker run -it --rm --name my-running-app hash_table sh
```
Then, a shell should be open in the container. In this shell, you can open the server and the client in parallel. When using the command provided below, The server will run in the background as long as the client is running. The output of both, server and client, will be printed. The only argument of the server specifies the size of the table. The table in the example below will be able to hold maximum 10 entries without collisions. The table is not resizable, so the size specified here is final. However, collisions are resolved by maintaining a list for each entry.

```
./server 10 & ./client && fg
```
All information relevant for the client should be printed by the client. That's why server and client should be also able to run in different terminals. However, this option is not tested, because I only tested the application using the Dockerfile.

Then, you should wait for the following output:
```
Started client
Server initialized successfully!
Command:
```
Then, you can type a command for the hash table. The answer to this commands will be preceeded with "Answer:" . Here is an example containing possible input and corresponding output.
```
Command: read key
Command: Answer: The key "key" doesn't exist in the table!

```
Your input in the example below is "read key". After getting the answer, you need to press enter to give the next command. The formatting looks a little strange in this example. That's because an answer is printed as soon as it's there. Theoritically, it is possible to give two commands before becoming both answers after each other. However, you would need to either give the commands extremely fast, or add sleep() to the input processing function to simulate following example:

```
Command: read key
Command: read key2
Command: Answer: The key "key" doesn't exist in the table!

Answer: The key "key2" doesn't exist in the table!

```
Another example:

```
Command: write key1,value
Command: read key1
Command: Answer: The value for the key "key1": "value" 
```
After using the hash_table, you need to delete it and exit all related processes by typing exit:
```
Command: exit
Exiting client
Exit server!
./server 10
```

### All provided commands
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
- The code needs to be cleaned up and refactored :)
- The project doesn't have any automatic tests anymore. It was only tested manually. Parts of the project were automatically tested during implementation. You can find these tests in the project history. However, they were deleted because they didn't comply with the new interface. The project needs automatic tests.
- The server can only communicate with one client. It would be nice to be able to have multiple clients.
- The table cannot be resized.
- Multithreading lock isn't fine granular for the sake of readability.
- The "#include"s need to be less chaotic. 
- The Makefile needs to be improved by adding a "clean" option. Also, .h should be included in the Makefile so that the Makefile knows to execute goals after changed .h files.


