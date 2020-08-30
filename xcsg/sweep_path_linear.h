// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017-2020 Carsten Arnholm
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

#ifndef SWEEP_PATH_LINEAR_H
#define SWEEP_PATH_LINEAR_H

#include "sweep_path.h"
#include "clipper_csg/polymesh2d.h"

// sweep_path_linear describes a linear sweep path, aka. linear extrude

class sweep_path_linear : public sweep_path {
public:
   sweep_path_linear(std::shared_ptr<const polymesh2d> pm2d, double h);
   virtual ~sweep_path_linear();

   // return the transformed profile at parameter p=[0,1] in the form of a polymesh3d
   virtual std::shared_ptr<const polymesh3d> profile(double p) const;

   // return the number of segments for the parameter range [0,1]
   virtual size_t nseg() const;

private:
   std::shared_ptr<const polymesh2d> m_pm2d;
   double                            m_h;
};

#endif // SWEEP_PATH_LINEAR_H
