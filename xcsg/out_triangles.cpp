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

#include "out_triangles.h"
#include <carve/poly.hpp>
#include <fstream>
#include "std_filename.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

out_triangles::out_triangles(std::shared_ptr<poly_vector> polyset)
: m_polyset(polyset)
{}

out_triangles::~out_triangles()
{}

static double face_area(std::vector<carve::geom::vector<3>>& p)
{
   // https://en.wikipedia.org/wiki/Heron's_formula
   double a = (p[1] - p[0]).length();
   double b = (p[2] - p[1]).length();
   double c = (p[0] - p[2]).length();
   double s = 0.5*(a+b+c);
   return sqrt(s*(s-a)*(s-b)*(s-c));
}

std::string out_triangles::write_stl(const std::string& xcsg_path, bool binary)
{
   if(binary)return write_stl_binary(xcsg_path);
   else      return write_stl_ascii(xcsg_path);
}

std::string  out_triangles::write_csg(const std::string& xcsg_path)
{
   boost::filesystem::path fullpath(xcsg_path);
   boost::filesystem::path csg_path = fullpath.parent_path() / fullpath.stem();
   std::string path = csg_path.string() + ".csg";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   std::ofstream out(path);

   out << "// OpenSCAD file created by xcsg : " << path << std::endl;;
   out << "union() {" << std::endl;

      for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {

         std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
         std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

         out << "\tpolyhedron( ";

         // ========= points / vertices =================
         out << " points=[ ";
         for(size_t ivert=0; ivert<vertices.size(); ivert++) {
            carve::poly::Geometry<3>::vertex_t& vtx = vertices[ivert];
            if(ivert > 0) out << ',';
            out << '[' << std::setprecision(16) << vtx.v[0] << ',' << std::setprecision(16) << vtx.v[1] << ',' << std::setprecision(16) <<  vtx.v[2] << ']';
         }
         out << "],";

         // ========= faces =================
         out << " faces=[ ";
         for(size_t iface = 0; iface<poly->faces.size(); ++iface) {
            carve::poly::Face<3>& face = poly->faces[iface];

            size_t nvert = face.nVertices();
            if(nvert != 3)throw std::logic_error("carve_triangulate:: detected non-triangular face after triangulation!");
            if(iface > 0) out << ',';
            out << '[';
            std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
            poly->faces[iface].getVertexLoop(vloop);
            for(size_t ivert=0; ivert<nvert; ivert++) {
               if(ivert > 0) out << ',';
               size_t index = poly->vertexToIndex(vloop[2-ivert]); // reverse vertex order in OpenSCAD
               out << index;
            }
            out << ']';
         }
         out << "] ";

         out << ");" << std::endl;
      }

   out << "};" << std::endl;

   m_files_written.insert(path);
   return path;
}


std::string out_triangles::write_off(const std::string& xcsg_path)
{
   boost::filesystem::path fullpath(xcsg_path);
   boost::filesystem::path csg_path = fullpath.parent_path() / fullpath.stem();

   std::string path;

   for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {

      std::ostringstream postfix;
      if(ipoly > 0)postfix << '_' << ipoly;
      postfix << ".off";

      path = csg_path.string() + postfix.str();
      std::replace(path.begin(),path.end(), '\\', '/');

      std::ofstream out(path);

      std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
      std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

      out << "OFF " << std::endl;
  // OFF comment line not supported by tetgen
  //    out << "# OFF file created by xcsg : " << path << std::endl;
      out << poly->vertices.size() << ' ' << poly->faces.size() << " 0 " <<  std::endl;  // numedges always zero

      // ========= vertices =================
      for(size_t ivert=0; ivert<vertices.size(); ivert++) {
         carve::poly::Geometry<3>::vertex_t& vtx = vertices[ivert];
         out << std::setprecision(16) << vtx.v[0] << ' ' << std::setprecision(16) << vtx.v[1] << ' ' << std::setprecision(16) <<  vtx.v[2] << std::endl;
      }

      // ========= faces =================
      for(size_t iface = 0; iface<poly->faces.size(); ++iface) {
         carve::poly::Face<3>& face = poly->faces[iface];

         size_t nvert = face.nVertices();
         std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
         poly->faces[iface].getVertexLoop(vloop);
         out << nvert << ' ';
         for(size_t ivert=0; ivert<nvert; ivert++) {
            size_t index = poly->vertexToIndex(vloop[ivert]);
            out << index << ' ';
         }
         out << std::endl;
      }
   }

   m_files_written.insert(path);
   return path;
}


std::string out_triangles::write_obj(const std::string& xcsg_path)
{
   boost::filesystem::path fullpath(xcsg_path);
   boost::filesystem::path csg_path = fullpath.parent_path() / fullpath.stem();
   std::string path = csg_path.string() + ".obj";
   std::replace(path.begin(),path.end(), '\\', '/');
   std::ofstream out(path);
   std::string object_id = fullpath.stem().string();

   out << "# OBJ file created by xcsg : " << path << std::endl;
   out  << "o " << object_id << std::endl;

   // ========= vertices =================
   for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {

      std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
      std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

      for(size_t ivert=0; ivert<vertices.size(); ivert++) {
         carve::poly::Geometry<3>::vertex_t& vtx = vertices[ivert];
         out << "v " << std::setprecision(16) << vtx.v[0] << ' ' << std::setprecision(16) << vtx.v[1] << ' ' << std::setprecision(16) <<  vtx.v[2] << std::endl;
      }
   }

   // ========= faces =================
   size_t vertex_offset = 0;
   for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {

      std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
      std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

      for(size_t iface = 0; iface<poly->faces.size(); ++iface) {
         carve::poly::Face<3>& face = poly->faces[iface];

         size_t nvert = face.nVertices();
         std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
         poly->faces[iface].getVertexLoop(vloop);
         out << "f ";
         for(size_t ivert=0; ivert<nvert; ivert++) {
            size_t index = poly->vertexToIndex(vloop[ivert]);

            // indices are 1-based in OBJ
            out << vertex_offset + 1+index << ' ';
         }
         out << std::endl;
      }

      vertex_offset += vertices.size();
   }
   m_files_written.insert(path);
   return path;
}


std::string  out_triangles::write_stl_ascii(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path amf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = amf_path.string() + ".stl";
   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   std::ofstream out(path);
   if(out.is_open()) {

      out << "solid xcsg " << std::endl;

      for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {
         std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
         std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

         size_t nfaces = poly->faces.size();
         for(size_t iface=0; iface< nfaces; iface++) {

            size_t nv = poly->faces[iface].nVertices();
            if(nv != 3)throw std::logic_error("out_triangles:: detected non-triangular face after triangulation!");

            typedef carve::geom::vector<3> vec3d;
            std::vector<vec3d> p;
            p.reserve(3);
            std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
            poly->faces[iface].getVertexLoop(vloop);
            for(size_t ivert=0; ivert<nv; ivert++) {
               size_t index = poly->vertexToIndex(vloop[ivert]);
               carve::poly::Geometry<3>::vertex_t& vtx = vertices[index];
               p.push_back(carve::geom::VECTOR(vtx.v[0],vtx.v[1],vtx.v[2]));
            }

            vec3d x = p[1] - p[0];
            vec3d y = p[2] - p[0];
            vec3d z = carve::geom::cross(x,y);
            double len = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);

            // facet normal does not require high precision, it is usually ignored, so we save some space instead
            if(len > 0) out << "facet normal "
                            << std::setprecision(8) << z[0]/len << ' '
                            << std::setprecision(8) << z[1]/len << ' '
                            << std::setprecision(8) << z[2]/len << std::endl;
            else        out << "facet normal 0 0 0" << std::endl;


            out << "\touter loop" << std::endl;
            for(size_t iv=0;iv<nv;iv++) {

               out << "\t\tvertex "<< std::setprecision(16) << p[iv].x <<
                               ' ' << std::setprecision(16) << p[iv].y <<
                               ' ' << std::setprecision(16) << p[iv].z << std::endl;
            }
            out << "\tendloop" << std::endl;
            out << "endfacet" << std::endl;
         }
      }
      out << "endsolid" << std::endl;
   }
   else {
      std::string message = "stl_io::write_ascii(...)  Failed to open: " + file_path;
      throw std::logic_error(message);
   }
   m_files_written.insert(path);
   return path;
}


std::string  out_triangles::write_stl_binary(const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path stl_path = fullpath.parent_path() / fullpath.stem();
   std::string path = stl_path.string() + ".stl";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   if(FILE* stl = std::fopen(path.c_str(),"wb")) {

      // write the header
      const size_t blen=80;
      char buffer[blen];
      for(size_t i=0; i<blen; i++) buffer[i]=' ';
      std::fwrite(buffer,sizeof(char),blen,stl);

      // write number of triangles
      uint32_t ntri=0;
      for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {
         std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
         ntri += static_cast<uint32_t>(poly->faces.size());
      }
      std::fwrite(&ntri,sizeof(uint32_t),1,stl);

      for(size_t ipoly=0; ipoly<m_polyset->size(); ipoly++) {

         std::shared_ptr<carve::poly::Polyhedron> poly = (*m_polyset)[ipoly];
         std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

         size_t nfaces = poly->faces.size();
         for(size_t iface=0; iface < nfaces; iface++) {

            size_t nv = poly->faces[iface].nVertices();
            if(nv != 3)throw std::logic_error("out_triangles:: detected non-triangular face after triangulation!");

            typedef carve::geom::vector<3> vec3d;
            std::vector<vec3d> p;
            p.reserve(3);
            std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
            poly->faces[iface].getVertexLoop(vloop);
            for(size_t ivert=0; ivert<nv; ivert++) {
               size_t index = poly->vertexToIndex(vloop[ivert]);
               carve::poly::Geometry<3>::vertex_t& vtx = vertices[index];
               p.push_back(carve::geom::VECTOR(vtx.v[0],vtx.v[1],vtx.v[2]));
            }

            // compute facet normal
            vec3d x = p[1] - p[0];
            vec3d y = p[2] - p[0];
            vec3d z = carve::geom::cross(x,y);
            double len = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);

            // we write regardless of area here, because we didn't check the areas when we computed the number of triangles
            float xyz[] = {static_cast<float>(z[0]/len), static_cast<float>(z[1]/len),  static_cast<float>(z[2]/len) };
            std::fwrite(xyz,sizeof(float),3,stl);

            for(size_t iv=0;iv<nv;iv++) {
               float xyz[] = {static_cast<float>(p[iv].x), static_cast<float>(p[iv].y),  static_cast<float>(p[iv].z) };
               std::fwrite(xyz,sizeof(float),3,stl);
            }

            // write the attribute byte count value
            uint16_t bcount = 0;
            std::fwrite(&bcount, sizeof(bcount), 1, stl);
         }
      }

      std::fclose(stl);
   }
   else {
      std::string message = "out_triangles::write_stl_binary(...)  Failed to open: " + file_path;
      throw std::logic_error(message);
   }
   m_files_written.insert(path);
   return path;
}

std::set<std::string> out_triangles::copy_to(const std::string& dir_path)
{
   namespace bfs = boost::filesystem;

   std::string target_dir = dir_path;

   // if the input path contains a file name, extract only the directory path
   std_filename target_fname(target_dir);
   if(target_fname.GetExt() != "") {
      target_dir = target_fname.GetPath();
   }

   // make sure the target directory exists
   if(!std_filename::Exists(target_dir)) {
      std_filename::create_directories(target_dir);
   }

   // traverse the files to be copied
   std::set<std::string> files_copied;
   for(auto& file : m_files_written) {
      std_filename source_file(file);
      std_filename target_file(file);
      target_file.SetPath(target_dir);
      bfs::copy_file(source_file.GetFullPath(),target_file.GetFullPath(),bfs::copy_option::overwrite_if_exists);

      if(std_filename::Exists(target_file.GetFullPath())) {
         std::string copied_path = target_file.GetFullPath();
         std::replace(copied_path.begin(),copied_path.end(), '\\', '/');
         files_copied.insert(copied_path);
      }
   }

   return std::move(files_copied);
}
