#include <clang-c/Index.h>
#include <string>
#include <sstream>

#include "parser.hh"

CXChildVisitResult Parser::parse(const CXCursor &theCursor, const CXCursor &theParent, ParserData* theData)
{
  // Give a decent default return behaviour that can be overridden by subclasses
    switch( theCursor.kind )
    {
    case CXCursor_ParmDecl :
    case CXCursor_VarDecl :
    case CXCursor_CallExpr :
    case CXCursor_DeclRefExpr :
      return CXChildVisit_Continue;
    case CXCursor_DeclStmt :
    case CXCursor_CompoundStmt :
    case CXCursor_ReturnStmt :
    case CXCursor_ForStmt :
    case CXCursor_DoStmt :
    case CXCursor_WhileStmt :
    case CXCursor_SwitchStmt :
    case CXCursor_CaseStmt :
    case CXCursor_IfStmt :
    case CXCursor_UnexposedExpr :
    case CXCursor_UnaryOperator :
    case CXCursor_ParenExpr :
    case CXCursor_CStyleCastExpr :
    case CXCursor_StmtExpr :
    case CXCursor_ConditionalOperator :
    case CXCursor_ArraySubscriptExpr :
      return CXChildVisit_Recurse;
    case CXCursor_IntegerLiteral :
    case CXCursor_CharacterLiteral :
    case CXCursor_FloatingLiteral :
    case CXCursor_BreakStmt :
    case CXCursor_GotoStmt :
    case CXCursor_DefaultStmt :
    case CXCursor_ContinueStmt :
    case CXCursor_MemberRefExpr :      
	return CXChildVisit_Continue;
    case CXCursor_CompoundAssignOperator:  // e.g. +=
    case CXCursor_BinaryOperator :    // Assignment=
      return CXChildVisit_Recurse;
    default:
      return CXChildVisit_Recurse;    
    }
    
    return CXChildVisit_Recurse;
}


// ----------
// ParserData


ParserData::ParserData(ModelNode* theNode)
  : myNode(theNode)
{
  myFilename=myNode->getArg("basePath") + myNode->getArg("name");
}

void ParserData::add(const string &theKey, int theValue)
{
  int val=get(theKey);
  if (val==-1) {
    return;
  }

  val += theValue;

  set(theKey, val);
}

void ParserData::set(const string &theKey, int theValue)
{
  data[theKey]=theValue;
}

int ParserData::get(const string &theKey)
{
  if (data.find(theKey)==data.end()) {
    return -1;
  }

  return data[theKey];
}

string ParserData::getFilename()
{
  return myFilename;
}
