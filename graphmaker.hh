#ifndef GRAPHMAKER_HH
#define GRAPHMAKER_HH

#include <vector>
#include <string>
#include <iostream>

#include "model.hh"
#include "config.hh"

class GraphMaker
{
public:
  GraphMaker(Model &theModel, Config &theConfig) : myModelPtr(&theModel), myConfigPtr(&theConfig) {};

  void makeGraph(Model &theModel, Config &theConfig);
  void makeNodes(Model &theModel, const string &theStereotype, const string &theFilter);
  void makeArches(Model &theModel, const string &theStereotype, const string &theFilter);
  void setOutput(iostream* theFile);
private:
  Model* myModelPtr;
  Config* myConfigPtr;
  iostream* myFile;
};

#endif
