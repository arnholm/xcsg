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
   
#ifndef XCSG_MAIN_H
#define XCSG_MAIN_H

#include "boost_command_line.h"
class cf_xmlNode;

class xcsg_main {
public:
   xcsg_main(const boost_command_line& m_cmd);
   virtual ~xcsg_main();

   bool run();

protected:

   bool run_xsolid(cf_xmlNode& node,const std::string& xcsg_file);
   bool run_xshape2d(cf_xmlNode& node,const std::string& xcsg_file);

private:
   boost_command_line m_cmd;
};

#endif // XCSG_MAIN_H
