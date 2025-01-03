// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

void _main();
void *getBaseAddr();

char moduleName[] = "victim.exe";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) _main();

	return TRUE;
}

void _main() {
    void *baseAddr = getBaseAddr();
    if (baseAddr == NULL) {
        std::cerr << "failed to find base address :(\n";
        return;
    }

    void *bufAddr = baseAddr + 0x3000;

    char string[] = "hello, world!\n";
    memcpy(bufAddr, &string, sizeof(string));
}

void *getBaseAddr() {
    MODULEENTRY32 moduleEntry = {};
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (!snapshot) return NULL;

    BOOL succ = Module32First(snapshot, &moduleEntry);

    while (succ) {
        if (!strcmp((char*)moduleEntry.szModule, moduleName)) {
            CloseHandle(snapshot);
            return (void*)moduleEntry.modBaseAddr;
        }
        succ = Module32Next(snapshot, &moduleEntry);
    }

    CloseHandle(snapshot);
    return NULL;
}