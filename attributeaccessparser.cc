
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
  CXChildVisitResult retval=Parser::parse(theCursor, theParent, theData);

  if (theCursor.kind==CXCursor_DeclRefExpr ||
      theCursor.kind==CXCursor_MemberRefExpr) {
    string name=clang_getCString(clang_getCursorDisplayName(theCursor));
    string calledParent=clang_getCString(clang_getCursorDisplayName(clang_getCursorSemanticParent(clang_getCursorReferenced(theCursor))));

    Debug::print(10, (string) "   Looking for AttributeAccess " +name + ":" + calledParent);

    set<ModelNode*>* nodes=myModelPtr->getNodes();
    myModelPtr->filterByType(*nodes,AttributeNode::t());
    myModelPtr->filterByArg(*nodes, "class", calledParent);
    myModelPtr->filterByArg(*nodes, "name", name);  

    if(nodes->size()!=0) {
      Debug::print(1, (string) "    Found attribute in: " +(*(nodes->begin()))->toString());
      
      AttributeAccessNode* n=new AttributeAccessNode(theCursor, name, theData->getNode()->target, (*(nodes->begin()))->target);
      n->setArg("count", "1");
      n->setArg("class", calledParent);
      myUsages.push_back(n);	   	      
    } else {
      Debug::print(1, "    not found as an attribute");
    }
  }
    

  return retval;
}


