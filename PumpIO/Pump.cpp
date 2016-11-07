#include <stdio.h>
#include <sstream>
#include "Pump.h"

Pump::Pump(int pumpID){
	myID = pumpID;
	// set up pump name
	std::ostringstream oss;
	pumpName = "P";
	oss << myID;
	pumpName += oss.str();
	ps = new CSemaphore("PS" + oss.str(), 0, 1);
	cs = new CSemaphore("CS" + oss.str(), 1, 1);

	// datapool to store shared data with the GSC
	CDataPool myPumpDataPool("Pump" + oss.str(), sizeof(struct pumpData));
	myPumpData = (struct pumpData *)(myPumpDataPool.LinkDataPool());
	//printf("Pump %d linked to pump datapool at address %p...\n", myID, myPumpData);

	// datapool to store shared data with the fuel tank
	CDataPool fuelDataPool("FuelTank", sizeof(struct fuelTankData));
	fuelData = (struct fuelTankData *)(fuelDataPool.LinkDataPool());

	//printf("Pump %d being constructed...\n", myID);
	screenMutex = new CMutex("PumpScreen");
	// pipeline for customer data, one customer at a time
	myPipe = new CTypedPipe<struct customerData>(pumpName, 1);
	myPipeMutex = new CMutex(pumpName, OWNED);

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
	printf("Polling pump %d...\n", myID);
	fflush(stdout);
	screenMutex->Signal();

	while(1){


		pumpEntryQueue->Signal(); 
		pumpFull->Wait();

		//myPipeMutex->Wait();
		myPipe->Read(&currentCustomer);
		
		screenMutex->Wait();
		MOVE_CURSOR(0, ((myID - 1) * 6)); 
		printf("[PUMP %i IN USE] \n"
				"Name:%s \n"
				"Credit Card: %i \n"
				"Fuel Type: %i \n"
				"Fuel Amount: %f \n", 
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
		// wait to dispense fuel
		cs->Wait();
		if (myPumpData->dispense == FALSE){
			screenMutex->Wait();
			MOVE_CURSOR(0, ((myID - 1) * 6));
			printf("Customer rejected \n");
			fflush(stdout);
			screenMutex->Signal();
		}
		else{
			myPumpData->dispensedFuel = 0;
			float purchaseCost = tank->getCost(myPumpData->fuelType); // cost is set at time of dispense
			while (myPumpData->fuelAmount - myPumpData->dispensedFuel >= 0.5){
				if (tank->decrement(myPumpData->fuelType)){
					myPumpData->dispensedFuel += DISPENSERATE;
					screenMutex->Wait();
					MOVE_CURSOR(0, ((myID - 1) * 6 + 2));
					printf("L: %.1f\n", myPumpData->dispensedFuel);
					printf("$: %.2f\n", purchaseCost*myPumpData->dispensedFuel);
					fflush(stdout);
					screenMutex->Signal();
				}
				else{
					screenMutex->Wait();
					MOVE_CURSOR(0, ((myID - 1) * 6 + 1));
					printf("Cannot dispense any more fuel\n");
					fflush(stdout);
					screenMutex->Signal();
					myPumpData->finalCost = purchaseCost*myPumpData->dispensedFuel;
				}
			}
		}
		screenMutex->Wait();
		MOVE_CURSOR(0, ((myID - 1) * 6));
		printf("Customer leaving Pump\n");
		fflush(stdout);
		screenMutex->Signal();
		ps->Signal();

		Sleep(2000);
		
		pumpExitQueue->Signal(); //wait to leave the pump
		pumpEmpty->Wait(); //signal the pump is free
	}

	return 0;
}

int Pump::displayPumpDataThread(void *ThreadArgs){
	//TODO: figure out what to put on this display
	return 0;
}

int Pump::main(void){
	//printf("Pump %d linked to fuel datapool at address %p...\n", myID, fuelData);
	
	// create thread to read pipeline in suspended state
	ClassThread<Pump> pipelineThread(this, &Pump::readCustomerPipelineThread, ACTIVE, NULL);
	// create thread to display data on the screen
	ClassThread<Pump> displayThread(this, &Pump::displayPumpDataThread, ACTIVE, NULL);

	tank = new FuelTank();

	Sleep(2000);
	
	
	displayThread.WaitForThread();
	pipelineThread.WaitForThread();

	return 0;
}
