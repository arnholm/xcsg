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

#include "sweep_path_linear.h"

sweep_path_linear::sweep_path_linear(std::shared_ptr<const polymesh2d> pm2d, double h)
: m_pm2d(pm2d)
, m_h(h)
{}

sweep_path_linear::~sweep_path_linear()
{}

std::shared_ptr<const polymesh3d> sweep_path_linear::profile(double p) const
{
   carve::math::Matrix t = carve::math::Matrix::TRANS(0.0,0.0,p*m_h);
   return std::shared_ptr<const polymesh3d>(new polymesh3d(m_pm2d,t));
}

size_t sweep_path_linear::nseg() const
{
   return 1;
}
