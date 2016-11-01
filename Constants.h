#ifndef __Constants__
#define __Constants__

#define NPUMPS 4

#define OCT82 0
#define OCT87 1
#define OCT92 2
#define OCT97 3
#define TANKSIZE 500

struct pumpData {
	int pumpID;
	int customerName;
	int creditCard;
	int fuelType;
	bool dispenseFuel;
};

struct fuelTankData {
	float fuelLevels[4];
};

struct customerData {
	int customerName;
	int creditCard;
	int fuelType;
	float tankEmptyVolume;
};

#endif