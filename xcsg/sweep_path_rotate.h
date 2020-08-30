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

#ifndef SWEEP_PATH_ROTATE_H
#define SWEEP_PATH_ROTATE_H

#include "sweep_path.h"
#include "clipper_csg/polymesh2d.h"

class sweep_path_rotate : public sweep_path {
public:
   // angle = total sweep rotation angle around Y.
   //         for pitch = 0,      angle>2*pi implies a torus)
   //         for fabs(pitch)>0,  angle>2*pi implies how many revolutions
   //
   // pitch = offset in y per revolution. Allows defining screw threads
   //
   sweep_path_rotate(std::shared_ptr<const polymesh2d> pm2d, double angle, double pitch = 0.0, int nseg = -1);
   virtual ~sweep_path_rotate();

   // return the transformed profile at parameter p=[0,1] in the form of a polymesh3d
   virtual std::shared_ptr<const polymesh3d> profile(double p) const;

   // return the number of segments for the parameter range [0,1]
   virtual size_t nseg() const;

private:
   std::shared_ptr<const polymesh2d> m_pm2d;
   double                            m_angle;  // extrusion angle
   double                            m_pitch;
   int                               m_nseg;   // number of extrusion segments
};

#endif // SWEEP_PATH_ROTATE_H
