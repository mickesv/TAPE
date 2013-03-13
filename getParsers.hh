#ifndef GETPARSERS_HH
#define GETPARSERS_HH

// Keep this separate just so that I do not need to modify anywhere else just to add a new type of parser.

#include <set>
#include "config.hh"
#include "debug.hh"
#include "parser.hh"

#include "functioncallparser.hh"
#include "loopdepthparser.hh"
#include "functionsizeparser.hh"
#include "gvaccessparser.hh"
#include "verboseparser.hh"
#include "parametercountparser.hh"

using namespace std;

void getParsers(set<Parser*> &theParsers, Model &theModel, Config &theConfig)
{
  vector<string> theList;
  theConfig.getList(theList,"parsers");

  // add an empty Parser to make sure that I do something (=nothing)
  Parser* p=new Parser();
  theParsers.insert(p);

  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    
    // I know I am inconsistent here; some parsers would like the config-objects, and others do not.
    // I am pragmatic and let them have their way.
    if((*i)=="functionCalls") {
      Debug::print(2, " Loading FunctionCallParser");
      theParsers.insert(new FunctionCallParser());      
    }

    if((*i)=="loopDepth") {
      Debug::print(2, " Loading LoopdepthParser");
      theParsers.insert(new LoopdepthParser());
    }

    if((*i)=="functionSize") {
      Debug::print(2, " Loading FunctionSizeParser");
      theParsers.insert(new FunctionSizeParser());
    }

    if((*i)=="globalVariableAccess") {
      Debug::print(2, " Loading GlobalVariableAccessParser");
      theParsers.insert(new GVAccessParser());
    }

    if((*i)=="verbose") {
      Debug::print(2, " Loading VerboseParser");
      theParsers.insert(new VerboseParser(theConfig));
    }

    if((*i)=="parameterCount") {
      Debug::print(2, " Loading ParameterCountParser");
      theParsers.insert(new ParameterCountParser(theConfig));
    }

  }    
}


#endif
