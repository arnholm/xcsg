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

#include "extrude_mesh.h"
#include "mesh_utils.h"
#include "sweep_mesh.h"
#include "sweep_path_linear.h"
#include "sweep_path_rotate.h"
#include "sweep_path_transform.h"
#include "sweep_path_spline.h"
#include "clipper_csg/dmesh_adapter.h"
#include "clipper_csg/tmesh_adapter.h"
#include "carve/mesh_simplify.hpp"

static const double pi = 4.0*atan(1.0);

double extrude_mesh::evaluate_max_x(std::shared_ptr<carve::mesh::MeshSet<3>> meshset)
{
   double max_x = -1.0;
   size_t nvert =  meshset->vertex_storage.size();
   for(size_t i=0;i<nvert;i++) {
      carve::mesh::MeshSet<3>::vertex_t& vertex = meshset->vertex_storage[i];
      max_x = (vertex.v[0] > max_x)? vertex.v[0] : max_x;
   }
   return max_x;
}

std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::linear_extrude(std::shared_ptr<clipper_profile> profile, double h, const carve::math::Matrix& t)
{
   // here, we "simply" perform a linear sweep

   std::shared_ptr<polyset2d> polyset = profile->polyset();
   double maxlen = mesh_utils::maxlen_factor()*polyset->greatest_extent();

 //  dmesh_adapter tess(maxlen);
   tmesh_adapter tess;
   tess.tesselate(polyset);

   std::shared_ptr<sweep_path_linear>  path(new sweep_path_linear(tess.mesh(),h));
   std::shared_ptr<xpolyhedron> poly = sweep_mesh(path,false).polyhedron();
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = poly->create_carve_mesh(t);


   carve::mesh::MeshSimplifier simplifier;
   double min_normal_angle=(pi/180.)*1E-4;  // 1E-4 degrees
   simplifier.mergeCoplanarFaces(meshset.get(),min_normal_angle);

   return meshset;
}


std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::rotate_extrude(std::shared_ptr<clipper_profile> profile, double angle, double pitch, const carve::math::Matrix& t)
{
   // when a complete rotation is specified with no pitch, the generated topology will be different
   // we signal this with the torus flag, and set the angle to 2*pi
   bool torus = ((fabs(angle) < 2*pi) || (fabs(pitch) > 0))? false : true;
   if(torus) {
      angle = -2*pi;
      cout << "...Info: rotate_extrude angle>=2*PI implies a torus" << endl;
   }

   std::shared_ptr<polyset2d> polyset = profile->polyset();
   double maxlen = mesh_utils::maxlen_factor()*polyset->greatest_extent();

   // first tesselate the 2d mesh
  // dmesh_adapter tess(maxlen);
   tmesh_adapter tess;
   tess.tesselate(polyset);

   // then use the 2d mesh as basis for sweep
   std::shared_ptr<sweep_path_rotate>  path(new sweep_path_rotate(tess.mesh(),angle,pitch));

   // extract the resulting polyhedron and turn it into a carve mesh
   std::shared_ptr<xpolyhedron> poly = sweep_mesh(path, torus).polyhedron();
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = poly->create_carve_mesh(t);

   // cleanup
   carve::mesh::MeshSimplifier simplifier;
   double min_normal_angle=(pi/180.)*1E-4;  // 1E-4 degrees
   simplifier.mergeCoplanarFaces(meshset.get(),min_normal_angle);

   return meshset;
}

std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::transform_extrude(const carve::math::Matrix& t_bot,         // bottom profile transform
                                                                         std::shared_ptr<clipper_profile> bottom,  // bottom profile
                                                                         const carve::math::Matrix& t_top,         // top profile transform
                                                                         std::shared_ptr<clipper_profile> top,     // top profile
                                                                         const carve::math::Matrix& t)             // final solid transform
{
   std::shared_ptr<polyset2d> pset_bot = bottom->polyset();
   std::shared_ptr<polyset2d> pset_top = top->polyset();

   if((pset_bot->size() != 1) || (pset_top->size() != 1)) {
      throw logic_error("disjoint profiles not supported for 'transform_extrude' ");
   }

   double maxlen_bot = mesh_utils::maxlen_factor()*pset_bot->greatest_extent();
   double maxlen_top = mesh_utils::maxlen_factor()*pset_top->greatest_extent();

   // bottom and top at polygons (possibly with holes)
   std::shared_ptr<polygon2d> poly_bot = *(pset_bot->begin());
   std::shared_ptr<polygon2d> poly_top = *(pset_top->begin());

   if( poly_bot->size() != poly_top->size() ) {
      throw logic_error("profiles not compatible for 'transform_extrude' - must have same number of contours");
   }

   // ok, make the polygons topologically compatible
   double epspnt = mesh_utils::secant_tolerance();
   polygon2d::make_compatible(*poly_bot,*poly_top,epspnt);

//   dmesh_adapter tess_bot(maxlen_bot),tess_top(maxlen_top);
   tmesh_adapter tess_bot,tess_top;
   tess_bot.tesselate(pset_bot);
   tess_top.tesselate(pset_top);

   std::shared_ptr<sweep_path_transform>  path(new sweep_path_transform(t_bot, tess_bot.mesh(),t_top,tess_top.mesh()));
   std::shared_ptr<xpolyhedron> poly = sweep_mesh(path,false).polyhedron();
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = poly->create_carve_mesh(t);

   carve::mesh::MeshSimplifier simplifier;
   double min_normal_angle=(pi/180.)*1E-4;  // 1E-4 degrees
   simplifier.mergeCoplanarFaces(meshset.get(),min_normal_angle);

   return meshset;
}

std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::sweep_extrude(std::shared_ptr<clipper_profile> profile,
                                                                     std::shared_ptr<const csplines::spline_path> spath,
                                                                     const carve::math::Matrix& t)
{
   std::shared_ptr<polyset2d> polyset = profile->polyset();
   double maxlen = mesh_utils::maxlen_factor()*polyset->greatest_extent();

   // tesselate the profile
//   dmesh_adapter tess(maxlen);
   tmesh_adapter tess;
   tess.tesselate(polyset);

   // use the 2d mesh as basis for sweep
   int nseg = -1;
   std::shared_ptr<sweep_path_spline>  path(new sweep_path_spline(tess.mesh(),spath,nseg));
   std::shared_ptr<xpolyhedron> poly = sweep_mesh(path,false).polyhedron();
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = poly->create_carve_mesh(t);

   carve::mesh::MeshSimplifier simplifier;
   double min_normal_angle=(pi/180.)*1E-4;  // 1E-4 degrees
   simplifier.mergeCoplanarFaces(meshset.get(),min_normal_angle);

   return meshset;
}

std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::clone_extrude(std::shared_ptr<carve::mesh::MeshSet<3>> meshset, const carve::math::Matrix& t)
{
   std::shared_ptr<carve::mesh::MeshSet<3>> mesh(meshset->clone());

   size_t nvert =  mesh->vertex_storage.size();
   for(size_t i=0;i<nvert;i++) {
      carve::mesh::MeshSet<3>::vertex_t& vertex = mesh->vertex_storage[i];
      if(vertex.v[2] > 0.0) {
         vertex = t * carve::geom::VECTOR(vertex.v[0],vertex.v[1],vertex.v[2]);
      }
   }
   return mesh;
}


std::shared_ptr<carve::mesh::MeshSet<3>> extrude_mesh::clone_transform(std::shared_ptr<carve::mesh::MeshSet<3>> meshset, const carve::math::Matrix& t)
{
   std::shared_ptr<carve::mesh::MeshSet<3>> mesh(meshset->clone());

   size_t nvert =  mesh->vertex_storage.size();
   for(size_t i=0;i<nvert;i++) {
      carve::mesh::MeshSet<3>::vertex_t& vertex = mesh->vertex_storage[i];
      vertex = t * carve::geom::VECTOR(vertex.v[0],vertex.v[1],vertex.v[2]);
   }
   return mesh;
}

