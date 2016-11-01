#include 	<stdio.h>
#include	<sstream>
#include 	"C:\RTExamples\rt.h"			
#include	"../Constants.h"

UINT __stdcall pumpThread(void *args)			// args points to any data passed to the child thread
{												// by the parent thread during the call
	for (int i = 0; i < 500; i++) {
		printf("Hello from child thread 1.....\n");
		SLEEP(200);
	}
	return 0;									// terminate child thread
}

int main()
{
	CRendezvous start("StartRendezvous", 3);
	CRendezvous end("EndRendezvous", 3);

	printf("Hello from gas station computer process....\n");
	Sleep(2000);

	printf("Creating pump threads...\n");
	
	CDataPool *pumpDataPools[NPUMPS];
	struct pumpData *pumps[NPUMPS];
	CSemaphore *ps[NPUMPS];
	CSemaphore *cs[NPUMPS];

	// create datapools to share with pumps
	for (int i = 0; i < NPUMPS; i++){
		std::ostringstream oss;
		string dataPoolName = "P";
		oss << i+1;
		dataPoolName += oss.str();
		pumpDataPools[i] = new CDataPool(dataPoolName, sizeof(struct pumpData));
		pumps[i] = (struct pumpData *)(pumpDataPools[i]->LinkDataPool());
		ps[i] = new CSemaphore("PS" + oss.str(), 0, 1);
		cs[i] = new CSemaphore("CS" + oss.str(), 1, 0);
		printf("GSC linked to pump datapool %d at address %p...\n", i+1, pumps[i]);
	}

	start.Wait();

	printf("Writing to datapool 1 to dispense fuel...\n");
	cs[0]->Wait();
	pumps[0]->dispenseFuel = true;
	ps[0]->Signal();

	end.Wait();

	return 0;		
}