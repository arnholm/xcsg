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
#include <map>
#include <iostream>
using namespace std;

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

void clipper_profile::AddPath(const ClipperLib::Path& path)
{
   m_paths.push_back(path);
}

std::shared_ptr<polyset2d> clipper_profile::polyset()
{
   std::shared_ptr<polyset2d> pset(new polyset2d());

   std::shared_ptr<polygon2d> polygon;
   for(size_t i=0; i<m_paths.size(); i++) {
      bool positive = Orientation(m_paths[i]);
      if(positive) {
         polygon = std::shared_ptr<polygon2d>(new polygon2d());
         pset->push_back(polygon);
      }
      if(!polygon.get()) {
         throw std::logic_error("polyset2d error: 1st polygon2d contour was negative??");
      }
      polygon->push_back(std::shared_ptr<contour2d>(new contour2d(m_paths[i])));
   }
   return pset;
}

void clipper_profile::positive_profiles(std::list<std::shared_ptr<clipper_profile>>& profiles )
{
   for(size_t i=0; i<m_paths.size(); i++) {
      bool positive = Orientation(m_paths[i]);
      if(positive) {
         std::shared_ptr<clipper_profile> prof = std::make_shared<clipper_profile>();
         prof->AddPath(m_paths[i]);
         profiles.push_back(prof);
      }
   }
}

void clipper_profile::sort()
{
//   cout << "   DEBUG: clipper_profile::sort(), number of paths= " << m_paths.size() << endl;

   // build a sorted map of paths, largest positive areas first
   std::multimap<double,ClipperLib::Path> path_map;
   for(size_t i=0; i<m_paths.size(); i++) {
      double area = ClipperLib::Area(m_paths[i]);
      path_map.insert(std::make_pair(-area,m_paths[i]));
   }
   m_paths.clear();
   m_paths.reserve(path_map.size());
   for(auto p : path_map) m_paths.push_back(p.second);
}
