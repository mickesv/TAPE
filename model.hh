#ifndef MODEL_HH
#define MODEL_HH

#include <string>
#include <set>
#include <map>
#include <vector>
#include "config.hh"

class ModelNode
{
public:
  string type;
  int source;
  int target;
  map<string,string> args;  

  ModelNode();
  ModelNode(const string &theType, const int &theSource, const int &theTarget, const string &theArgs="");

  bool operator<(const ModelNode &theNode) const;

  virtual void setId(const int &theId) { target=theId;};

  string getArg(const string &theArgName);
  void setArg(const string &theArgName, const string &theValue);

  string toString(const char sep=';', const char subsep=',');
protected:
  void parseArgs(map<string, string> &argList, const string &theArgs);
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
  set<ModelNode*> get(const int theSource);
  set<ModelNode*> get(const string &theType);
  set<ModelNode*> get(const string &theType, const string &theName);
  
  void debugPrint(int theLevel);
private:
  Config* myConfig;
  set<ModelNode*> myNodes;

  int myCounter;
};

#endif
