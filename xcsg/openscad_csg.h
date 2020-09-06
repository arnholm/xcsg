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

#ifndef OPENSCAD_CSG_H
#define OPENSCAD_CSG_H

#include <memory>
#include <fstream>
#include <string>
#include <carve/matrix.hpp>
class xpolyhedron;
class polygon2d;

class openscad_csg {
public:

   // provide the full path to the output file. The file extension wil be changed to .csg
   openscad_csg(const std::string& xcsg_path);
   virtual ~openscad_csg();

   // return path to the file created
   std::string path() const;

   void write_polyhedron(std::shared_ptr<xpolyhedron> poly, bool raw=true );
   void write_polygon(std::shared_ptr<polygon2d> poly, bool raw=true  );

   void write_transform(const carve::math::Matrix& t);

protected:
   void write_polyhedron_raw(std::shared_ptr<xpolyhedron> poly );
   void write_polygon_raw(std::shared_ptr<polygon2d> poly );

   void write_polyhedron_pretty(std::shared_ptr<xpolyhedron> poly );
   void write_polygon_pretty(std::shared_ptr<polygon2d> poly );


private:
   std::ofstream m_out;
   std::string   m_path;
};

#endif // OPENSCAD_CSG_H
