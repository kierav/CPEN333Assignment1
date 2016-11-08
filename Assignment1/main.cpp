#include <stdio.h>
#include "C:/RTExamples/rt.h"
#include "../Constants.h"
#include "Car.h"

char* customernames[] = { "Matt", "Kiera", "Gus", "Riley", "Ben", "Todd", "Doug", "John", "Darrell", "Denice",
"Lolo", "Jade", "Lucy", "Lutz", "Hacker9000", "Aaron", "Jack", "Peter", "Sal", "Barbara",
"Etienne", "Calvin", "Ingrid", "Shane", "Lukas", "Jordan", "William", "Hayley", "Connor", "Jocelyn",
"FalcoMaster", "Hacker2", "TheFlash", "Superman", "Ironman", "Hulk", "Gooseman", "Batman", "Spiderman", "Gerald" };
int main(void){
	CRendezvous start("StartRendezvous", 3);
	CRendezvous end("EndRendezvous", 3);

	printf("Creating Child Processes.....\n");
	CProcess GSC("C:\\Users\\Matt Cordoba\\Dropbox\\CPEN 333\\A1MCKV\\CPEN333Assignment1\\Debug\\GasStationComputer.exe 12",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process in the same window				
		ACTIVE						// process is active immediately
		);
	CProcess pumpIO("C:\\Users\\Matt Cordoba\\Dropbox\\CPEN 333\\A1MCKV\\CPEN333Assignment1\\Debug\\PumpIO.exe 12",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process in the same window					
		ACTIVE							// process is active immediately
		);

	printf("Child Processes Activated.....\n");

	GSC.Resume();
	pumpIO.Resume();

	start.Wait();

	// create a car 
	Car *cars[NUM_CUSTOMERS];
	int i;
	for (i = 0; i < NUM_CUSTOMERS; i++){
		cars[i] = new Car(customernames[i], (i % 4) + 1);
		cars[i]->setDesFuel(rand() % 71);
		cars[i]->setFuelGrade(rand() % 4);
		cars[i]->Resume();
		
	}
	for (i = 0; i < NUM_CUSTOMERS; i++){
		cars[i]->WaitForThread();
	}
	/*
	Car car1("MattsCar", 2);
	car1.setDesFuel(30);
	car1.setFuelGrade(OCT87);
	car1.Resume();

	car1.WaitForThread();
	*/
	Sleep(2000);
	printf("Press RETURN to end...\n");
	getchar();
	end.Wait();

	// wait for the child process to end
	printf("Waiting For Child1 to Terminate.....\n");
	GSC.WaitForProcess();

	printf("Waiting For Child2 to Terminate.....\n");
	pumpIO.WaitForProcess();					// wait for the child process to end

	//delete cars array pointer to prevent memory leaks
	for (i = 0; i < NUM_CUSTOMERS; i++){
		delete cars[i];
	}
	return 0;
}