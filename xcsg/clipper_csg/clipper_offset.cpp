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

#include "clipper_offset.h"
#include <iostream>
using namespace std;

clipper_offset::clipper_offset()
{}

clipper_offset::~clipper_offset()
{}


bool clipper_offset::compute(std::shared_ptr<clipper_profile> profile, double delta, bool round, bool chamfer)
{
//   cout <<  "clipper_offset delta="<<delta<<",  round="<<round <<",  chamfer=" << chamfer << endl;

   if(round) {
      return compute(profile,delta,ClipperLib::jtRound);
   }
   else {
      if(chamfer) {
         return compute(profile,delta,ClipperLib::jtSquare);
      }
      else {
         return compute(profile,delta,ClipperLib::jtMiter);
      }
   }
}

// compute offset, store resut in member (input also affected)
bool clipper_offset::compute(std::shared_ptr<clipper_profile> profile, double delta, ClipperLib::JoinType op)
{
   double miterLimit = 1000000;
   ClipperLib::ClipperOffset offset(miterLimit);
   offset.ArcTolerance = 0.005* TO_CLIPPER;
   offset.AddPaths(profile->paths(),op,ClipperLib::etClosedPolygon);
   std::shared_ptr<clipper_profile> result(new clipper_profile);
   offset.Execute(result->paths(), delta * TO_CLIPPER);
   ClipperLib::CleanPolygons(result->paths());
   m_profile = result;

   return true;
}

// return the current profile
std::shared_ptr<clipper_profile> clipper_offset::profile()
{
   return m_profile;
}
