
#include <clang-c/Index.h>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>

#include "gvaccessparser.hh"
#include "parser.hh"
#include "model.hh"
#include "debug.hh"

class VarDecl
{
public:
  string varName;
  int id;
  stack<int> depth;
private:
};


void GVAccessParser::startFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::startFunction(theModel, theFunction, theData);
  myVariables.clear();
  myUsages.clear();

  // Get all Global Variables in this file and move over to my temporary structure.
  set<ModelNode*>* myGV = theModel.getNodes();
  theModel.filterByType(*myGV,"FileDeclaresVariable");
  theModel.filterBySource(*myGV, theFunction->source);  
  VarDecl* vd;
  for(set<ModelNode*>::iterator i=myGV->begin(); i!=myGV->end(); i++) {
    vd=new VarDecl();
    vd->varName=(*i)->getArg("name");
    vd->id=(*i)->target;
    vd->depth.push(0);
    myVariables[vd->varName]=vd; // Redundant information, I know, but it makes it easier to find. Can clean up VarDecl.
  }
}

void GVAccessParser::endFunction(Model &theModel, FunctionNode* theFunction, ParserData* theData)
{
  Parser::endFunction(theModel, theFunction, theData);

  // Merge all accesses of the same global variables and count them
  for(vector<GlobalVariableAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {
    GlobalVariableAccessNode* base=(*i);
    
    for(vector<GlobalVariableAccessNode*>::iterator j=i; j!=myUsages.end(); ) {
      GlobalVariableAccessNode* it=(*j);

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
  for(vector<GlobalVariableAccessNode*>::iterator i=myUsages.begin(); i!=myUsages.end(); i++) {
    myModelPtr->add(*i);
  }
}

CXChildVisitResult GVAccessParser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  CXChildVisitResult retval=CXChildVisit_Recurse;

  // Ascend
  bool asc=false;
  int myDepth=0;

  while (ascendCompound(theCursor, theData)) {
    asc=true;
  }

  // Clear Overridden global variables
  if(asc) {
    for(map<string,VarDecl*>::iterator i=myVariables.begin(); i!=myVariables.end();i++) {      
      while ((*i).second->depth.top()>myDepth) {
	(*i).second->depth.pop();
      }
    }   
  }
  
  // For debugging purposes, get the cursor kind and name as a string
  // This leaks memory...
  string myKind = clang_getCString(clang_getCursorKindSpelling(theCursor.kind));
  string myName = clang_getCString(clang_getCursorDisplayName(theCursor));
  
  switch( theCursor.kind )
    {
    case CXCursor_EnumDecl:
    case CXCursor_ParmDecl :
    case CXCursor_VarDecl :
      {
	// Register the declaration and see if it matches any of my global vars.
	//Debug::print(1, (string) "   Declare variable " + myName + " # " + myKind);

	if (myVariables.find(myName)!=myVariables.end()) {
	  Debug::print(100, (string) "    Overriding Global Variable Name " + myName);
	  myVariables[myName]->depth.push(getCompoundDepth());
	}
	retval=CXChildVisit_Recurse;
	break;
      }
    case CXCursor_DeclRefExpr :
      {
	retval=CXChildVisit_Recurse;
	  
	// Make sure I am not referring to a function
	set<ModelNode*>* nodes=myModelPtr->getNodes();
	myModelPtr->filterByType(*nodes,"FileDeclaresFunction");
	myModelPtr->filterBySource(*nodes, theData->getNode()->source);
	myModelPtr->filterByArg(*nodes, "name", myName);
	if (nodes->size()!=0) {
	  // There is a function by this name, so I am going to assume (not entirely pukkah) that we are in fact dealing with a function call and abort.
	  break;
	}
	// else, it is a variable. Let's see if it is a global variable...
	if (myVariables.find(myName)!=myVariables.end()) {
	  VarDecl* vd=myVariables[myName];

	  if(vd->depth.top()==0) {
	    Debug::print(100, (string) "    Using a global variable: " + myName);

	    GlobalVariableAccessNode* gvan=new GlobalVariableAccessNode(theCursor, myName, theData->getNode()->target, vd->id);
	    stringstream s;
	    s << 1;
	    gvan->setArg("count", s.str());

	    myUsages.push_back(gvan);
	  } else {
	    Debug::print(100, (string) "    Using an overridden global variable: " + myName);
	  }
	}

	break;
      }
    case CXCursor_VariableRef:
      {
	// Using the variable
	// Don't know when this is actually used...
	Debug::print(1, (string) "   (NOT TAKEN CARE OF) Using variable " + myName + " # " + myKind);

	retval=CXChildVisit_Recurse;
	break;
      }
    case CXCursor_CompoundStmt :
    case CXCursor_ForStmt :
    case CXCursor_DoStmt :
    case CXCursor_WhileStmt :
    case CXCursor_SwitchStmt :
    case CXCursor_CaseStmt :
    case CXCursor_IfStmt :
    case CXCursor_StmtExpr :
      {
	// Descend
	descendCompound(theCursor, theData);
	myDepth++;
	retval=CXChildVisit_Recurse;
	break;
      }
    case CXCursor_ParenExpr :
      {
	Debug::print(100, (string) "   Should I take care of this? " + myName + "#" + myKind);
      }
    default:
      retval=CXChildVisit_Recurse;    
    }
    
    return retval;
}

