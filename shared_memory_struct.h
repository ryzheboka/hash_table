struct SharedMemory
{
	char requests[32][256];
	int request_write_index;
	int request_read_index;
	char answers[32][256];
	int answer_write_index;
	int answer_read_index;
};