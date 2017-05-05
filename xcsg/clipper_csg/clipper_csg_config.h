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
   
#ifndef CLIPPER_CSG_CONFIG_H_INCLUDED
#define CLIPPER_CSG_CONFIG_H_INCLUDED

#include "clipper.hpp"
const ClipperLib::cInt TO_CLIPPER = (1 << 16);

#ifdef _MSC_VER

   /// MSVC compiler

   #ifdef CLIPPER_CSG_IMPLEMENTATION
      #define CLIPPER_CSG_PUBLIC
      #define CLIPPER_CSG_EXTERN
   #else
      #define CLIPPER_CSG_PUBLIC
      #define CLIPPER_CSG_EXTERN
   #endif
   #define CLIPPER_CSG_PRIVATE

#elif __GNUC__

   /// gnu gcc

   #define CLIPPER_CSG_PUBLIC
   #define CLIPPER_CSG_PRIVATE
   #define CLIPPER_CSG_EXTERN

#else

  #error "Unknown compiler"

#endif


#endif // CLIPPER_CSG_CONFIG_H_INCLUDED

