#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Event.h>
#include <Thread.h>
#include <CommPort.h>

typedef enum RingState {
	rsIdle = 0,
	rsRing,
	rsAnswer,
	rsHangUp,
	rsNoCarrier,
} RingState;

typedef struct _FaxModem
{
	Thread* thread;
	int deviceResult;
	Event* deviceEvent;
	Event* deviceReady;
	CommPort* commPort;
	int callerID; // enable caller id
	// random ring
	Thread* ringThread;
	Event* ringEvent;
	Event* onHook;
	int ringRunning;
	int randomTimeout;
	int ringState;
	// listen
	Thread* listenThread;
	int done;
	char number[20];
	char name[100];
} FaxModem;

// include VSPE API header and library
#include "VSPE_API.h"

void createDevices(void* data)
{
	FaxModem* faxModem = (FaxModem*)data;
    // ****************************
    // STEP 1 - INITIALIZATION
    // ****************************
    const char* activationKey = "SRBGMZSYPuIHWILsmLjF5CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPRJEEYxlJg+4gdYguyYuMXkIPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9CDyBL3GQYD0IPIEvcZBgPQg8gS9xkGA9JyUaC2ZWE1DZV2+wYWlRm7FFYrW3MDbZg8MkQsOQ8r1IPIEvcZBgPQg8gS9xkGA9LzrhjimHDiMlKqr6pSiw9CDl9n+0bAgFr2ho7nXjCoTMHYzt4tsbEkJGNktLGVG42SZ63UbmIUNcKmfhSzXldVCLhfvZv3StR9c/vkYG471Nh62eC1qIYuBUvm+a3BK8iR0POD8w5ovtuYr0T8aQP3eh4b8lUwnPHG9NRJxerttq/+/zX7c++9LDSQym3ThbWesK+A+X/vNw9qDgYt1dsJxDEEytsCRiT7bTiV5Djh1RlpIwETXWA089hiE9OYd7GpjKLq5dQOqSVcA3Fg1Wfdbqn/yn8q0/AIDOd0iZlbVeLY68zKh1Di4gGEoa1kR8EOBp2mxeaFrfwUm3DsJ5Pc04f7aEw9XljfBUwl/bAs3LVH5HRii8lXZvUVvnnfpcQ==1F250CF0960AE1C09E9450C816DE1232"; // <-----  PUT ACTIVATION KEY HERE
    bool result;
    // activate VSPE API
    result = vspe_activate(activationKey);
    if(result == false){
        printf("VSPE API activation error\n");
        faxModem->deviceResult = 1;
		Event_post(faxModem->deviceReady);
		return;
    }
    // initialize VSPE python binding subsystem
    result = vspe_initialize();
    if(result == false){
        printf("Initialization error\n");
        faxModem->deviceResult = 1;
		Event_post(faxModem->deviceReady);
		return;
    }
    // remove all existing devices
    vspe_destroyAllDevices();
    // stop current emulation
    result = vspe_stopEmulation();
    if(result == false)
    {
        printf("Error: emulation can not be stopped: maybe one of VSPE devices is still used.\n");
        vspe_release();
        faxModem->deviceResult = 1;
		Event_post(faxModem->deviceReady);
		return;
    }
    // *********************************
    // Dynamically creating devices
    // *********************************
    // Create Pair device (COM4 => COM5)
    int deviceId = vspe_createDevice("Pair", "4;5;0");
    if(deviceId == -1)
    {
        printf("Error: can not create device\n");
        vspe_release();
        faxModem->deviceResult = 1;
		Event_post(faxModem->deviceReady);
		return;
    }
    // ****************************
    // STEP 2 - EMULATION LOOP
    // ****************************
    // start emulation
    result = vspe_startEmulation();
    if(result == false)
    {
        printf("Error: can not start emulation\n");
        vspe_release();
        faxModem->deviceResult = 1;
		Event_post(faxModem->deviceReady);
		return;
    }
    faxModem->deviceResult = 0;
	Event_post(faxModem->deviceReady);
    // emulation loop
    Event_wait(faxModem->deviceEvent);
    // ****************************
    // STEP 3 - EXIT
    // ****************************

    // stop emulation before exit (skip this call to force kernel devices continue to work)
    result = vspe_stopEmulation();
    if(result == false)
    {
        printf("Error: emulation can not be stopped: maybe one of VSPE devices is still used.\n");
        faxModem->deviceResult = 1;
		return;
    }
    vspe_release();
    faxModem->deviceResult = 0;
    printf("Emulation stopped successful.\n");
}

void sendString(FaxModem *faxModem, const char * str) 
{
	CommPort_write(faxModem->commPort, (unsigned char*)str, strlen(str));
}

void sendOK(FaxModem *faxModem) 
{
	sendString(faxModem, "\r\nOK\r\n");
}

void randomRing(void* data)
{
	FaxModem* faxModem = (FaxModem*)data;
	char cmd[100];
	srand(time(NULL));
	while(faxModem->ringRunning)
	{
		// above 5 seconds and 6 minutes bellow
		faxModem->randomTimeout = 5000 + (rand() % (6 * 60 + 1)) * 1000;
		printf("Waiting %dmin %ds to Ring...\n", faxModem->randomTimeout / 60000, 
			(faxModem->randomTimeout % 60000) / 1000);
		Event_waitEx(faxModem->ringEvent, faxModem->randomTimeout);
		Event_reset(faxModem->ringEvent);
		if(!faxModem->ringRunning)
			break;
		printf("Ringing...\n");
		faxModem->ringState = rsRing;
		sendString(faxModem, "\r\nRING\r\n");
		if(faxModem->callerID)
		{
			sendString(faxModem, "\r\nDATE = 0720\r\n");
			sendString(faxModem, "TIME = 2351\r\n");
			strcpy(cmd, "NAME = ");
			strcat(cmd, faxModem->name);
			strcat(cmd, "\r\n");
			sendString(faxModem, cmd);
			strcpy(cmd, "NMBR = ");
			strcat(cmd, faxModem->number);
			strcat(cmd, "\r\n");
			sendString(faxModem, cmd);
		}
		Thread_wait(5000);
		int i;
		for(i = 0; i < 5; i++)
		{
			if(faxModem->ringState == rsRing) {
				sendString(faxModem, "\r\nRING\r\n");
				Thread_wait(5000);
			} else 
				break;
		}
		if(faxModem->ringState == rsAnswer)
		{
			faxModem->onHook = Event_create();
			Event_wait(faxModem->onHook);
			if(faxModem->ringState == rsAnswer)
				faxModem->ringState = rsIdle;
			Event_free(faxModem->onHook);
			faxModem->onHook = NULL;
		} else {			
			faxModem->ringState = rsNoCarrier;
		}
		sendString(faxModem, "\r\nNO CARRIER\r\n");
	}
}

void createRandomRing(FaxModem *faxModem) 
{
	faxModem->ringEvent = Event_create();
	faxModem->ringThread = Thread_create(randomRing, faxModem);
	faxModem->ringRunning = 1;
	Thread_start(faxModem->ringThread);
}

void freeRandomRing(FaxModem *faxModem) 
{
	faxModem->ringRunning = 0;
	faxModem->ringState = rsNoCarrier;
	if(faxModem->onHook != NULL)
		Event_post(faxModem->onHook);
	Event_post(faxModem->ringEvent);
	Thread_join(faxModem->ringThread);
	Thread_free(faxModem->ringThread);
	Event_free(faxModem->ringEvent);
}

void proccessCommand(FaxModem *faxModem, const char * cmd) 
{
	printf("%s\n", cmd);
	if(stricmp(cmd, "AT") == 0 || stricmp(cmd, "ATZ") == 0)
		sendOK(faxModem);
	else if(stricmp(cmd, "AT+VCID=1") == 0 || stricmp(cmd, "AT#CID=1") == 0 ||
		    stricmp(cmd, "AT#CC1") == 0 || stricmp(cmd, "AT%CCID=1") == 0 || 
			stricmp(cmd, "AT*ID1") == 0)
	{
		faxModem->callerID = 1;
		printf("Caller ID enabled\n");
		sendOK(faxModem);
	}
	else if(stricmp(cmd, "AT+VCID=0") == 0 || stricmp(cmd, "AT#CID=0") == 0 ||
		    stricmp(cmd, "AT#CC0") == 0 || stricmp(cmd, "AT%CCID=0") == 0 || 
			stricmp(cmd, "AT*ID0") == 0)
	{
		faxModem->callerID = 0;
		printf("Caller ID disabled\n");
		sendOK(faxModem);
	}
	else if(stricmp(cmd, "ATA") == 0 || stricmp(cmd, "ATH1") == 0)
	{
		if(faxModem->ringState == rsRing) 
		{
			printf("Ready to connect\n");
			faxModem->ringState = rsAnswer;
			sendOK(faxModem);			
		} else {
			printf("Error not ringing\n");
			sendString(faxModem, "\r\nERROR\r\n");
		}
	}
	else if(stricmp(cmd, "ATH0") == 0)
	{
		if(faxModem->ringState == rsRing || faxModem->ringState == rsAnswer) 
		{
			printf("Hang up\n");
			faxModem->ringState = rsHangUp;
			sendOK(faxModem);
			if(faxModem->onHook != NULL)
				Event_post(faxModem->onHook);
		} else {
			printf("Error not on-hook\n");
			sendString(faxModem, "\r\nERROR\r\n");
		}
	}
}

void listenCommands(void* data)
{
	FaxModem* faxModem = (FaxModem*)data;
	char buffer[256];
	int pos = 0, prev_pos, i;
	int bytesAvailable, bytesRead, bytesToRead;
	
	while(!faxModem->done)
	{
		bytesAvailable = 0;
		if(!CommPort_wait(faxModem->commPort, &bytesAvailable))
			break;
		while(bytesAvailable > 0)
		{
			bytesToRead = bytesAvailable < (255 - pos)? bytesAvailable: (255 - pos);
			bytesRead = CommPort_read(faxModem->commPort, (unsigned char*)buffer + pos, bytesToRead);
			if(!bytesRead)
				break;
			bytesAvailable -= bytesRead;
			prev_pos = pos;
			pos += bytesRead;
			buffer[pos] = '\0';
			// find carriage return
			i = prev_pos;
			while(i < pos)
			{
				if(buffer[i] == 13 || buffer[i] == 10) {
					buffer[i] = '\0';
					proccessCommand(faxModem, buffer);
					if(buffer[i] == 10)
						i++; // skip LF
					i++; // skip CR
					// copy remaining command and find again
					memcpy(buffer, buffer + i, pos - i);
					pos = pos - i;
					buffer[pos] = '\0';
					i = 0;
					continue;
				}
				i++;
			}
		}
	}
}

void mainMenu(FaxModem * faxModem)
{
	char cmd[256];
	while(scanf(" %255[^\n]", cmd) != EOF && stricmp(cmd, "EXIT") != 0)
	{
		// force ring
		if(stricmp(cmd, "RING") == 0)
		{
			Event_post(faxModem->ringEvent);
			printf("OK\n");
		}
		// force ring
		else if(strnicmp(cmd, "SET ", 4) == 0)
		{
			if(strnicmp(cmd + 4, "NUMBER ", 7) == 0)
			{
				strcpy(faxModem->number, cmd + 11);
				printf("OK\n");
			}
			else if(strnicmp(cmd + 4, "NAME ", 5) == 0)
			{
				strcpy(faxModem->number, cmd + 9);
				printf("OK\n");
			}
		}
	}
	printf("Exiting...\n");
}

int main(int argc, char** argv)
{
	FaxModem faxModem;
	memset(&faxModem, 0, sizeof(FaxModem));
	strcpy(faxModem.name, "MZSW CREATIVE SOFTWARE");
	strcpy(faxModem.number, "086987654321");
	faxModem.deviceEvent = Event_create();
	faxModem.deviceReady = Event_create();
	faxModem.thread = Thread_create(createDevices, &faxModem);
	Thread_start(faxModem.thread);
    Event_wait(faxModem.deviceReady);
	if(!faxModem.deviceResult)
	{
		faxModem.commPort = CommPort_create("COM5");
		createRandomRing(&faxModem);
		faxModem.listenThread = Thread_create(listenCommands, &faxModem);
		Thread_start(faxModem.listenThread);
		mainMenu(&faxModem);
		faxModem.done = 1;
		freeRandomRing(&faxModem);
		CommPort_free(faxModem.commPort);
		Thread_join(faxModem.listenThread);
		Thread_free(faxModem.listenThread);
	}
	Event_post(faxModem.deviceEvent);
	Thread_join(faxModem.thread);
	Thread_free(faxModem.thread);
	Event_free(faxModem.deviceEvent);
	Event_free(faxModem.deviceReady);
	return 0;
}