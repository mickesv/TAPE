#include <clang-c/Index.h>
#include <string>
#include <sstream>

#include "parser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"
#include "config.hh"
#include "warnings.hh"
#include "parametercountparser.hh"

void ParameterCountParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  theData->set("parameterCount", 0);
}

void ParameterCountParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);
  int pcount=theData->get("parameterCount");
  int maxPCount=myConfig->getInt("maxParameters");

  if (pcount>maxPCount) {
    Warnings* w=Warnings::getInstance();
    CXCursor* theFunctionCursor=theFunction->getCursor();
    stringstream s;
    s << "May use too many parameters. Threshold value is " << maxPCount << ". ";
    s << "Current number of parameters is " << pcount << ".";

    w->addWarning(*theFunctionCursor, theData->getNode()->getArg("name"), s.str());
  }
}

CXChildVisitResult ParameterCountParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{  
  if (theCursor.kind==CXCursor_ParmDecl) {
    theData->add("parameterCount", 1);
  }

  return CXChildVisit_Continue;
}
