#include "C:/RTExamples/rt.h"
#include "../Constants.h"

#ifndef __Car__
#define __Car__

class Car : public ActiveClass{
private:
	int desPump;
	struct customerData myData;
	CTypedPipe<struct customerData> *myPipe;
	CMutex *myPipeMutex;
public:
	Car(int carID, int desPump);
	void setDesFuel(float maxDesFuel);
	void setFuelGrade(int fuelGrade);
	int main(void);
};
#endif 