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
    type=t();
    source=-1;
    target=-1;
    setArg("name", theName);
  };
  static string t();
};


class FileNode : public ModelNode
{
public:
  FileNode(const string &theBasePath, const string &theName, const int &theProjectId) {
    type=t();
    source=theProjectId;
    target=-1;
    setArg("name", theName);
    setArg("basePath", theBasePath);
  };  
  static string t();
};


class FunctionNode : public ModelNode
{
public:
  FunctionNode(const CXCursor &theCursor, const string &theName, const int &theFileId) {
    type=t();
    source=theFileId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }
  
  static string t();
private:
};

class VariableNode : public ModelNode
{
public:
  VariableNode(const CXCursor &theCursor, const string &theName, const int &theFileId) {
    type=t();
    source=theFileId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }

  static string t();
};


// C++ stuff below
class ClassNode : public ModelNode
{
public:
  ClassNode(const CXCursor &theCursor, const string &theName, const int &theFileId) {
    type=t();
    source=theFileId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }

  static string t();
};

class InheritanceNode : public ModelNode
{
public:
  InheritanceNode(const CXCursor &theCursor, const string &theName, const string &theBaseClass, const int &theClassId, const int &theBaseClassId) {
    type=t();
    source=theBaseClassId;
    target=theClassId;
    setArg("name", theName);
    setArg("class", theBaseClass); // Would rather call this "BaseClass", but keeps it as "class" for consistency.
    myCursor=theCursor;
  }

  virtual void setId(const int &theId) { };
  virtual void findMissing(Model &theModel, Config &theConfig);
  static string t();
};


class MethodNode : public ModelNode
{
public:
  MethodNode(const CXCursor &theCursor, const string &theName, const int &theClassId) {
    type=t();
    source=theClassId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }

  static string t();
  virtual void findMissing(Model &theModel, Config &theConfig);
};

class AttributeNode : public ModelNode
{
public:
  AttributeNode(const CXCursor &theCursor, const string &theName, const int &theClassId) {
    type=t();
    source=theClassId;
    target=-1;
    setArg("name", theName);
    myCursor=theCursor;
  }

  static string t();
  virtual void findMissing(Model &theModel, Config &theConfig);
};


#endif
