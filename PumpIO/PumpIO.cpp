#include <stdio.h>
#include "C:\RTExamples\rt.h"	
#include "Pump.h"

int main()
{
	CRendezvous start("StartRendezvous", 3);
	CRendezvous end("EndRendezvous", 3);
	CMutex pumpScreenMutex("PumpScreen");

	printf("Hello from pump IO process....\n");
	Sleep(2000);
	
	// Create pump threads
	Pump *pumps[NPUMPS];
	for (int i = 0; i < NPUMPS; i++){
		pumps[i] = new Pump(i + 1);
	}

	start.Wait();
	Sleep(2000);

	for (int i = 0; i < NPUMPS; i++){
		pumps[i]->Resume();
	}

	end.Wait();

	for (int i = 0; i < NPUMPS; i++){
		pumps[i]->RequestTerminate();
	}


	return 0;		
}