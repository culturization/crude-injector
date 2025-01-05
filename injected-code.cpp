#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <map>

std::wstring mainModuleName = L"victim.exe";
std::map<std::wstring, uintptr_t> moduleAddresses;

void _main();
void getModuleAddresses();

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) _main();

	return 1;
}

// GetModuleHandleA
// GetProcAddress

void _main() {
    getModuleAddresses();

    for (const auto& [key, _] : moduleAddresses) std::wcout << key << "\n";

    if (moduleAddresses.count(mainModuleName) == 0) {
        std::cerr << "failed to find base address\n";
        return;
    }

    uintptr_t baseAddr = moduleAddresses[mainModuleName];
    uintptr_t bufAddr = baseAddr + 0x3000;

    char string[] = "hello, world!\n";
    memcpy((void*)bufAddr, &string, sizeof(string));
}

void getModuleAddresses() {
    MODULEENTRY32W moduleEntry = {};
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (!snapshot) return;

    if (!Module32FirstW(snapshot, &moduleEntry)) {
        CloseHandle(snapshot);
        return;
    };

    do {
        std::wstring currModuleName(moduleEntry.szModule);
        moduleAddresses[currModuleName] = (uintptr_t)moduleEntry.modBaseAddr;
    } while (Module32NextW(snapshot, &moduleEntry));

    CloseHandle(snapshot);
    return;
}