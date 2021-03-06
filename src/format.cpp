#include <string>
#include <sstream>
#include "format.h"

using std::string;
using std::to_string;
using std::ostringstream;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{
    int hh = seconds/3600;
    int mm = (seconds - hh*3600)/60;
    int ss = seconds - hh*3600 - mm*60;
    ostringstream ostring;
    ostring<< hh << ":" << mm << ":" << (ss<10? "0":"")<<ss;
    return ostring.str();
}