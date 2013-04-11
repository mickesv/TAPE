#ifndef ATTRIBUTEACCESS_HH
#define ATTRIBUTEACCESS_HH

#include <map>
#include "parser.hh"

// class VarDecl;

// This is where stuff is recorded in the model.
class AttributeAccessNode : public ModelNode
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




class AttributeAccessParser : public Parser
{
public:
  AttributeAccessParser() {};

  virtual void startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual void endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData);
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);

private:
  vector<AttributeAccessNode*> myUsages; 
};


#endif
