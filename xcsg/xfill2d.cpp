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

#include "cf_xmlNode.h"

#include "xfill2d.h"

#include "xcsg_factory.h"
#include "xpolygon.h"
#include "clipper_boolean.h"
#include "xshape2d_collector.h"


xfill2d ::xfill2d ()
{}

xfill2d::xfill2d (const cf_xmlNode& node)
{
   if(node.tag() != "fill2d")throw std::logic_error("Expected xml tag fill2d, but found " + node.tag());
   set_transform(node);
   xshape2d_collector::collect_children(node,m_incl);
}


xfill2d ::~xfill2d ()
{}

size_t xfill2d::nbool()
{
   return 1;
}

std::shared_ptr<clipper_profile> xfill2d ::create_clipper_profile(const carve::math::Matrix& t) const
{
   // traverse underlying objects, fill holes and union the result
   clipper_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<clipper_profile> profile = (*i)->create_clipper_profile(t*get_transform());

      // split the profile into single path profiles with only positive winding order
      std::list<std::shared_ptr<clipper_profile>> profiles;
      profile->positive_profiles(profiles);

      // union the profiles to obtain a single profile again
      // the effect is that all holes dissapear, but outer contours remain
      for(auto& p : profiles) {
         csg.compute(p,ClipperLib::ctUnion);
      }
   }
   return csg.profile();
}

std::shared_ptr<carve::mesh::MeshSet<3>> xfill2d ::create_carve_mesh(const carve::math::Matrix& t) const
{
   return 0;
}


