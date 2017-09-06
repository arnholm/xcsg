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

#include "clipper_profile.h"

clipper_profile::clipper_profile()
{}

clipper_profile::~clipper_profile()
{}

ClipperLib::Paths& clipper_profile::paths()
{
   return m_paths;
}

void clipper_profile::AddPaths(std::shared_ptr<ClipperLib::Paths> paths)
{
   m_paths.reserve(m_paths.size()+paths->size());
   for(size_t i=0;i<paths->size(); i++) {
      m_paths.push_back((*paths)[i]);
   }
}

std::shared_ptr<polyset2d> clipper_profile::polyset()
{
   std::shared_ptr<polyset2d> pset(new polyset2d());

   std::shared_ptr<polygon2d> polygon;
   for(size_t i=0; i<m_paths.size(); i++) {
      bool positive = Orientation(m_paths[i]);
      if(positive) {
         polygon = std::shared_ptr<polygon2d>(new polygon2d());
         pset->insert(polygon);
      }
      if(!polygon.get()) {
         throw std::logic_error("polyset2d error: 1st polygon2d contour was negative??");
      }
      polygon->push_back(std::shared_ptr<contour2d>(new contour2d(m_paths[i])));
   }
   return pset;
}


void clipper_profile::fill_holes()
{
   // to fill the holes, we simply discard the negative paths
   ClipperLib::Paths filled_paths;
   filled_paths.reserve(m_paths.size());
   for(size_t i=0; i<m_paths.size(); i++) {
      bool positive = Orientation(m_paths[i]);
      if(positive) {
         filled_paths.push_back(m_paths[i]);
      }
   }
   m_paths = filled_paths;
}
