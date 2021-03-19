#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 10
#define MEMORY_SIZE 150

struct node
{
	int id;
	int size;
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size

int currentMemoryIndex = 0; //to keep track of the current memory index that can be the start point of the next allocation

bool notjoined = true; //this is a bool variable to terminate the execution of the server function

void my_malloc(int thread_id, int size)
{
	//This function will add the struct to the queue
	node willBeAdded; //node creation
	willBeAdded.id = thread_id; 
	willBeAdded.size = size;
	myqueue.push(willBeAdded);
}

void * server_function(void *)
{
	//This function will run as long as the program duration until the threads are joined in the main
	//This function should grant or decline a thread depending on memory size.
	
	while(notjoined){
		//we take item by item in the queue
		while(myqueue.empty() == false){
			node currentNode = myqueue.front(); //take the first item in the queue
			int id = currentNode.id;
			int size = currentNode.size;
			myqueue.pop(); //remove that item in consideration
			if (MEMORY_SIZE - currentMemoryIndex >= size){ //if there is enough space in memory
				thread_message[id] = currentMemoryIndex; //update the corresponding index of the thread_message array to the starting point of the allocation
				currentMemoryIndex = currentMemoryIndex + size; //update the next starting point of allocation by incrementing by size
			}
			else{ //if there is not enough space in memory
				thread_message[id] = -1; //the message for that thread is -1 to indicate not enough memory
			}
			sem_post(&semlist[id]); //the request is evaluated, we can make an up on that threads semaphore so that it can continue to execute.
		}
	}
	return 0;
}

//This function will create a random size, and call my_malloc
void * thread_function(void * id) 
{
	//create the random size between 1 and MEMORY_SIZE/6
	int maxRandom = MEMORY_SIZE / 6;
	int randomSize = rand() % maxRandom + 1; 

	int * ptr = (int *) id; //we will cast the void type into an int pointer type because the id passed here is not the id but its address in the memory. 
	//whenever we need the value of the id of the thread, we will use dereferencing. 
	
	pthread_mutex_lock(&sharedLock); //we will lock the mutex because inside of the my_malloc function we access a shared variable queue
	my_malloc(*ptr, randomSize);

	//Block
	sem_wait(&semlist[*ptr]); //this is an up operation. we block that thread until the request is processed by the server thread
	//the up operation will be done by the server thread after the evaluation
	
	//Then fill the memory with id's or give an error prompt
	if (thread_message[*ptr] != -1){ //after the evaluation by the server thread, if the message coming back is NOT -1, then there is available memory
		int startIndex = currentMemoryIndex - randomSize; //this is the start address of the allocation. 
		for (int j = startIndex; j < currentMemoryIndex; j++){ //starting from the start point, until the next slot of allocation, we will update the memory array
			char value = '0' + *ptr; //this is the conversion from int to char
			memory[j] = value; //update on the memory array
		}
	}
	else{ //then there is no available memory since the coming message is -1 from server thread
		cout << "Thread " << *ptr << ": Not enough memory\n" << endl;
	}
	
	pthread_mutex_unlock(&sharedLock); //we unlock because we exit from the critical region
	return 0; //this is because each thread will request a memory allocation only once and we should terminate the thread execution after the request and response is done. 
}

void init()	 
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
  	{char zero = '0'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory() 
{
 // You need to print the whole memory array here.
	cout << "Content of the memory" << endl;
	for(int k = 0; k < MEMORY_SIZE; k++){
		cout << memory[k];
	}
}

int main (int argc, char *argv[])
 {
	 //You need to create a thread ID array here
	 int id_array[NUM_THREADS];
	 for(int id = 0; id < NUM_THREADS; id++){
		id_array[id] = id;
	 }

 	init();	// call init
	srand(time(NULL)); //to randomize

 	//You need to create threads with using thread ID array, using pthread_create()
	pthread_t threads[NUM_THREADS];
	for(int th = 0; th < NUM_THREADS; th++){
		pthread_create(&threads[th], NULL, thread_function, (void *) & id_array[th]);
	}

 	//You need to join the threads
	for (int t= 0; t < NUM_THREADS; t++){
		pthread_join(threads[t], NULL);
	}
	
 	dump_memory(); // print the content of the memory
 	
 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
 	}

	notjoined = false; //this is for the termination of the server thread.
 	printf("\nTerminating...\n");
	return 0;
 }