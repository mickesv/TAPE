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
  Debug::print(10, "Cleaning up Model object");
}

void Model::populate(string theFileName)
{
  CSVFile myFile;
  myFile.setName(theFileName);
  vector<string> oneLine;

  bool eof=myFile.getLine(oneLine);

  while(!eof) {
    // Build the model based on the oneLine;
    if (oneLine.size()<=3){
      oneLine.push_back("");
    }

    ModelNode* myNode=new ModelNode(oneLine[0], atoi(oneLine[1].c_str()), atoi(oneLine[2].c_str()), oneLine[3]);
    myNodes.insert(myNode);
    myCounter++;
    eof=myFile.getLine(oneLine);
  }  
}


void Model::add(ModelNode* theNode)
{
  theNode->setId(myCounter++);

  Debug::print(10, (string) (" Adding Node: "+ theNode->toString()));  

  myNodes.insert(theNode);
}

set<ModelNode*> Model::get(const int theSource)
{
  set<ModelNode*> myRetSet;

  for(set<ModelNode*>::iterator i=myNodes.begin(); i!=myNodes.end(); i++)
    {
      if ((*i)->source==theSource){
	myRetSet.insert(*i);
      }
    }

  return myRetSet;
}

set<ModelNode*> Model::get(const string &theType)
{
  set<ModelNode*> myRetSet;

  for(set<ModelNode*>::iterator i=myNodes.begin(); i!=myNodes.end(); i++)
    {
      if ((*i)->type==theType){
	myRetSet.insert(*i);
      }
    }

  return myRetSet;
}

set<ModelNode*> Model::get(const string &theType, const string &theName)
{
  set<ModelNode*> myRetSet;

  for(set<ModelNode*>::iterator i=myNodes.begin(); i!=myNodes.end(); i++)
    {
      if ((*i)->type==theType && (*i)->getArg("name")==theName){
	myRetSet.insert(*i);
      }
    }

  return myRetSet;
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


bool ModelNode::operator<(const ModelNode &theNode) const
{
  if (type==theNode.type &&
      source<theNode.source &&
      target<theNode.target) {
    return true;
  }

  return false;
}

string ModelNode::toString()
{
  char sep=';';
  stringstream s;
  s << type << sep << source << sep << target << sep;
  bool first=true;

  for(map<string, string>::iterator i=args.begin(); i!=args.end(); i++) {
    if (first!=true) {
      s << ",";
    }
    first=false;
    s << (*i).first << "=" << (*i).second;
  }

  return s.str();
}

string ModelNode::getArg(const string &theArgName)
{
  if (args.find(theArgName)==args.end()) {
    return "";
  }
  return args[theArgName];
}

void ModelNode::setArg(const string &theArgName, const string &theValue)
{
  args[theArgName]=theValue;
}

void ModelNode::parseArgs(map<string, string> &argList, const string &theArgs)
{
  vector<string> splitArgs=StringStuff::strSplit(theArgs, ',');
  
  for(vector<string>::iterator i=splitArgs.begin(); i!=splitArgs.end(); i++) {
    vector<string> oneArg=StringStuff::strSplit((*i), '=');
    for(vector<string>::iterator j=oneArg.begin(); j!=oneArg.end(); j++) {
      Debug::print(100, (string) "###" + *i + " # " + *j);
      argList[*j]=*(++j);
    }
  }  
}
