// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017-2020 Carsten Arnholm
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

#ifndef BOOST_COMMAND_LINE_H
#define BOOST_COMMAND_LINE_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>

class boost_command_line {
public:
   boost_command_line(int argc , char **argv);
   virtual ~boost_command_line();

   void show_version();
   void show_help();

   // count occurrence(s) of command line options
   size_t count(const std::string& option);

   template <typename T>
   T get(const std::string& option) { return vm[option].as<T>(); }

   bool parsed_ok() { return m_parse_ok; }

   size_t max_bool() const { return m_max_bool; }

private:
   boost::program_options::options_description generic;
   boost::program_options::options_description hidden;
   boost::program_options::options_description allowed;
   boost::program_options::variables_map vm;
   bool  m_parse_ok;
   bool  m_help_shown;
   bool  m_version_shown;
   size_t m_max_bool;
};

#endif // BOOST_COMMAND_LINE_H
