#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"
#include "Car.h"


int main(void){
	CRendezvous start("StartRendezvous", 3);
	CRendezvous end("EndRendezvous", 3);

	printf("Creating Child Processes.....\n");

	CProcess GSC("C:\\Users\\person\\Documents\\VisualStudio2013\\Projects\\Assignment1\\Debug\\GasStationComputer.exe 12",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process in the same window				
		ACTIVE						// process is active immediately
		);

	CProcess pumpIO("C:\\Users\\person\\Documents\\VisualStudio2013\\Projects\\Assignment1\\Debug\\PumpIO.exe 12",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process in the same window					
		ACTIVE							// process is active immediately
		);

	printf("Child Processes Activated.....\n");

	GSC.Resume();
	pumpIO.Resume();

	start.Wait();

	// create a car 
	Car car1(1, 2);
	car1.setDesFuel(30);
	car1.setFuelGrade(OCT87);
	car1.Resume();

	car1.WaitForThread();
	Sleep(2000);
	printf("Press RETURN to end...\n");
	getchar();
	end.Wait();

	// wait for the child process to end
	printf("Waiting For Child1 to Terminate.....\n");
	GSC.WaitForProcess();

	printf("Waiting For Child2 to Terminate.....\n");
	pumpIO.WaitForProcess();					// wait for the child process to end

	return 0;
}