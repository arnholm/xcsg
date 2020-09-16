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

#include "xsweep.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xspline_path.h"
#include "xshape2d.h"
#include "extrude_mesh.h"
#include "clipper_boolean.h"
#include "xshape2d_collector.h"

xsweep::xsweep()
{}

xsweep::xsweep(const cf_xmlNode& const_node)
{
   set_transform(const_node);

   cf_xmlNode node = const_node;
   xshape2d_collector::collect_children(node,m_incl);

   for(auto i=node.begin(); i!=node.end(); i++) {

      cf_xmlNode sub(i);
      if(sub.tag()=="spline_path") {
         if(!m_path.get()) m_path = std::shared_ptr<xspline_path>(new xspline_path(sub));
         else throw logic_error("xsweep: More than one spline_path specified.");
      }
   }

   if(m_incl.size()==0) throw logic_error("xsweep: No sweep profile specified.");
   if(!m_path.get()) throw logic_error("xsweep: No sweep path specified.");
}

xsweep::~xsweep()
{}


std::shared_ptr<carve::mesh::MeshSet<3>> xsweep::create_carve_mesh(const carve::math::Matrix& t) const
{
   // create profile in native 2d system
   clipper_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      // create profile in native 2d system
      csg.compute((*i)->create_clipper_profile(carve::math::Matrix()),ClipperLib::ctUnion);
   }


   // apply 3d transformation when creating 3d mesh
   std::shared_ptr<const csplines::spline_path> spline(new csplines::spline_path(m_path->cp()));

   return  extrude_mesh::sweep_extrude(csg.profile(),spline,t*get_transform());
}


size_t xsweep::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}
