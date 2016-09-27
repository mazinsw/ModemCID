#include <private/Platform.h>
#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		return ModemCID_initialize();
	case DLL_PROCESS_DETACH:
		ModemCID_terminate();
		break;
	}
	return TRUE; // succesful
}
#ifdef DEBUGLIB
#include "ModemCID.h"
#include "Thread.h"
#include <stdio.h>
#include <windows.h>

int main(int argc, char** argv)
{
	DWORD startTick;
	ModemEvent event;
	DWORD endTick;
	ModemCID* lib;
	
	startTick = GetTickCount();
	ModemCID_initialize();
	lib = ModemCID_create(NULL);
	while(ModemCID_pollEvent(lib, &event))
	{
		switch(event.type)
		{
		case MODEM_EVENT_CONNECTED:
			endTick = GetTickCount() - startTick;
			printf("Modem connected\n");
			printf("Time: %.3lfs\n", endTick / 1000.0);
			break;
		case MODEM_EVENT_DISCONNECTED:
			printf("Modem disconnected\n");
			startTick = GetTickCount();
			break;
		case MODEM_EVENT_RING:
			printf("Modem riging\n");
			break;
		case MODEM_EVENT_CALLERID:
			printf("Number: %s\n", event.data);
			break;
		case MODEM_EVENT_ONHOOK:
			printf("Modem on-hook\n");
			break;
		case MODEM_EVENT_OFFHOOK:
			printf("Modem off-hook\n");
			break;
		case MODEM_EVENT_HANGUP:
			printf("Modem hang up\n");
			break;
		default:
			printf("Event %d received\n", event.type);
		}
	}
	ModemCID_free(lib);
	ModemCID_terminate();
	return 0;
}
#endif