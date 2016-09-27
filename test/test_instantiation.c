#include <stdio.h>
#include <stdlib.h>
#include <ModemCID.h>
#include <windows.h>
typedef ModemCID * (LIBCALL * ModemCID_createFunc)(const char*);
typedef void (LIBCALL * ModemCID_freeFunc)(ModemCID*);

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary("../bin/x86/ModemCID.dll");
	ModemCID_createFunc _ModemCID_create;
	ModemCID_freeFunc _ModemCID_free;
	if(hModule == 0)
		return 1;
	_ModemCID_create = (ModemCID_createFunc)GetProcAddress(hModule, "ModemCID_create");
	_ModemCID_free = (ModemCID_freeFunc)GetProcAddress(hModule, "ModemCID_free");
	
	ModemCID* lib = _ModemCID_create(NULL);
	system("pause");
	_ModemCID_free(lib);
	FreeLibrary(hModule);
	return 0;
}
