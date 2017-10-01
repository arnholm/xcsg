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

#ifndef DPATH_H
#define DPATH_H

#include <cstddef>
#include <list>
#include <vector>
#include "dentity.h"
#include "dtriangle.h"
#include "dpos2d.h"
#include <unordered_set>
class dcoedge;
class dedge;

// Topology
// ========
// A dloop is is an ordered sequence of cedges defining a single geometric contour, a closed loop.
// The coedge directions shall be consistent through the loop. The underlying edges may be directed opposite.
// The coedge forward directions shall be consistent with the ordered sequence of cedges.
// Outer loops shall be sequenced CCW, as seen from +Z. This correponds to positive signed area.
// Inner loops shall be sequenced CW, as seen from +Z.  This correponds to negative signed area.

class dloop : public dentity {
public:
   friend class dprofile;

   typedef std::list<dcoedge*>::const_iterator coedge_iterator;
   typedef std::list<dcoedge*>::const_reverse_iterator coedge_reverse_iterator;

   // traversal of coedges in forward loop sequence
   coedge_iterator begin() const { return m_coedges.begin(); }
   coedge_iterator end() const   { return m_coedges.end(); }

   // traversal of coedges in reverse loop sequence
   coedge_reverse_iterator rbegin() const { return m_coedges.rbegin(); }
   coedge_reverse_iterator rend() const   { return m_coedges.rend(); }

   // number of coedges
   size_t size() const     { return m_coedges.size(); }

   // compute the signed area of the loop, applying "the material is always to the left" in both cases
   // loops surrounding material have positive areas (CCW)
   // loops surrounding holes have negative areas (CW)
   double signed_area() const;

   // return all loop points as a vector of positions, optionally in reverse order
   std::vector<dpos2d> get_pos(bool fwd) const;

   // delete all contained coedges
   void clear();

   // split the coedge in 2 new ones and return the split vertex index
   size_t split_coedge(dcoedge* coedge);

   // collect any triangles connected to edge if edge intersects this loop
   void collect_overlapping_triangles(dedge* edge, std::unordered_set<dtriangle*>& overlapping);

protected:
   dloop(dmesh* mesh);
   virtual ~dloop();

   void push_back(dcoedge* coedge);

private:
   std::list<dcoedge*> m_coedges;
};

#endif // DPATH_H
