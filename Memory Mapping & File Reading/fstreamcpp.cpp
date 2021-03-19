#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

int main(){
  ifstream myFile("loremipsum.txt");
  
  clock_t begin = clock(); //clock is ticking
  char character; //each character will be stored inside this variable
  int count = 0; //number of occurences of character "a"
  
  while(!myFile.eof()){
    myFile >> character; //read the current character to character variable
    if (character == 'a'){
      count++;
    }
  }
  
  myFile.close();
  clock_t end = clock(); //stop the clock
  
  double timeElapsed = double(end-begin) / CLOCKS_PER_SEC; //measure the time elapsed
  
  cout << "The number of occurences of character a is " << count << endl;
  cout << "The running time: " << timeElapsed << endl;
  
  return 0;
}