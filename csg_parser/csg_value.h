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

#ifndef CSG_VALUE_H
#define CSG_VALUE_H

#include <string>
#include <memory>

// abstract parameter value for csg node
class csg_value {
public:
   csg_value(size_t line_no);
   virtual ~csg_value();

   // convert a value string into a csg_value, possibly a scalar or (nested) vector
   static std::shared_ptr<csg_value> parse(const std::string& value_str, size_t line_no);
   static std::shared_ptr<csg_value> parse_vector(const std::string& value_str, size_t line_no);

   virtual bool is_vector() const { return false; }

   // 1 for scalars, 1+ for vectors
   virtual size_t size() const = 0;

   // returns reference to value index i for vectors
   virtual std::shared_ptr<csg_value> get(size_t i) const;

   // translation to basic types, for scalars
   virtual std::string to_string() const;
   virtual bool to_bool() const;
   virtual int to_int() const;
   virtual double to_double() const;

   size_t line_no() const { return m_line_no; }

private:
   size_t m_line_no;   // csg file line no
};

#endif // CSG_VALUE_H
