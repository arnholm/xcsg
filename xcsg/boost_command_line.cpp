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

#include "boost_command_line.h"
#include "version.h"

#include <boost/program_options/parsers.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <sstream>
#include <numeric>
#include <vector>
#include <list>
#include <exception>
using namespace std;

using namespace boost;
namespace po = boost::program_options;

#include <iostream>
#include <fstream>

// http://www.boost.org/doc/libs/1_60_0/doc/html/program_options/tutorial.html

boost_command_line::boost_command_line(int argc , char **argv)
: generic("\nxcsg command line options & arguments ("+std::string(XCSG_version)+")")
, hidden("Hidden options")
, m_parse_ok(false)
, m_help_shown(false)
, m_version_shown(false)
, m_max_bool(std::numeric_limits<size_t>::max())
{
   generic.add_options()
        ("help,h",  "Show this help message.")
        ("version,v",  "Show program version (numeric part).")
        ("amf",   "AMF output format (Additive Manufacturing Format)")
        ("csg",   "CSG output format (OpenSCAD)")
        ("dxf",   "DXF output format (AutoCAD DXF - 2D only)")
        ("svg",   "SVG output format (Scalar Vector Graphics - 2D only)")
        ("stl",   "STL output format (STereoLitography)")
        ("astl",  "STL output format (STereoLitography) - ASCII")
        ("obj",   "OBJ output format (Wavefront format)")
        ("off",   "OFF output format (Geomview Object File Format)")
        ("max_bool", po::value<size_t>(),  "Max number of booleans allowed")
        ("fullpath", "Show full file paths.")
         ;

   hidden.add_options()
        ("xcsg-file",   po::value<string>(),         "input file") ;

   allowed.add(generic).add(hidden);

   // Declare that input-file can be specified without the "--input-file" specifier
   // declare that we require exactly 1 such parameter (-1 would mean unlimited number)
   po::positional_options_description p;
   p.add("xcsg-file", 1);

   // Parse the command line catching and displaying any
   // parser errors
   list<string> error_list;
   size_t error_count=0;

   try {
      po::store(po::command_line_parser( argc, argv).options(allowed).positional(p).run(), vm);
      po::notify(vm);
   }
   catch (std::exception &e) {

      // parser error
      error_list.push_back("ERROR: " + string(e.what()) );
      error_count++;
   }

   size_t help_count=0;
   if(vm.count("version")) {
      show_version();
      help_count++;
   }

   if(vm.count("help")) {
      show_help();
      help_count++;
   }

   // Check input file name
   if(vm.count("xcsg-file") == 0){
      // no message here, it is handled below
      error_count++;
   }
   else {
      boost::filesystem::path fullpath(get<std::string>("xcsg-file"));
      if(fullpath.extension() != ".xcsg") {
         ostringstream sout;
         sout << "ERROR: Input file extension must be '.xcsg', file name was " << fullpath;
         error_list.push_back(sout.str());
         error_count++;
      }
   }

   // check the output format specifiers
   size_t out_count = vm.count("amf") + vm.count("csg") + vm.count("stl") + vm.count("astl") + vm.count("obj") + vm.count("off") + vm.count("dxf") + vm.count("svg");
   if(out_count == 0  && vm.count("xcsg-file")>0) {

      // input file name specified, but no output format(s)
      ostringstream sout;
      sout << "ERROR: Input file name specified, but no output format(s). Use --help to see options.";
      error_list.push_back(sout.str());
      error_count++;
   }

   // check combination of input file and output specifiers
   if(vm.count("xcsg-file") == 0 && out_count>0 ) {
      error_list.push_back("ERROR: Output format(s) specified, but no input file name.");
      error_count++;
   }

   if(vm.count("xcsg-file") == 0 && out_count==0 ) {
      if(help_count==0) error_list.push_back("ERROR: No input file specified");
      error_count++;
   }

   if(vm.count("max_bool") > 0) {
      m_max_bool = get<size_t>("max_bool");
   }

   // some things are counted as errors without error message
   // this causes m_parse_ok to be false and the program stops
   if(out_count == 0)  error_count++;
   if(help_count==0 && error_count>0) {

      // the user did not ask for help but still didn't provide good parameters,
      // so help him anyway.
      show_help();
   }

   // parse is ok only if input parameters are sufficient to start processing
   m_parse_ok = (error_count==0);

   // display the error messages
   for(auto& s : error_list) {
      cout << s << endl;
   }
}

void boost_command_line::show_version()
{
   if(!m_version_shown) {
      cout <<  string(XCSG_version).substr(1) << endl;
      m_version_shown = true;
   }
}

void boost_command_line::show_help()
{
   if(!m_help_shown) {
      cout << generic << "  <xcsg-file>\t\tpath to input .xcsg file (required)" << endl << endl;
      m_help_shown = true;
   }
}

size_t boost_command_line::count(const std::string& option)
{
   return vm.count(option);
}

boost_command_line::~boost_command_line()
{}
