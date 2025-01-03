#include <windows.h>
#include <iostream>
#include <thread>

char buf[] = "original buffer contents\n";

int main() {
  SetConsoleTitleA("GTA 6");
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  while (1) {
    std::cout << buf;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}
