#include <sstream>

#include "componentmaker.hh"
#include "debug.hh"
#include "model.hh"
#include "config.hh"

void ComponentMaker::start(Model &theModel, Config &theConfig)
{
  vector<string> theList;
  theConfig.getList(theList, "componentFilters");

  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    if((*i)=="gatherNodeChains") {
      Debug::print(2, " Gathering node chains");
      gatherNodeChains(theModel, theConfig);
    }

    if((*i)=="distanceMatrix") {
      Debug::print(2, " Generating distance matrix");
      distanceMatrix(theModel, theConfig);
    }
  }    
}

void ComponentMaker::distanceMatrix(Model &theModel, Config &theConfig)
{
  
}

void ComponentMaker::gatherNodeChains(Model &theModel, Config &theConfig)
{
  // This is probably the most complicated way possible of doing this. I am sorry for that.
  // TODO: simplify this!

  // Graph Nodes
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,"FileDeclaresFunction");
  set<ModelNode*>* moreNodes=theModel.getNodes();
  theModel.filterByType(*moreNodes,"ProjectHasComponent");
  theNodes->insert(moreNodes->begin(), moreNodes->end());
  delete(moreNodes);

  // Graph Arches
  set<ModelNode*>* theArches=theModel.getNodes();
  theModel.filterByType(*theArches,"FunctionCallsFunction");
  moreNodes=theModel.getNodes();
  theModel.filterByType(*moreNodes,"ComponentCall");
  theArches->insert(moreNodes->begin(), moreNodes->end());
  delete(moreNodes);

  set<ModelNode*> tmpIn;
  set<ModelNode*> tmpOut;
  set<ModelNode*>::iterator i=theNodes->begin();

  while (i!=theNodes->end()) {
    Debug::print(10, (string) "  Inspecting: " + (*i)->toString());
    int inDeg=getInDegrees(*theArches, (*i)->target);
    int outDeg=getOutDegrees(*theArches, (*i)->target);
        
    if (inDeg==1 || outDeg==1) {
      Debug::print(100, "   Indeg==1 or OutDeg==1, Looking Upwards to find the root of the chain");
      int parent=(*i)->target; // In case I stumbled upon the root node of the chain immediately
      int oldParent; // Semantically this is of course nonsense since the OldParent is the graph-child of parent...

      tmpIn.clear();
      tmpIn.insert(theArches->begin(), theArches->end());
      theModel.filterByTarget(tmpIn, parent);

      bool foundRoot=false;
      while (!foundRoot) {
	oldParent=parent;
	parent=(*(tmpIn.begin()))->source;

	tmpIn.clear();
	tmpIn.insert(theArches->begin(), theArches->end());
	theModel.filterByTarget(tmpIn, parent);

	inDeg=getInDegrees(*theArches, parent);
	outDeg=getOutDegrees(*theArches, parent);
	
	if (inDeg!=1) {
	  Debug::print(100, "   Root found. Indeg!=1");
	  foundRoot=true;
	}

	if (outDeg!=1) {
	  Debug::print(100, "   Root found, but Outdeg!=1. Backing up one step");
	  parent=oldParent;
	  foundRoot=true;
	}
      }

      // Let's check if this is a chain that I have already found
      set<ModelNode*>* tmpNodes=theModel.getNodes();
      theModel.filterByType(*tmpNodes, "ComponentContains");
      theModel.filterByTarget(*tmpNodes, parent);

      if (tmpNodes->size()==0) {
	// new chain. Find all the elements in it.
	Debug::print(100, "   New chain. Locating elements");
	tmpOut.clear();
	tmpOut.insert(theArches->begin(), theArches->end());
	theModel.filterBySource(tmpOut, parent);

	if (tmpOut.size()!=1) {
	  // This was a single occurence, and not a chain. Do nothing.
	  Debug::print(100, "   Skipping: Just the one node. No chain.");
	} else {
	  int child=(*(tmpOut.begin()))->target;
	  int oldChild=-1;
	  bool foundEnd=false;
	  set<int> theChain;
	  theChain.insert(parent);
	  while (!foundEnd) {
	    Debug::print(100, "   Looking downwards for end of chain");
	    oldChild=child;
	    child=(*(tmpOut.begin()))->target;
	    
	    tmpOut.clear();
	    tmpOut.insert(theArches->begin(), theArches->end());
	    theModel.filterBySource(tmpOut, child);
	    
	    inDeg=getInDegrees(*theArches, child);
	    outDeg=getOutDegrees(*theArches, child);
	    
	    
	    if (outDeg!=1) {
	      Debug::print(100, "   Root found. Outdeg!=1.");
	      foundEnd=true;
	    }
	    
	    if (inDeg!=1) {
	      Debug::print(100, "   Root found, but Indeg!=1.  Backing up one step");
	      child=oldChild;
	      foundEnd=true;
	    }

	    theChain.insert(child);
	  }

	  // Create Nodes and add to Model
	  // Find the ModelNode to get the name
	  set<ModelNode*>* tmpNodes=theModel.getNodes();
	  theModel.filterByType(*tmpNodes, "FileDeclaresFunction");
	  theModel.filterByTarget(*tmpNodes, parent);
	  ModelNode* theParent=*(tmpNodes->begin());
	  delete(tmpNodes);

	  stringstream ss;
	  ss << theChain.size();

	  ComponentNode *cn=new ComponentNode((string) "C" + theParent->getArg("name"), -1);
	  cn->setArg("CType", "nodeChain");
	  cn->setArg("size", ss.str());
	  theModel.add(cn);

	  for(set<int>::iterator q=theChain.begin(); q!=theChain.end(); q++) {
	    // Get the name...
	    set<ModelNode*>* tmpNodes=theModel.getNodes();
	    theModel.filterByType(*tmpNodes, "FileDeclaresFunction");
	    theModel.filterByTarget(*tmpNodes, *q);
	    ModelNode* theN=*(tmpNodes->begin());
	    delete(tmpNodes);

	    ComponentContainsNode *ccn=new ComponentContainsNode(theN->getArg("name"), cn->target, *q);
	    ccn->setArg("Component", cn->getArg("name"));
	    theModel.add(ccn);
	  }


	}
      } else {
	Debug::print(100, (string) "   Skipping: Already part of a component");
      }

      delete(tmpNodes);

    }

    i++;
  }

  delete(theNodes);
  delete(theArches);
}

int ComponentMaker::getInDegrees(const set<ModelNode*> &theArches, const int theNode) const
{
  int c=0;
  for(set<ModelNode*>::iterator i=theArches.begin(); i!=theArches.end(); i++){
    if ((*i)->target==theNode) {
      c++;
    }
  }  

  return c;
}

int ComponentMaker::getOutDegrees(const set<ModelNode*> &theArches, const int theNode) const
{
  int c=0;
  for(set<ModelNode*>::iterator i=theArches.begin(); i!=theArches.end(); i++){
    if ((*i)->source==theNode) {
      c++;
    }
  }    

  return c;
}

