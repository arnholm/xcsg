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
   
#ifndef DMESH_ADAPTER_H
#define DMESH_ADAPTER_H

#include "polyset2d.h"
#include "polymesh2d.h"
class dmesh;

// dmesh_adapter (replaces libtess2d) manages meshing of 2d polygons and stores the result in a polymesh2d

class dmesh_adapter {
public:
   dmesh_adapter();
   virtual ~dmesh_adapter();

   // tesselate all polygons in the polyset into the same mesh
   bool tesselate(std::shared_ptr<polyset2d> polyset);

   // return the contained mesh
   std::shared_ptr<polymesh2d> mesh() { return m_mesh; }

protected:

   // add conour of single polygon
   bool add_contour(std::shared_ptr<polygon2d> poly, dmesh& mesher);

private:
   std::shared_ptr<polymesh2d> m_mesh;
};

#endif // DMESH_ADAPTER_H
