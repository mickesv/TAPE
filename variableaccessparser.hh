#ifndef VARIABLEACCESS_HH
#define VARIABLEACCESS_HH

#include <map>
#include "parser.hh"
#include "config.hh"

// This is where stuff is recorded in the model.

class VariableAccessNode : public ModelNode
{ // Add this just to have a common base class for this part of the system...
public:
protected:
  VariableAccessNode() {};
};

class AttributeAccessNode : public VariableAccessNode
{
public:
  AttributeAccessNode(const CXCursor &theCursor, const string &theName, const int &theFunctionId, const int &theVariableId) {
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


class GlobalVariableAccessNode : public VariableAccessNode
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



class VariableAccessParser : public Parser
{
public:
  VariableAccessParser(Config& theConfig) {myConfig=&theConfig;};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  vector<VariableAccessNode*> myUsages; 

  Config* myConfig;
};


#endif
