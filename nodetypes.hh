#ifndef NODETYPES_HH
#define NODETYPES_HH

#include "model.hh"
#include "basicnodetypes.hh"
#include "functioncallparser.hh"
#include "gvaccessparser.hh"
#include "componentmaker.hh"

// Keep these here to facilitate getting an overview of the different
// nodetypes, and so that we may easily change in the future, should
// we so desire.

// Basic Node types
string ModelNode::t() {return "UnknownNode";};
string ProjectNode::t() {return "ProjectHasId";};
string FileNode::t() {return "ProjectHasFile";};
string FunctionNode::t() {return "FileDeclaresFunction";};
string VariableNode::t() {return "FileDeclaresVariable";};
string ClassNode::t() {return "FileDeclaresClass";};
string InheritanceNode::t() {return "ClassInheritsClass";};
string MethodNode::t() {return "ClassContainsMethod";};
string AttributeNode::t() {return "ClassContainsAttribute";};

// Parser defined Node types
string CallNode::t() {return "FunctionCallsFunction";};
string GlobalVariableAccessNode::t() {return "FunctionAccessVariable";};
string ComponentNode::t() {return "ProjectHasComponent";};
string ComponentContainsNode::t() {return "ComponentContains";};
string ComponentCallNode::t() {return "ComponentCalls";};


#endif
