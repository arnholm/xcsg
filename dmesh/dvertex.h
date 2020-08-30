// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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

#ifndef DVERTEX_H
#define DVERTEX_H

#include "dentity.h"
#include "dpos2d.h"

// Topology
// ========
// a dvertex is a topological point with reference to a geometrical position.

class dvertex  {
public:
   friend class dmesh;

   // return vertex position
   const dpos2d& pos() const { return m_pos; }

protected:

   dvertex(const dpos2d& pos);
   virtual ~dvertex();

   // change the vertex position
   void set_pos(const dpos2d& pos);

private:
   dpos2d m_pos;
};

#endif // DVERTEX_H
