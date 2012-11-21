#ifndef FUNCTIONCALLPARSER_HH
#define FUNCTIONCALLPARSER_HH

#include <clang-c/Index.h>
#include <vector>
#include "model.hh"
#include "parser.hh"

// This is where stuff is recorded in the model.
class CallNode : public ModelNode
{
public:
  CallNode(const CXCursor &theCursor, const string &theName, const int& theCaller, const int &theCalled) {
    type="FunctionCallsFunction";
    source=theCaller;
    target=theCalled;
    setArg("target", theName);
    myCursor=theCursor;    
  }

  virtual void setId(const int &theId) { }; // Do not need any id.  
  CXCursor* getCursor() { return &myCursor; };

private:
  CXCursor myCursor;
};


class FunctionCallParser : public Parser
{
public:
  FunctionCallParser() {};
  virtual void startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData);

  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData *theData);  

private:
  vector<CallNode*> myCalls;
};




#endif
