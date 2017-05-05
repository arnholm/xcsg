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
   
#include "clipper_boolean.h"

#include <boost/timer.hpp>
#include "boolean_timer.h"

clipper_boolean::clipper_boolean()
{}

clipper_boolean::~clipper_boolean()
{}


bool clipper_boolean::compute(std::shared_ptr<clipper_profile> b, ClipperLib::ClipType op)
{
   bool success = true;
   if(!m_profile.get()) {
      m_profile = b;
   }
   else {
      boost::timer timer;
      success = false;
      ClipperLib::Clipper clipper;
      clipper.AddPaths(m_profile->paths(),ClipperLib::ptSubject,true);
      clipper.AddPaths(b->paths(),ClipperLib::ptClip,true);
      std::shared_ptr<clipper_profile> result(new clipper_profile);
      success = clipper.Execute(op, result->paths(), ClipperLib::pftNonZero, ClipperLib::pftNonZero);
      if(success) {
         ClipperLib::CleanPolygons(result->paths());
         m_profile = result;
      }
      boolean_timer::singleton().add_elapsed(timer.elapsed());
   }
   return success;
}

std::shared_ptr<clipper_profile> clipper_boolean::profile()
{
   return m_profile;
}
