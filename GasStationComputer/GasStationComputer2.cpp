#include 	<stdio.h>
#include	<sstream>
#include 	"C:\RTExamples\rt.h"			
#include	"../Constants.h"
#include	"FuelTank.h"

FuelTank myTank;
CMutex screenMutex("GSCScreenMutex");

void drawFuel(int fuelTank, float amount){
	int numBars = int(amount / TANKSIZE * 10);
	screenMutex.Wait();
	if (numBars <= 2)
		TEXT_COLOUR(12, 0);
	else
		TEXT_COLOUR(10, 0);
	MOVE_CURSOR(6 + fuelTank * 14, 12);
	for (int i = 0; i < numBars; i++){
		cout << "|";
	}
	screenMutex.Signal();
}

void setUpScreen(){
	CLEAR_SCREEN();
	CURSOR_OFF();
	TEXT_COLOUR(9, 0);
	MOVE_CURSOR(0, 0);
	cout << "\n--------------------------------------------------------------" << endl;
	cout << "\n                      GAS STATION COMPUTER                    " << endl;
	cout << "\n--------------------------------------------------------------" << endl;
	TEXT_COLOUR(15, 0);
	cout << "\n FUEL TANKS          " << endl;
	cout << "\n        OCT 82        OCT 87        OCT 92        OCT 97      " << endl;
	TEXT_COLOUR(10, 0);
	//	cout << "\n      __________    __________    __________    __________" << endl;
	cout << "\n      ||||||||||    ||||||||||    ||||||||||    ||||||||||    " << endl;
	MOVE_CURSOR(0, 20);
	cout << "KEYBOARD Commands:" << endl;
	cout << "\nD + i: dispense fuel from tank i (0 to" << NTANKS << ")" << endl;
	cout << "\nR + i: refill fuel tank i (0 to" << NTANKS << ")" << endl;
	cout << "\nC + i: change cost of fuel tank i (0 to" << NTANKS << ")" << endl;
}

UINT __stdcall pumpThread(void *args)			// args points to any data passed to the child thread
{
	int ID = *(int *)(args);
	
	std::ostringstream oss;
	string dataPoolName = "Pump";
	oss << ID;
	dataPoolName += oss.str();
	CDataPool dp(dataPoolName, sizeof(struct pumpData));
	struct pumpData *myPool = (struct pumpData *)(dp.LinkDataPool());
	CSemaphore ps("PS" + oss.str(), 0, 1);
	CSemaphore cs("CS" + oss.str(), 1, 1);
	//screenMutex.Wait();
	//MOVE_CURSOR(0, 15);
	//TEXT_COLOUR(15, 0);
	//printf("GSC linked to pump datapool %d at address %p...\n", ID, myPool);
	//screenMutex.Signal();
	
	while (1){
		ps.Wait();
		screenMutex.Wait();
		MOVE_CURSOR(0, 16);
		TEXT_COLOUR(15, 0);
		printf("Customer %d is at Pump %d\n", myPool->customerName, ID);
		screenMutex.Signal();
		cs.Signal();
		//
	}

	return 0;									// terminate child thread
}

UINT __stdcall fuelTankThread(void *args)			// args points to any data passed to the child thread
{
	while (1){
		for (int i = 0; i < NTANKS; i++){
			drawFuel(i, myTank.read(i));
			Sleep(2000);
		}
	}
	return 0;
}


int main()
{
	CRendezvous start("StartRendezvous", 3);
	CRendezvous end("EndRendezvous", 3);

	printf("Hello from gas station computer process....\n");
	Sleep(2000);

	printf("Creating pump threads...\n");

	CThread *pumpThreads[NPUMPS];
	int IDs[NPUMPS];
	for (int i = 0; i < NPUMPS; i++){
		IDs[i] = i + 1;
		pumpThreads[i] = new CThread(pumpThread, SUSPENDED, &IDs[i]);
	}

	CThread fuelThread(fuelTankThread, SUSPENDED);

	setUpScreen();

	start.Wait();

	fuelThread.Resume();
	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Resume();
	}

	end.Wait();

	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Suspend();
	}
	fuelThread.Suspend();

	return 0;		
}


