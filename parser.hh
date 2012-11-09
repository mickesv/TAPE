#ifndef PARSER_HH
#define PARSER_HH

#include <clang-c/Index.h>
#include <set>

#include "model.hh"
#include "config.hh"
#include "basicnodetypes.hh"

class Extractor;

class ParserData
{
  friend class Extractor;
public:
  ParserData(ModelNode* theNode);

  void add(const string &theKey, int theValue);
  void set(const string &theKey, int theValue);
  int get(const string &theKey);
  
  string getFilename();
  ModelNode* getNode() { return myNode; };

private:
  ModelNode* myNode;
  string myFilename;
  map<string, int> data;
};

class Parser
{
public:
  Parser() {};
  

  virtual void startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData) { myModelPtr=&theModel; };
  virtual void endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData) { myModelPtr=&theModel; };
  virtual CXChildVisitResult parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData);
 
protected:
  Model* myModelPtr;

private:
};



#endif
