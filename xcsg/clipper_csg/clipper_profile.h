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

#ifndef CLIPPER_PROFILE_H
#define CLIPPER_PROFILE_H

#include "clipper_csg_config.h"
#include "clipper.hpp"
#include "polyset2d.h"
#include <list>

// A clipper_profile represents the result of successive 2d booleans
// It can represent any complex 2d profile, possibly multiple polygons with holes.

class clipper_profile {
public:
   clipper_profile();
   virtual ~clipper_profile();

   // add a number of paths to the profile, following the rules of Clipper
   void AddPaths( std::shared_ptr<ClipperLib::Paths> paths);

   // return access to the Clipper paths
   ClipperLib::Paths& paths();

   // return a set of polygons for this profile
   std::shared_ptr<polyset2d> polyset();

   // split this profile into a number of single contour profiles containing only positive winding order paths
   // negative winding order paths are discareded
   void positive_profiles(std::list<std::shared_ptr<clipper_profile>>& profiles );

protected:
   void AddPath(  const ClipperLib::Path& path);

private:
   ClipperLib::Paths m_paths;
};

#endif // CLIPPER_PROFILE_H
