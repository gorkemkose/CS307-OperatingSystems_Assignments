#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int main(){
  int file = open("loremipsum.txt", O_RDONLY); //open the file in readOnly mode
  struct stat s;
  size_t size;
  int status = fstat(file, &s);
  size = s.st_size; //Get the size of the file
  
  clock_t begin = clock(); //clock is ticking
  char *ptr = mmap(0, size, PROT_READ, MAP_PRIVATE, file, 0); //call the mapping function to map the file to memory
  
  //error handling
  if (ptr == MAP_FAILED){
    printf("Mapping failed\n");
    return 1;
  }
  
  close(file); //file is mapped to memory, we can close it 
  ssize_t n = read(1, ptr,size); //either write or read is necessary, otherwise code does not compile :/
  
  int count = 0; //the number of occurences of "a"
  size_t i;
  for(i = 0; i <= size; i++){
    if (ptr[i] == 'a'){
      count++;
    }
  }
  
  status = munmap(ptr, size); //we processed everything, we will unmap to free the memory
  
  //error handling
  if (status != 0){
    printf("Unmapping failed\n");
    return 1;
  }
  
  clock_t end = clock(); //stop the clock
  
  double timeElapsed = ((double)(end-begin)) / CLOCKS_PER_SEC; //measure the time elapsed 
  
  printf("The number of occurences of character a: %d\n", count);
  printf("The time elapsed: %f\n", timeElapsed);
  
  return 0;
}
