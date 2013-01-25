#ifndef LOOPDEPTHPARSER_HH
#define LOOPDEPTHPARSER_HH

#include <clang-c/Index.h>
#include <stack>
#include "parser.hh"

class LoopdepthParser : public Parser
{
public:
  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

protected:

private:
};


#endif
