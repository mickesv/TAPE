#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "model.hh"
#include "config.hh"
#include "csvfile.hh"
#include "debug.hh"
#include "stringstuff.hh"

using namespace std;

Model::Model()
  : myCounter(1)
{
}

Model::Model(Config &theConfig)
  : myConfig(&theConfig), myCounter(1)
{
}

Model::~Model()
{
  // Safety check to see that I don't accidentally create a whole bunch of objects
  Debug::print(100, "Cleaning up Model object");
}

void Model::populate(string theFileName)
{
  CSVFile myFile;
  myFile.setName(theFileName);
  vector<string> oneLine;

  bool eof=myFile.getLine(oneLine);

  while(!eof) {
    // Build the model based on the oneLine;
    if(oneLine.size()==0) {
      eof=myFile.getLine(oneLine);
      continue;
    }

    while (oneLine.size()<=3){
      Debug::print(10, " ->Missing argument. Adding...");
      oneLine.push_back("");
    }

    ModelNode* myNode=new ModelNode(oneLine[0], atoi(oneLine[1].c_str()), atoi(oneLine[2].c_str()), oneLine[3]);
    myNodes.insert(myNode);
    Debug::print(10, (string) " Added Node " + myNode->toString());
    myCounter++;
    eof=myFile.getLine(oneLine);
  }  
}

void Model::save(string theFileName)
{
  CSVFile myFile;
  myFile.setName(theFileName);
  

  for(set<ModelNode*>::iterator i=myNodes.begin(); i!=myNodes.end(); i++) {
    myFile.writeLine((*i)->toString(myFile.sep(), myFile.subsep()));
  }
}



void Model::add(ModelNode* theNode)
{
  theNode->setId(myCounter++);

  Debug::print(10, (string) (" Adding Node: "+ theNode->toString()));  

  myNodes.insert(theNode);
}


set<ModelNode*>* Model::getNodes() const
{
  set<ModelNode*>* myRetSet=new set<ModelNode*>();
  myRetSet->insert(myNodes.begin(), myNodes.end());
  return myRetSet;
}

void Model::filterBySource(set<ModelNode*> &theNodes, const int theSource) const
{
  set<ModelNode*> myRetSet;
  for(set<ModelNode*>::iterator i=theNodes.begin(); i!=theNodes.end(); i++){
    if ((*i)->source==theSource){
      myRetSet.insert(*i);
    }
  }

  theNodes.swap(myRetSet);
}

void Model::filterByTarget(set<ModelNode*> &theNodes, const int theTarget) const
{
  set<ModelNode*> myRetSet;
  for(set<ModelNode*>::iterator i=theNodes.begin(); i!=theNodes.end(); i++){
    if ((*i)->target==theTarget){
      myRetSet.insert(*i);
    }
  }
  
  theNodes.swap(myRetSet);
}

void Model::filterByType(set<ModelNode*> &theNodes, const string &theType) const
{
  set<ModelNode*> myRetSet;
  for(set<ModelNode*>::iterator i=theNodes.begin(); i!=theNodes.end(); i++){
    if ((*i)->type==theType){
      myRetSet.insert(*i);
    }
  }
  
  theNodes.swap(myRetSet);
}

void Model::filterByArg(set<ModelNode*> &theNodes, const string &theArgName, const string &theArg) const
{
  set<ModelNode*> myRetSet;
  for(set<ModelNode*>::iterator i=theNodes.begin(); i!=theNodes.end(); i++){
    if ((*i)->getArg(theArgName)==theArg){
      myRetSet.insert(*i);
    }
  }
  
  theNodes.swap(myRetSet);
}

void Model::debugPrint(int theLevel)
{
  if(theLevel>Debug::myLevel) { return; }

  for(set<ModelNode*>::iterator i=myNodes.begin(); i!=myNodes.end(); i++) {
    Debug::print(theLevel,(*i)->toString());
  }
}

// --------------------
// ModelNode

ModelNode::ModelNode()
{
  type="";
  source=target=0;
}

ModelNode::ModelNode(const string &theType, const int &theSource, const int &theTarget, const string &theArgs)
{
  type=theType;
  source=theSource;
  target=theTarget;
  parseArgs(args, theArgs);
}

ModelNode::~ModelNode()
{
  // Added this in a fit of weakness to see whether I accidentally deletes these objects somewhere
  Debug::print(100, (string) " Deleting ModelNode object: " + toString());
}

bool ModelNode::operator<(const ModelNode &theNode) const
{
  if (type==theNode.type &&
      source<theNode.source &&
      target<theNode.target) {
    return true;
  }

  return false;
}

string ModelNode::toString(const char sep, const char subsep)
{
  stringstream s;
  s << type << sep << source << sep << target << sep;

  for(map<string, string>::iterator i=args.begin(); i!=args.end();) {
    s << (*i).first << "=" << (*i).second;
    if (++i!=args.end()) {
      s << subsep;
    }
  }

  return s.str();
}

string ModelNode::getArg(const string &theArgName)
{
  if (args.find(theArgName)==args.end()) {
    return "";
  }

  string myS=args[theArgName];
  if ((myS[0]=='"') &&
      (myS[myS.length()-1]=='"')) {
    myS.erase(0,1);
    myS.erase(myS.length()-1,1);
  }

  return myS;
}

void ModelNode::setArg(const string &theArgName, const string &theValue)
{
  string* myS=new string(theValue);
  if (myS->find('=')!=string::npos) {
    myS->insert(0, 1, '"');
    myS->append(1, '"');
  }

  args[theArgName]=*myS;
}

void ModelNode::parseArgs(map<string, string> &argList, const string &theArgs)
{
  vector<string> splitArgs;
  StringStuff::strSplit(splitArgs, theArgs, ',');
  
  for(vector<string>::iterator i=splitArgs.begin(); i!=splitArgs.end(); i++) {
    vector<string> oneArg;
    StringStuff::strSplit(oneArg, (*i), '=');
    for(vector<string>::iterator j=oneArg.begin(); j!=oneArg.end(); ) {
      Debug::print(100, (string) "   Parsing Argument " + *i + "  key=" + *j);
      string key=*j;
      string val="";
      j++;
      if (j!=oneArg.end()) {
	val=(*j);
	argList[key]=val;
      }
    }
  }  
}
