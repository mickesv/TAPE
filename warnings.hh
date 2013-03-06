#ifndef WARNINGS_HH
#define WARNINGS_HH

#include <clang-c/Index.h>

#include "config.hh"

class Warnings
{
public:
  static Warnings* getInstance();
  static void initialise(Config &theConfig);
  
  void addWarning(const CXCursor &theCursor, const string theFunctionName, const string theWarning);
  
protected:
  Warnings(Config &theConfig);

private:

  static Warnings* myInstance;
  Config* myConfig;
  
};

#endif
