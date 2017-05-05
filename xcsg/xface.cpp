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
   
#include "xface.h"
#include <map>

xface::xface(cf_xmlNode& face)
{
   std::map<size_t,size_t> v;
   int max_id=-1;
   for(auto i=face.begin(); i!=face.end(); i++) {
      cf_xmlNode fv(i);
      if(!fv.is_attribute_node()) {
         if("fv" == fv.tag()) {
            size_t id = v.size();
            v[id] = fv.get_property("index",-1);
         }
         else {
            throw logic_error("face: expected 'fv' but found " + fv.tag());
         }
      }
   }

   // copy to member variable
   m_indices.resize(v.size());
   for(auto i=v.begin(); i!=v.end(); i++) {
      m_indices[i->first] = i->second;
   }
}

xface xface::reverse_copy() const
{
   xface the_copy(*this);
   return the_copy.in_place_reverse();
}

xface& xface::in_place_reverse()
{
   std::reverse(m_indices.begin(),m_indices.end());
   return *this;
}
