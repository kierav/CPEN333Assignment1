#include 	<stdio.h>
#include	<iostream>
#include	<sstream>
#include	<conio.h>
#include	<ctype.h>
#include	<time.h>
#include	<list>
#include 	"C:\RTExamples\rt.h"			
#include	"../Constants.h"
#include	"FuelTank.h"

FuelTank myTank;
CMutex screenMutex("GSCScreenMutex");
bool dispense[NPUMPS];
bool reject[NTANKS];
bool flashFuel = FALSE;
list<struct transaction> history; 

void drawFuel(int fuelTank, float amount){
	int numBars = int(amount / TANKSIZE * 10);
	screenMutex.Wait();
	CURSOR_OFF();
	if (numBars > 2) {
		TEXT_COLOUR(10, 0);
	}
	else if (flashFuel == FALSE) {
		TEXT_COLOUR(12, 0);
		flashFuel = TRUE;
	}
	else {
		TEXT_COLOUR(0, 0);
		flashFuel = FALSE;
	}
	MOVE_CURSOR(6 + fuelTank * 14, 11);
	for (int i = 0; i < 10; i++){
		if (i < numBars) {
			printf("|");
			fflush(stdout);
		}
		else {
			printf(" ");
			fflush(stdout);
		}
	}
	MOVE_CURSOR(6 + fuelTank * 14, 12);
	printf("%.2f L", amount);
	fflush(stdout);
	TEXT_COLOUR(15, 0);
	MOVE_CURSOR(0, 26);
	screenMutex.Signal();
}
void clearLine(int lineNumber)
{
	screenMutex.Wait();
	MOVE_CURSOR(0, lineNumber);
	printf("                                                    "
		"                      \n");
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
	TEXT_COLOUR(15, 0);
	cout << "KEYBOARD Commands:" << endl;
	cout << "D + i: dispense fuel to pump i (1 to " << NPUMPS << ")" << endl;
	cout << "Q + i: reject customer at pump i (1 to " << NPUMPS << ")" << endl;
	cout << "R + i: refill fuel tank i (1 to " << NTANKS << ")" << endl;
	cout << "C + i: change cost of fuel tank i (1 to " << NTANKS << ")" << endl;
	cout << "S + 1: display all transactions until now" << endl;
	CURSOR_ON();
}
void printTransactionsHistory(){
	screenMutex.Wait();
	MOVE_CURSOR(0, 30);
	cout << "============================================ \n";
	cout << "Transaction History for Today\n";
	char timebuff[32];
	struct tm newtime;
	for (auto v : history)
	{
		_localtime64_s(&newtime, &v.endTime);
		asctime_s(timebuff, 32, &newtime);
		cout << "Name:" << v.customerName;
		cout << ", Credit Card:" << v.creditCard;
		cout << ", Dispensed Fuel:" << v.dispensedFuel;
		cout << ", Final Cost:" << v.finalCost;
		cout << ", Fuel Type:" << v.fuelType;
		cout << ", End Time:" << timebuff << "\n";
	}

	screenMutex.Signal();
}
void readKeyCmds(){
	// one key has already been pressed
	int cmd1;
	int cmd2;
	screenMutex.Wait();
	MOVE_CURSOR(0, 26);
	CURSOR_ON();
	screenMutex.Signal();
	cmd1 = _getche();
	cmd1 = toupper(cmd1);
	cmd2 = _getche() - '0' - 1;
	float cost;

	switch (cmd1){
	case 'D':
		if (cmd2 < NPUMPS && cmd2 >= 0){
			dispense[cmd2] = TRUE;
		}
		break;
	case 'Q':
		if (cmd2 < NPUMPS && cmd2 >= 0){
			reject[cmd2] = TRUE;
		}
		break;
	case 'R':
		if (cmd2 < NTANKS && cmd2 >= 0){
			myTank.fill(cmd2);
		}
		break;
	case 'C':
		if (cmd2 < NTANKS && cmd2 >= 0){
			screenMutex.Wait();
			MOVE_CURSOR(0, 27);
			cout << "Enter new cost:" << endl;
			cin >> cost; //TODO: maybe make this safer
			screenMutex.Signal();
			myTank.setCost(cmd2, cost);
		}
		break;
	case 'S':
		printTransactionsHistory();
		break;
	default:
		screenMutex.Wait();
		MOVE_CURSOR(0, 27);
		printf("Invalid command");
		fflush(stdout);
		screenMutex.Signal();
	}

	Sleep(300);
	screenMutex.Wait();
	fflush(stdin);
	MOVE_CURSOR(0, 26);
	CURSOR_OFF();
	printf("                      \n                         \n                  \n");
	fflush(stdout);
	MOVE_CURSOR(0, 26);
	CURSOR_ON();
	screenMutex.Signal();
}

UINT __stdcall pumpThread(void *args)			// args points to any data passed to the child thread
{
	int ID = *(int *)(args);
	
	std::ostringstream oss;
	string dataPoolName = "Pump";
	oss << ID;
	dataPoolName += oss.str();
	CDataPool dp("Pump" + oss.str(), sizeof(struct pumpData));
	struct pumpData *myPool = (struct pumpData *)(dp.LinkDataPool());
	CSemaphore ps("PS" + oss.str(), 0, 1);
	CSemaphore cs("CS" + oss.str(), 1, 1);
	//screenMutex.Wait();
	//MOVE_CURSOR(0, 15);
	//TEXT_COLOUR(15, 0);
	//printf("GSC linked to pump datapool %d at address %p...\n", ID, myPool);
	//screenMutex.Signal();
	
	while (1){
		// wait for a customer to arrive
		//screenMutex.Wait();
		//MOVE_CURSOR(0, 16 + 2 * (ID - 1));
		//printf("Hi there", myPool->customerName, ID);
		//fflush(stdout);
		//screenMutex.Signal();

		ps.Wait();
		clearLine(16 + (ID - 2));
		screenMutex.Wait();
		MOVE_CURSOR(0, 16 + (ID -2));
		TEXT_COLOUR(2+(ID), 0);
		printf("PUMP%d STATUS: ", ID);
		printf("%s is at Pump %d", myPool->customerName, ID);
		fflush(stdout);
		printf(", Card: %d", myPool->creditCard);
		if (myPool->fuelType == OCT82)
			printf(", Type: OCT82");
		else if (myPool->fuelType == OCT87)
			printf(", Type: OCT87");
		else if (myPool->fuelType == OCT92)
			printf(", Type: OCT92");
		else if (myPool->fuelType == OCT97)
			printf(", Type: OCT97");
		printf(", Amount: %0.3f \n", myPool->fuelAmount);
		fflush(stdout);
		screenMutex.Signal();

		// dispense fuel or reject customer
		while (!dispense[ID-1] && !reject[ID-1]) {
			// do nothing
		}
		if (dispense[ID - 1] == TRUE){
			myPool->dispense = 1;
			dispense[ID - 1] = FALSE;
			reject[ID - 1] = FALSE;
			clearLine(16 + (ID - 2));
			screenMutex.Wait();
			MOVE_CURSOR(0, 16 + (ID - 2));
			TEXT_COLOUR(2 + (ID), 0); 
			printf("PUMP%d STATUS: ", ID);
			printf("Dispensing fuel at Pump %d\n",ID);
			fflush(stdout);
			screenMutex.Signal();
		}
		else {
			myPool->dispense = 0;
			reject[ID - 1] = FALSE;
			clearLine(16 + (ID - 2));
			screenMutex.Wait();
			MOVE_CURSOR(0, 16 + (ID - 2));
			TEXT_COLOUR(2 + (ID), 0); 
			printf("PUMP%d STATUS: ", ID);
			printf("Not dispensing fuel at Pump %d\n", ID);
			fflush(stdout);
			screenMutex.Signal();
		}
		cs.Signal();

		//TODO: is there any data to update while fuel is being dispensed
		// get final data
		ps.Wait();
		struct transaction complete;
		strcpy_s(complete.customerName, myPool->customerName);
		complete.creditCard = myPool->creditCard;
		complete.fuelType = myPool->fuelType;
		complete.dispensedFuel = myPool->dispensedFuel;
		complete.finalCost = myPool->finalCost;
		complete.endTime = myPool->transactionEndTime;
		//record the complete transaction into history variable to display in RAM if needed
		history.push_back(complete);
		cs.Signal();
		clearLine(16 + (ID - 2));
		screenMutex.Wait();
		MOVE_CURSOR(0, 16 + (ID - 2));
		TEXT_COLOUR(2 + (ID), 0); 
		printf("PUMP %d STATUS: %s paid %.2f for %.1f L of fuel type %d\n", myPool->customerName, ID, myPool->finalCost, myPool->dispensedFuel, myPool->fuelType);
		fflush(stdout);
		screenMutex.Signal();
	}

	return 0;									
}

UINT __stdcall fuelTankThread(void *args)			
{
	while (1){
		for (int i = 0; i < NTANKS; i++){
			drawFuel(i, myTank.read(i));
			Sleep(100);
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
		dispense[i] = FALSE;
		reject[i] = FALSE;
	}

	CThread fuelThread(fuelTankThread, SUSPENDED);

	setUpScreen();

	start.Wait();

	fuelThread.Resume();
	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Resume();
	}

	while (1){
		screenMutex.Wait();
		MOVE_CURSOR(0, 26);
		CURSOR_ON();
		screenMutex.Signal();
		TEST_FOR_KEYBOARD();
		readKeyCmds();
	}

	end.Wait();
	
	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Suspend();
	}
	fuelThread.Suspend();

	return 0;		
}


