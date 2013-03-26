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

  // Basic printout
  stringstream s;
  s << "Verbose: " << "(" << *theLocLine << "," << *theLocCol << ") ";
  s << ckind << ":" << cname << ":" << ctype;

  Debug::print(1, (string) s.str());

  // Check for range
  if (Debug::debugLevel(2)) {
    CXSourceRange theRange=clang_getCursorExtent(theCursor);
    CXSourceLocation theStart=clang_getRangeStart(theRange);
    CXSourceLocation theEnd=clang_getRangeEnd(theRange);
    unsigned* theStartLine=new unsigned();
    unsigned* theStartCol=new unsigned();
    unsigned* theEndLine=new unsigned();
    unsigned* theEndCol=new unsigned();
    
    clang_getPresumedLocation(theStart, NULL, theStartLine, theStartCol);
    clang_getPresumedLocation(theEnd, NULL, theEndLine, theEndCol);
    
    s.str("");
    s.clear();
    s << "Verbose:  Range start " << *theStartLine << "," << *theStartCol;
    s << " end " << *theEndLine << "," << *theEndCol;
    
    Debug::print(2, (string) s.str());
  }

  // Just an experiment, to see if I could find the source code for the current statement.
  /*
  {
    const char *startBuf, *endBuf;
    unsigned startLine, startColumn, endLine, endColumn;
    clang_getDefinitionSpellingAndExtent(theCursor, &startBuf, &endBuf,
					 &startLine, &startColumn,
					 &endLine, &endColumn);
    
    s.str("");
    s.clear();
    s << "(" << startLine << ":" << startColumn << ")->(" << endLine << ":" << endColumn << ") ";
    while (startBuf <= endBuf) {
      s << *startBuf++;
    }
    Debug::print(1, (string) s.str());
  }
  */

  // Fool around with tokenization
  if (Debug::debugLevel(3))
  {
    CXSourceRange theRange = clang_getCursorExtent(theCursor);
    CXToken* theTokens;
    unsigned numTokens;
    CXTranslationUnit theTU=clang_Cursor_getTranslationUnit(theCursor);
    clang_tokenize(theTU, theRange, &theTokens, &numTokens);

    s.str("");
    s.clear();
    s << "Verbose:  Tokens: #";
    
    for (unsigned i = 0; i < numTokens; i++) {
      // Have a look at the tokens
      string tname=clang_getCString(clang_getTokenSpelling(theTU, theTokens[i]));
      s << tname << "#";
    }

    Debug::print(3, (string) s.str());

    clang_disposeTokens(theTU, theTokens, numTokens);
  }
  // Always recurse
  return CXChildVisit_Recurse;
}
