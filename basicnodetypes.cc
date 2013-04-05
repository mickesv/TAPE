#include <clang-c/Index.h>

#include "basicnodetypes.hh"
#include "nodetypes.hh"

void InheritanceNode::findMissing(Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes, ClassNode::t());        

  if (source==-1) {
    theModel.filterByArg(*theNodes, "name", getArg("class"));
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }    
  }

  if (target==-1) {
    theModel.filterByArg(*theNodes, "name", getArg("name"));
    if (!theNodes->empty()) {
      target=(*(theNodes->begin()))->target;
    }
  }
}


void MethodNode::findMissing(Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes, ClassNode::t());        
  
  if (source==-1) {
    theModel.filterByArg(*theNodes, "name", getArg("class"));
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }    
  }
}

void AttributeNode::findMissing(Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes, ClassNode::t());        
  
  if (source==-1) {
    theModel.filterByArg(*theNodes, "name", getArg("class"));
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }    
  }    
}




