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
   
#ifndef SWEEP_PATH_H
#define SWEEP_PATH_H

#include <memory>
#include "polymesh3d.h"

// sweep path is the abstract base for sweeping a 2d mesh to become a 3d mesh
// The general approach taken before sweeping is
//  - perform 2d booleans to obtain    : std::shared_ptr<clipper_profile> profile = ...
//  - extract the 2d polygon set       : std::shared_ptr<polyset2d> pset = profile->polyset();
//  - create the 2d mesh using dmesh   : std::shared_ptr<polymesh2d> m2d = tess->mesh();
// Then pass the polymesh2d to the actual sweep_path class, such as e.g. sweep_path_linear

class sweep_path {
public:
   sweep_path();
   virtual ~sweep_path();

   // return the transformed profile at parameter p=[0,1] in the form of a polymesh3d
   virtual std::shared_ptr<const polymesh3d> profile(double p) const = 0;

   // return the number of segments for the parameter range [0,1]
   virtual size_t nseg() const = 0;
};

#endif // SWEEP_PATH_H
