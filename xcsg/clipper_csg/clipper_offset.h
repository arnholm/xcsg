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
   
#ifndef CLIPPER_OFFSET_H
#define CLIPPER_OFFSET_H

#include "clipper_profile.h"
#include <memory>

class clipper_offset {
public:
   clipper_offset();
   virtual ~clipper_offset();

   bool compute(std::shared_ptr<clipper_profile> profile, double delta, bool round, bool chamfer);

   // return the offset profile
   std::shared_ptr<clipper_profile> profile();

protected:
   // compute offset, store resut in member (input not affected)
   bool compute(std::shared_ptr<clipper_profile> profile, double delta, ClipperLib::JoinType op);

private:
   std::shared_ptr<clipper_profile>  m_profile;
};

#endif // CLIPPER_OFFSET_H
