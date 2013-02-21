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

    if ((*i)=="classes") {
      Debug::print(2, " Extracting Classes");
      makeClassNodes(theModel, theOptions);
      makeClassInheritanceArches(theModel, theOptions);
    }


    if((*i)=="functions") {
      Debug::print(2, " Extracting Functions");
      makeNodes(theModel, theOptions, "Function", FunctionNode::t());
      if (theOptions.find("classes")==theOptions.end()) {
	makeNodes(theModel, theOptions, "Method", MethodNode::t());
      }
    }

    if((*i)=="functionCalls") {
      if(theOptions.find("classes")!=theOptions.end()) {
	Debug::print(2, " Extracting Method Calls");
	makeClassArches(theModel, theOptions);
      } else {
	Debug::print(2, " Extracting FunctionCalls");
	makeArches(theModel, theOptions, "Call", CallNode::t());
      }
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

void GraphMaker::makeClassNodes(Model &theModel, map<string,bool> &theOptions)
{
  // Treating classes as a special case to get them as annotated and UML-like as possible
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,ClassNode::t());
  
  *myFile << "// Class Nodes" << endl;
  *myFile << "rankdir=TD;" << endl;
  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    if ((*i)->target==-1) {
      continue;
    }
    
    *myFile << (*i)->target
	    << " [shape=\"record\""
	    << "\n   label=\"{class " << safeForPrint((*i)->getArg("name")) << "\\n|" << flush;
      
    // add attributes
    set<ModelNode*>* n=theModel.getNodes();
    theModel.filterBySource(*n, (*i)->target);
    theModel.filterByType(*n,AttributeNode::t());
    for(set<ModelNode*>::iterator j=n->begin(); j!=n->end(); j++) {
      if ((*j)->target==-1) {
	continue;
      }
      *myFile << safeForPrint((*j)->getArg("name")) << "\\l" << flush;
    }
    *myFile << "|" << flush;

    // add methods
    n=theModel.getNodes();
    theModel.filterBySource(*n, (*i)->target);
    theModel.filterByType(*n,MethodNode::t());
    for(set<ModelNode*>::iterator j=n->begin(); j!=n->end(); j++) {
      if ((*j)->target==-1) {
	continue;
      }
      *myFile << safeForPrint((*j)->getArg("name")) << "\\l" << flush;
    }

    *myFile << "}\"];" << endl;
  }
  *myFile << endl;
  
}

void GraphMaker::makeClassInheritanceArches(Model &theModel, map<string,bool> &theOptions)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,InheritanceNode::t());
  int callerClass=-1;
  int calledClass=-1;

  *myFile << "// Inheritance Arches" << endl;
  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    Debug::print(100, (string) "  GeneratingInheritance " + (*i)->toString());
    *myFile << (*i)->source << "->"
	    << (*i)->target
	    << " [label=\"<<Inherits>>\", arrowhead=\"empty\"];"<< endl;
  }

  *myFile << endl;

}


void GraphMaker::makeClassArches(Model &theModel, map<string,bool> &theOptions)
{
  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes,ClassNode::t());
  int callerClass=-1;
  int calledClass=-1;

  *myFile << "// Method Call Arches" << endl;

  // Iterate over all classes
  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    Debug::print(100, (string) "  Inspecting class " + (*i)->toString());
    callerClass=(*i)->target;
    set<ModelNode*>* n=theModel.getNodes();
    theModel.filterBySource(*n,callerClass);
    theModel.filterByType(*n,MethodNode::t());

    set<int> myCalled;

    // Iterate over all methods in class *i
    for(set<ModelNode*>::iterator j=n->begin(); j!=n->end(); j++) {
      Debug::print(100, (string) "  Inspecting method " + (*j)->toString());
      set<ModelNode*>* n2=theModel.getNodes();
      theModel.filterByType(*n2,CallNode::t());
      theModel.filterBySource(*n2,(*j)->target);

      // Iterate over all method calls from method *j
      for(set<ModelNode*>::iterator k=n2->begin(); k!=n2->end(); k++) {
	Debug::print(100, (string) "  Inspecting method call " + (*k)->toString());
	set<ModelNode*>* n3=theModel.getNodes();
	theModel.filterByType(*n3,MethodNode::t());
	theModel.filterByTarget(*n3,(*k)->target);

	if(n3->size()>=1) {
	  calledClass=(*(n3->begin()))->source;
	}

	if ((callerClass==-1 ||
	    calledClass==-1) ||
	    (callerClass==calledClass) ||
	    (myCalled.find(calledClass)!=myCalled.end())
	    ) {
	  continue;	  
	}
    
	
	myCalled.insert(calledClass);

	*myFile << callerClass << "->"
		<< calledClass
		<< " [label=\"<<M_Call>>\"];"<< endl;
      }
    }
  }

  *myFile << endl;

  *myFile << "// Regular function calls" << endl;
  theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes, CallNode::t());
  set<ModelNode*>* n2=theModel.getNodes();
  theModel.filterByType(*n2,FunctionNode::t());
  set<ModelNode*>* n3=theModel.getNodes();
  theModel.filterByType(*n3,MethodNode::t());

  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {

    if ((*i)->source==-1 ||
	(*i)->target==-1) {
      continue;
    }
    
    int mSource=-1;
    int mTarget=-1;

    for(set<ModelNode*>::iterator k=n2->begin(); k!=n2->end(); k++) {
      if((*k)->target==(*i)->source) {
	mSource=(*i)->source;
      }

      if((*k)->target==(*i)->target) {
	mTarget=(*i)->target;
      }

    } 

    if (mTarget==-1) { // Not a call to a function. Find the class instead
      for(set<ModelNode*>::iterator k=n3->begin(); k!=n3->end(); k++) {
	if((*k)->target==(*i)->target) {
	  mTarget=(*k)->source;
	}
      }
    }


    if (mSource==-1) {
      continue;
    }

    *myFile << mSource << "->"	  
	    << mTarget
	    << " [label=\"<<Call>>\"];"<< endl;
      
  }
  *myFile << endl;
  

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
	    << " [label=\"<<" << theStereotype << ">> " << safeForPrint((*i)->getArg("name")) << flush;

    if (theOptions.find("classes")==theOptions.end()) {
      *myFile << "::" << safeForPrint((*i)->getArg("class")) << flush;
    }
    *myFile << "\"];" << endl;
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
    *myFile << " label=\"<<" << theStereotype << ">> " << safeForPrint((*i)->getArg("name")) << "\\n"
	    << " CType=" << (*i)->getArg("CType") << ", size=" << safeForPrint((*i)->getArg("size")) << "\";" << endl;

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

string GraphMaker::safeForPrint(const string &theString) const
{
  string s=theString;
  string badchars="<>";
  string::size_type pos=s.find_first_of(badchars);;

  while(pos!=string::npos) {
    s.insert(pos,"\\");
    pos=s.find_first_of(badchars,pos+2);
  }


  return s;
}
