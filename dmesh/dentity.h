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
   
#ifndef DENTITY_H
#define DENTITY_H

class dmesh;

// Topology
// ========
// A dentity is a base class for some of the topology objects.
// The main purpose is to allow them to access the mesh they are part of.
// Sometimes several independent meshes exist at the same time.

class dentity {
public:
   dentity(dmesh* mesh);
   virtual ~dentity();

   // return pointer to my mesh
   dmesh* get_mesh() { return m_mesh; }
   const dmesh* get_mesh() const { return m_mesh; }

private:
   dmesh* m_mesh;
};

#endif // DENTITY_H
