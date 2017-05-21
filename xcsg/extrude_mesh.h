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

#ifndef EXTRUDE_MESH_H
#define EXTRUDE_MESH_H

#include <memory>
#include <carve/mesh.hpp>
#include "clipper_csg/clipper_profile.h"
#include <carve/matrix.hpp>
#include "csplines/spline_path.h"

// extrude 2d to 3d

class extrude_mesh {
public:
   // returns a linear 3d extrusion of the 2d input profile
   static std::shared_ptr<carve::mesh::MeshSet<3>> linear_extrude(std::shared_ptr<clipper_profile> profile, double h, const carve::math::Matrix& t);

   // returns a rotated 3d extrusion of the 2d input profile
   static std::shared_ptr<carve::mesh::MeshSet<3>> rotate_extrude(std::shared_ptr<clipper_profile> profile, double angle, double pitch, const carve::math::Matrix& t);

   // returns a transformed extrusion from bottom to top
   static std::shared_ptr<carve::mesh::MeshSet<3>> transform_extrude(const carve::math::Matrix& t_bot,         // bottom profile transform
                                                                     std::shared_ptr<clipper_profile> bottom,  // bottom profile
                                                                     const carve::math::Matrix& t_top,         // top profile transform
                                                                     std::shared_ptr<clipper_profile> top,     // top profile
                                                                     const carve::math::Matrix& t);            // final solid transform

   static std::shared_ptr<carve::mesh::MeshSet<3>> sweep_extrude(std::shared_ptr<clipper_profile> profile,
                                                                 std::shared_ptr<const csplines::spline_path> path,
                                                                 const carve::math::Matrix& t);

   static double evaluate_max_x(std::shared_ptr<carve::mesh::MeshSet<3>> meshset);

   // returns an extruded clone of the input mesh, i.e. transform only the 2nd level of vertices
   static std::shared_ptr<carve::mesh::MeshSet<3>> clone_extrude(std::shared_ptr<carve::mesh::MeshSet<3>> meshset, const carve::math::Matrix& t);

   // transforms a clone of the input mesh, i.e. transform all vertices
   static std::shared_ptr<carve::mesh::MeshSet<3>> clone_transform(std::shared_ptr<carve::mesh::MeshSet<3>> meshset, const carve::math::Matrix& t);

};

#endif // EXTRUDE_MESH_H
