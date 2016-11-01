#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"

// Monitor type class
class FuelTank {
private:
	struct fuelTankData;
	CMutex *mutex;

public:
	// constructor and destructor
	FuelTank();
	int read(int tank);
	bool fill(int tank);
	bool increment(int tank);
	bool decrement(int tank);
};