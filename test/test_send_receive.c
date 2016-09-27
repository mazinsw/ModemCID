#include <stdio.h>
#include <stdlib.h>
#include <ModemCID.h>
#include <windows.h>

typedef ModemCID * (LIBCALL * ModemCID_createFunc)(const char*);
typedef int (LIBCALL * ModemCID_pollEventFunc)(ModemCID*, ModemEvent*);
typedef void (LIBCALL * ModemCID_cancelFunc)(ModemCID*);
typedef void (LIBCALL * ModemCID_freeFunc)(ModemCID*);
typedef void (LIBCALL * ModemCID_setConfigurationFunc)(ModemCID*, const char*);
typedef const char * (LIBCALL * ModemCID_getConfigurationFunc)(ModemCID*);

int main(int argc, char *argv[])
{
	DWORD startTick;
	ModemEvent event;
	DWORD endTick;
	ModemCID* lib;
	HMODULE hModule;
	
	startTick = GetTickCount();
	hModule = LoadLibrary("../bin/x86/ModemCID.dll");
	ModemCID_createFunc _ModemCID_create;
	ModemCID_pollEventFunc _ModemCID_pollEvent;
	ModemCID_setConfigurationFunc _ModemCID_setConfiguration;
	ModemCID_getConfigurationFunc _ModemCID_getConfiguration;
	ModemCID_cancelFunc _ModemCID_cancel;
	ModemCID_freeFunc _ModemCID_free;
	if(hModule == 0)
		return 1;
	_ModemCID_create = (ModemCID_createFunc)GetProcAddress(hModule, "ModemCID_create");
	_ModemCID_pollEvent = (ModemCID_pollEventFunc)GetProcAddress(hModule, "ModemCID_pollEvent");
	_ModemCID_setConfiguration = (ModemCID_setConfigurationFunc)GetProcAddress(hModule, "ModemCID_setConfiguration");
	_ModemCID_getConfiguration = (ModemCID_getConfigurationFunc)GetProcAddress(hModule, "ModemCID_getConfiguration");
	_ModemCID_cancel = (ModemCID_cancelFunc)GetProcAddress(hModule, "ModemCID_cancel");
	_ModemCID_free = (ModemCID_freeFunc)GetProcAddress(hModule, "ModemCID_free");
	lib = _ModemCID_create(NULL);
	while(_ModemCID_pollEvent(lib, &event))
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
	_ModemCID_free(lib);
	FreeLibrary(hModule);
	return 0;
}
