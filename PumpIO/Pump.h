#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"

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
	CSemaphore *ps;
	CSemaphore *cs;
public:
	Pump(int pumpID);
	int readCustomerPipelineThread(void *ThreadArgs);
	int displayPumpDataThread(void *ThreadArgs);
};

#endif