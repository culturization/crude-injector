#include <windows.h>
#include <iostream>
#include <psapi.h>

bool inject(HANDLE hProcess, const char* dllName) {
  // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualallocex
  PVOID remoteBuffer = VirtualAllocEx(
    hProcess,                     // process
    NULL,
    strlen(dllName),              // size of allocated memory
    (MEM_RESERVE | MEM_COMMIT),   // some flags
    PAGE_EXECUTE_READWRITE
  );

  if (remoteBuffer == NULL) {
    std::cerr << "VirtualAllocEx error\n";
    return false;
  }

  // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea
  HMODULE hKernel = GetModuleHandleA("kernel32.dll");

  // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress
  LPVOID functionPtr = (LPVOID)GetProcAddress(
    hKernel,        // module handle
    "LoadLibraryA"  // function name
  );

  if (functionPtr == NULL) {
    std::cerr << "GetProcAddress error\n";
    return false;
  }

  // https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory
  if(!WriteProcessMemory(
    hProcess,             // proc
    remoteBuffer,         // address where the code will be written to
    dllName,              // payload
    strlen(dllName),      // payload len
    NULL                  // [output] number of bytes written
  )) {
    std::cerr << "WriteProcessMemory error\n";
    return false;
  }

  // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createremotethread
  HANDLE remoteThread = CreateRemoteThread(
    hProcess,   // process handle
    NULL,       // thread attributes
    0,          // initial stack size
    (LPTHREAD_START_ROUTINE)functionPtr, // executed start address
    // The function must exist in the remote process, so we use a function from the kernel
    (LPVOID)remoteBuffer,       // thread parameter
    0,          // some flags
    NULL        // ptr to a var that receives the thread id
  );

  if (remoteThread == NULL) {
    std::cerr << "CreateRemoteThread error\n";
    return false;
  }

  std::cout << "DLL was injected succesfully!\nWaiting for thread to close...\n";

  WaitForSingleObject(remoteThread, INFINITE);

  std::cout << "Thread closed\n";

  VirtualFreeEx(hProcess, remoteBuffer, strlen(dllName), MEM_RELEASE);

  CloseHandle(remoteThread);

  return true;
}

int main() {
  const char* dllName = "C:\\injected.dll";

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  si.cb = sizeof(si);

  // MessageBox (0, (LPCTSTR)"From DLL\n", (LPCTSTR)"Process Detach", MB_ICONINFORMATION);

  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  // https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
  if (!CreateProcess(
    TEXT("victim.exe"),     // prog name
    NULL,                   // command line
    NULL,
    NULL,
    FALSE,
    0,                      // creation flags
    NULL,
    NULL,
    &si,                    // [output] startup info ptr
    &pi                     // [output] process info ptr
  )) {
    std::cerr << "CreateProcess error\n";
    return 1;
  }

  inject(pi.hProcess, dllName);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}