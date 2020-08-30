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

#include "xpolyhedron.h"
#include <carve/polyhedron_decl.hpp>
#include <carve/input.hpp>
#include "qhull/qhull3d.h"
#include "carve_boolean.h"
#include "csg_parser/cf_xmlNode.h"
#include "mesh_utils.h"
#include <map>

/*
static double face_area(const std::vector<xvertex>& p)
{
   // https://en.wikipedia.org/wiki/Heron's_formula
   double a = (p[1] - p[0]).length();
   double b = (p[2] - p[1]).length();
   double c = (p[0] - p[2]).length();
   double s = 0.5*(a+b+c);
   return sqrt(s*(s-a)*(s-b)*(s-c));
}
*/

static double face_area(const std::vector<xvertex>& p)
{
   double area = 0.0;

   typedef carve::geom::vector<3> vec3d;
   if(p.size() == 3) {

      // triangle
      vec3d x = p[1] - p[0];
      vec3d y = p[2] - p[0];
      vec3d normal = carve::geom::cross(x,y);
      area = 0.5*normal.length();
   }
   else {

      // general polygon
      vec3d normal = carve::geom::VECTOR(0.0,0.0,0.0);
      size_t n = p.size();
      vec3d b  = p[n-2];
      vec3d c  = p[n-1];
      for(size_t i=0; i<n; i++ ) {

         vec3d a = b;
         b       = c;
         c       = p[i];

         double dx = b.y * (c.z - a.z);
         double dy = b.z * (c.x - a.x);
         double dz = b.x * (c.y - a.y);

         normal += carve::geom::VECTOR(dx,dy,dz);
      }
      area = 0.5*normal.length();
   }
   return area;
}

xpolyhedron::xpolyhedron()
{}

xpolyhedron::~xpolyhedron()
{}

xpolyhedron::xpolyhedron(const xpolyhedron& other)
: m_vertices(other.m_vertices)
, m_faces(other.m_faces)
{}

xpolyhedron::xpolyhedron(const cf_xmlNode& const_node)
{
    if(const_node.tag() != "polyhedron")throw logic_error("Expected xml tag polyhedron, but found " + const_node.tag());

    set_transform(const_node);

    cf_xmlNode node = const_node;
    for(auto i=node.begin(); i!=node.end(); i++) {

       cf_xmlNode sub(i);
       if(!sub.is_attribute_node()) {
          if("vertices" == sub.tag()) {

             // be sure to sort the vertices
             map<size_t,xvertex> v;
             for(auto iv=sub.begin(); iv!=sub.end(); iv++) {

                // get next vertex
                cf_xmlNode vertex(iv);
                if("vertex" == vertex.tag()) {
                   size_t id = v.size();
                   v[id]= carve::geom::VECTOR( vertex.get_property("x",0.0),
                                               vertex.get_property("y",0.0),
                                               vertex.get_property("z",0.0));
                }
                else {
                   throw logic_error("polyhedron: expected tag 'vertex' but found " + vertex.tag());
                }
             }

             // copy to member variable
             m_vertices.resize(v.size());
             for(auto i=v.begin(); i!=v.end(); i++) {
                m_vertices[i->first] = i->second;
             }
          }
          else if("faces" == sub.tag()) {

             map<int,xface> f;
             int id = -1;
             for(auto iv=sub.begin(); iv!=sub.end(); iv++) {
                // get next face
                cf_xmlNode face(iv);
                if(!face.is_attribute_node()) {
                   if("face" == face.tag()) {
                      f[++id] = xface(face);
                   }
                   else {
                      throw logic_error("polyhedron: expected tag 'face' but found " + face.tag());
                   }
                }
             }

             // copy to member variable
             m_faces.resize(f.size());
             for(auto i=f.begin(); i!=f.end(); i++) {
                m_faces[i->first] = i->second;
             }
          }
          else if("tmatrix" == sub.tag()) {
             // skip this, already handled via set_transform(...)
          }
          else {
             throw logic_error("polyhedron: expected one of 'tmatrix', 'vertices' or 'faces' but found " + sub.tag());
          }
       }
    }
}

std::shared_ptr<carve::mesh::MeshSet<3>> xpolyhedron::create_carve_mesh(const carve::math::Matrix& t) const
{
   bool reverse_face = mesh_utils::is_left_hand(t);

   carve::input::PolyhedronData data;
   carve::input::Options options;

   if(m_faces.size() > 0) {

      // conventional polyhedron
      data.reserveVertices(static_cast<int>(m_vertices.size()));
      for(size_t i=0; i<m_vertices.size(); i++) {
         data.addVertex(t*get_transform()*m_vertices[i]);
      }

      data.reserveFaces(static_cast<int>(m_faces.size()),4);
      for(size_t i=0; i<m_faces.size(); i++) {
         const xface& face = m_faces[i];
         if(reverse_face) data.addFace(face.rbegin(),face.rend());
         else             data.addFace(face.begin(),face.end());
      }
   }
   else if(m_vertices.size() > 2) {

      // No faces specified, but several vertices
      // Create instead a convex hull polyhedron, using only the vertices
      qhull3d qhull;
      for(size_t i=0; i<m_vertices.size(); i++) {
         xvertex vertex = t*get_transform()*m_vertices[i];
         qhull.push_back(vertex.v[0],vertex.v[1],vertex.v[2]);
      }

      carve_boolean csg;
      csg.compute(qhull);
      return csg.mesh_set();
   }

   return std::shared_ptr<carve::mesh::MeshSet<3>>(data.createMesh(options));
}

void xpolyhedron::v_reserve(size_t nverts)
{
   m_vertices.reserve(nverts);
}

size_t xpolyhedron::v_add(const xvertex& pos)
{
   size_t index = m_vertices.size();
   m_vertices.push_back(pos);
   return index;
}

size_t xpolyhedron::v_size() const
{
   return m_vertices.size();
}

const xvertex& xpolyhedron::v_get(size_t v_ind) const
{
   return m_vertices.at(v_ind);
}

void xpolyhedron::f_reserve(size_t nfaces)
{
   m_faces.reserve(nfaces);
}

size_t xpolyhedron::f_add(const xface& face, bool reverse_face)
{
   size_t index = m_faces.size();
   if(reverse_face) m_faces.push_back(face.reverse_copy());
   else             m_faces.push_back(face);
   return index;
}

size_t xpolyhedron::f_size() const
{
   return m_faces.size();
}

const xface& xpolyhedron::f_get(size_t f_ind) const
{
   return m_faces.at(f_ind);
}

bool  xpolyhedron::check_polyhedron(ostream& out, size_t& num_non_tri)
{
   map<size_t,size_t> edge_count;
   size_t face_error=0;

   num_non_tri = 0;

   for(auto i=m_faces.begin(); i!=m_faces.end(); i++) {
      const xface& face = *i;

      std::vector<xvertex> p;
      p.reserve(face.size());
      for(size_t i=0; i<face.size(); i++) {
         p.push_back(m_vertices[face[i]]);
      }
      if(!(face_area(p)>0.0))face_error++;

      num_non_tri += (face.size()!=3)? 1 : 0;

      // number of edges == number of vertices
      size_t nedge = face.size();
      size_t last_edge = nedge-1;
      for(size_t iedge=0; iedge<nedge; iedge++) {
         size_t iv0 = face[iedge];
         size_t iv1 = (iedge<last_edge)? face[iedge+1] : face[0];
         size_t edge_id = std::min(iv0,iv1)*1000000 + std::max(iv0,iv1);
         edge_count[edge_id]++;
      }
   }

   size_t nerr = 0;
   size_t nc1  = 0;
   for(auto i=edge_count.begin(); i!=edge_count.end(); i++) {
      if(i->second != 2) {
         nerr++;
         if(i->second == 1)nc1++;
      }
   }

   if(nerr == 0) {
      out << "...Polyhedron is water-tight (edge use-count check OK)" << endl;
   }
   else {
      out << ">>> Warning: Polyhedron is not water-tight, it has " << edge_count.size() << " edges, " << nerr << " with wrong use count, " << nc1 << " with use-count==1" << endl;
   }
   if(face_error == 0) {
      out << "...Polyhedron has no degenerated faces (face area check OK)" << endl;
   }
   else {
      out << ">>> Warning: Polyhedron has " << face_error << " zero area faces." << endl;
   }

   out << "...Polyhedron has "<< num_non_tri << " non-triangular faces" << endl;
   return ((face_error+nerr)==0);
}


std::shared_ptr<carve::poly::Polyhedron> xpolyhedron::create_carve_polyhedron()
{
   carve::input::PolyhedronData data;
   data.points = m_vertices;
   data.reserveFaces(static_cast<int>(m_faces.size()),4);
   for(auto i=m_faces.begin(); i!=m_faces.end(); i++) {
      const xface& face = *i;
      data.addFace(face.begin(),face.end());
   }
   carve::input::Options options;
   return std::shared_ptr<carve::poly::Polyhedron>(data.create(options));
}
