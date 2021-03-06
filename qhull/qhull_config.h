// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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

#ifndef QHULL_CONFIG_H_INCLUDED
#define QHULL_CONFIG_H_INCLUDED

#ifdef _MSC_VER

   /// MSVC compiler

   #ifdef QHULL_IMPLEMENTATION
      #define QHULL_PUBLIC
      #define QHULL_EXTERN
   #else
      #define QHULL_PUBLIC
      #define QHULL_EXTERN
   #endif
   #define QHULL_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define QHULL_PUBLIC
   #define QHULL_PRIVATE
   #define QHULL_EXTERN

#else

  #error "Unknown compiler"

#endif


#endif // QHULL_CONFIG_H_INCLUDED

