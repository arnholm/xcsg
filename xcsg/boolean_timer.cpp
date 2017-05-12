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

#include "boolean_timer.h"
#include <iostream>
#include <iomanip>

boolean_timer::boolean_timer()
{}

boolean_timer::~boolean_timer()
{}

void boolean_timer::init(int nbool)
{
   m_nbool = 0;
   m_nbool_tot = (nbool>0)? nbool : 1;
   m_progress = 0;
   m_progress_report = 0;
}

void boolean_timer::add_elapsed(double esec)
{
   int millisec        = static_cast<int>(1000*esec);
   m_elapsed_millisec += millisec;
   m_nbool++;
   m_progress = (1000.0*m_nbool)/m_nbool_tot;

   // report progress at every 5% progress
   if( (m_progress - m_progress_report) >= 50) {
      size_t p = m_progress;
      m_progress_report = p;

      double percent = m_progress*0.1;
      std::cout << std::setprecision(3) << "...boolean progress: " << percent <<"% " << std::endl;
   }
}


double boolean_timer::thread_elapsed()
{
   return m_elapsed_millisec*0.001;
}
