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

#include "boolean_timer.h"
#include <boost/date_time.hpp>

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
      boost::posix_time::ptime p1 = boost::posix_time::microsec_clock::universal_time();

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
      boost::posix_time::time_duration  ptime_diff = boost::posix_time::microsec_clock::universal_time() - p1;
      double elapsed_sec = 0.001*ptime_diff.total_milliseconds();

      boolean_timer::singleton().add_elapsed(elapsed_sec);
   }
   return success;
}

bool clipper_boolean::minkowski_sum(std::shared_ptr<clipper_profile> a, std::shared_ptr<clipper_profile> b_brush )
{
   ClipperLib::Clipper clipper;
   ClipperLib::Paths& a_paths = a->paths();
   ClipperLib::Paths& b_paths = b_brush->paths();
   if(b_paths.size() != 1) {
      throw std::logic_error("clipper_boolean::minkowski_sum, 'b' parameter must contain exactly one path");
   }

   boost::posix_time::ptime p1 = boost::posix_time::microsec_clock::universal_time();

   ClipperLib::Path& pattern = b_paths[0];
   std::shared_ptr<clipper_profile> result(new clipper_profile);
   bool pathIsClosed = true;
   ClipperLib::MinkowskiSum(pattern,a_paths,result->paths(),pathIsClosed);
   bool success = result->paths().size() > 0;
   if(success) {
      ClipperLib::CleanPolygons(result->paths());
      m_profile = result;
   }

   boost::posix_time::time_duration  ptime_diff = boost::posix_time::microsec_clock::universal_time() - p1;
   double elapsed_sec = 0.001*ptime_diff.total_milliseconds();

   boolean_timer::singleton().add_elapsed(elapsed_sec);

   return success;
}

std::shared_ptr<clipper_profile> clipper_boolean::profile()
{
   return m_profile;
}
