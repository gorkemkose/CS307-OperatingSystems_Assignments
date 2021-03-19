#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

int M[2][50];
int occupiedSeats = 0;
int turn = 1;
bool noSeatsLeft = false;

//a function to generate random numbers between 1-100
int randomSeatGenerator(){
	return rand() % 100 + 1;
}

//this function is used by both of the threads, param is the id of the thread
void *agency_function(void* param){
	int *idPointer = (int*) param;

	while (occupiedSeats < 100){ //while plane is not full, threads can be scheduled and make reservation
		while (turn != *idPointer){}; //busy waiting

		if (occupiedSeats == 100) noSeatsLeft = true; //this is for printing "No Seats Left" message at the end

			printf("Agency %d Entered Critical Region\n", *idPointer); //*idPointer is the agency id which is either 1 or 2

			int seatNum = randomSeatGenerator();
			int row, column;

			//determining the row and column numbers with a given random seat number
			if (seatNum < 51){
				if (seatNum == 50){
					row = 0;
					column = 49;
				}
				else{
					row = 0;
					column = seatNum % 50 -1;
				}
			}
			else{
				if (seatNum == 100){
					row = 1;
					column = 49;
				}
				else{
					row = 1;
					column = seatNum % 50 -1;
				}
			}

			//reserve the seat, increment the occupied seats and message
			if (!M[row][column]){ //if it is 0, which means it is not reserved yet
				M[row][column] = *idPointer;
				occupiedSeats++;
				printf("Seat number %d is reserved by Agency %d\n", seatNum, *idPointer);
			}

			//we are leaving the critical region and switch the turn soon!!
			printf("Agency %d Exit Critical Region\n", *idPointer);

			if (noSeatsLeft) printf("No Seats Left\n");
			printf("\n");

			//turn is switching to the other thread
			if (turn == 1)
				turn = 2;
			else turn = 1;
	}
}

int main(){

	srand(time(NULL));

	//matrix initialization
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 50; j++){
			M[i][j] = 0;
		}
	}

	//thread initialization
	pthread_t agency1, agency2;
	int agencyid1 = 1, agencyid2 = 2;
	int agency1result = pthread_create(&agency1, NULL, agency_function, (void*) &agencyid1);
	int agency2result = pthread_create(&agency2, NULL, agency_function, (void*) &agencyid2);

	//threads will join to the main thread when occupiedSeats == 100
	pthread_join(agency1, NULL);
	pthread_join(agency2, NULL);

	//when they join, there is no reservation possible because all of the seats are reserved by either agency 1 or agency 2
	printf("Plane is full:\n\n");

	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 50; j++){
			printf("%d ", M[i][j]); 
		}
		printf("\n");
	}

	return 0;
}