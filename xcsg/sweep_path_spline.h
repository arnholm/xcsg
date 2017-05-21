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

#ifndef SWEEP_PATH_SPLINE_H
#define SWEEP_PATH_SPLINE_H

#include "sweep_path.h"
#include "clipper_csg/polymesh2d.h"
#include "csplines/spline_path.h"

class sweep_path_spline : public sweep_path {
public:
   sweep_path_spline(std::shared_ptr<const polymesh2d> pm2d, std::shared_ptr<const csplines::spline_path> path, int nseg=-1);
   virtual ~sweep_path_spline();

   // return the transformed profile at parameter p=[0,1] in the form of a polymesh3d
   virtual std::shared_ptr<const polymesh3d> profile(double p) const;

   // return the number of segments for the parameter range [0,1]
   virtual size_t nseg() const;

protected:

private:
   std::shared_ptr<const polymesh2d>            m_pm2d;
   std::shared_ptr<const csplines::spline_path> m_path;
   int                                          m_nseg;   // number of sweep segments
};

#endif // SWEEP_PATH_SPLINE_H
