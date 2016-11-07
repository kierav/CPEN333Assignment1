#include "FuelTank.h"

FuelTank::FuelTank(){ 
	mutex = new CMutex("FuelMutex");
	tankDataPool = new CDataPool("FuelTankDataPool", sizeof(struct fuelTankData));
	tank = (struct fuelTankData *)(tankDataPool->LinkDataPool());

	// tank is initialized to be full by default;
	for (int i = 0; i < NTANKS; i++){
		tank->fuelLevels[i] = TANKSIZE;
		tank->fuelCosts[i] = 1 + 0.1*i; // starting cost of 1$ per liter
	}
}

float FuelTank::read(int fuelType){
	float desFuelLevel;
	mutex->Wait();
	desFuelLevel = tank->fuelLevels[fuelType];
	mutex->Signal();
	return desFuelLevel;
}

float FuelTank::getCost(int fuelType){
	float cost;
	mutex->Wait();
	cost = tank->fuelCosts[fuelType];
	mutex->Signal();
	return cost;
}

void FuelTank::fill(int fuelType){
	mutex->Wait();
	tank->fuelLevels[fuelType] = TANKSIZE;
	mutex->Signal();
}

bool FuelTank::dispense(int fuelType, float fuelAmount){
	bool Status = FALSE;
	if (this->read(fuelType) > MINLEVEL){
		Status = TRUE;
		while (fuelAmount > 0 && Status == TRUE){
			Status = this->decrement(fuelType);
			fuelAmount = fuelAmount - DISPENSERATE;
		}
	}
	return Status;
}

bool FuelTank::increment(int fuelType){
	mutex->Wait();
	bool Status = FALSE;
	if (tank->fuelLevels[fuelType] < TANKSIZE){
		Status = TRUE;
		tank->fuelLevels[fuelType] = tank->fuelLevels[fuelType] + DISPENSERATE;
	}
	mutex->Signal();
	return Status;
}

bool FuelTank::decrement(int fuelType){
	mutex->Wait();
	bool Status = FALSE;
	if (tank->fuelLevels[fuelType] > 0){
		Status = TRUE;
		tank->fuelLevels[fuelType] = tank->fuelLevels[fuelType] - DISPENSERATE;
	}
	mutex->Signal();
	return Status;
}

void FuelTank::setCost(int fuelType, float cost){
	mutex->Wait();
	tank->fuelCosts[fuelType] = cost;
	mutex->Signal();
}