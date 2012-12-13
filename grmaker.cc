#include <string>
#include <fstream>
#include <sstream>


#include "config.hh"
#include "model.hh"
#include "basicnodetypes.hh"
#include "parser.hh"
#include "debug.hh"
#include "graphmaker.hh"
#include "nodetypes.hh"

using namespace std;

int main(int argc, char* argv[])
{
  // Generic start for the entire family of tools
  Config myConfig(argc, argv);
  Model myModel(myConfig);
  Debug::setLevel(myConfig.getInt("debugLevel"));
  Debug::print(1, "----------------------------------------");
  Debug::print(1, (string) "Starting " + argv[0]);
  myConfig.debugPrint(100);

  // Start of tool-specific stuff
  // ------------------------------
  GraphMaker myGraphMaker(myModel, myConfig);
  iostream* s=new stringstream();

  if(myConfig.get("save")!="false") {
    s=new fstream(myConfig.get("graphFile").c_str(), ios_base::out);
  }
  
  myGraphMaker.setOutput(s);
    

  // Fill it up with what is previously already done
  if(myConfig.get("read")!="false") {
    Debug::print(1, "Reading model...");
    myModel.populate(myConfig.get("modelFile"));
    Debug::print(1, "Making graph...");
    myGraphMaker.makeGraph(myModel, myConfig);
  }

  // Save
  if(myConfig.get("save")!="false") {
    Debug::print(1, "Closing graphFile...");    
    ((fstream*) s)->close();
  }

  Debug::print(1, (string) "End of " + argv[0]);
  Debug::print(1, "----------------------------------------");
}
