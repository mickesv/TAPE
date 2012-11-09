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

using namespace std;

void getParsers(set<Parser*> &theParsers, Model &theModel, Config &theConfig)
{
  vector<string> theList=theConfig.getList("parsers");

  // add an empty Parser to make sure that I do something (=nothing)
  Parser* p=new Parser();
  theParsers.insert(p);

  for(vector<string>::iterator i=theList.begin(); i!=theList.end(); i++) {
    
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

  }    
}


#endif
