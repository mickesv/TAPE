#include <string>

#include "stringstuff.hh"
#include "debug.hh"

vector<string> StringStuff::strSplit(const string &theString, const char &theSeparator)
{
  vector<string> myList;
  int prev_pos=0;
  int pos=theString.find_first_of(theSeparator, 0);

  // TODO: Something is volatile in here and causes the occasional segmentation fault. Probaly stack stuff
  Debug::print(200, (string) "StringStuff::strSplit " + theString);

  while(pos!=string::npos) {
    string subs=theString.substr(prev_pos, pos-prev_pos);
    myList.push_back(lrtrim(subs));
    pos++;
    prev_pos=pos;
    pos=theString.find_first_of(theSeparator, pos);
  }

  if (prev_pos<theString.length()) {
    myList.push_back(lrtrim(theString.substr(prev_pos)));
  }

  return myList;
}

string StringStuff::rtrim(const string &s, const string &delim)
{
  return s.substr( 0, s.find_last_not_of( delim ) + 1 );
}

string StringStuff::ltrim(const string &s, const string &delim)
{
  return s.substr( s.find_first_not_of( delim ) );
}

string StringStuff::lrtrim(const string &s, const string &delim)
{
  return ltrim( rtrim( s, delim ), delim );  
}
