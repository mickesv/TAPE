#ifndef COMPONENTMAKER_HH
#define COMPONENTMAKER_HH

#include <string>

#include "model.hh"
#include "config.hh"


class ComponentMaker
{
public:
  ComponentMaker(Model &theModel, Config &theConfig) : myModelPtr(&theModel), myConfigPtr(&theConfig) {};

  void start(Model &theModel, Config &theConfig);

  void gatherNodeChains(Model &theModel, Config &theConfig);
  void distanceMatrix(Model &theModel, Config &theConfig);
private:
  int getInDegrees(const set<ModelNode*> &theArches, const int theNode) const;
  int getOutDegrees(const set<ModelNode*> &theArches, const int theNode) const;

  Model* myModelPtr;
  Config* myConfigPtr;
};

class ComponentNode : public ModelNode
{
public:
  ComponentNode(const string &theName, const int &theProjectId) {
    type=t();
    source=theProjectId;
    target=-1;
    setArg("name", theName);
  }

  static string t();
private:
};

class ComponentContainsNode : public ModelNode
{
public:
  ComponentContainsNode(const string &theName, const int &theComponentId, const int &theTargetId) {
    type=t();
    source=theComponentId;
    target=theTargetId;
    setArg("contained", theName);
  }

  virtual void setId(const int &theId) { }; // Do not need any id.    
  static string t();
private:
};


class ComponentCallNode : public ModelNode
{
public:
  ComponentCallNode(const string &theName, const int &theComponentId, const int &theTargetId) {
    type=t();
    source=theComponentId;
    target=theTargetId;
    setArg("target", theName);
  }

  virtual void setId(const int &theId) { }; // Do not need any id.  
  static string t();

private:
};


#endif
