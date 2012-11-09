
#include <string>
#include <stack>
#include <sstream>
#include "functionsizeparser.hh"

using namespace std;

/* Calculation of metrics as follows:
   ----------
   McCabe (http://manpages.ubuntu.com/manpages/natty/man1/pmccabe.1.html)
    Modified McCabe: +1 for every for, if, while, switch, &&, ||,  and  ?
    Original McCabe: as above, but count case instead of switch
    I am using the original McCabe, except that I can't get hold of && and ||.
    also, multiple exits (returns) ought to be treated differently...
 */


void FunctionSizeParser::startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  theData->set((string) "mcCabe", 0);
}

void FunctionSizeParser::endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);
  stringstream ss;
  ss << theData->get("mcCabe");
  theFunction->setArg("size", ss.str());
}

CXChildVisitResult FunctionSizeParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{

  switch(theCursor.kind) {
    case CXCursor_DoStmt :
    case CXCursor_ForStmt :
    case CXCursor_WhileStmt :
    case CXCursor_CaseStmt :
    case CXCursor_DefaultStmt :
    case CXCursor_IfStmt :      
    case CXCursor_ConditionalOperator :
      {
	theData->add("mcCabe", 1);
	break;
      }
  default:
    break;
  }
  return Parser::parse(theCursor, theParent, theData);
}

