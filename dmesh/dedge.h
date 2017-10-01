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

#ifndef DEDGE_H
#define DEDGE_H

#include "dentity.h"
#include "dvec2d.h"
#include "dline2d.h"
#include "dcoedge.h"
class dtriangle;
#include <unordered_set>

// Topology
// ========
// A dedge is connected to 2 vertices, defined by their vertex indices.
// The first vertex index is always the lowest index value, no exception.
// The dedge maintains a set of coedges referring to it

class dedge : public dentity {
public:
   friend class dmesh;

   typedef std::unordered_set<dcoedge*> dcoedge_set;
   typedef dcoedge_set::iterator coedge_iterator;

   // return vertex indices
   size_t vertex1() const { return m_iv1; }
   size_t vertex2() const { return m_iv2; }

   // coedge traversal, i.e. over coedges referring to this edge
   coedge_iterator begin() { return m_users.begin(); }
   coedge_iterator end()   { return m_users.end(); }

   // coedge reference management
   size_t use_count() const { return m_users.size(); }
   void addref(dcoedge* coedge) { m_users.insert(coedge); }
   void release(dcoedge* coedge) { m_users.erase(coedge); }

   // compute the direction from iv1 to iv2
   dvec2d dir() const;

   // return the length of the edge
   double length() const;

   // return the edge as a dline2d
   dline2d line() const;

   // return the triangles connected to this edge
   std::unordered_set<dtriangle*> triangles() const;

   template<typename T> bool is_referenced_from() const;

protected:

   // return the edge key, used for edge lookup based on vertex indices
   static size_t key(size_t iv1, size_t iv2);

   dedge(dmesh* mesh, size_t iv1, size_t iv2);
   virtual ~dedge();

private:
   size_t       m_iv1;   // ALWAYS the lowest vertex index
   size_t       m_iv2;   // ALWAYS the highest vertex index
   dcoedge_set  m_users;
};


template<typename T>
bool dedge::is_referenced_from() const
{
   for(auto coedge : m_users) {
      if(dynamic_cast<T*>(coedge->parent())) return true;
   }
   return false;
}

#endif // DEDGE_H
