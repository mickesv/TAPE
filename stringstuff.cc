#include <string>

#include "stringstuff.hh"
#include "debug.hh"

void StringStuff::strSplit(vector<string> &theResult, const string &theString, const char &theSeparator)
{
  Debug::print(200, (string) "StringStuff::strSplit " + theString);

  int prev_pos=0;
  int pos=theString.find_first_of(theSeparator, 0);

  int q1pos=theString.find_first_of('"',0);
  int q2pos=q1pos;
  if (q1pos!=string::npos) {
    q2pos=theString.find_first_of('"',q1pos+1);
  }


  // TODO: Something is volatile in here and causes the occasional segmentation fault. Probaly stack stuff

  while(pos!=string::npos) {
    string subs=theString.substr(prev_pos, pos-prev_pos);
    theResult.push_back(lrtrim(subs));
    pos++;
    prev_pos=pos;

    if ((pos>=q1pos) && (pos<=q2pos)) {
      pos=q2pos+1;
      
      // Find next quote, if any
      q1pos=theString.find_first_of('"',pos);
      q2pos=q1pos;
      if (q1pos!=string::npos) {
	q2pos=theString.find_first_of('"',q1pos+1);
      }
    }
    pos=theString.find_first_of(theSeparator, pos);
  }

  // Add whatever is left too
  if (prev_pos<theString.length()) {
    theResult.push_back(lrtrim(theString.substr(prev_pos)));
  }
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
