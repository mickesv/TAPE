#ifndef WARNINGS_HH
#define WARNINGS_HH

#include <clang-c/Index.h>

#include "config.hh"

class Warnings
{
public:
  static Warnings* getInstance();
  static void initialise(Config &theConfig);
  static int summarise();
  
  void addWarning(const CXCursor &theCursor, const string theFunctionName, const string theWarning);
  void addWarning(const string &theFunctionName, const string &theWarning);
  
protected:
  Warnings(Config &theConfig);

private:

  static Warnings* myInstance;
  static int myWarningsCount;
  Config* myConfig;

  
};

#endif
