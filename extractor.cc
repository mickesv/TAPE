#include <clang-c/Index.h>

#include "extractor.hh"
#include "basicnodetypes.hh"
#include "model.hh"
#include "config.hh"
#include "debug.hh"

char *clangargs[]={""};

void Extractor::extractDeclarations(FileNode* theNode)
{
  CXIndex myIdx = clang_createIndex(1,1);
  string myFullName = theNode->getArg("basePath") + theNode->getArg("name");  

  Debug::print(2, (string) "Extracting declarations from " + myFullName);
  CXTranslationUnit myTU = clang_createTranslationUnitFromSourceFile( myIdx, myFullName.c_str(), 
								      sizeof(clangargs)/sizeof(char*), clangargs, 0, 0);
  if (myTU==NULL) return;

  myParserData=new ParserData(theNode);
  clang_visitChildren(clang_getTranslationUnitCursor(myTU), declarationsVisitor, this);

  // cleanup
  delete myParserData;

}


void Extractor::parseFunction(set<Parser*> &theParsers, FunctionNode* theFunction)
{
  myParsers=theParsers;
  myParserData=new ParserData(theFunction);

  Debug::print(3, (string) " Parsing Function " + theFunction->getArg("name"));
  
  // Tell everyone that we are starting
  for(set<Parser*>::iterator i=myParsers.begin(); i!=myParsers.end(); i++) {
    (*i)->startFunction(*myModelPtr, theFunction, myParserData);
  }

  // Recurse the function
  clang_visitChildren(*(theFunction->getCursor()), functionVisitor, this);
    

  // Tell everyone that we are done
  for(set<Parser*>::iterator i=myParsers.begin(); i!=myParsers.end(); i++) {
    (*i)->endFunction(*myModelPtr, theFunction, myParserData);
  }

  // cleanup
  delete myParserData;
}


CXChildVisitResult Extractor::declarationsVisitor(CXCursor theCursor, CXCursor theParent, CXClientData data)
{
  Extractor* This = (Extractor*) data;
  
  // In all essence, this is borrowed from Andrew's codebase.

  string filename = getFilename(theCursor);
  string myFullName = This->myParserData->getFilename();

  if( filename==myFullName && theCursor.kind==CXCursor_FunctionDecl &&
      clang_equalCursors(clang_getCursorDefinition(theCursor),theCursor))  {
    // Defs are their own defs, decls are not
    string nameargs = clang_getCString(clang_getCursorDisplayName(theCursor));
    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    FunctionNode* f=new FunctionNode(theCursor, name, This->myParserData->myNode->target);
    This->myModelPtr->add(f);
  } else if( theCursor.kind==CXCursor_VarDecl ) {
    // Only record variable definitions, not declarations. In both static and extern cases the
    // usrs are valid as globally unique names (ie static names from different translation
    // units are guaranteed to be unique). The declarations lack some type info and are only
    // aliases of a definition somewhere else.
    CXCursor def = clang_getCursorDefinition(theCursor);
    string name = clang_getCString(clang_getCursorSpelling(theCursor));
    if( clang_equalCursors(theCursor,def) ) {
      VariableNode* vn=new VariableNode(theCursor,name, This->myParserData->myNode->target);
      This->myModelPtr->add(vn);
    }
  } else {
    // Cases to ignore
    // cout << clang_getCString(clang_getCursorKindSpelling(cursor.kind)) << endl;
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
