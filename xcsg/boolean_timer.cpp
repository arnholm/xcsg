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
   m_elapsed_sec = 0.0;
   m_elapsed_report = 0.0;
   m_nbool = 0;
   m_nbool_tot = (nbool>0)? nbool : 1;
}

void boolean_timer::add_elapsed(double esec)
{
   m_elapsed_sec += esec;
   m_nbool++;

   // report only of more than 2 seconds have passed since previous report
   if( (m_elapsed_sec - m_elapsed_report) > 2.0) {
      double percent = (100.0*m_nbool)/m_nbool_tot;
      std::cout << "xcsg booleans: " << std::setprecision(3) << percent <<"% , time used: "
                                     << std::setprecision(3) << m_elapsed_sec << " [s], "
                                     << std::setprecision(3) << m_elapsed_sec/60.0 << " [min]"
                                     << std::endl;
      m_elapsed_report = m_elapsed_sec;
   }
}

double boolean_timer::elapsed()
{
   return m_elapsed_sec;
}
