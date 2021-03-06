#include <stdio.h>
#include <sstream>
#include "Pump.h"
#include <ctime>

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
	myPumpDataPool  = new CDataPool("Pump" + oss.str(), sizeof(struct pumpData));
	myPumpData = (struct pumpData *)(myPumpDataPool->LinkDataPool());
	//printf("Pump %d linked to pump datapool at address %p...\n", myID, myPumpData);

	//printf("Pump %d being constructed...\n", myID);
	screenMutex = new CMutex("PumpScreen");
	// pipeline for customer data, one customer at a time
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
Pump::~Pump(){
	delete pumpEmpty;
	delete pumpFull;
	delete pumpEntryQueue;
	delete pumpExitQueue;
	delete myPumpData;
	delete myPipe;
	delete myPipeMutex;
	delete screenMutex;
	delete ps;
	delete cs;
	delete tank;
	delete pumpHoseRemoved;
	delete pumpHoseReturned;
}
void Pump::clearLine(int lineNumber)
{
	screenMutex->Wait();
	MOVE_CURSOR(0, lineNumber);
	printf("                                                    "
		"                      \n");
	screenMutex->Signal();
}
int Pump::readCustomerPipelineThread(void *ThreadArgs){
	struct customerData currentCustomer;

	Sleep(1000);
	screenMutex->Wait();
	TEXT_COLOUR(2 + (myID), 0);
	MOVE_CURSOR(0, ((myID-1)*6));
	printf("Polling pump %d...\n", myID);
	fflush(stdout);
	screenMutex->Signal();
	for (int i = 0; i < 6; i++){
		clearLine(((myID - 1) * 6 + i));
	}
	while(1){

		pumpEntryQueue->Signal(); 
		screenMutex->Wait();
		pumpFull->Wait();
		myPipeMutex->Wait();

		while (myPipe->TestForData() < 1){
			myPipeMutex->Signal();

			myPipeMutex->Wait();
		}
		myPipe->Read(&currentCustomer);

		MOVE_CURSOR(0, ((myID - 1) * 6));
		TEXT_COLOUR(2 + (myID), 0);
		printf("Customer is arriving...\n");
		fflush(stdout);
		screenMutex->Signal();
		Sleep(300);

		screenMutex->Wait();
		MOVE_CURSOR(0, ((myID - 1) * 6)); 
		TEXT_COLOUR(2 + (myID), 0);
		printf("[PUMP %i IN USE]             \n"
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
		if (myPumpData->dispense != 1){
			screenMutex->Wait();
			TEXT_COLOUR(2 + (myID), 0);
			MOVE_CURSOR(40, ((myID - 1) * 6));
			printf("Customer rejected \n");
			fflush(stdout);
			screenMutex->Signal();
			Sleep(700);
		}
		else{
			pumpHoseRemoved->Wait();
			screenMutex->Wait();
			TEXT_COLOUR(2 + (myID), 0);
			MOVE_CURSOR(40, ((myID - 1) * 6));
			printf("Removing gas hose from pump... \n");
			fflush(stdout);
			screenMutex->Signal();
			Sleep(700);

			screenMutex->Wait();
			TEXT_COLOUR(2 + (myID), 0);
			MOVE_CURSOR(40, ((myID - 1) * 6));
			printf("Dispensing fuel...               \n");
			fflush(stdout);
			screenMutex->Signal();
			Sleep(500);

			myPumpData->dispensedFuel = 0;
			float purchaseCost = tank->getCost(myPumpData->fuelType); // cost is set at time of dispense
			while (myPumpData->fuelAmount - myPumpData->dispensedFuel >= 0.5){
				if (tank->decrement(myPumpData->fuelType)){
					myPumpData->dispensedFuel += DISPENSERATE;
					screenMutex->Wait();
					TEXT_COLOUR(2 + (myID), 0);
					MOVE_CURSOR(40, ((myID - 1) * 6 + 2));
					printf("L: %.1f\n", myPumpData->dispensedFuel);
					MOVE_CURSOR(40, ((myID - 1) * 6 + 3));
					printf("$: %.2f\n", purchaseCost*myPumpData->dispensedFuel);
					MOVE_CURSOR(40, ((myID - 1) * 6 + 4));
					printf("T: OCT%i\n", fuelTypes[myPumpData->fuelType]);
					fflush(stdout);
					screenMutex->Signal();
				}
				else{
					screenMutex->Wait();
					TEXT_COLOUR(2 + (myID), 0);
					MOVE_CURSOR(40, ((myID - 1) * 6 + 1));
					printf("Cannot dispense any more fuel\n");
					fflush(stdout);
					screenMutex->Signal();
				}
				Sleep(50);
			}
			myPumpData->finalCost = purchaseCost*myPumpData->dispensedFuel;
			myPumpData->transactionEndTime = time(nullptr);

			screenMutex->Wait();
			TEXT_COLOUR(2 + (myID), 0);
			MOVE_CURSOR(40, ((myID - 1) * 6));
			printf("Returning gas hose to pump...            \n");
			fflush(stdout);
			screenMutex->Signal();
			pumpHoseReturned->Signal();
			Sleep(700);
		}
		ps->Signal();

		Sleep(2000);
		pumpExitQueue->Signal(); //wait to leave the pump
		pumpEmpty->Wait(); //signal the pump is free

		screenMutex->Wait();
		TEXT_COLOUR(2 + (myID), 0);
		MOVE_CURSOR(40, ((myID - 1) * 6));
		printf("Customer is leaving...            \n");
		fflush(stdout);
		screenMutex->Signal();
		Sleep(700);

		myPipeMutex->Signal();

		for (int i = 0; i < 6; i++){
			clearLine(((myID - 1) * 6 + i));
		}
		screenMutex->Wait();
		TEXT_COLOUR(2 + (myID), 0);
		MOVE_CURSOR(0, ((myID - 1) * 6));
		printf("No customer at pump awaiting new customer \n");
		fflush(stdout);
		for (int j = 0; j < 5; j++){
			printf("... ");
			Sleep(200);
		}
		printf("\n");
		fflush(stdout);
		screenMutex->Signal();

		for (int i = 0; i < 6; i++){
			clearLine(((myID - 1) * 6 + i));
		}
		screenMutex->Wait();
		TEXT_COLOUR(2 + (myID), 0);
		MOVE_CURSOR(0, ((myID - 1) * 6));

		printf("[PUMP %i VACANT] \n", myID);
		fflush(stdout);
		screenMutex->Signal();
	}

	return 0;
}

int Pump::main(void){
	//printf("Pump %d linked to fuel datapool at address %p...\n", myID, fuelData);
	
	// create thread to read pipeline in suspended state
	ClassThread<Pump> pipelineThread(this, &Pump::readCustomerPipelineThread, ACTIVE, NULL);

	tank = new FuelTank();

	Sleep(2000);
	
	pipelineThread.WaitForThread();

	return 0;
}
