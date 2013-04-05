
#include <clang-c/Index.h>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>

#include "attributeaccessparser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"

/*
class VarDecl
{
public:
  string varName;
  int id;
  stack<int> depth;
private:
};
*/

void AttributeAccessParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  myFunction=theFunction;
  myUsages.clear();  
}

void AttributeAccessParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);

  // Merge all accesses of the same attribute and count them
  for(vector<AttributeAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {
    AttributeAccessNode* base=(*i);
    
    for(vector<AttributeAccessNode*>::iterator j=i; j!=myUsages.end(); ) {
      AttributeAccessNode* it=(*j);

      if ((base != it) &&
	  (base->source == it->source) &&
	  (base->target == it->target)) {

	// First, get the call counter right
	string cs=base->getArg("count");
	int counter=atoi(cs.c_str());
	stringstream ss;
	ss << ++counter;
	base->setArg("count", ss.str());

	/* Not used for Global Variable access
	// Then, get the maxLoopDepth (if available) right
	cs=base->getArg("maxLoopDepth");
	int bld=atoi(cs.c_str());
	cs=it->getArg("maxLoopDepth");
	int ild=atoi(cs.c_str());
	
	if(bld<ild) {
	  base->setArg("maxLoopDepth", cs);
	}
	*/
	
	myUsages.erase(j);
      } else {
	j++;
      }
    }
  }
  
  // Add nodes to model
  for(vector<AttributeAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {    
    myModelPtr->add(*i);
  }


}

CXChildVisitResult AttributeAccessParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  CXChildVisitResult retval=CXChildVisit_Recurse;
  int theAttributeId=-1;


  if (theCursor.kind==CXCursor_DeclRefExpr ||
      theCursor.kind==CXCursor_MemberRefExpr) {
    string name=clang_getCString(clang_getCursorDisplayName(theCursor));
    string calledParent=clang_getCString(clang_getCursorDisplayName(clang_getCursorSemanticParent(clang_getCursorReferenced(theCursor))));

    Debug::print(10, (string) "Looking for AttributeAccess " +name + ":" + calledParent);

    // Match CalledParent with my class declarations to find the right one
    set<ModelNode*>* nodes=myModelPtr->getNodes();
    myModelPtr->filterByType(*nodes,AttributeNode::t());
    myModelPtr->filterByArg(*nodes, "class", calledParent);
    myModelPtr->filterByArg(*nodes, "name", name);
        
    if (nodes->size()!=0) {
      // Found at least one node that matches (ought to be one-and-only-one)      
      theAttributeId=(*(nodes->begin()))->target;


      // Find out if it is in my class hierarchy
      string theName=myFunction->getArg("class");
      set<ModelNode*>* nodes2=myModelPtr->getNodes();
      myModelPtr->filterByType(*nodes2, ClassNode::t());
      myModelPtr->filterByTarget(*nodes2, myFunction->source);
      int theId=myFunction->target; // Might as well give it a default value
      if(nodes2->size()!=0) {
	theId=(*(nodes2->begin()))->target;
      }
      
      bool done=false;
      bool found=false;
      while(!done) {
	Debug::print(11, (string) " Matching " + theName + ":" + calledParent);
	if (theName==calledParent) {
	  found=true;
	  done=true;
	} else {
	  Debug::print(11, " Not in my class; recursing");
	  set<ModelNode*>* nodes2=myModelPtr->getNodes();
	  myModelPtr->filterByType(*nodes2, InheritanceNode::t());
	  myModelPtr->filterByTarget(*nodes2, theId);

	  if(nodes2->size()!=0) {
	    theId=(*(nodes2->begin()))->target;
	    theName=(*(nodes2->begin()))->getArg("class");
	    Debug::print(11, (string) " Recursing into " + theName);
	  } else {
	    Debug::print(11, " Not found");
	    done=true;
	  }
	}       	

	
      }

      if (found==true) {
	// Gotcha. Let's create a node and store it.


	AttributeAccessNode* n=new AttributeAccessNode(theCursor, name, myFunction->target, theAttributeId);
	stringstream s;
	s << 1;
	n->setArg("count", s.str());
	n->setArg("class", calledParent);
	if (calledParent!=theName) {
	  n->setArg("attributeClass", theName);
	}

	myUsages.push_back(n);	   	
      }
      
    }    
  }

  return retval;
}

