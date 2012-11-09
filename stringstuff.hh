#ifndef STRINGSTUFF_HH
#define STRINGSTUFF_HH

#include <string>
#include <vector>

using namespace std;

class StringStuff
{
public:
  StringStuff() {};

  static vector<string> strSplit(const string &theString, const char &theSeparator=',');
  static string rtrim(const string &s, const string  &delim=" \f\n\r\t\v");
  static string ltrim(const string &s, const string &delim=" \f\n\r\t\v");
  static string lrtrim(const string &s, const string &delim=" \f\n\r\t\v");

private:
};

#endif
