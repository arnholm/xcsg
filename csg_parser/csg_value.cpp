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

#include "csg_value.h"
#include "csg_scalar.h"
#include "csg_vector.h"
#include <stdexcept>

csg_value::csg_value(size_t line_no)
: m_line_no(line_no)
{}

csg_value::~csg_value()
{}

std::string csg_value::to_string() const
{
   throw std::runtime_error(".csg file line " + std::to_string(m_line_no) +", csg_value::to_string(), value is not a scalar");
}

bool csg_value::to_bool() const
{
   throw std::runtime_error(".csg file line " + std::to_string(m_line_no) +", csg_value::to_bool(), value is not a scalar");
}

int csg_value::to_int() const
{
   throw std::runtime_error(".csg file line " + std::to_string(m_line_no) +", csg_value::to_int(), value is not a scalar");
}

double csg_value::to_double() const
{
   throw std::runtime_error(".csg file line " + std::to_string(m_line_no) +", csg_value::to_double(), value is not a scalar");
}

std::shared_ptr<csg_value> csg_value::get(size_t i) const
{
   throw std::runtime_error(".csg file line " + std::to_string(m_line_no) +", csg_value::get(), value is not a vector");
}

std::shared_ptr<csg_value> csg_value::parse(const std::string& value_str, size_t line_no)
{
   std::shared_ptr<csg_value> value;

   // recursively parse the string
   // when a vector is found [
   //    call this function again, until corresponding ] is found

   for(size_t i=0; i<value_str.size(); i++) {
      char c = value_str[i];

      if( c==',') continue;

      if(c == '[') {
         // found an array, find the corresponding ']' from the right side
         size_t i2 = value_str.find_last_of(']');

         // strip the [] pair from both sides
         // so we end up with a string of comma separated values (which may be vectors)
         return parse_vector(value_str.substr(i+1,i2-1),line_no);
      }
      else {
         // found a scalar
         return std::make_shared<csg_scalar>(value_str,line_no);
      }
   }

   return value;
}

std::shared_ptr<csg_value> csg_value::parse_vector(const std::string& values, size_t line_no)
{
   std::vector<std::shared_ptr<csg_value>> vec;
   std::string token;

   size_t icount = 0;
   for(size_t i=0; i<values.size(); i++) {
      char c = values[i];

      if(c == '[') icount++;
      if(c == ']') icount--;
      token +=c;

      if(icount==0 && c == ']') {
         // end of vector data
         size_t len = token.size();
         std::string vals = token.substr(1,len-2);
         vec.push_back(parse_vector(vals,line_no));
         token  = "";
         icount = 0;

         // eat following comma
         i++;
      }
      else if(icount==0 && (c == ',' || i==values.size()-1) ) {
         // end of scalar data
         size_t len = (c == ',')? token.size()-1 : token.size();
         std::string val = token.substr(0,len);
         vec.push_back(std::make_shared<csg_scalar>(val,line_no));
         token  = "";
         icount = 0;
      }
   }
   return std::make_shared<csg_vector>(vec,line_no);
}


