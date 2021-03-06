#ifndef GVACCESS_HH
#define GVACCESS_HH

#include <map>
#include "parser.hh"

class VarDecl;

// This is where stuff is recorded in the model.
class GlobalVariableAccessNode : public ModelNode
{
public:
  GlobalVariableAccessNode(const CXCursor &theCursor, const string &theName, const int &theFunctionId, const int &theVariableId) {
    type=t();
    source=theFunctionId;
    target=theVariableId;
    setArg("name", theName);
    myCursor=theCursor;
  }

  virtual void setId(const int &theId) { }; // Do not need any id.  
  static string t();
private:
};




class GVAccessParser : public Parser
{
public:
  GVAccessParser() {};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  map<string, VarDecl*> myVariables;
  vector<GlobalVariableAccessNode*> myUsages;
};


#endif
