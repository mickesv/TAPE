#include <clang-c/Index.h>
#include <fstream>
#include <string>
#include <iostream>
#include <set>

#include "config.hh"
#include "model.hh"
#include "basicnodetypes.hh"
#include "parser.hh"
#include "extractor.hh"
#include "debug.hh"
#include "getParsers.hh"

using namespace std;

void addBasics(Model &theModel, Config &theConfig)
{  
  ProjectNode* pn=new ProjectNode(theConfig.get("projectName"));

  set<ModelNode*>* theNodes=theModel.getNodes();
  theModel.filterByType(*theNodes, pn->type);  

  if (theNodes->size() == 0) { // Not previously defined
    theModel.add(pn);

    // Then, we can assume that the files are not previously added either
    FileNode* fn;
    string bp=theConfig.get("basePath");
    vector<string> files;
    theConfig.getList(files, "file");
    for (vector<string>::iterator i=files.begin(); i!=files.end(); i++) {
      fn=new FileNode(bp, *i, pn->target);
      theModel.add(fn);
    }
  }  
}

void addDeclarations(Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theFiles=theModel.getNodes();
  theModel.filterByType(*theFiles, FileNode::t());
  Extractor e(theModel, theConfig);

  for(set<ModelNode*>::iterator i=theFiles->begin(); i!=theFiles->end(); i++) {
    e.extractDeclarations((FileNode*) (*i));
  }
}

void secondPass(Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theNodes=theModel.getNodes();

  for(set<ModelNode*>::iterator i=theNodes->begin(); i!=theNodes->end(); i++) {
    if ((*i)->source==-1 || (*i)->target==-1) {
	Debug::print(10, (string) " Investigating " +(*i)->toString());
	(*i)->findMissing(theModel, theConfig);
	if ((*i)->source!=-1 && (*i)->target!=-1) {
	  Debug::print(10, (string) " Fixed " +(*i)->toString());
	}	      
      }
  }  
}

void startParsing(set<Parser*> &theParsers, Model &theModel, Config &theConfig)
{
  set<ModelNode*>* theFunctions=theModel.getNodes();
  theModel.filterByType(*theFunctions, FunctionNode::t());

  set<ModelNode*>* theMethods=theModel.getNodes();
  theModel.filterByType(*theMethods, MethodNode::t());

  Extractor e(theModel, theConfig);
  Debug::print(3, " Parsing C Functions");
  for(set<ModelNode*>::iterator i=theFunctions->begin(); i!=theFunctions->end(); i++) {
    e.parseFunction(theParsers, (*i));
  }
  Debug::print(3, " Parsing C++ Methods");
  for(set<ModelNode*>::iterator i=theMethods->begin(); i!=theMethods->end(); i++) {
    e.parseFunction(theParsers, (*i));
  }  
}


int main(int argc, char* argv[])
{
  // Generic start for the entire family of tools
  Config myConfig(argc, argv);
  Model myModel(myConfig);
  Debug::setLevel(myConfig.getInt("debugLevel"));
  Debug::print(1, "----------------------------------------");
  Debug::print(1, (string) "Starting " + argv[0]);
  myConfig.debugPrint(100);

  // Start of tool-specific stuff
  // ------------------------------

  // clang version

  string clangVersion=clang_getCString(clang_getClangVersion());
  Debug::print(3, (string) "Using " + clangVersion);


  // Decide which parsers to use
  Debug::print(1, "Loading parsers...");
  set<Parser*> myParsers;
  getParsers(myParsers, myModel, myConfig);

  // Fill it up with what is previously already done
  if(myConfig.get("read")=="true") {
    Debug::print(1, "Reading model...");
    Debug::print(1, " WARNING: This may create redundant nodes!");
    myModel.populate(myConfig.get("modelFile"));
  }

  // Then add basic stuff
  Debug::print(1, "Adding project initialiser and files...");
  addBasics(myModel, myConfig);

  // Go get the functions and global variables
  Debug::print(1, "Extracting declarations...");
  addDeclarations(myModel, myConfig);

  // Run a second pass to take care of as many misses as possible
  Debug::print(1, "Second pass of extracting declarations...");
  secondPass(myModel, myConfig);

  // ... And get Parsing
  Debug::print(1, "Parsing...");
  startParsing(myParsers, myModel, myConfig);

  // ------------------------------
  // End of tool-specific stuff
  // Generic end for the entire family of tools

  // Save
  if(myConfig.get("save")!="false") {
    Debug::print(1, "Saving model...");
    myModel.save(myConfig.get("modelFile"));
  }

  Debug::print(1, (string) "End of " + argv[0]);
  Debug::print(1, "----------------------------------------");
}
