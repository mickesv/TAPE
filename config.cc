
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
      cout << "Displaying help text..." << endl;
      cout << "\nUsage: " << argv[0] << "[options] [key]=[value]" << endl;
      cout << "\nOptions:" << endl;
      cout << "  -help           Displays this information" << endl;
      cout << "  [key]=[value]   Sets the value of [key] to [value]" << endl;
      cout << "\nBasic keys include:" << endl;
      cout << "  debugLevel      An int specifying the amount of" << endl;
      cout << "                  debug info to print. Default=1" << endl;
      cout << "  cfgFile         The name of the configuration file. Default=config.cfg" << endl;
      //      cout << "  read            if true, start by reading the modelFile." << endl;
      cout << "  save            if true, save the modelFile once done. Default=true" << endl;
      cout << "\nConfiguration file:" << endl;
      cout << " The configuration file is constructed in the same" << endl;
      cout << " way using [key]=[value] pairs." << endl;
      cout << "\n In addition to the keys above, the following ought to be used:" << endl;
      cout << "  projectName     The name of the project" << endl;
      cout << "  modelFile       Filename of the produced/used model" << endl;
      cout << "  basePath        The base path of the source code for the project" << endl;
      cout << "  file            One file in the project" << endl;
      cout << "                  (this key can be used repeatedly)" << endl;
      cout << "  parsers         Parsers to use for extracting information" << endl;
      cout << "                  from the source files." << endl;
      cout << "                  Basic parsers include: functionCalls, loopDepth, functionSize" << endl;
      cout << "------------------------------" << endl;
      cout << "Resuming..." << endl;
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
    char theLine[512];;
    while(!in.eof()){
      in.getline(theLine, 512);
      lineNum++;
      if (((string) theLine).size()==0) {
	continue;
      }

      string s=StringStuff::lrtrim((string) theLine);

      int comment=s.find("//"); // Deal with comments
      if (comment !=string::npos) {
	s=s.substr(0, comment);
      }

      if (s.size()!=0) {

	vector<string> myArg=StringStuff::strSplit(s,'=');

	// Sanity checks
	if(myArg.size()<2) {
	  stringstream ss;
	  ss << "--> Invalid configuration directive on line: " << lineNum;
	  Debug::print(1, ss.str());	  
	} else if (myArg.size()>2) {
	  stringstream ss;
	  ss << "--> Possibly Invalid configuration directive on line: " << lineNum
	     << "\n    Trying to recover...";
	  Debug::print(1, ss.str());
	  
	  string theString=myArg[1];
	  for(int i=2; i<myArg.size(); i++) {
	    theString+="=";
	    theString+=myArg[i];
	  }
	  myArg[1]=theString;
	  Debug::print(10, (string) "Recovered argument is: " + myArg[0] + "=" + myArg[1]);
	}

	// Add stuff to my config.
	if (StringStuff::lrtrim(myArg[0]) == "file") { // handle files separately
	  myFiles.push_back(StringStuff::lrtrim(myArg[1]));
	} else {
	  // First, make sure that the value does not already exist
	  if (get(StringStuff::lrtrim(myArg[0]))=="") {
	      myValues[StringStuff::lrtrim(myArg[0])]=StringStuff::lrtrim(myArg[1]);
	    }
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
