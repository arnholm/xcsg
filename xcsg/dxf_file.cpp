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

#include "dxf_file.h"
#include "clipper_csg/polyset2d.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

dxf_file::dxf_file()
{}

dxf_file::~dxf_file()
{}

void dxf_file::write_item(int gc, const std::string& value)
{
   m_out << "  " << gc <<  std::endl << value <<  std::endl;
}

void dxf_file::write_item(int gc, double value)
{
   m_out << "  " << gc <<  std::endl << value <<  std::endl;
}

void dxf_file::write_item(int gc, int value)
{
   m_out << "  " << gc <<  std::endl << value <<  std::endl;
}


std::string dxf_file::write( std::shared_ptr<polyset2d> polyset, const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path dxf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = dxf_path.string() + ".dxf";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   m_out.open(path);
   if(!m_out.is_open())  throw std::runtime_error("Could not open file: " + path);

   // write header
   write_item(999,"DXF file created by xcsg (https://github.com/arnholm/xcsg)");
   write_item(0,"SECTION");
   write_item(2,"BLOCKS");
   write_item(0,"ENDSEC");

   // write entities, only LWPOLYLINE written
   write_item(0,"SECTION");
   write_item(2,"ENTITIES");
   for(auto i=polyset->begin(); i!=polyset->end(); i++) {
      std::shared_ptr<polygon2d> poly = *i;
      size_t nc = poly->size();
      for(size_t ic=0;ic<nc;ic++) {
         write_lwpolyline(poly->get_contour(ic));
      }
   }
   write_item(0,"ENDSEC");

   // write footer
   write_item(0,"SECTION");
   write_item(2,"OBJECTS");
   write_item(0,"ENDSEC");
   write_item(0,"EOF");

   return path;
}

void dxf_file::write_lwpolyline(std::shared_ptr<contour2d> contour)
{
   write_item(0,"LWPOLYLINE");
   write_item(8,0);  // layer 0
   write_item(70,1); // closed polyline
   for(size_t i=0; i<contour->size();i++) {
      const dpos2d& vtx = (*contour)[i];
      write_item(10,vtx.x());
      write_item(20,vtx.y());
   }
}
