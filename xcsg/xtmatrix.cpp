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
   
#include "xtmatrix.h"
#include "cf_xmlNode.h"

xtmatrix::xtmatrix()
{}

xtmatrix::xtmatrix(carve::math::Matrix& t)
: m_t(t)
{}

xtmatrix::xtmatrix(cf_xmlNode& node)
{
   int irow = 0;
   for(auto i=node.begin();i!=node.end();i++) {
      cf_xmlNode row(i);
      if(row.tag() == "trow") {
         if(irow < 4) {
            m_t.m[0][irow] = row.get_property("c0",0.0);
            m_t.m[1][irow] = row.get_property("c1",0.0);
            m_t.m[2][irow] = row.get_property("c2",0.0);
            m_t.m[3][irow] = row.get_property("c3",0.0);
            irow++;
         }
      }
   }
}

xtmatrix::~xtmatrix()
{}

const carve::math::Matrix& xtmatrix::t() const
{
   return m_t;
}
