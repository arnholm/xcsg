// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE. ALL COPIES OF THIS FILE MUST INCLUDE THIS LICENSE.
// EndLicense:

#include <iostream>
#include <sstream>
using namespace std;

#include <boost/date_time.hpp>
namespace bpt = boost::posix_time;
namespace bdt = boost::date_time;

#include "boost_command_line.h"
#include "xcsg_main.h"


string elapsed_time(bpt::ptime time_begin, bpt::ptime time_end)
{
   bpt::time_duration diff = time_end - time_begin;
   double total_seconds = 0.001*diff.total_milliseconds();
   int hours    = int(total_seconds/3600);
   int mins     = int(total_seconds/60) - hours*60;
   double secs  = total_seconds - hours*3600 - mins*60;
   std::ostringstream out;
   out << hours<<"h " <<mins<<"m "<<secs<<"s";
   return out.str();
}

int main(int argc, char **argv)
{
   // record the time when program starts
   bpt::ptime  time_begin = bdt::microsec_clock<bpt::ptime>::local_time();

   boost_command_line cmd(argc,argv);
   if(cmd.parsed_ok()) {
      // command line parameters accepted
      try {
         xcsg_main engine(cmd);
         if(engine.run()) {

            // report the elapsed time
            cout << "xcsg finished using "<< elapsed_time(time_begin,bdt::microsec_clock<bpt::ptime>::local_time()) << endl;
            return 0;
         }
      }
      catch(std::exception& ex) {
         cout << "xcsg finished with exception: " << ex.what() << endl;
         return 1;
      }
   }
   return 0;
}


