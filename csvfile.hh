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
  bool writeLine(string theLine);

  char sep(void) { return ';'; };
  char subsep(void) {return ','; };
private:
  fstream myFile;
  string myFileName;
};

#endif
