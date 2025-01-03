#include <windows.h>
#include <iostream>
#include <psapi.h>

bool inject(HANDLE hProcess, const char* dllName) {
  // Allocates some memory for the DLL filename
  LPVOID remoteBuffer = VirtualAllocEx(hProcess, NULL, strlen(dllName), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
  if (remoteBuffer == NULL) return false;

  // Gets the address of a function that will be used to load the DLL later
  HMODULE hKernel = GetModuleHandleA("kernel32.dll");
  LPTHREAD_START_ROUTINE functionPtr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "LoadLibraryA");
  if (functionPtr == NULL) return false;

  // Writes the name of the dll to the virtual memory that was recently allocated
  if (!WriteProcessMemory(hProcess, remoteBuffer, dllName, strlen(dllName), NULL)) return false;

  // Creates a thread that loads the DLL
  HANDLE remoteThread = CreateRemoteThread(hProcess, NULL, 0, functionPtr, remoteBuffer, 0, NULL);
  if (remoteThread == NULL) return false;

  std::cout << "DLL was injected succesfully!\nWaiting for thread to close...\n";

  WaitForSingleObject(remoteThread, INFINITE);

  std::cout << "Thread closed\n";

  VirtualFreeEx(hProcess, remoteBuffer, strlen(dllName), MEM_RELEASE);
  CloseHandle(remoteThread);

  return true;
}

int main() {
  const char* dllName = "C:\\injected.dll";

  STARTUPINFO si = {};
  PROCESS_INFORMATION pi = {};
  si.cb = sizeof(si);

  // SetConsoleOutputCP(CP_UTF8);
  // SetConsoleCP(CP_UTF8);

  // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
  if (!CreateProcess(TEXT("victim.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    std::cerr << "CreateProcess error\n";
    return 1;
  }

  if (!inject(pi.hProcess, dllName)) {
    std::cerr << "failed to inject dll\n";
    return 1;
  };

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}