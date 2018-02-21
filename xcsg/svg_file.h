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

#ifndef SVG_FILE_H
#define SVG_FILE_H

#include <memory>
#include <string>
#include <fstream>
#include "dmesh/dpos2d.h"
class polyset2d;
class contour2d;
class dbox2d;

class svg_file {
public:
   svg_file();
   virtual ~svg_file();

   // export to SVG, return the path to the file created
   // input is full path to file, file extension will be replaced to ".svg"
   std::string  write( std::shared_ptr<polyset2d> polyset, const std::string& file_path);

private:
   dpos2d to_svg(const dpos2d& p, const dbox2d& box);

   // convert the contour into an SVG path sequence
   std::string write_contour(std::shared_ptr<contour2d> contour, const dbox2d& box);
};

#endif // SVG_FILE_H
