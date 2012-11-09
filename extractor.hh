#ifndef EXTRACTOR_HH
#define EXTRACTOR_HH

#include <clang-c/Index.h>

#include "model.hh"
#include "config.hh"
#include "parser.hh"

class Extractor
{
public:
  Extractor(Model &theModel, Config &theConfig) : myModelPtr(&theModel), myConfig(theConfig) {};

  void extractDeclarations(FileNode* theNode);
  void parseFunction(set<Parser*> &theParsers, FunctionNode* theFunction);

private:
  static string getFilename(const CXCursor &c);  

  static CXChildVisitResult declarationsVisitor(CXCursor theCursor, CXCursor theParent, CXClientData data);
  static CXChildVisitResult functionVisitor(CXCursor theCursor, CXCursor theParent, CXClientData data);



  Model* myModelPtr;
  Config myConfig;
  set<Parser*> myParsers;
  ParserData* myParserData;

};

#endif