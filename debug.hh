#ifndef DEBUG_HH
#define DEBUG_HH

#include <string>

class Debug
{
public:
  static void print(const int theLevel, const char* theMessage);  
  static void print(const int theLevel, const std::string& theMessage);  
  static void print(const int theLevel, const int theMessage);  
  static void setLevel(const int theLevel);

  static int myLevel;
};


#endif
