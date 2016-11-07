#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"

#ifndef __FuelTank__
#define __FuelTank__

// Monitor type class
class FuelTank {
private:
	struct fuelTankData *tank;
	CDataPool *tankDataPool;
	CMutex *mutex;

public:
	// constructor and destructor
	FuelTank();
	~FuelTank(){ delete mutex; delete tankDataPool; }
	float read(int fuelType);
	float getCost(int fuelType);
	void fill(int fuelType);
	bool dispense(int fuelType, float fuelAmount);
	bool increment(int fuelType);
	bool decrement(int fuelType);
	void setCost(int fuelType, float cost);
};

#endif