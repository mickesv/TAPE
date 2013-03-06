#include <clang-c/Index.h>
#include <sstream>

#include "warnings.hh"
#include "debug.hh"
#include "config.hh"

Warnings* Warnings::myInstance=0;

Warnings::Warnings(Config &theConfig)
{
  myConfig=&theConfig;
}

Warnings* Warnings::getInstance()
{
  return myInstance;
}

void Warnings::initialise(Config &theConfig)
{
  myInstance=new Warnings(theConfig);
}

void Warnings::addWarning(const CXCursor &theCursor, const string theFunctionName, const string theWarning)
{
  CXSourceLocation theLocation=clang_getCursorLocation(theCursor);
  unsigned* theLocLine=new unsigned();
  unsigned* theLocCol=new unsigned();
  CXString* theFileName=new CXString();
  clang_getPresumedLocation(theLocation, theFileName, theLocLine, theLocCol);
    
  stringstream ss;
  ss << clang_getCString(*theFileName) << " (" << *theLocLine << "," << *theLocCol << ") ";
  ss << "in function " << theFunctionName << ": ";
  ss << theWarning;
  Debug::print(1, (string) "WARNING " + ss.str());

  clang_disposeString(*theFileName);    
}
