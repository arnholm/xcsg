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

#include "xshape2d_collector.h"
#include "xcsg_factory.h"

void xshape2d_collector::collect_children(const cf_xmlNode& parent, ShapeSet& A)
{
   size_t icount = 0;
   cf_xmlNode tmp(parent);
   for(auto i=tmp.begin(); i!=tmp.end(); i++) {
      cf_xmlNode sub(i);
      if(xcsg_factory::singleton().is_shape2d(sub)) {
         A.insert(xcsg_factory::singleton().make_shape2d(sub));
         icount++;
      }
   }

   if(icount == 0) {
      throw logic_error("Expected 2d shape under " + parent.tag() + ", but found none.");
   }
}

void xshape2d_collector::collect_children(const cf_xmlNode& parent, ShapeSet& A, size_t nA, ShapeSet& B)
{
   size_t icount=0;
   cf_xmlNode tmp(parent);
   for(auto i=tmp.begin(); i!=tmp.end(); i++) {
      cf_xmlNode sub(i);
      if(xcsg_factory::singleton().is_shape2d(sub)) {
         if(icount < nA) {
            A.insert(xcsg_factory::singleton().make_shape2d(sub));
         }
         else {
            B.insert(xcsg_factory::singleton().make_shape2d(sub));
         }
         icount++;
      }
   }

   if(icount == 0) {
      throw logic_error("Expected 2d shape under " + parent.tag() + ", but found none.");
   }
}

void xshape2d_collector::collect_children(const cf_xmlNode& parent, ShapeVector& A)
{
   size_t icount=0;
   cf_xmlNode tmp(parent);
   for(auto i=tmp.begin(); i!=tmp.end(); i++) {
      cf_xmlNode sub(i);
      if(xcsg_factory::singleton().is_shape2d(sub)) {
         A.push_back(xcsg_factory::singleton().make_shape2d(sub));
          icount++;
      }
   }
   if(icount == 0) {
      throw logic_error("Expected 2d shape under " + parent.tag() + ", but found none.");
   }
}
