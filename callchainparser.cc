#include <clang-c/Index.h>
#include <string>
#include <list>
#include <sstream>


#include "parser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"
#include "config.hh"
#include "warnings.hh"
#include "callchainparser.hh"

// The name of this class is not the best, but it'll do for now.
class CallChainTokenList
{
public:
  CallChainTokenList(CXCursor theCursor, unsigned startL, unsigned startC, unsigned endL, unsigned endC, const string &theTokens);

  bool isWithin(unsigned theL, unsigned theC);
  bool isWithin(CallChainTokenList *theT);
  
  CXCursor myCursor;
  unsigned myStartLine;
  unsigned myStartCol;
  unsigned myEndLine;
  unsigned myEndCol;
  string myTokens;
private:
};

CallChainTokenList::CallChainTokenList(CXCursor theCursor, unsigned startL, unsigned startC, unsigned endL, unsigned endC, const string &theTokens)
{
  myCursor=theCursor;
  myStartLine=startL;
  myStartCol=startC;
  myEndLine=endL;
  myEndCol=endC;
  myTokens=theTokens;
}

bool CallChainTokenList::isWithin(unsigned theL, unsigned theC)
{
  if (theL==myEndLine) {
    if (theC < myEndCol) {
      return true;
    } else {
      return false;
    }
  } else if (theL==myStartLine) {
    if (theC > myStartCol) {
      return true;
    } else {
      return false;
    }
  } else if ((theL > myStartLine) &&
	     (theL < myEndLine)) {
    return true;
  }
}

bool CallChainTokenList::isWithin(CallChainTokenList *theT) {
  return (isWithin(theT->myStartLine, theT->myStartCol) &&
	  isWithin(theT->myEndLine, theT->myEndCol));
}

// ----------

void CallChainParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
}

void CallChainParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);
}

CXChildVisitResult CallChainParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  CallChainTokenList *theToken=0;
  CallChainTokenList *theFirst=0;
  CXChildVisitResult retVal=Parser::parse(theCursor, theParent, theData);

  if (!myList.empty()) {
    theFirst=myList.front();
  }
  
  // Get the range of the current cursor
  CXSourceRange theRange=clang_getCursorExtent(theCursor);
  CXSourceLocation theStart=clang_getRangeStart(theRange);
  CXSourceLocation theEnd=clang_getRangeEnd(theRange);
  unsigned theStartLine;
  unsigned theStartCol;
  unsigned theEndLine;
  unsigned theEndCol;
    
  clang_getPresumedLocation(theStart, NULL, &theStartLine, &theStartCol);
  clang_getPresumedLocation(theEnd, NULL, &theEndLine, &theEndCol);

  // if it is a callexpr or a member reference, let's do some checkings
  if ((theCursor.kind==CXCursor_CallExpr) ||
      (theCursor.kind==CXCursor_MemberRefExpr)) {
    Debug::print(10, "   CallChainParser Entering CallExpr or MemberRefExpr");

    // Get the tokens
    CXToken* theTokens;
    unsigned numTokens;
    CXTranslationUnit theTU=clang_Cursor_getTranslationUnit(theCursor);
    clang_tokenize(theTU, theRange, &theTokens, &numTokens);

    stringstream s;
    s << "#";    
    for (unsigned i = 0; i < numTokens; i++) {
      string tname=clang_getCString(clang_getTokenSpelling(theTU, theTokens[i]));
      s << tname << "#";
    }
    // Create an element for my queue
    theToken=new CallChainTokenList(theCursor, theStartLine, theStartCol, theEndLine, theEndCol, (string) s.str());
    retVal=CXChildVisit_Recurse;
  }

  stringstream s;
  s << theStartLine << ":" << theStartCol;

  // Before doing anything else, I need to take care of the queue, if I have dropped out of the previous statement.
  // Am I still within the same statement?
  if (theFirst!=0) {
    if (!theFirst->isWithin(theEndLine, theEndCol)) {      
      // Outside this statement, Now I need to do something with my carefully accumulated queue
      Debug::print(20, s.str() + " Fell out of statement. Let's parse the queue");
      for(list<CallChainTokenList*>::iterator i=myList.begin(); i!=myList.end(); i++) {
	Debug::print(20, (string) (*i)->myTokens);
      }

      // Get the last element
      while(!myList.empty()) {
	CallChainTokenList *t=myList.back();
	myList.pop_back();
	
	// Count the calls
	int calls=countCalls(t);
	int maxCCCount=myConfig->getInt("maxCallChains");
	if (calls >= maxCCCount) {
	  Warnings* w=Warnings::getInstance();
    
	  stringstream s;
	  s << "Counted to " << calls << " calls in a chain. Threshold value is " << maxCCCount << ".";
	  w->addWarning(theCursor, theData->getNode()->getArg("name"), s.str());
	}

	
	// Remove this string from all the rest that it is a part of
	for(list<CallChainTokenList*>::iterator i=myList.begin(); i!=myList.end(); i++) {
	  if ((*i)->isWithin(t)) {
	    (*i)->myTokens=(*i)->myTokens.erase((*i)->myTokens.rfind(t->myTokens), t->myTokens.length());
	  }
	}
      }

      Debug::print(20, "----------");
      // Once I am done, the list ought to be clear
      myList.clear();
    }
  }


  // Coming back to the current statement
  // If it was a call statement, have a look at what is currently last in the list  
  if (theToken!=0) {
    if (!myList.empty()) {    
      CallChainTokenList *t=myList.back();

      if (!(t->myStartLine==theToken->myStartLine &&
	    t->myStartCol==theToken->myStartCol)) {
	// If it starts at the same place, this is a subset, and can be ignored.
	// Otherwise, add it to the list	
	Debug::print(20, (string) s.str() + " Adding CCT " + theToken->myTokens);
	myList.push_back(theToken);
      }
      else {
	Debug::print(20, (string) s.str() + " Ignoring CCT " + theToken->myTokens);
      }
    } else {     
      // Just add it
      Debug::print(20, (string) s.str() + " Adding (start) CCT " + theToken->myTokens);
      myList.push_back(theToken);
    }
  }


  return retVal;
}

int CallChainParser::countCalls(CallChainTokenList *theT)
{
  int count=0;
  string theS=theT->myTokens;
  size_t pos=0;
  
  while (pos!=string::npos) {
    pos=theS.find("->",pos+1);
    if (pos!=string::npos) {
      count++;
    }
  }  

  pos=0;
  while (pos!=string::npos) {
    pos=theS.find(".",pos+1);
    if (pos!=string::npos) {
      count++;
    }
  }  

  return count;
}
