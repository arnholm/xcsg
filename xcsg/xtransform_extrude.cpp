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
   
#include "xtransform_extrude.h"
#include "carve_boolean.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "extrude_mesh.h"
#include <boost/lexical_cast.hpp>

#include "clipper_boolean.h"

#include "xshape2d_collector.h"

xtransform_extrude::~xtransform_extrude()
{}

xtransform_extrude::xtransform_extrude(const cf_xmlNode& node)
{
   if(node.tag() != "transform_extrude")throw logic_error("Expected xml tag 'transform_extrude', but found " + node.tag());
   set_transform(node);

   xshape2d_collector::collect_children(node,m_incl);

   if(m_incl.size() != 2) throw logic_error("exactly 2 2d_shapes required for 'transform_extrude' " + boost::lexical_cast<std::string>(m_incl.size()));
}

std::shared_ptr<carve::mesh::MeshSet<3>> xtransform_extrude::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<clipper_profile> bottom = m_incl[0]->create_clipper_profile(carve::math::Matrix());
   carve::math::Matrix t_bot               = m_incl[0]->get_transform();
   std::shared_ptr<clipper_profile> top    = m_incl[1]->create_clipper_profile(carve::math::Matrix());
   carve::math::Matrix t_top               = m_incl[1]->get_transform();

   size_t np_bot = bottom->paths().size();
   size_t np_top = top->paths().size();
   if(np_bot != np_top) {
      throw logic_error("top and bottom shapes are incompatible for 'transform_extrude', i.e. different number of paths ");
   }

   // apply 3d transformation when creating 3d mesh
   return  extrude_mesh::transform_extrude(t_bot,bottom,t_top,top,t*get_transform());
}


size_t xtransform_extrude::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}
