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

// include xshape2d so that xvertex is defines
#include "xshape2d.h"

#include "project_mesh.h"
#include "primitives2d.h"

#include <carve/poly.hpp>
#include "carve_boolean.h"
#include "carve_triangulate.h"

#include "clipper_boolean.h"
#include "xpolyhedron.h"

#include <iostream>
using namespace std;

std::shared_ptr<clipper_profile> project_mesh::project(std::shared_ptr<carve::mesh::MeshSet<3>> meshset)
{
   clipper_boolean csg_clipper;

   carve_boolean csg_carve;
   csg_carve.compute(meshset,carve::csg::CSG::OP::UNION);
   size_t nmani = csg_carve.size();

   size_t npoly = 0;

   // The mesh may contain non-triangular faces, so we must triangulate into polyhedra
   for(size_t imani=0; imani<nmani; imani++) {

      cout << "manifold " << imani << " of " << nmani << endl;

      // create polyset with triangles from manifold
      carve_triangulate triangulate;
      std::shared_ptr<xpolyhedron> poly = csg_carve.create_manifold(imani);

      size_t num_non_tri = 0;
      poly->check_polyhedron(cout,num_non_tri);

      if(num_non_tri > 0) {
         cout << "...Triangulating lump ... " << std::endl;
         cout << "...Triangulation completed with " << triangulate.compute2d(poly->create_carve_polyhedron())<< " triangle faces " << endl;;
      }
      else {
         // triangulation not required
         triangulate.add(poly->create_carve_polyhedron());
      }

      cout << "...Computing projection" << std::endl;

      // extract triangle faces
      std::shared_ptr<carve_triangulate::poly_vector>  polyset = triangulate.carve_polyset();
      for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {

         std::shared_ptr<carve::poly::Polyhedron> poly = (*polyset)[ipoly];
         std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

         for(size_t iface = 0; iface<poly->faces.size(); ++iface) {

            // coordinates of face to be stored in this vector
            std::vector<xvertex> face_coords;

            carve::poly::Face<3>& face = poly->faces[iface];
            size_t nvert = face.nVertices();
            std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
            face.getVertexLoop(vloop);
            for(size_t ivert=0; ivert<nvert; ivert++) {
               size_t index = poly->vertexToIndex(vloop[ivert]);
               carve::poly::Geometry<3>::vertex_t& vertex = vertices[index];
               face_coords.push_back(vertex.v);
            }

            // we now have the triangle face coordinates.
            std::shared_ptr<polygon2d> polygon = primitives2d::make_polygon(face_coords);

            // here, there is always just a single contour
            std::shared_ptr<contour2d> cont = polygon->get_contour(0);

            // skip any triangles with zero area, i.e. face was perpendicular to XY plane.
            // we also consider only triangles with positive winding order. The ones with negative winding
            // are always hidden by those with positive winding order (CCW) and can safely be ignored
            double signed_area = cont->signed_area();
            if(signed_area > 0.0) {

               // turn triangle into a clipper profile and add its paths to the projection (a single path)
               std::shared_ptr<clipper_profile> poly_prof = std::make_shared<clipper_profile>();
               poly_prof->AddPaths(polygon->paths());
               csg_clipper.compute(poly_prof,ClipperLib::ctUnion);

               npoly++;
            }
         }
      }
   }

   cout << "...Projection computed from " << npoly << " triangles." << endl;

   // make sure paths are sorted with positive path first
   csg_clipper.sort();

   // return completed projection profile
   return csg_clipper.profile();

}

/*
std::shared_ptr<clipper_profile> project_mesh::project(std::shared_ptr<carve::mesh::MeshSet<3>> meshset)
{
   size_t nmesh = meshset->meshes.size();

   // vertices are shared between the meshes in the mesh set
   // const std::vector<carve::mesh::MeshSet<3>::vertex_t>& vertex_storage = meshset->vertex_storage;

   clipper_boolean csg;

   // traverse meshes in mesh set
   for(size_t imesh=0; imesh<nmesh; imesh++) {

      carve::mesh::Mesh<3>* mesh = meshset->meshes[imesh];

      // traverse faces of this mesh and convert them into polygon2d
      size_t nfaces = mesh->faces.size();
      for (size_t iface=0; iface<nfaces; iface++) {

         // coordinates of face to be stored in this vector
         std::vector<xvertex> face_coords;
         face_coords.reserve(24);  // triangular faces mostly

         // traverse face vertices and extract vertex coordinates
         carve::mesh::Face<3>* face = mesh->faces[iface];
         std::vector<carve::mesh::Face<3>::vertex_t*> verts;
         face->getVertices(verts);
         set<carve::mesh::Vertex<3>*> vseen;
         carve::mesh::Face<3>::vertex_t* vprev=0;
         for(size_t i=0;i<verts.size();i++) {
            carve::mesh::Face<3>::vertex_t* vertex = verts[i];

            cout << " face " << iface << " vertex " << i << " " << vertex
                             << " " << vertex->v[0] << " " << vertex->v[1] << " " << vertex->v[2]
                             << endl;

            if(vseen.find(vertex) == vseen.end()) {
               face_coords.push_back(vertex->v);
               vseen.insert(vertex);
            }
            else {
            //   cout << " face " << iface << " dup vertex " << i << " of " << verts.size()  << " " << vertex << endl;
            }
            vprev = vertex;
         }

         // create 2d polygon
         // This causes z coordinate to be dropped, i.e. projection to XY plane
         std::shared_ptr<polygon2d> poly = primitives2d::make_polygon(face_coords);
         std::shared_ptr<contour2d> cont = poly->get_contour(0);

         // skip any polygons with zero area, i.e. face was perpendicular to XY plane
         double signed_area = cont->signed_area();

         if(fabs(signed_area) > 0.0) {

            // make sure winding order is CCW
            if(signed_area < 0.0) cont->reverse();

            // turn polygon into a clipper profile and add its paths to the projection using clipper
            std::shared_ptr<clipper_profile> poly_prof = std::make_shared<clipper_profile>();
            poly_prof->AddPaths(poly->paths());
            csg.compute(poly_prof,ClipperLib::ctUnion);
         }
      }
   }

   // make sure paths are sorted with positive path first
   csg.sort();

   // return completed projection profile
   return csg.profile();
}

*/
