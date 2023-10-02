//#include <iostream>
//#include <thread>
//#include <chrono>
//#include <random>
//#include <windows.h>
//#include <sstream>
//
//#define MESS 0
//#if 0
//#define INITIAL_DELAY 0
//#define DELAY 3
//#else
//#define INITIAL_DELAY 50
//#define DELAY 100
//#endif
//
//static auto _ = []() {
//  std::thread{ []() {
//      std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_DELAY));
//
//#if MESS == 0
//      // SMILEYS
//      for(int i = 0; ; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
//
//        CONSOLE_SCREEN_BUFFER_INFO csbi;
//        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1, rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
//        std::cout << "\033[s\033[" << (rand() % rows) << ";" << (rand() % (columns - 3)) << "H\033[38;5;" << (rand()%255) << "m(-:\033[u";
//      }
//#elif MESS == 1
//      // not thread safe
//      std::stringstream buffer;
//      std::streambuf *old = std::cout.rdbuf(buffer.rdbuf());
//      size_t total = 0;
//      while (true) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        std::string text = buffer.str();
//        buffer.str("");
//        if (text.size() > 20)
//          text.insert(3, "A");
//        old->sputn(text.c_str(), text.size());
//        total += text.size();
//      }
//#elif MESS == 2
//      // MATRIX
//      const char *CODES[] = { "\033[32m", "\033[92m", "\033[0m", "" };
//      std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_DELAY));
//      for (int i = 0; ; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(3));
//      
//        CONSOLE_SCREEN_BUFFER_INFO csbi;
//        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1, rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
//        std::stringstream ss;
//        ss << "\033[s\033[H";
//        for (int j = 0; j < rows; j++) {
//          int r = (std::cos(j)*.5f-.5f + i/(200.f+std::sin(j)*150.f)) * columns;
//          r = min(r, columns);
//          ss << "\033[" << j << ";0H";
//          for (int k = 0; k < r; k++) {
//            ss << (unsigned char)(rand() % (218-179) + 179);
//            ss << CODES[rand() % 4];
//          }
//        }
//        ss << "\033[u";
//        //std::cout << "\033[s\033[" << (rand() % rows) << ";" << (rand() % (columns - 3)) << "H\033[38;5;" << (rand() % 255) << "m(-:\033[u";
//        std::cout << ss.str();
//      }
//#elif MESS == 3
//      for (int i = 0; ; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
//        //std::cout << MESSAGES[i % (sizeof()];
//      }
//#endif
//  }}.detach();
//
//  return 0;
//}();
