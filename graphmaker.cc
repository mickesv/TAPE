#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "graphmaker.hh"
#include "debug.hh"
#include "model.hh"
#include "config.hh"

#include "basicnodetypes.hh"
#include "componentmaker.hh"
#include "functioncallparser.hh"
#include "gvaccessparser.hh"

using namespace std;

void GraphMaker::makeGraph(Model &theModel, Config &theConfig)
{
  vector<string> theList;
  map<string, bool> theOptions;
  theConfig.getList(theList, "graphElements");

  // Start the output
  *myFile << "digraph {" << endl;
  *myFile << " rankdir=LR;" << endl;
  *myFile << " compound=true;" << endl;
  *myFile << endl;

  // Find out the options
  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    theOptions[*i]=true;
  }

  // Invoke the right method. I am not using the options map since I want to keep the order as in the config file for now...
  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    if((*i)=="components") {
      Debug::print(2, " Extracting Components");
      if(theOptions.find("functions") != theOptions.end()) {
	makeSubGraphs(theModel, theOptions, "Component", ComponentNode::t());
	makeArches(theModel, theOptions, "ComponentCall", ComponentCallNode::t());
      } else {
	makeNodes(theModel, theOptions, "Component", ComponentNode::t());
	makeArches(theModel, theOptions, "ComponentCall", ComponentCallNode::t());
      }
    }    

    if((*i)=="functions") {
      Debug::print(2, " Extracting Functions");
      makeNodes(theModel, theOptions, "Function", FunctionNode::t());
    }

    if((*i)=="functionCalls") {
      Debug::print(2, " Extracting FunctionCalls");
      makeArches(theModel, theOptions, "Call", CallNode::t());
    }

    if((*i)=="globalVariableAccess") {
      Debug::print(2, " Extracting GlobalVariableAccess");
      makeNodes(theModel, theOptions, "Global Variable", VariableNode::t());
      makeArches(theModel, theOptions, "GVAccess", GlobalVariableAccessNode::t());
    }    
  }

  // end the graph
  *myFile << "}" << endl;
}

void GraphMaker::makeNodes(Model &theModel, map<string,bool> &theOptions, const string &theStereotype, const string &theFilter)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,theFilter);
  
  *myFile << "// " << theStereotype << " Nodes" << endl;
  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    if ((*i)->target==-1) {
      continue;
    }
    
    *myFile << (*i)->target
	    << " [label=\"<<" << theStereotype << ">> " << (*i)->getArg("name") << "\"];" << endl;
  }
  *myFile << endl;
}

void GraphMaker::makeArches(Model &theModel, map<string,bool> &theOptions, const string &theStereotype, const string &theFilter)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,theFilter);
  
  *myFile << "// " << theStereotype << " Arches" << endl;
  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    if ((*i)->source==-1 ||
	(*i)->target==-1) {
      continue;
    }
    

    *myFile << (*i)->source << "->"
	    << (*i)->target
	    << " [label=\"<<" << theStereotype << ">>\"];"<< endl;
  }
  *myFile << endl;

}

void GraphMaker::makeSubGraphs(Model &theModel, map<string,bool> &theOptions, const string &theStereotype, const string &theFilter)
{
  set<ModelNode*>* theComponents=theModel.getNodes();
  theModel.filterByType(*theComponents,theFilter);
  
  *myFile << "// " << theStereotype << " Subgraphs" << endl;
  
  for(set<ModelNode*>::iterator i=theComponents->begin(); i!=theComponents->end(); i++) {
    if ((*i)->target==-1) {
      continue;
    }
  
    *myFile << "subgraph cluster_" << (*i)->target << " {" << endl;
    *myFile << " label=\"<<" << theStereotype << ">> " << (*i)->getArg("name") << "\\n"
	    << " CType=" << (*i)->getArg("CType") << ", size=" << (*i)->getArg("size") << "\";" << endl;

    if (theOptions.find("functions")!=theOptions.end()) {
      set<ModelNode*>* theFunctions=theModel.getNodes();
      theModel.filterByType(*theFunctions, ComponentContainsNode::t());
      theModel.filterBySource(*theFunctions, (*i)->target);

      for(set<ModelNode*>::iterator j=theFunctions->begin(); j!=theFunctions->end(); j++) {
	*myFile << (*j)->target << ";" << "// " << (*j)->getArg("contained") << endl;
      }

    }

    *myFile << "}" << endl;
  }

  
}

void GraphMaker::setOutput(iostream* theFile)
{
  myFile=theFile;
}

