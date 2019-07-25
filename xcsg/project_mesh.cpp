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

// include xshape2d so that xvertex is defines
#include "xshape2d.h"

#include "project_mesh.h"
#include "primitives2d.h"

#include "clipper_boolean.h"
#include <iostream>
using namespace std;

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
         /*
            cout << " face " << iface << " vertex " << i << " " << vertex
                             << " " << vertex->v[0] << " " << vertex->v[1] << " " << vertex->v[2]
                             << endl;
         */
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
