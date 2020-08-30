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

#ifndef DXF_FILE_H
#define DXF_FILE_H

#include <memory>
#include <string>
#include <fstream>
class polyset2d;
class contour2d;

class dxf_file {
public:
   dxf_file();
   virtual ~dxf_file();

   // export to DXF, return the path to the file created
   // input is full path to file, file extension will be replaced to ".dxf"
   std::string  write( std::shared_ptr<polyset2d> polyset, const std::string& file_path);

protected:
   void write_item(int gc, const std::string& value);
   void write_item(int gc, double value);
   void write_item(int gc, int value);
   void write_lwpolyline(std::shared_ptr<contour2d> contour);

private:
   std::ofstream m_out;
};

#endif // DXF_FILE_H
