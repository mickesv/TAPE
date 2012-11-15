#include <fstream>
#include <string>
#include <vector>

#include "csvfile.hh"
#include "stringstuff.hh"

using namespace std;

CSVFile::CSVFile()
{
}

CSVFile::~CSVFile()
{
  close();
}

void CSVFile::setName(string theFileName)
{
  myFileName=theFileName;
}

void CSVFile::close()
{
  myFile.close();
}

bool CSVFile::getLine(vector<string> &theLine)
{
  if(!myFile.is_open()) {
    myFile.open(myFileName.c_str(), ios::in);    
  }

  if (!myFile.is_open() || myFile.eof()) {
    return true;
  } else {
    char myLine[512];
    myFile.getline(myLine, 512);
    string s=(string) myLine;
    if (s!="") {
      theLine=StringStuff::strSplit(s, sep());
    }
    return false;
  }
}

bool CSVFile::writeLine(string theLine)
{
  if(!myFile.is_open()) {
    myFile.open(myFileName.c_str(), ios::out);    
  }
  if (!myFile.is_open()) {
    return true;
  } else {
    myFile << theLine << endl;
  }

  
  return true;
}
