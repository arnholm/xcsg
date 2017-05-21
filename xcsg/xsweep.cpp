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

#include "xsweep.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xspline_path.h"
#include "xshape2d.h"
#include "extrude_mesh.h"
#include "clipper_boolean.h"

xsweep::xsweep()
{}

xsweep::xsweep(const cf_xmlNode& const_node)
{
   set_transform(const_node);

   cf_xmlNode node = const_node;
   for(auto i=node.begin(); i!=node.end(); i++) {

      cf_xmlNode sub(i);
      if(!m_profile.get() && xcsg_factory::singleton().is_shape2d(sub)) {
         m_profile = xcsg_factory::singleton().make_shape2d(sub);
      }
      else if(!m_path.get() && sub.tag()=="spline_path") {
         m_path = std::shared_ptr<xspline_path>(new xspline_path(sub));
      }
   }

   if(!m_profile.get()) throw logic_error("xsweep: No sweep profile specified.");
   if(!m_path.get()) throw logic_error("xsweep: No sweep path specified.");
}

xsweep::~xsweep()
{}


std::shared_ptr<carve::mesh::MeshSet<3>> xsweep::create_carve_mesh(const carve::math::Matrix& t) const
{
   // create profile in native 2d system
   clipper_boolean csg;
   csg.compute(m_profile->create_clipper_profile(carve::math::Matrix()),ClipperLib::ctUnion);

   // apply 3d transformation when creating 3d mesh
   std::shared_ptr<const csplines::spline_path> spline(new csplines::spline_path(m_path->cp()));

   return  extrude_mesh::sweep_extrude(csg.profile(),spline,t*get_transform());
}
