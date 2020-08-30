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

#ifndef XSHAPE2D_COLLECTOR_H
#define XSHAPE2D_COLLECTOR_H

#include <set>
#include <vector>
#include <memory>
#include "xshape2d.h"
#include "csg_parser/cf_xmlNode.h"

// xshape2d_collector is a helper class for collecting child shape2d nodes from XML

class xshape2d_collector {
public:
   typedef std::unordered_set<std::shared_ptr<xshape2d>> ShapeSet;
   typedef std::vector<std::shared_ptr<xshape2d>> ShapeVector;

   // collect all children into A
   static void collect_children(const cf_xmlNode& parent, ShapeSet& A);

   // collect nA first children into A, the rest into B
   static void collect_children(const cf_xmlNode& parent, ShapeSet& A, size_t nA, ShapeSet& B);

   // collect all children into A
   static void collect_children(const cf_xmlNode& parent, ShapeVector& A);
};

#endif // XSHAPE2D_COLLECTOR_H
