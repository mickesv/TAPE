#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "graphmaker.hh"
#include "debug.hh"
#include "model.hh"
#include "config.hh"

using namespace std;

void GraphMaker::makeGraph(Model &theModel, Config &theConfig)
{
  vector<string> theList;
  theConfig.getList(theList, "graphElements");

  *myFile << "digraph {" << endl;
  *myFile << " rankdir=LR" << endl;

  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    if((*i)=="components") { // This will probably not be sufficient for components, but let's wait and see...
      Debug::print(2, " Extracting Components");
      makeNodes(theModel, "Component", "ProjectHasComponent");
      makeArches(theModel, "Contains", "ComponentContains");
      makeArches(theModel, "ComponentCall", "ComponentCalls");
    }    

    if((*i)=="functions") {
      Debug::print(2, " Extracting Functions");
      makeNodes(theModel, "Function", "FileDeclaresFunction");
    }

    if((*i)=="functionCalls") {
      Debug::print(2, " Extracting FunctionCalls");
      makeArches(theModel, "Call", "FunctionCallsFunction");
    }

    if((*i)=="globalVariableAccess") {
      Debug::print(2, " Extracting GlobalVariableAccess");
      makeNodes(theModel, "Global Variable", "FileDeclaresVariable");
      makeArches(theModel, "GVAccess", "FunctionAccessVariable");
    }    
  }

  *myFile << "}" << endl;
}

void GraphMaker::makeNodes(Model &theModel, const string &theStereotype, const string &theFilter)
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

void GraphMaker::makeArches(Model &theModel, const string &theStereotype, const string &theFilter)
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

void GraphMaker::setOutput(iostream* theFile)
{
  myFile=theFile;
}
