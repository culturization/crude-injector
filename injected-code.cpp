// dllmain.cpp : Defines the entry point for the DLL application.
#include <stdio.h>
#include <windows.h>

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	// MessageBox (0, (LPCTSTR)"From DLL\n", (LPCTSTR)"Process Detach", MB_ICONINFORMATION);

    while (1) {
        printf("123132123");
    }

	return TRUE;
}