#include <stdio.h>
#include <sstream>
#include "Pump.h"

Pump::Pump(int pumpID){
	myID = pumpID;
	printf("Pump %d being constructed...\n", myID);
}

int Pump::readCustomerPipelineThread(void *ThreadArgs){
	// pipeline for customer data, one customer at a time
	myPipe = new CTypedPipe<struct customerData>(pumpName, 1);
	myPipeMutex = new CMutex(pumpName, OWNED);
	struct customerData currentCustomer;

	Sleep(2000);

	printf("Starting to poll pipe for pump %d...\n", myID);
	while(1){
		myPipeMutex->Wait();
		myPipe->Read(&currentCustomer);
		myPipeMutex->Signal();
		printf("Customer %d arrived at pump %d...\n", currentCustomer.customerName, myID);
		
		cs->Wait();
		myPumpData->customerName = currentCustomer.customerName;
		myPumpData->creditCard = currentCustomer.creditCard;
		myPumpData->fuelType = currentCustomer.fuelType;
		ps->Signal();
		printf("Pump %d produced customer data for GSC\n", myID);

		Sleep(10);
	}

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
	printf("Pump %d linked to pump datapool at address %p...\n", myID, myPumpData);

	// datapool to store shared data with the fuel tank
	CDataPool fuelDataPool("FuelTank", sizeof(struct fuelTankData));
	fuelData = (struct fuelTankData *)(fuelDataPool.LinkDataPool());
	printf("Pump %d linked to fuel datapool at address %p...\n", myID, fuelData);
	
	// create thread to read pipeline in suspended state
	ClassThread<Pump> pipelineThread(this, &Pump::readCustomerPipelineThread, ACTIVE, NULL);

	Sleep(2000);
	//printf("Pump %d read value for dispense fuel...\n", myID);
	//ps.Wait();
	//if (myPumpData->dispenseFuel)
	//	printf("Pump %d dispense fuel...\n", myID);
	//ps.Signal();

	pipelineThread.WaitForThread();

	return 0;
}