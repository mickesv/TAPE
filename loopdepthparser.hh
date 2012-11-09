#ifndef LOOPDEPTHPARSER_HH
#define LOOPDEPTHPARSER_HH

#include <clang-c/Index.h>
#include <stack>
#include "parser.hh"

class LoopdepthParser : public Parser
{
public:
  virtual void startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  bool isWithinCurrentCompound(CXSourceLocation theLocation);

  stack<CXSourceLocation> myCSEnds;  
};


#endif
