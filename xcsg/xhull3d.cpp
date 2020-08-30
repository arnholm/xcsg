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

#include "xhull3d.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "carve_boolean.h"
#include "qhull/qhull3d.h"
#include "xsolid_collector.h"

xhull3d::xhull3d()
{}

xhull3d::xhull3d(const cf_xmlNode& node)
{
   if(node.tag() != "hull3d")throw logic_error("Expected xml tag hull3d, but found " + node.tag());
   set_transform(node);
   xsolid_collector::collect_children(node,m_incl);
}


xhull3d::~xhull3d()
{}

size_t xhull3d::nbool()
{
   return 1;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xhull3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   qhull3d qhull;

   // accumulate vertices of underlying objects
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<carve::mesh::MeshSet<3>> meshset = (*i)->create_carve_mesh(t*get_transform());
      size_t nvert =  meshset->vertex_storage.size();
      qhull.reserve(qhull.nvertices()+nvert);
      for(size_t i=0;i<nvert;i++) {
         carve::mesh::MeshSet<3>::vertex_t& vertex = meshset->vertex_storage[i];
         qhull.push_back(vertex.v[0],vertex.v[1],vertex.v[2]);
	//	   cout << "vertex " <<  vertex.v[0] << "  " <<  vertex.v[1] << "  " << vertex.v[2] << endl;
      }
   }

   carve_boolean csg;
  // cout << " xhull3d:: csg.compute(qhull)" << endl;
   csg.compute(qhull);
  // cout << " xhull3d:: csg.compute(qhull) OK " << csg.mesh_set()->vertex_storage.size() << " " << csg.mesh_set()->meshes.size()  << endl;
   return csg.mesh_set();
}


