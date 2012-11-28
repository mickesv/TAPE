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

  // Collect all leaf nodes that are only called by one other node
  void gatherNodeChains(Model &theModel, Config &theConfig);
private:
  Model* myModelPtr;
  Config* myConfigPtr;
};

class ComponentNode : public ModelNode
{
public:
  ComponentNode(const string &theName, const int &theProjectId) {
    type="ProjectHasComponent";
    source=theProjectId;
    target=-1;
    setArg("name", theName);
  }

private:
};

class ComponentContainsNode : public ModelNode
{
public:
  ComponentContainsNode(const string &theName, const int &theComponentId, const int &theTargetId) {
    type="ComponentContains";
    source=theComponentId;
    target=theTargetId;
    setArg("contained", theName);
  }

  virtual void setId(const int &theId) { }; // Do not need any id.    

private:
};


class ComponentCallNode : public ModelNode
{
public:
  ComponentCallNode(const string &theName, const int &theComponentId, const int &theTargetId) {
    type="ComponentCalls";
    source=theComponentId;
    target=theTargetId;
    setArg("target", theName);
  }

  virtual void setId(const int &theId) { }; // Do not need any id.  
  

private:
};


#endif
