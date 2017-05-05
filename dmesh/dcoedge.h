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
   
#ifndef DCOEDGE_H
#define DCOEDGE_H

#include <cstddef>

class dentity;
class dedge;
class dtriangle;

// Topology
// ========
// A dcoedge refers to a dedge. It carries a flag indicating the direction of the coedge relative to the dedge.
// a dcoedge is uniquely owned by its parent.

class dcoedge {
public:
   friend class dtriangle;
   friend class dloop;
   friend class dprofile;

   // return the parent object
   const dentity* parent() const { return m_parent; }
   dentity* parent() { return m_parent; }

   // return the dcoedge oriented vertex1
   size_t vertex1() const;

   // return the dcoedge oriented vertex2
   size_t vertex2() const;

   // return the forward flag
   bool fwd() const { return m_fwd; }

   // reverse orientation of coedge
   void reverse() { m_fwd = !m_fwd; }

   // return underlying edge
   dedge* edge() { return m_edge; }
   const dedge* edge() const { return m_edge; }

   // return a clone of this coedge, but with specified parent
   dcoedge* clone(dentity* parent);

protected:
   dcoedge(dentity* parent, dedge* edge, bool fwd);
   virtual ~dcoedge();

private:
   dentity*   m_parent; // parent can be dtriangle or dloop
   dedge*     m_edge;   // the edge referred to from this coedge
   bool       m_fwd;    // true when dcoedge has same dir as dedge
};

#endif // DCOEDGE_H
