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

#include "xshape2d.h"
#include "csg_parser/cf_xmlNode.h"
#include "xtmatrix.h"

xshape2d::xshape2d()
{}

xshape2d::~xshape2d()
{}

void xshape2d::set_transform(const cf_xmlNode& parent)
{
   cf_xmlNode tm_node;
   if(parent.get_child("tmatrix",tm_node)) {
      xtmatrix tm(tm_node);
      m_t = tm.t();
   }
}


const carve::math::Matrix& xshape2d::get_transform() const
{
   return m_t;
}

