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
   
#ifndef XSOLID_COLLECTOR_H
#define XSOLID_COLLECTOR_H

#include <set>
#include <memory>
#include "xsolid.h"
#include "cf_xmlNode.h"

// xsolid_collector is a helper class for collecting child solid nodes from XML

class xsolid_collector {
public:
   typedef std::unordered_set<std::shared_ptr<xsolid>> ShapeSet;

   // collect all children into A
   static void collect_children(const cf_xmlNode& parent, ShapeSet& A);

   // collect nA first children into A, the rest into B
   static void collect_children(const cf_xmlNode& parent, ShapeSet& A, size_t nA, ShapeSet& B);

};

#endif // XSOLID_COLLECTOR_H
