#include <fstream>
#include <string>
#include <vector>

#include "csvfile.hh"
#include "stringstuff.hh"
#include "debug.hh"

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

  theLine.clear();

  if (!myFile.is_open() || myFile.eof()) {
    return true;
  } else {
    char myLine[512];
    myFile.getline(myLine, 512);
    bool eof=true;
    if (!(myFile.rdstate() & ifstream::goodbit)) {
      eof=false;
    }
    string s=(string) myLine;
    if (s!="" || !eof) {
      StringStuff::strSplit(theLine, s, sep());
    } 
    return eof;
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
