#include 	<stdio.h>
#include	<sstream>
#include 	"C:\RTExamples\rt.h"			
#include	"../Constants.h"

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
	printf("GSC linked to pump datapool %d at address %p...\n", ID, myPool);
	
	while (1){
		ps.Wait();
		printf("Customer %d is at Pump %d\n", myPool->customerName, ID);
		cs.Signal();
	}

	return 0;									// terminate child thread
}

UINT __stdcall fuelTankThread(void *args)			// args points to any data passed to the child thread
{
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

	start.Wait();

	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Resume();
	}

	end.Wait();

	for (int i = 0; i < NPUMPS; i++){
		pumpThreads[i]->Suspend();
	}

	return 0;		
}