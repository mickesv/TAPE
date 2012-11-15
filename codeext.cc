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

  if (theModel.get(pn->type).size() == 0) { // Not previously defined
    theModel.add(pn);

    // Then, we can assume that the files are not previously added either
    FileNode* fn;
    string bp=theConfig.get("basePath");
    vector<string> files=theConfig.getList("file");
    for (vector<string>::iterator i=files.begin(); i!=files.end(); i++) {
      fn=new FileNode(bp, *i, pn->target);
      theModel.add(fn);
    }
  }  
}

void addDeclarations(Model &theModel, Config &theConfig)
{
  set<ModelNode*> theFiles=theModel.get("ProjectHasFile");
  Extractor e(theModel, theConfig);

  for(set<ModelNode*>::iterator i=theFiles.begin(); i!=theFiles.end(); i++) {
    e.extractDeclarations((FileNode*) (*i));
  }
}

void startParsing(set<Parser*> &theParsers, Model &theModel, Config &theConfig)
{
  set<ModelNode*> theFunctions=theModel.get("FileDeclaresFunction");
  Extractor e(theModel, theConfig);

  for(set<ModelNode*>::iterator i=theFunctions.begin(); i!=theFunctions.end(); i++) {
    e.parseFunction(theParsers, (FunctionNode*) (*i));
  }  
}

/* TODO
 * - Add linkage to functioncalls
 * - GlobalVariableAccessParser
 * - Have another go at StringStuff
 * - Document how to create a new parser (=inherit from Parser, add to getParsers, add to config file, usage of ParserData*)
 */

int main(int argc, char* argv[])
{
  // Generic start for the entire family of tools
  Debug::print(1, "----------------------------------------");
  Debug::print(1, (string) "Starting " + argv[0]);
  Config myConfig(argc, argv);
  Model myModel(myConfig);
  Debug::setLevel(myConfig.getInt("debugLevel"));
  myConfig.debugPrint(100);

  // Start of tool-specific stuff
  // ------------------------------

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
  Debug::print(1, "Getting function and global variable declarations...");
  addDeclarations(myModel, myConfig);

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
