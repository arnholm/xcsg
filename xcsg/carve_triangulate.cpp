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

#include "carve_triangulate.h"
#include <carve/tree.hpp>
#include <carve/csg_triangulator.hpp>

#include "carve_triangulate_face.h"
#include <forward_list>

// #include <boost/filesystem.hpp>
// #include <boost/filesystem/convenience.hpp>

carve_triangulate::carve_triangulate()
: m_polyset( new poly_vector )
{}

carve_triangulate::~carve_triangulate()
{}

/*
static double polyhedron_edge_length(const carve::poly::Polyhedron::vertex_t* v0,
                                     const carve::poly::Polyhedron::vertex_t* v1)
{
   double dx = v1->v[0] - v0->v[0];
   double dy = v1->v[1] - v0->v[1];
   double dz = v1->v[2] - v0->v[2];
   return sqrt(dx*dx + dy*dy + dz*dz);
   return 0;
}

static double polyhedron_face_area( const carve::poly::Polyhedron::vertex_t* va,
                                    const carve::poly::Polyhedron::vertex_t* vb,
                                    const carve::poly::Polyhedron::vertex_t* vc)
{
   // https://en.wikipedia.org/wiki/Heron's_formula

   double a = polyhedron_edge_length(va,vb);
   double b = polyhedron_edge_length(vb,vc);
   double c = polyhedron_edge_length(vc,va);
   double s = 0.5*(a+b+c);
   return sqrt(s*(s-a)*(s-b)*(s-c));
}
*/
static double polyhedron_face_area( const carve::poly::Polyhedron::vertex_t* va,
                                    const carve::poly::Polyhedron::vertex_t* vb,
                                    const carve::poly::Polyhedron::vertex_t* vc)
{
   typedef carve::geom::vector<3> vec3d;

   vec3d x = vb->v - va->v;
   vec3d y = vc->v - va->v;
   vec3d normal = carve::geom::cross(x,y);
   return 0.5*normal.length();
}

size_t carve_triangulate::compute(std::shared_ptr<carve::poly::Polyhedron> poly, bool improve, bool canonicalize, bool degen_check)
{
   // copy all vertices from onput polyhedron
   std::vector<carve::poly::Vertex<3> > out_vertices = poly->vertices;

   // storage for triangulated vertices
   std::vector<carve::poly::Face<3> > out_faces;

   // compute number of output faces
   size_t N = 0;
   for (size_t i = 0; i < poly->faces.size(); ++i) {
      carve::poly::Face<3> &f = poly->faces[i];
      N += f.nVertices() - 2;
   }
   out_faces.reserve(N);

   // triangulate each face
   size_t nzero_dropped = 0;
   for(size_t i = 0; i < poly->faces.size(); ++i) {
      carve::poly::Face<3> &f = poly->faces[i];
      std::vector<carve::triangulate::tri_idx> result;

      std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
      f.getVertexLoop(vloop);

      carve::triangulate::triangulate(carve::poly::p2_adapt_project<3>(f.project), vloop, result);
      if(improve && vloop.size()>3) {
         carve::triangulate::improve(carve::poly::p2_adapt_project<3>(f.project), vloop, result);
      }

      // add triangle faces to output
      for (size_t j = 0; j < result.size(); ++j) {

         // check the area of the triangulated face, ignore if degenerate
        double face_area = (degen_check)? polyhedron_face_area( vloop[result[j].a], vloop[result[j].b], vloop[result[j].c] ) : 1.0;

        if(0 <  face_area ) {
            out_faces.push_back(carve::poly::Face<3>(
               &out_vertices[poly->vertexToIndex_fast(vloop[result[j].a])],
               &out_vertices[poly->vertexToIndex_fast(vloop[result[j].b])],
               &out_vertices[poly->vertexToIndex_fast(vloop[result[j].c])]
            ));
        }
        else {
           nzero_dropped++;
        }
      }
   }
   if(nzero_dropped>0) std::cout << ">>> Warning: dropped "<<nzero_dropped <<" zero area triangles(s) during triangulation." << std::endl;

   // create the new polyhedron
   std::shared_ptr<carve::poly::Polyhedron> poly_triangle(new carve::poly::Polyhedron(out_faces, out_vertices));
   if(canonicalize) {
      poly_triangle->canonicalize();
   }

   // add to the vector of triangulated polyhedrons
   m_polyset->push_back(poly_triangle);

   // return number of faces after triangulation
   return poly_triangle->faces.size();
}

void carve_triangulate::add(std::shared_ptr<carve::poly::Polyhedron> poly)
{
   m_polyset->push_back(poly);
}

size_t carve_triangulate::compute2(std::shared_ptr<carve::poly::Polyhedron> poly)
{
   // the number of vertices shall remain unchanged
//   const std::vector<carve::poly::Vertex<3>>& vertices = poly->vertices;

   // triangulated faces
   std::forward_list<carve::poly::Face<3> > tri_faces;
   // unfortunately, std::forward_list has no size() member, so we must keep track of the face count ourselves
   size_t tri_faces_size = 0;

   // Traverse the input faces
   for (size_t i = 0; i < poly->faces.size(); ++i) {
      carve::poly::Face<3>& f = poly->faces[i];
      size_t nv  = f.nVertices();
      if(nv == 3) {
         tri_faces.push_front(f);
         tri_faces_size++;
      }
      else if(nv == 4) {

         // just split the face into 2 triangles
         std::vector<const carve::poly::Vertex<3> *> vloop;
         f.getVertexLoop(vloop);
         std::vector<const carve::poly::Vertex<3> *> tri_vloop1 =  { vloop[0], vloop[1], vloop[2]};
         std::vector<const carve::poly::Vertex<3> *> tri_vloop2 =  { vloop[0], vloop[2], vloop[3]};
         tri_faces.push_front(carve::poly::Face<3>(tri_vloop1));
         tri_faces.push_front(carve::poly::Face<3>(tri_vloop2));
         tri_faces_size += 2;

      }
      else if(nv > 4){
         // 5 or more vertices, this face must be triangulated

         // convert the face vertex loop to vector of vertex indices: vind refers to poly->vertices
         // and also compute vector of projected 2d coordinates for the same face vertices
         std::vector<size_t> vind;
         std::vector<carve::geom2d::P2> vxy;

         // get the face vertex loop
         std::vector<const carve::poly::Vertex<3> *> vloop;
         f.getVertexLoop(vloop);

         // obtain the face vertex indices and
         // project 3d vertex coordinates into 2d
         carve::poly::p2_adapt_project<3> projector(f.project);
         vind.reserve(nv);
         vind.reserve(nv);
         for(size_t iv=0; iv<nv; iv++) {
            vind.push_back(poly->vertexToIndex_fast(vloop[iv]));
            vxy.push_back(projector(vloop[iv]));
         }

         // use the face triangulator
         carve_triangulate_face triangulator;
         std::vector<std::vector<size_t>> tri_vinds = triangulator.compute(vind,vxy);

         for(auto& tri_vind : tri_vinds) {
           std::vector<const carve::poly::Vertex<3> *> tri_vloop =  { &poly->vertices[tri_vind[0]], &poly->vertices[tri_vind[1]], &poly->vertices[tri_vind[2]] };
           tri_faces.push_front(carve::poly::Face<3>(tri_vloop));
           tri_faces_size++;
         }
      }
   }

   // create the triangulated polyhedron
   std::vector<carve::poly::Face<3> > faces;
   faces.reserve(tri_faces_size);
   for(auto& f : tri_faces) {
      faces.push_back(f);
   }

   std::shared_ptr<carve::poly::Polyhedron> poly_triangle(new carve::poly::Polyhedron(faces, poly->vertices));

   // add to the vector of triangulated polyhedrons
   m_polyset->push_back(poly_triangle);

   // return number of faces after triangulation
   return poly_triangle->faces.size();
}

