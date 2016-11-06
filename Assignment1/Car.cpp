#include <stdio.h>
#include <sstream>
#include "Car.h"


Car::Car(int carID, int designatedPump){
	desPump = designatedPump;
	myData.creditCard = rand() % 10000000000000000;
	myData.customerName = carID;
	// default values for fuel
	myData.fuelType = OCT82;
	myData.fuelAmount = 50;
	printf("Creating car %d and going to Pump %d...\n", myData.customerName, desPump);
}

void Car::setDesFuel(float maxDesFuel){
	myData.fuelAmount = maxDesFuel;
}
void Car::setFuelGrade(int fuelGrade){
	myData.fuelType = fuelGrade;
}

int Car::main(void){
	// set up pump name
	std::ostringstream oss;
	string pumpName = "P";
	oss << desPump;
	pumpName += oss.str();

	myPipe = new CTypedPipe<struct customerData>(pumpName, 1);
	myPipeMutex = new CMutex(pumpName);
	
	printf("My data is name: %d, credit card %d, fuel type %d, amount %2.1f\n", myData.customerName, myData.creditCard, myData.fuelType, myData.fuelAmount);
	myPipeMutex->Wait();
	myPipe->Write(&myData);
	myPipeMutex->Signal();
	printf("Writing customer data to Pump %d...\n", desPump);
	
	getchar();
	printf("Pipe got closed\n");
	return 0;
}