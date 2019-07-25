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

#ifndef CLIPPER_BOOLEAN_H
#define CLIPPER_BOOLEAN_H

#include "clipper_csg/clipper_profile.h"
#include <memory>

class clipper_boolean {
public:
   clipper_boolean();
   virtual ~clipper_boolean();

   // compute boolean against current mesh using a MeshSet as "b"
   bool compute(std::shared_ptr<clipper_profile> b, ClipperLib::ClipType op);

   // sort contained profile paths according to area, with positive areas first
   void sort();

   // return the current profile
   std::shared_ptr<clipper_profile> profile();

   // compute minkowski sum of a and b_brush
   // a is assumed to be the main object and "b_brush" is "brushed" along the a path
   bool minkowski_sum(std::shared_ptr<clipper_profile> a, std::shared_ptr<clipper_profile> b_brush );

private:
   std::shared_ptr<clipper_profile>  m_profile;
};

#endif // CLIPPER_BOOLEAN_H
