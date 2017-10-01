// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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

#ifndef DPROFILE_H
#define DPROFILE_H

#include <ostream>

class dmesh;
#include "dbox2d.h"
#include "dloop.h"
#include <list>

// Topology
// ========
// a dprofile represents 2d profile for a given dmesh. Usually, the dprofile is owned/managed by the dmesh it refers to.
// The profile is described by a collection of positive and negative loops representing outer and inner loops respectively.

class dprofile : public dentity {
public:
   friend class dmesh;

   typedef std::list<dloop*>::const_iterator const_iterator;

   // return number of loops in profile
   size_t size() const { return m_loops.size(); }

   // traversal of loops
   const_iterator begin() const { return m_loops.begin(); }
   const_iterator end() const   { return m_loops.end(); }

   // sort loops according to absolute area
   void sort_loops();

   // number of vertices referenced by profile
   size_t vertex_size() const;

   // clear all loops in the profile.
   // Note that the edges referred to are not deleted even if the use count drops to zero
   void clear();

   // export the profile in .p2d format.
   void export_p2d(std::ostream& out) const;

   // import the profile from .p2d format
   void import_p2d(std::istream& in, bool move_to_1st_quadrant);

   // return the profile bounding box
   dbox2d bounding_box() const;

protected:
   dprofile(dmesh* mesh);
   virtual ~dprofile();

   // add a loop to the profile
   void add_loop(const std::vector<dpos2d>& points);

   // compute profile from mesh containing triangles
   // NOTE: this may throw std::exception if the model is ambigous, call in try/catch clause
   void compute(dmesh* mesh);

private:
   std::list<dloop*> m_loops;
};

#endif // DPROFILE_H
