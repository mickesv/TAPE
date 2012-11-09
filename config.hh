#ifndef CONFIG_HH
#define CONFIG_HH

#include <map>
#include <vector>
#include <string>



/*
  Reads a configuration file (default: "config.cfg") of the format:

  token=value
  // comment line
  tokenList=value,value,value

*/

using namespace std;

class Config
{
public:
  Config();
  Config(int argc, char* argv[]);
  ~Config();
  
  string get(const string &theKey);
  int getInt(const string &theKey);
  vector<string> getList(const string &theKey);

  void debugPrint(const int theLevel);
private:
  void parseArgs(int argc, char* argv[]);
  void readFile(string theFileName="config.cfg");


  map<string,string> myValues;
  vector<string> myFiles;
};


#endif  
