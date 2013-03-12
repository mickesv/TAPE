#include <clang-c/Index.h>
#include <string>
#include <sstream>

#include "parser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"
#include "verboseparser.hh"

void VerboseParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);

  Debug::print(1, "Verbose: --------------------");
  Debug::print(1, (string) "Verbose: Starting Function " + theFunction->getArg("name"));
}

void VerboseParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);

  Debug::print(1, (string) "Verbose: Ending Function " + theFunction->getArg("name"));
  Debug::print(1, "Verbose: --------------------");
}

CXChildVisitResult VerboseParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  string cname=clang_getCString(clang_getCursorDisplayName(theCursor));
  string ckind=clang_getCString(clang_getCursorKindSpelling(clang_getCursorKind(theCursor)));
  string ctype=clang_getCString(clang_getTypeKindSpelling(clang_getCursorType(theCursor).kind));
  CXSourceLocation theLocation=clang_getCursorLocation(theCursor);
  unsigned* theLocLine=new unsigned();
  unsigned* theLocCol=new unsigned();
  clang_getPresumedLocation(theLocation, NULL, theLocLine, theLocCol);
    
  stringstream s;
  s << "Verbose: " << "(" << *theLocLine << "," << *theLocCol << ") ";
  s << ckind << ":" << cname << ":" << ctype;

  Debug::print(1, (string) s.str());

  // Always recurse
  return CXChildVisit_Recurse;
}
