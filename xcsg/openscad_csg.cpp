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

#include "openscad_csg.h"
#include <iomanip>
#include <stdexcept>
#include "clipper_csg/polygon2d.h"
#include "clipper_csg/contour2d.h"
#include "xpolyhedron.h"
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

openscad_csg::openscad_csg(const std::string& xcsg_path)
{
   boost::filesystem::path fullpath(xcsg_path);
   boost::filesystem::path csg_path = fullpath.parent_path() / fullpath.stem();
   m_path = csg_path.string() + ".csg";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(m_path.begin(),m_path.end(), '\\', '/');

   m_out.open(m_path);
   if(!m_out.is_open())  throw std::runtime_error("Could not open file: " + m_path);

   m_out << "// OpenSCAD file created by xcsg using CARVE: " << m_path << endl;
   m_out << "union() {" << std::endl;
}

openscad_csg::~openscad_csg()
{
   m_out << "};" << std::endl;
}

std::string openscad_csg::path() const
{
   return m_path;
}


void openscad_csg::write_polyhedron(std::shared_ptr<xpolyhedron> poly, bool raw)
{
   if(raw) write_polyhedron_raw(poly);
   else    write_polyhedron_pretty(poly);
}

void openscad_csg::write_polygon(std::shared_ptr<polygon2d> poly, bool raw)
{
   if(raw) write_polygon_raw(poly);
   else    write_polygon_pretty(poly);
}

void openscad_csg::write_polyhedron_raw(std::shared_ptr<xpolyhedron> poly)
{
   m_out << "\tpolyhedron( ";;

   m_out << "points=[ ";
   for(size_t ivert=0; ivert<poly->v_size(); ivert++) {
      const carve::geom3d::Vector& pos = poly->v_get(ivert);
      if(ivert > 0) m_out << ',';
      m_out << '[' << std::setprecision(16) << pos[0] << ',' << std::setprecision(16) <<  pos[1] << ',' << std::setprecision(16) <<  pos[2] << ']';;
   }
   m_out << "],";;

   m_out << " faces=[ ";
   for(size_t iface=0; iface<poly->f_size(); iface++) {
      const xface& face = poly->f_get(iface);
      if(iface > 0) m_out << ',';
      m_out << '[';
      size_t iv=0;
      for(auto i=face.rbegin(); i!=face.rend(); i++) {
         if(iv++ > 0) m_out << ',';
         m_out << *i;
      }
      m_out << ']';
   }
   m_out << "] ";

   m_out << ");" << std::endl;
}


void openscad_csg::write_polyhedron_pretty(std::shared_ptr<xpolyhedron> poly)
{
   m_out << "\tpolyhedron( " << std::endl;

   m_out << "\t\tpoints=[ "<< std::endl;
   for(size_t ivert=0; ivert<poly->v_size(); ivert++) {
      const carve::geom3d::Vector& pos = poly->v_get(ivert);
      m_out << "\t\t";
      if(ivert > 0) m_out << ',';
      m_out << '[' << std::setprecision(16) << pos[0] << ',' << std::setprecision(16) <<  pos[1] << ',' << std::setprecision(16) <<  pos[2] << ']' << std::endl;
   }
   m_out << "\t\t]," << std::endl;

   m_out << "\t\tfaces=[ "<< std::endl;
   for(size_t iface=0; iface<poly->f_size(); iface++) {
      m_out << "\t\t";
      const xface& face = poly->f_get(iface);
      if(iface > 0) m_out << ',';
      m_out << '[';
      size_t iv=0;
      for(auto i=face.rbegin(); i!=face.rend(); i++) {
         if(iv++ > 0) m_out << ',';
         m_out << *i;
      }
      m_out << ']' << std::endl;
   }
   m_out << "\t\t] " << std::endl;

   m_out << "\t);" << std::endl;
}

void openscad_csg::write_polygon_raw(std::shared_ptr<polygon2d> poly )
{
   // number of contours in polygon
   size_t nc = poly->size();

   m_out << "\tpolygon( ";

   // first write all the  points in all contours
   size_t ivcount = 0;
   m_out << "points=[ ";
      for(size_t ic=0; ic<nc; ic++) {
         std::shared_ptr<const contour2d> contour = (*poly)[ic];
         for(size_t i=0; i<contour->size(); i++) {
            dpos2d pos = (*contour)[i];
            if(ivcount == 0) m_out << ' ';
            else             m_out << ',';
            m_out << '[' << std::setprecision(16) << pos.x() << ',' << std::setprecision(16) <<   pos.y() << ']';
            ivcount++;
         }
      }
   m_out << "],";

   // then write all the contours, referring to the points
   ivcount = 0;
   m_out << "paths=[ ";
      for(size_t ic=0; ic<nc; ic++) {
         std::shared_ptr<const contour2d> contour = (*poly)[ic];
         size_t nv = contour->size();
         m_out << '[';
         for(size_t iv=0; iv<nv; iv++) {
            if(iv > 0) m_out << ',';
            m_out << ivcount++;
         }
         m_out << ']';
      }
   m_out << "] ";

   m_out << ");" << std::endl;
}


void openscad_csg::write_polygon_pretty(std::shared_ptr<polygon2d> poly )
{
   // number of contours in polygon
   size_t nc = poly->size();

   m_out << "\tpolygon( " << std::endl;

   // first write all the  points in all contours
   size_t ivcount = 0;
   m_out << "\t\tpoints=[ "<< std::endl;
      for(size_t ic=0; ic<nc; ic++) {
         std::shared_ptr<const contour2d> contour = (*poly)[ic];
         for(size_t i=0; i<contour->size(); i++) {
            dpos2d pos = (*contour)[i];
            m_out << "\t\t";
            if(ivcount == 0) m_out << ' ';
            else             m_out << ',';
            m_out << '[' << std::setprecision(16) << pos.x() << ',' << std::setprecision(16) <<   pos.y() << ']' << std::endl;
            ivcount++;
         }
      }
   m_out << "\t\t]," << std::endl;

   // then write all the contours, referring to the points
   ivcount = 0;
   m_out << "\t\tpaths=[ "<< std::endl;
      for(size_t ic=0; ic<nc; ic++) {
         std::shared_ptr<const contour2d> contour = (*poly)[ic];
         size_t nv = contour->size();
         if(ic == 0) m_out << "\t\t ";
         else        m_out << std::endl << "\t\t,";
         m_out << '[';
         for(size_t iv=0; iv<nv; iv++) {
            if(iv > 0) m_out << ',';
            m_out << ivcount++;
         }
         m_out << ']';
      }
   m_out << std::endl << "\t\t] "<< std::endl;

   m_out << "\t);" << std::endl;
}

void openscad_csg::write_transform(const carve::math::Matrix& t)
{
   m_out << "\tmultmatrix([";
   for(size_t irow=0;irow<4; irow++) {
      if(irow > 0) m_out << ',';
      m_out << '[';
      for(size_t icol=0;icol<4; icol++) {
         if(icol > 0) m_out << ',';
         m_out << setprecision(16) <<  t.m[icol][irow];
      }
      m_out << ']';
   }

   m_out << "])" << std::endl;
}
