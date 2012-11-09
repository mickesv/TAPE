#include <iostream>
#include <sstream>
#include <string>
#include "debug.hh"

int Debug::myLevel=1;

void Debug::print(const int theLevel, const char* theMessage)
{
  if (Debug::myLevel>=theLevel) {
    std::cout << theMessage << std::endl;
  }
}

void Debug::print(const int theLevel, const std::string& theMessage)
{
  print(theLevel, theMessage.c_str());
}

void Debug::print(const int theLevel, const int theMessage)
{
  std::stringstream ss;
  ss << theMessage;
  print(theLevel, ss.str());
}
  
void Debug::setLevel(const int theLevel)
{
  Debug::myLevel=theLevel;
}
