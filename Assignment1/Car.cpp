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
	string pumpHoseReturnedName = "PumpHoseReturned";
	pumpHoseReturnedName += oss.str();
	string pumpHoseRemovedName = "PumoHoseRemoved";
	pumpHoseRemovedName += oss.str();
	pumpEmpty = new CSemaphore(emptyName, 0, 1);
	pumpFull = new CSemaphore(fullName, 0, 1);
	pumpEntryQueue = new CSemaphore(entryName, 0, 1);
	pumpExitQueue = new CSemaphore(exitName, 0, 1);
	pumpHoseRemoved = new CSemaphore(pumpHoseRemovedName, 0, 1);
	pumpHoseReturned = new CSemaphore(pumpHoseReturnedName, 0, 1);
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


	printf("Customer %s has swiped his card at Pump %d ... \n", myData.customerName, desPump);
	Sleep(1000);
	if (myData.fuelType == OCT82)
		printf("Customer %s has selected fuel grade OCT82... \n", myData.customerName);
	else if (myData.fuelType == OCT87)
		printf("Customer %s has selected fuel grade OCT87... \n", myData.customerName);
	else if (myData.fuelType == OCT92)
		printf("Customer %s has selected fuel grade OCT92... \n", myData.customerName);
	else if (myData.fuelType == OCT97)
		printf("Customer %s has selected fuel grade OCT97... \n", myData.customerName);
	Sleep(1000);
	pumpHoseRemoved->Signal();
	printf("Customer %s has removed gas hose and is awaiting GSC approval ... \n", myData.customerName, desPump);
	pumpHoseReturned->Wait();
	//printf("Writing customer data to Pump %d...\n", desPump);

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
	delete pumpHoseRemoved;
	delete pumpHoseReturned;
}