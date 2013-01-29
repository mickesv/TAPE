#include <clang-c/Index.h>

#include "extractor.hh"
#include "basicnodetypes.hh"
#include "model.hh"
#include "config.hh"
#include "debug.hh"


void Extractor::extractDeclarations(FileNode* theNode)
{
  CXIndex myIdx = clang_createIndex(1,1);
  string myFullName = theNode->getArg("basePath") + theNode->getArg("name");  

  char** clangargs;
  int clangargc=getClangArgs(clangargs);


  Debug::print(2, (string) " Extracting declarations from " + theNode->getArg("name"));
  CXTranslationUnit myTU = clang_createTranslationUnitFromSourceFile( myIdx, myFullName.c_str(), 
								      clangargc, clangargs, 0, 0);
  if (myTU==NULL) {
    Debug::print(1, "--> Error when extracting declarations.\n    Best guess is to check the clangArgs parameter.");
    return;
  }

  myParserData=new ParserData(theNode);
  clang_visitChildren(clang_getTranslationUnitCursor(myTU), declarationsVisitor, this);

  // cleanup
  delete myParserData;

}


void Extractor::parseFunction(set<Parser*> &theParsers, ModelNode* theFunction)
{
  myParsers=theParsers;
  myParserData=new ParserData(theFunction);

  Debug::print(2, (string) " Parsing Function " + theFunction->getArg("name"));
  
  // Tell everyone that we are starting
  Debug::print(100, "   Starting up parsers");
  for(set<Parser*>::iterator i=myParsers.begin(); i!=myParsers.end(); i++) {
    (*i)->startFunction(*myModelPtr, theFunction, myParserData);
  }

  // Recurse the function
  Debug::print(100, "   Recursing function");
  clang_visitChildren(*(theFunction->getCursor()), functionVisitor, this);
    
  // Tell everyone that we are done
  Debug::print(100, "   Closing parsers");
  for(set<Parser*>::iterator i=myParsers.begin(); i!=myParsers.end(); i++) {
    (*i)->endFunction(*myModelPtr, theFunction, myParserData);
  }

  // cleanup
  delete myParserData;
}


CXChildVisitResult Extractor::declarationsVisitor(CXCursor theCursor, CXCursor theParent, CXClientData data)
{
  Extractor* This = (Extractor*) data;
  
  // In all essence, the C parts are borrowed from Andrew's codebase.

  string filename = getFilename(theCursor);
  string myFullName = This->myParserData->getFilename();

  // The first two are C functions and global variables. The rest are for C++
  if( filename==myFullName && theCursor.kind==CXCursor_FunctionDecl &&
      clang_equalCursors(clang_getCursorDefinition(theCursor),theCursor))  {
    // C Functions

    // Defs are their own defs, decls are not
    // string nameargs = clang_getCString(clang_getCursorDisplayName(theCursor));
    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    FunctionNode* n=new FunctionNode(theCursor, name, This->myParserData->myNode->target);
    This->myModelPtr->add(n);
  } else if( theCursor.kind==CXCursor_VarDecl ) {
    // C variables (global)

    // Only record variable definitions, not declarations. In both static and extern cases the
    // usrs are valid as globally unique names (ie static names from different translation
    // units are guaranteed to be unique). The declarations lack some type info and are only
    // aliases of a definition somewhere else.
    CXCursor def = clang_getCursorDefinition(theCursor);
    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    if( clang_equalCursors(theCursor,def) ) {
      VariableNode* n=new VariableNode(theCursor,name, This->myParserData->myNode->target);
      This->myModelPtr->add(n);
    }

  } else if( filename==myFullName && theCursor.kind==CXCursor_ClassDecl ) {
    // C++ Classes

    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    Debug::print(100, (string) "  Found Class " + name);
    ClassNode* n= new ClassNode(theCursor, name,  This->myParserData->myNode->target);
    This->myModelPtr->add(n);

    return CXChildVisit_Recurse; // Recurse to find inlined methods
  } else if ( theCursor.kind==CXCursor_CXXBaseSpecifier ) {
    // C++ Inheritance

    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    name=name.substr(6);
    string parentName = clang_getCString(clang_getCursorSpelling(theParent));

    Debug::print(100, (string) "  Found Inheritance " + parentName + " inherits from " + name);

    // Look for the Id's
    set<ModelNode*>* theNodes=This->myModelPtr->getNodes();
    This->myModelPtr->filterByType(*theNodes, ClassNode::t());        
    This->myModelPtr->filterByArg(*theNodes, "name", parentName);        
    int source=-1;
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }

    theNodes=This->myModelPtr->getNodes();
    This->myModelPtr->filterByType(*theNodes, ClassNode::t());        
    This->myModelPtr->filterByArg(*theNodes, "name", name);        
    int target=-1;
    if (!theNodes->empty()) {
      target=(*(theNodes->begin()))->target;
    }

    InheritanceNode* n= new InheritanceNode(theParent, parentName, name, source, target);
    This->myModelPtr->add(n);

  } else if ( filename==myFullName &&
	      (theCursor.kind==CXCursor_CXXMethod ||
	       theCursor.kind==CXCursor_Constructor ||
	       theCursor.kind==CXCursor_Destructor ||
	       theCursor.kind==CXCursor_ConversionFunction)
	      && clang_equalCursors(clang_getCursorDefinition(theCursor),theCursor)) {
    // C++ Methods of various types

    CXCursor semParent=clang_getCursorSemanticParent(theCursor);
    Debug::print(100, (string) "  Found Class Method " + clang_getCString(clang_getCursorSpelling(semParent)) + "::" + clang_getCString(clang_getCursorSpelling(theCursor)));
    
    // Find the Class Node
    set<ModelNode*>* theNodes=This->myModelPtr->getNodes();
    This->myModelPtr->filterByType(*theNodes, ClassNode::t());        
    This->myModelPtr->filterByArg(*theNodes, "name", clang_getCString(clang_getCursorSpelling(semParent)));        
    int source=-1;
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }

    // Add to model
    string name = clang_getCString(clang_getCursorSpelling(theCursor));    
    MethodNode* n=new MethodNode(theCursor, name, source);
    n->setArg("class", clang_getCString(clang_getCursorSpelling(semParent)));
    This->myModelPtr->add(n);    
        
  } else if ( filename==myFullName &&
	      (theCursor.kind==CXCursor_FieldDecl)) {
    // C++ Class Attributes
    CXCursor semParent=clang_getCursorSemanticParent(theCursor);
    Debug::print(100, (string) "  Found Attribute " + clang_getCString(clang_getCursorSpelling(semParent)) + "::" + clang_getCString(clang_getCursorSpelling(theCursor)));

    // Find the Class Node
    set<ModelNode*>* theNodes=This->myModelPtr->getNodes();
    This->myModelPtr->filterByType(*theNodes, ClassNode::t());        
    This->myModelPtr->filterByArg(*theNodes, "name", clang_getCString(clang_getCursorSpelling(semParent)));        
    int source=-1;
    if (!theNodes->empty()) {
      source=(*(theNodes->begin()))->target;
    }

    // Add to model
    string name = clang_getCString(clang_getCursorSpelling(theCursor));    
    AttributeNode* n=new AttributeNode(theCursor, name, source);
    n->setArg("class", clang_getCString(clang_getCursorSpelling(semParent)));
    This->myModelPtr->add(n);        

  } else {
    // Cases to ignore
    //Debug::print(1, (string) "   Ignoring " + clang_getCString(clang_getCursorKindSpelling(theCursor.kind)) + ":" + clang_getCString(clang_getCursorSpelling(theCursor)));
  }

  return CXChildVisit_Continue;

}

CXChildVisitResult Extractor::functionVisitor(CXCursor theCursor, CXCursor theParent, CXClientData data)
{
  Extractor* This=(Extractor*) data;
  CXChildVisitResult retVal=CXChildVisit_Continue;

  for(set<Parser*>::iterator i=This->myParsers.begin(); i!=This->myParsers.end(); i++) {
    CXChildVisitResult rv;
    rv=(*i)->parse(theCursor, theParent, This->myParserData);
    if (rv==CXChildVisit_Recurse) {
      retVal=rv;
    }
  }

  return retVal;
}


string Extractor::getFilename(const CXCursor &c)
{
  // CCP from Andrew's codebase
  CXSourceLocation where = clang_getCursorLocation(c);
  CXFile file;
  unsigned line,column,offset;
  clang_getExpansionLocation(where, &file, &line, &column, &offset);
  if(file==NULL)
    return "unknownfile";
  return clang_getCString( clang_getFileName(file) );
}

int Extractor::getClangArgs(char** &cArgs)
{
  vector<string> sArgs;
  myConfig.getList(sArgs, "clangArgs");

  if(sArgs.size()==0) {
    cArgs=new char*[1];
    return 0;
  }

  cArgs=new char*[sArgs.size()];

  // How much space do I need?
  int size=0;
  for(vector<string>::iterator i=sArgs.begin(); i!=sArgs.end();i++) {
    size+=(*i).size();
    size++; // to accomodate the null termination
  }
  
  char* theArgs=new char[size];

  // Populate it
  int count=0;
  int pos=0;
  for(vector<string>::iterator i=sArgs.begin(); i!=sArgs.end();i++) {
    cArgs[count++]=theArgs+pos;
    strcpy(theArgs+pos, (*i).c_str());
    pos+=(*i).size()+1;
  }

  return sArgs.size();
}
