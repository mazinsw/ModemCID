#include <stdio.h>
#include <stdlib.h>
#include <ModemCID.h>
#include <windows.h>

int main(int argc, char *argv[])
{
	HMODULE hModule = LoadLibrary("../bin/x86/ModemCID.dll");
	if(hModule == 0)
		return 1;
	FreeLibrary(hModule);
	return 0;
}
