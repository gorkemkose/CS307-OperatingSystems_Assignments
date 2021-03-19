#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
  char fileName[20] = "loremipsum.txt";
  FILE *fp;
  
  clock_t begin = clock(); //clock is ticking
  fp = fopen(fileName, "r"); //open the file in read mode
  
  if (fp != NULL){ //if the opening is successful
    char c; //the current character will be stored inside c
    int count = 0; //number of occurences of "a"
    
    while((c = fgetc(fp)) != EOF){
      if (c == 'a'){
        count++;
      }
    }
    
    fclose(fp);
    clock_t end = clock(); //stop the clock
    
    double timeElapsed = ((double)(end-begin)) / CLOCKS_PER_SEC; //measure the elapsed time
    
    printf("The number of occurences of character a: %d\n", count);
    printf("The time elapsed: %f\n", timeElapsed);
  }
  
  return 0;
}