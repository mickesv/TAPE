#ifndef CSVFILE_HH
#define CSVFILE_HH

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class CSVFile
{
public:
  CSVFile();
  ~CSVFile();
  
  void setName(string theFileName);
  void close();

  bool getLine(vector<string> &theLine);  
private:
  fstream myFile;
  string myFileName;
};

#endif
