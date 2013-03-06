#ifndef PARAMETERCOUNTPARSER_HH
#define PARAMETERCOUNTPARSER_HH

#include "parser.hh"
#include "config.hh"

// Warns if parametercounts are larger than a threshold value

class ParameterCountParser : public Parser
{
public:
  ParameterCountParser(Config &theConfig) {myConfig=&theConfig;};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  Config* myConfig;
};


#endif
