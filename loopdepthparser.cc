
#include <string>
#include <stack>
#include "loopdepthparser.hh"

using namespace std;

void LoopdepthParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  theData->set("maxLoopDepth", 0);
}

void LoopdepthParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);
}

CXChildVisitResult LoopdepthParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  while (ascendCompound(theCursor, theData)) {
    theData->add("maxLoopDepth", -1);
  }

  switch( theCursor.kind )
    {
    case CXCursor_DoStmt :
    case CXCursor_ForStmt :
      // TODO: If a callExpr is used in the initialisation-part of a for loop, it should not have increased loopDepth.
    case CXCursor_WhileStmt :
      {
	descendCompound(theCursor, theData);
	theData->add("maxLoopDepth", 1);
	break;
      }
    default:
      break;
    }
  
  return Parser::parse(theCursor, theParent, theData);
}


