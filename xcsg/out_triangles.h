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
   
#ifndef OUT_TRIANGLES_H
#define OUT_TRIANGLES_H

#include <vector>
#include <memory>
#include <carve/csg.hpp>
#include <ostream>

class out_triangles {
public:
   typedef std::vector<std::shared_ptr<carve::poly::Polyhedron>> poly_vector;

   out_triangles(std::shared_ptr<poly_vector> polyset);
   virtual ~out_triangles();

   // export to (formatted) STL, return the path to the file created
   // input is full path to .xcsg file, stl to be stored in same folder
   std::string  write_stl(const std::string& xcsg_path, bool binary);

   // export to OFF, return the path to the file created
   // input is full path to .xcsg file, OFF to be stored in same folder
   std::string  write_off(const std::string& xcsg_path);

   // export to wavefront OBJ, return the path to the file created
   // input is full path to .xcsg file, OFF to be stored in same folder
   std::string  write_obj(const std::string& xcsg_path);

   // export to OpenSCAD .csg
   std::string  write_csg(const std::string& xcsg_path);

private:
   // write to ASCII STL, return the path to the file created
   // input is full path to file, file extension will be replaced to ".stl"
   std::string  write_stl_ascii(const std::string& file_path);

   // write to binary STL, return the path to the file created
   // input is full path to file, file extension will be replaced to ".stl"
   std::string  write_stl_binary(const std::string& file_path);

private:
   std::shared_ptr<poly_vector> m_polyset;
};

#endif // OUT_TRIANGLES_H
