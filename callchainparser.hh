#ifndef CALLCHAINPARSER_HH
#define CALLCHAINPARSER_HH

#include <list>

#include "parser.hh"
#include "config.hh"

// Warns if call chains are longer than a threshold value

class CallChainTokenList;

class CallChainParser : public Parser
{
public:
  CallChainParser(Config &theConfig) {myConfig=&theConfig;};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  int countCalls(CallChainTokenList *theT);

  Config* myConfig;
  list<CallChainTokenList*> myList;
};


#endif
