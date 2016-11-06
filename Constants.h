#ifndef __Constants__
#define __Constants__

#define NPUMPS 4

#define OCT82 0
#define OCT87 1
#define OCT92 2
#define OCT97 3
#define NTANKS 4
#define TANKSIZE 500
#define MINLEVEL 200
#define DISPENSERATE 0.5

#define CARFUELTANK 70

struct pumpData {
	int pumpID;
	int customerName;
	int creditCard;
	int fuelType;
	float fuelAmount;
	bool dispenseFuel;
	int transactionEndTime;
};

struct fuelTankData {
	float fuelLevels[NTANKS];
	float fuelCosts[NTANKS];
};

struct customerData {
	int customerName;
	int creditCard;
	int fuelType;
	float fuelAmount;
};

#endif