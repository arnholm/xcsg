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
         face_coords.reserve(3);  // triangular faces mostly

         // traverse face vertices and extract vertex coordinates
         carve::mesh::Face<3>* face = mesh->faces[iface];
         std::vector<carve::mesh::Face<3>::vertex_t*> verts;
         face->getVertices(verts);
         for(size_t i=0;i<verts.size();i++) {
            carve::mesh::Face<3>::vertex_t* vertex = verts[i];
            face_coords.push_back(vertex->v);
         }

         // create 2d polygon and make sure winding order is CCW
         std::shared_ptr<polygon2d> poly = primitives2d::make_polygon(face_coords);
         std::shared_ptr<contour2d> cont = poly->get_contour(0);
         if(cont->signed_area() < 0.0) cont->reverse();

         // turn polygon into a clipper profile and add its paths to the projection using clipper
         std::shared_ptr<clipper_profile> poly_prof = std::make_shared<clipper_profile>();
         poly_prof->AddPaths(poly->paths());
         csg.compute(poly_prof,ClipperLib::ctUnion);
      }
   }

   // return completed projection profile
   return csg.profile();
}
