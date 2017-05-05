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
   
#include "sweep_path_transform.h"

sweep_path_transform::sweep_path_transform(const carve::math::Matrix& t_bot, std::shared_ptr<const polymesh2d> bot,
                                           const carve::math::Matrix& t_top, std::shared_ptr<const polymesh2d> top)
: m_t_bot(t_bot)
, m_bot(bot)
, m_t_top(t_top)
, m_top(top)
{}

sweep_path_transform::~sweep_path_transform()
{}

std::shared_ptr<const polymesh3d> sweep_path_transform::profile(double p) const
{
   if(p>0.0){
     return std::shared_ptr<const polymesh3d>(new polymesh3d(m_top,m_t_top));
   }
   return std::shared_ptr<const polymesh3d>(new polymesh3d(m_bot,m_t_bot));
}

size_t sweep_path_transform::nseg() const
{
   return 1;
}
