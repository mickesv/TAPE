#ifndef VERBOSEPARSER_HH
#define VERBOSEPARSER_HH

#include "parser.hh"
#include "config.hh"

// The VerposeParser simply prints as much information as possible about everything

class VerboseParser : public Parser
{
public:
  VerboseParser(Config &theConfig) { myConfig=&theConfig;};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  Config *myConfig;
};


#endif
