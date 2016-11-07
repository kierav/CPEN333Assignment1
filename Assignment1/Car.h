#include "C:/RTExamples/rt.h"
#include "../Constants.h"

#ifndef __Car__
#define __Car__

class Car : public ActiveClass{
private:
	int desPump;
	struct customerData myData;
	CTypedPipe<struct customerData> *myPipe;
	CSemaphore *pumpEmpty;
	CSemaphore *pumpFull;
	CSemaphore *pumpEntryQueue;
	CSemaphore *pumpExitQueue;
	CMutex *myPipeMutex;
public:
	Car(char *name, int desPump);
	~Car();
	void setDesFuel(float maxDesFuel);
	void setFuelGrade(int fuelGrade);
	int main(void);
};
#endif 