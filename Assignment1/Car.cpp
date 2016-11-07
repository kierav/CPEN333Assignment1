#include <stdio.h>
#include <sstream>
#include "Car.h"


Car::Car(char *name, int designatedPump){
	desPump = designatedPump;
	myData.creditCard = rand() % 10000000000000000;
	strcpy_s(myData.customerName, name);
	// default values for fuel
	myData.fuelType = OCT82;
	myData.fuelAmount = 50;
	printf("Creating customer %s and queing at Pump %d...\n", myData.customerName, desPump);
	// set up pump name
	std::ostringstream oss;
	string pumpName = "P";
	oss << desPump;
	pumpName += oss.str();

	myPipe = new CTypedPipe<struct customerData>(pumpName, 1);
	myPipeMutex = new CMutex(pumpName);
	
	//set up semaphores for queing at the pump
	string emptyName = "Empty";
	emptyName += oss.str();
	string fullName = "Full";
	fullName += oss.str();
	string entryName = "EntryQueue";
	entryName += oss.str();
	string exitName = "ExitQueue";
	exitName += oss.str();
	pumpEmpty = new CSemaphore(emptyName, 0, 1);
	pumpFull = new CSemaphore(fullName, 0, 1);
	pumpEntryQueue = new CSemaphore(entryName, 0, 1);
	pumpExitQueue = new CSemaphore(exitName, 0, 1);
}

void Car::setDesFuel(float maxDesFuel){
	myData.fuelAmount = maxDesFuel;
}
void Car::setFuelGrade(int fuelGrade){
	myData.fuelType = fuelGrade;
}

int Car::main(void){

	pumpEntryQueue->Wait(); //wait to use the pump
	pumpFull->Signal(); //indicate we are using the pump

	//send the data
	myPipeMutex->Wait();
	myPipe->Write(&myData);
	myPipeMutex->Signal();
	printf("Customer %s has swiped his car at Pump %d ... \n", myData.customerName, desPump);
	//printf("Writing customer data to Pump %d...\n", desPump);
	printf("Pipe got closed\n");

	pumpExitQueue->Wait(); //wait to leave the pump
	pumpEmpty->Signal(); //signal the pump is free
	return 0;
}
Car::~Car(){
	delete myPipe;
	delete myPipeMutex;
	delete pumpFull;
	delete pumpEmpty;
	delete pumpEntryQueue;
	delete pumpExitQueue;

}