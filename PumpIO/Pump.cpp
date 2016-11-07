#include <stdio.h>
#include <sstream>
#include "Pump.h"

Pump::Pump(int pumpID){
	myID = pumpID;
	//printf("Pump %d being constructed...\n", myID);
	screenMutex = new CMutex("PumpScreen");
	// pipeline for customer data, one customer at a time
	myPipe = new CTypedPipe<struct customerData>(pumpName, 1);
	myPipeMutex = new CMutex(pumpName, OWNED);

	//set up semaphores for queing at the pump
	std::ostringstream oss;
	oss << myID;
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
Pump::~Pump(){
	delete pumpEmpty;
	delete pumpFull;
	delete pumpEntryQueue;
	delete pumpExitQueue;
	delete myPumpData;
	delete fuelData;
	delete myPipe;
	delete myPipeMutex;
	delete screenMutex;
	delete ps;
	delete cs;
	delete tank;
}

int Pump::readCustomerPipelineThread(void *ThreadArgs){
	struct customerData currentCustomer;

	Sleep(1000);

	screenMutex->Wait();
	MOVE_CURSOR(0, ((myID-1)*6));
	printf("Starting to poll pipe for pump %d...\n", myID);
	fflush(stdout);
	screenMutex->Signal();

	while(1){
		myPipeMutex->Wait();
		myPipe->Read(&currentCustomer);
		myPipeMutex->Signal();
		screenMutex->Wait();
		MOVE_CURSOR(0, ((myID - 1) * 6)); 
		printf("[PUMP %i IN USE] \n"
				"Name:%s \n"
				"Credit Card: %i \n"
				"Fuel Type: %i \n"
				"Fuel Amount: %i \n", 
				myID,
				currentCustomer.customerName, 
				currentCustomer.creditCard,
				currentCustomer.fuelType,
				currentCustomer.fuelAmount);
		fflush(stdout);
		screenMutex->Signal();

		cs->Wait(); 
		strcpy_s(myPumpData->customerName, currentCustomer.customerName);
		myPumpData->creditCard = currentCustomer.creditCard;
		myPumpData->fuelType = currentCustomer.fuelType;
		myPumpData->fuelAmount = currentCustomer.fuelAmount;
		ps->Signal();
		//printf("Pump %d produced customer data for GSC\n", myID);
		Sleep(10);
	}

	return 0;
}

int Pump::displayPumpDataThread(void *ThreadArgs){
	//TODO: figure out what to put on this display
	return 0;
}

int Pump::main(void){
	// set up pump name
	std::ostringstream oss;
	pumpName = "P";
	oss << myID;
	pumpName += oss.str();
	ps = new CSemaphore("PS" + oss.str(), 0, 1);
	cs = new CSemaphore("CS" + oss.str(), 1, 1);

	// datapool to store shared data with the GSC
	CDataPool myPumpDataPool("Pump"+oss.str(), sizeof(struct pumpData));
	myPumpData = (struct pumpData *)(myPumpDataPool.LinkDataPool());
	//printf("Pump %d linked to pump datapool at address %p...\n", myID, myPumpData);

	// datapool to store shared data with the fuel tank
	CDataPool fuelDataPool("FuelTank", sizeof(struct fuelTankData));
	fuelData = (struct fuelTankData *)(fuelDataPool.LinkDataPool());
	//printf("Pump %d linked to fuel datapool at address %p...\n", myID, fuelData);
	
	// create thread to read pipeline in suspended state
	ClassThread<Pump> pipelineThread(this, &Pump::readCustomerPipelineThread, ACTIVE, NULL);
	// create thread to display data on the screen
	ClassThread<Pump> displayThread(this, &Pump::displayPumpDataThread, ACTIVE, NULL);

	tank = new FuelTank();

	Sleep(2000);
	
	while (1){

	}

	displayThread.WaitForThread();
	pipelineThread.WaitForThread();

	return 0;
}