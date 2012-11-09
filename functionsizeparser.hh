#ifndef FUNCTIONSIZEPARSER_HH
#define FUNCTIONSIZEPARSER_HH

#include <clang-c/Index.h>
#include <stack>
#include "parser.hh"

class FunctionSizeParser : public Parser
{
public:
  virtual void startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
};


#endif
