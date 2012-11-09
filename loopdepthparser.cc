
#include <string>
#include <stack>
#include "loopdepthparser.hh"

using namespace std;

void LoopdepthParser::startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  theData->set((string) "maxLoopDepth", 0);
}

void LoopdepthParser::endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);
}

CXChildVisitResult LoopdepthParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  CXSourceLocation theLocation=clang_getCursorLocation(theCursor);
  while (!isWithinCurrentCompound(theLocation)) {
    myCSEnds.pop();
    theData->add("maxLoopDepth", -1);
  }

  switch( theCursor.kind )
    {
    case CXCursor_DoStmt :
    case CXCursor_ForStmt :
      // TODO: If a callExpr is used in the initialisation-part of a for loop, it should not have increased loopDepth.
    case CXCursor_WhileStmt :
      {
	CXSourceRange theRange=clang_getCursorExtent(theCursor);
	myCSEnds.push(clang_getRangeEnd(theRange));
	theData->add("maxLoopDepth", 1);
	break;
      }
    default:
      break;
    }
  
  return Parser::parse(theCursor, theParent, theData);
}


bool LoopdepthParser::isWithinCurrentCompound(CXSourceLocation theLocation)
{
  if(myCSEnds.empty()) {
    // Top scope, I guess we are always within this.
    return true;
  }

  unsigned* theEndLine=new unsigned();
  unsigned* theEndCol=new unsigned();
  unsigned* theLocLine=new unsigned();
  unsigned* theLocCol=new unsigned();

  clang_getPresumedLocation(myCSEnds.top(), NULL, theEndLine, theEndCol);
  clang_getPresumedLocation(theLocation, NULL, theLocLine, theLocCol);
  
  if (*theLocLine==*theEndLine) {
    if (*theLocCol < *theEndCol) {
      return true;
    } else {
      return false;
    }
  } else if (*theLocLine < *theEndLine) {
    return true;
  }

  // Default: I am no longer within the compound statement
  return false;
    
}

