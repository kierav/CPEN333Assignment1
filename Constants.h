#ifndef __Constants__
#define __Constants__

#define NPUMPS 4

#define OCT82 0
#define OCT87 1
#define OCT92 2
#define OCT97 3
#define NTANKS 4
const int fuelTypes[] = { 82, 87, 92, 97 };
#define TANKSIZE 500
#define MINLEVEL 200
#define DISPENSERATE 0.5
#define MAX_NAME_LENGTH 20
#define NUM_CUSTOMERS 40

#define CARFUELTANK 70
#define MAX_NAME_LENGTH 20

struct pumpData {
	int pumpID;
	char customerName[MAX_NAME_LENGTH];
	int creditCard;
	int fuelType;
	float fuelAmount;
	int dispense;
	float dispensedFuel;
	float finalCost;
	time_t transactionEndTime;
};

struct fuelTankData {
	float fuelLevels[NTANKS];
	float fuelCosts[NTANKS];
};

struct customerData {
	char customerName[MAX_NAME_LENGTH];
	int creditCard;
	int fuelType;
	float fuelAmount;
};

struct transaction {
	char customerName[MAX_NAME_LENGTH];
	int creditCard;
	int fuelType;
	float dispensedFuel;
	float finalCost;
	time_t endTime;
};

#endif