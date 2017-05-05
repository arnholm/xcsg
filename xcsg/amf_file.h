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
   
#ifndef AMF_FILE_H
#define AMF_FILE_H

class cf_xmlNode;
#include <vector>
#include <memory>
#include <carve/csg.hpp>
#include <ostream>

class amf_file {
public:
   typedef std::vector<std::shared_ptr<carve::poly::Polyhedron>> poly_vector;

   amf_file();
   virtual ~amf_file();

   // export to AMF, return the path to the file created
   // input is full path to file, file extension will be replaced to ".amf"
   std::string  write(std::shared_ptr<poly_vector> polyset, const std::string& file_path);

protected:
   bool to_amf_object(cf_xmlNode& parent, std::shared_ptr<poly_vector> polyset, size_t index);

};

#endif // AMF_FILE_H
