
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <string>
#include <map>


#include "config.hh"
#include "stringstuff.hh"
#include "debug.hh"

using namespace std;

Config::Config()
{
  readFile();
}

Config::Config(int argc, char* argv[])
{
  parseArgs(argc, argv);
  readFile(get("cfgFile"));  
}

Config::~Config()
{
}

string Config::get(const string &theKey)
{
  return myValues[theKey];
}

int Config::getInt(const string &theKey)
{
  string s=myValues[theKey];
  if (s=="") {
    return -1; // May need to find a better default value.   
  } else {
    return atoi(s.c_str());
  }
}


vector<string> Config::getList(const string &theKey)
{
  if (theKey=="file") {
    return myFiles;
  }

  string s = myValues[theKey];
  if (s=="") {
    vector<string> l;
    return l;
  } else {
    return StringStuff::strSplit(s, ',');
  }
}


void Config::parseArgs(int argc, char* argv[])
{
  while (argc>1 && argv[argc-1] != "\0") {
    string s=argv[argc-1];

    if (s=="-help") {
      cout << "help" << endl;
    } else 

      {
	// default:
	vector<string> myArg=StringStuff::strSplit(s,'=');
	myValues[StringStuff::lrtrim(myArg[0])]=StringStuff::lrtrim(myArg[1]);
      }

    
    argc--;
  }

  // and unless we already have this defined:
  if(myValues.find("cfgFile")==myValues.end()){
    myValues["cfgFile"]="config.cfg";
  }
}

void Config::readFile(string theFileName)
{
  ifstream in;

  in.open(theFileName.c_str(), ios::in);
  Debug::print(1, "Reading configuration file: " + theFileName);
  
  int lineNum=0;

  if(in.is_open()) {
    char theLine[256];;
    while(!in.eof()){
      in.getline(theLine, 256);
      lineNum++;
      string s=StringStuff::lrtrim((string) theLine);

      if (s[0]!='/' && s[1]!='/') { // not a comment

	vector<string> myArg=StringStuff::strSplit(s,'=');

	if (myArg.size()==2) {	
	  if (StringStuff::lrtrim(myArg[0]) == "file") { // handle files separately
	    myFiles.push_back(StringStuff::lrtrim(myArg[1]));
	  } else {
	    myValues[StringStuff::lrtrim(myArg[0])]=StringStuff::lrtrim(myArg[1]);
	  }

	} else {
	  stringstream ss;
	  ss << "--> Invalid configuration directive on line: " << lineNum;	  
	  Debug::print(1, ss.str());
	}

      }
    }

    in.close();
  }
}

void Config::debugPrint(const int theLevel)
{
  if (theLevel>Debug::myLevel) { return; }

  // Once I am done, print out all my values, for debugging purposes.
  Debug::print(theLevel, "----------\nConfiguration:");
  
  for(map<string,string>::iterator i=myValues.begin(); i!=myValues.end();i++){
    Debug::print(theLevel, (*i).first + " = " + (*i).second);
  }

  Debug::print(theLevel, "\nFiles:");
  
  for(vector<string>::iterator i=myFiles.begin(); i!=myFiles.end(); i++) {
    Debug::print(theLevel, get("basePath")+(*i));
  }

  Debug::print(theLevel, "----------");

}
