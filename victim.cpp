#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

int main() {
  SetConsoleTitleA("GTA 6");
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  std::vector<std::string> lyrics {
      "fuck you russia game",
      "gta for usa",
      "russia no game gta",
      "вы все лохи игра выйдет только на территории сша и других стран нато",
      "так что вы все облажались"
  };

  while(1) {
      for (std::string str : lyrics) {
          std::cout << str << "\n";
      }
      std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}