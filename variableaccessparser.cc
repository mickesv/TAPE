
#include <clang-c/Index.h>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>

#include "variableaccessparser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"
#include "warnings.hh"

void VariableAccessParser::startFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  myUsages.clear();  
}

void VariableAccessParser::endFunction(Model &theModel, ModelNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);

  // Merge all accesses of the same attribute and count them
  for(vector<VariableAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {
    VariableAccessNode* base=(*i);
    
    for(vector<VariableAccessNode*>::iterator j=i; j!=myUsages.end(); ) {
      VariableAccessNode* it=(*j);

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
  int maxAttributeUsage=myConfig->getInt("maxAttributeUsage");
  for(vector<VariableAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {    
    // Throw a warning if accessed attribute too often
    string cs=(*i)->getArg("count");
    int count=atoi(cs.c_str());
    if (count>maxAttributeUsage) {
      Warnings* w=Warnings::getInstance();
      
      stringstream s;
      s << "Attribute " << (*i)->getArg("name")
	<< " accessed " << count << " times. Threshold value is " << maxAttributeUsage << ".";
      w->addWarning(theData->getNode()->getArg("name"), s.str());
      Debug::print(20, "Logged Warning");      
    }

    myModelPtr->add(*i);
  }


}

CXChildVisitResult VariableAccessParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  CXChildVisitResult retval=Parser::parse(theCursor, theParent, theData);

  if (theCursor.kind==CXCursor_DeclRefExpr ||
      theCursor.kind==CXCursor_MemberRefExpr) {
    string name=clang_getCString(clang_getCursorDisplayName(theCursor));
    string calledParent=clang_getCString(clang_getCursorDisplayName(clang_getCursorSemanticParent(clang_getCursorReferenced(theCursor))));
    
    Debug::print(10, (string) "   AttributeAccess " +name + ":" + calledParent);

    set<ModelNode*>* nodes=myModelPtr->getNodes();
    myModelPtr->filterByType(*nodes,AttributeNode::t());
    myModelPtr->filterByArg(*nodes, "class", calledParent);
    myModelPtr->filterByArg(*nodes, "name", name);  

    if(nodes->size()!=0) {
      Debug::print(15, (string) "    Found attribute in: " +(*(nodes->begin()))->toString());
      
      AttributeAccessNode* n=new AttributeAccessNode(theCursor, name, theData->getNode()->target, (*(nodes->begin()))->target);
      n->setArg("count", "1");
      n->setArg("class", calledParent);
      myUsages.push_back(n);	   	      
    } else {
      Debug::print(15, "    not found as an attribute. Looking for a global variable.");

      // Get the node for this file      
      set<ModelNode*>* nodes=myModelPtr->getNodes();
      myModelPtr->filterByType(*nodes,FileNode::t());
     
      for (set<ModelNode*>::iterator i=nodes->begin(); i!=nodes->end(); i++) {
	string fullName=(*i)->getArg("basePath") + (*i)->getArg("name");
	Debug::print(20, (string) "     Matching against " + fullName);
	if (fullName==calledParent) {
	  Debug::print(15, (string) "    Found file: " +(*i)->toString());

	  // Now, let's find the variable declaration withibn that file
	  set<ModelNode*>* nodes2=myModelPtr->getNodes();
	  myModelPtr->filterByType(*nodes2,VariableNode::t());
	  myModelPtr->filterBySource(*nodes2,(*i)->target);
	  myModelPtr->filterByArg(*nodes2,"name", name);
	  
	  if (nodes2->size()!=0) {
	    Debug::print(15, (string) "    Found attribute: " + (*(nodes2->begin()))->toString());

	    GlobalVariableAccessNode* n=new GlobalVariableAccessNode(theCursor, name, theData->getNode()->target, (*(nodes2->begin()))->target);
	    n->setArg("count", "1");

	    myUsages.push_back(n);
	  }
	}
      }      
    }
  }
    

  return retval;
}


