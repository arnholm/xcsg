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

#include "xlinear_extrude.h"
#include "carve_boolean.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "extrude_mesh.h"
#include "clipper_boolean.h"
#include "xshape2d_collector.h"

xlinear_extrude::xlinear_extrude(double dz)
: m_dz(dz)
{}

xlinear_extrude::~xlinear_extrude()
{}

xlinear_extrude::xlinear_extrude(const cf_xmlNode& node)
: m_dz(1.0)
{
   if(node.tag() != "linear_extrude")throw logic_error("Expected xml tag 'linear_extrude', but found " + node.tag());
   set_transform(node);
   m_dz  = node.get_property("dz",1.0);

   xshape2d_collector::collect_children(node,m_incl);
}

std::shared_ptr<carve::mesh::MeshSet<3>> xlinear_extrude::create_carve_mesh(const carve::math::Matrix& t) const
{
   clipper_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      // create profile in native 2d system
      csg.compute((*i)->create_clipper_profile(carve::math::Matrix()),ClipperLib::ctUnion);
   }

   // apply 3d transformation when creating 3d mesh
   return  extrude_mesh::linear_extrude(csg.profile(),m_dz,t*get_transform());
}


size_t xlinear_extrude::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}
