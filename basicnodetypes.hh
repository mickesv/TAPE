#ifndef BASICNODETYPES_HH
#define BASICNODETYPES_HH

#include <clang-c/Index.h>
#include <string>

#include "model.hh"
#include "config.hh"
#include "debug.hh"

class ProjectNode : public ModelNode
{
public:
  ProjectNode(const string &theName) {
    type="ProjectHasId";
    source=-1;
    target=-1;
    setArg("name", theName);
  };
};


class FileNode : public ModelNode
{
public:
  FileNode(const string &theBasePath, const string &theName, const int &theProjectId) {
    type="ProjectHasFile";
    source=theProjectId;
    target=-1;
    setArg("name", theName);
    setArg("basePath", theBasePath);
  };  

private:
};


class FunctionNode : public ModelNode
{
public:
  FunctionNode(const CXCursor &theCursor, const string &theName, const int &theFileId) {
    type="FileDeclaresFunction";
    source=theFileId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }

  CXCursor* getCursor() { return &myCursor; };  
private:
  CXCursor myCursor;
};

class VariableNode : public ModelNode
{
public:
  VariableNode(const CXCursor &theCursor, const string &theName, const int &theFileId) {
    type="FileDeclaresVariable";
    source=theFileId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }


  CXCursor* getCursor() { return &myCursor; };

private:
  CXCursor myCursor;

};


#endif
