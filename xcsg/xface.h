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

#ifndef XFACE_H
#define XFACE_H

#include "csg_parser/cf_xmlNode.h"
#include <vector>

// xface reprecents a planar face in a xpolyhedron
// The face may contain 3 or more vertices

class xface {
public:
   typedef std::vector<size_t>::const_iterator const_iterator;
   typedef std::vector<size_t>::const_reverse_iterator const_reverse_iterator;

   xface() {}

   xface(cf_xmlNode& face);

   // triangle facce
   xface(size_t v1, size_t v2, size_t v3) { m_indices = {v1,v2,v3 }; }

   // quad face
   xface(size_t v1, size_t v2, size_t v3, size_t v4) { m_indices = {v1,v2,v3,v4 }; }

   // polygon face (possibly more than 4 vertices)
   xface(const std::vector<size_t>& indices) : m_indices(indices) {}
   virtual ~xface() {}

   // number of vertices in face
   size_t size() const  { return m_indices.size(); }

   // return vertex index
   size_t operator[](size_t iv) const { return m_indices[iv]; }

   // traverse face vertices in forward (CCW) direction
   const_iterator begin() const { return m_indices.begin();}
   const_iterator end() const { return m_indices.end();}

   // traverse face vertices in reverse (CW) direction
   const_reverse_iterator rbegin() const { return m_indices.rbegin();}
   const_reverse_iterator rend() const { return m_indices.rend();}

   // create a reversed copy of this face
   xface reverse_copy() const;

   // perform in-place reversal of the face
   xface& in_place_reverse();
private:
   std::vector<size_t> m_indices;
};

#endif // XFACE_H
