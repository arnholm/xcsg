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
   
#ifndef BOOLEAN_TIMER_H
#define BOOLEAN_TIMER_H


class boolean_timer {
public:
   static boolean_timer& singleton()  { static boolean_timer instance; return instance;  }

   // call init before starting booleans, provide estimated number of booleans
   void init(int nbool);

   // measure time in each boolean and add elapsed seconds by calling add_elapsed
   void add_elapsed(double esec);

   // return total elapsed so far
   double elapsed();

protected:
   boolean_timer();
   virtual ~boolean_timer();

private:
   double m_elapsed_sec;    // total elapsed time added
   double m_elapsed_report; // last reported elapsed time
   int    m_nbool_tot;      // total number of booleans
   int    m_nbool;          // number of booleans processed so far
};

#endif // BOOLEAN_TIMER_H
