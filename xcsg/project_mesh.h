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


#ifndef PROJECT_MESH_H
#define PROJECT_MESH_H

#include <carve/mesh.hpp>
#include "clipper_csg/clipper_profile.h"

// project 3d to 2d

class project_mesh {
public:
   static std::shared_ptr<clipper_profile> project(std::shared_ptr<carve::mesh::MeshSet<3>> mesh);
};

#endif // PROJECT_MESH_H
