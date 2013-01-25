#include <clang-c/Index.h>
#include <set>
#include <string>
#include <sstream>

#include "functioncallparser.hh"
#include "model.hh"

using namespace std;

void FunctionCallParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  myCalls.clear();
}

void FunctionCallParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);

  // Merge all calls to the same function and count
  for(vector<CallNode*>::iterator i=myCalls.begin(); i!=myCalls.end(); i++) {
    CallNode* base=(*i);
    
    // Name lookup screwup
    if (base->target==-1) { continue; }
    
    for(vector<CallNode*>::iterator j=i; j!=myCalls.end(); ) {
      CallNode* it=(*j);

      if ((base != it) &&
	  (base->source == it->source) &&
	  (base->target == it->target)) {

	// First, get the call counter right
	string cs=base->getArg("count");
	int counter=atoi(cs.c_str());
	stringstream ss;
	ss << ++counter;
	base->setArg("count", ss.str());

	// Then, get the maxLoopDepth (if available) right
	cs=base->getArg("maxLoopDepth");
	int bld=atoi(cs.c_str());
	cs=it->getArg("maxLoopDepth");
	int ild=atoi(cs.c_str());
	
	if(bld<ild) {
	  base->setArg("maxLoopDepth", cs);
	}

	myCalls.erase(j);
      } else {
	j++;
      }
    }
  }
  
  // Add nodes to model
  for(vector<CallNode*>::iterator i=myCalls.begin(); i!=myCalls.end(); i++) {
    myModelPtr->add(*i);
  }
}


CXChildVisitResult FunctionCallParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  if(theCursor.kind==CXCursor_CallExpr) {
    string name=clang_getCString(clang_getCursorDisplayName(theCursor));
    string calledParent=clang_getCString(clang_getCursorDisplayName(clang_getCursorSemanticParent(clang_getCursorReferenced(theCursor))));

    // It ought to be possible to use the cursors and clang_equalCursors to find the right one, but I did not get this to work, so I resort to names matching instead...

    int called=-1;    
    set<ModelNode*>* nodes=myModelPtr->getNodes();

    Debug::print(10, (string) "  Call to " + name + "::" + calledParent);

    if (name=="") {
      Debug::print(10, "  Could not identify name.");
      return  CXChildVisit_Continue;
    }
    
    myModelPtr->filterByType(*nodes,MethodNode::t());
    myModelPtr->filterByArg(*nodes, "name", name);
    myModelPtr->filterByArg(*nodes, "class", calledParent);
    
    if (nodes->size()>=1) {
	called=(*(nodes->begin()))->target;
    }

    if (called==-1) {
      Debug::print(100, "  Not a C++ method. Looking for C functions");
      nodes=myModelPtr->getNodes();
      myModelPtr->filterByType(*nodes,FunctionNode::t());
      myModelPtr->filterByArg(*nodes, "name", name);
      if(nodes->size()>=1) {
	called=(*(nodes->begin()))->target;
      }      
    }
					 
    if (called==-1) {
      Debug::print(100, "  Could not find called function, defaulting to -1.");
    }

    /*
    if(nodes->size()>=1) {
      called=(*(nodes->begin()))->target;
    }

    if (called==-1) {
      // Then, look for C functions
      nodes=myModelPtr->getNodes();
      myModelPtr->filterByType(*nodes,FunctionNode::t());
      myModelPtr->filterByArg(*nodes, "name", name);
      if(nodes->size()>=1) {
	called=(*(nodes->begin()))->target;
      }
    }
    */

    CallNode* cn=new CallNode(theCursor, name, theData->getNode()->target, called);

    // TODO: check linkage of function call...

    // Initialise the call counter
    stringstream s;
    s << 1;
    cn->setArg("count", s.str());

    // Set the maxLoopDepth (if available)
    int mld=theData->get("maxLoopDepth");
    if (mld != -1) {
      s.str("");
      s << mld;
      cn->setArg("maxLoopDepth", s.str());
    }

    Debug::print(100, "  Function call: " + cn->toString());
    myCalls.push_back(cn);
  }

  return CXChildVisit_Continue;
}

