#include <windows.h>
#include <iostream>

const wchar_t dllPath[] = L"C:\\injected.dll";
const wchar_t gamePath[] = L"victim.exe";

bool inject(HANDLE hProcess, LPVOID remoteBuffer);

int main() {
  STARTUPINFOW si = {};
  PROCESS_INFORMATION pi = {};
  si.cb = sizeof(si);

  if (!CreateProcessW(gamePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    std::cerr << "failed to create a process\n";
    return 1;
  }

  // Allocates some memory for the DLL filename
  LPVOID remoteBuffer = VirtualAllocEx(pi.hProcess, NULL, sizeof(dllPath), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
  if (remoteBuffer == NULL) return false;

  bool injectResult = inject(pi.hProcess, remoteBuffer);

  VirtualFreeEx(pi.hProcess, remoteBuffer, sizeof(dllPath), MEM_RELEASE);

  if (!injectResult) {
    std::cerr << "failed to inject dll\n";
    return 1;
  };

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

bool inject(HANDLE hProcess, LPVOID remoteBuffer) {
  // Gets the address of a function that will be used to load the DLL later
  HMODULE hKernel = GetModuleHandleA("kernel32.dll");
  LPTHREAD_START_ROUTINE functionPtr = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "LoadLibraryW");
  if (functionPtr == NULL) return false;

  // Writes the name of the dll to the virtual memory that was recently allocated
  if (!WriteProcessMemory(hProcess, remoteBuffer, dllPath, sizeof(dllPath), NULL)) return false;

  // Creates a thread that loads the DLL
  HANDLE remoteThread = CreateRemoteThread(hProcess, NULL, 0, functionPtr, remoteBuffer, 0, NULL);
  if (remoteThread == NULL) return false;

  WaitForSingleObject(remoteThread, INFINITE);
  CloseHandle(remoteThread);

  return true;
}