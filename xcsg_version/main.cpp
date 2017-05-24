#include <iostream>
using namespace std;

#include "xcsg/version.h"

// this is a utility that simply displays the xcsg version number
// without the 'v' prefix, useful in packaging scripts
int main(int argc, char **argv)
{
   string version(XCSG_version);
   cout << version.substr(1) << endl;
   return 0;
}


