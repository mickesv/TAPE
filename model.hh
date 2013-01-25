#ifndef MODEL_HH
#define MODEL_HH

#include <clang-c/Index.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "config.hh"

class ModelNode
{
public:
  static string t();
  string type;
  int source;
  int target;
  map<string,string> args;  

  ModelNode();
  ModelNode(const string &theType, const int &theSource, const int &theTarget, const string &theArgs="");
  ~ModelNode();
  bool operator<(const ModelNode &theNode) const;

  virtual void setId(const int &theId) { target=theId;};

  string getArg(const string &theArgName);
  void setArg(const string &theArgName, const string &theValue);

  string toString(const char sep=';', const char subsep=',');

  CXCursor* getCursor() { return &myCursor; };
protected:
  void parseArgs(map<string, string> &argList, const string &theArgs);
  CXCursor myCursor;
private:
};


class Model
{
public:
  Model();
  Model(Config &theConfig);
  ~Model();
  
  void populate(string theFileName);
  void save(string theFileName);

  void add(ModelNode* theNode);

  set<ModelNode*>* getNodes() const;
  void filterBySource(set<ModelNode*> &theNodes, const int theSource) const;
  void filterByTarget(set<ModelNode*> &theNodes, const int theTarget) const;
  void filterByType(set<ModelNode*> &theNodes, const string &theType) const;
  void filterByArg(set<ModelNode*> &theNodes, const string &theArgName, const string &theArg) const;
  
  void debugPrint(int theLevel);
private:

  Config* myConfig;
  set<ModelNode*> myNodes;

  int myCounter;
};

#endif
