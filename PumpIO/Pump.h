#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"
#include "../GasStationComputer/FuelTank.h"

#ifndef __Pump__
#define __Pump__

class Pump : public ActiveClass{
private:
	int myID;
	string pumpName;
	int main(void);
	struct pumpData *myPumpData;
	struct fuelTankData *fuelData;
	CTypedPipe<struct customerData> *myPipe;
	CMutex *myPipeMutex;
	CMutex *screenMutex;
	CSemaphore *ps;
	CSemaphore *cs;
	CSemaphore *pumpEmpty;
	CSemaphore *pumpFull;
	CSemaphore *pumpEntryQueue;
	CSemaphore *pumpExitQueue;
	CSemaphore *pumpHoseRemoved;
	CSemaphore *pumpHoseReturned;
	FuelTank *tank;

public:
	Pump(int pumpID);
	~Pump();
	int readCustomerPipelineThread(void *ThreadArgs);
	int displayPumpDataThread(void *ThreadArgs);
};

#endif