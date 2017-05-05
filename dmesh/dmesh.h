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
   
#ifndef DMESH_H
#define DMESH_H

#include <istream>
#include <ostream>

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "dentity.h"
#include "dpos2d.h"
#include "dprofile.h"

class dvertex;
class dedge;
class dcoedge;
class dtriangle;


// Topology/Geometry
// =================
// dmesh is the main entry point for 2d Delaunay meshing.
// It can mesh from a raw point cloud (option A) or a predefined profile (option B)
//
// Each dmesh instance represents a unique 2d mesh. Several instances can coexist.

class dmesh {
public:
   friend class dtriangle;
   friend class dprofile;
   friend class dloop;

   typedef std::unordered_set<dtriangle*>::iterator    triangle_iterator;
   typedef std::unordered_map<size_t,dedge*>::iterator edge_iterator;

   dmesh();
   virtual ~dmesh();

   // Option A: triangulate from cloud of raw positions. Here it is important to "pepper" the
   //           area where the material is supposed to be, with points closer than prune radius.
   //           After triangulating, call prune_radius(r) to remove large triangles. What remains
   //           is assumed to be material. Finally call compute_profile() to establish the profile
   //           from the outer and inner edge loops. Retrieve profile with get_profile();

   // A1 triangulate point cloud
   bool triangulate_point_cloud(const std::vector<dpos2d>& points);
   // A2 Calling prune_radius(r) removes triangles with circumcircle radius > r
   void prune_radius(double r);
   // A3 compute profile from the triangle mesh. Edges with use_count=1 considered
   bool compute_profile();


   // Option B: Triangulate from a known profile consisting of loops (loop=closed contour).
   //           Build each loop by calling add_loop(points). The outer countours must be CCW
   //           as seen from +Z. Inner contours must be CW. Then call triangulate_profile()
   //           to create the triangles in the areas bounded by the loops.

   // B1 (optional) pre-reserve space for vertices. Typically sum of points in loops
   void reserve_vertices(size_t nv);
   // B2 Add a new loop to the profile, this creates vertices, edges and loops
   void add_loop(const std::vector<dpos2d>& points);
   // B3 triangulate the profile. Any pre-existing triangles will be removed
   bool triangulate_profile(bool rmv_nonmat=true, bool split_loops=true, bool rmv_unbound=true);

   // add vertices to triangle centroids as long as the area is > limit
   // defines a line from centroid (param=0) to circumcircle centre (p=1) where vertex is inserted
   bool triangulate_refine_area(double area_limit, double param, bool rmv_nonmat=true, bool split_loops=true, bool rmv_unbound=true);

   // split edges longer than given length, returns number of edges split
   size_t split_long_edges(double length);

   // export the mesh dprofile to .p2d format (2d profile text format)
   void export_p2d(std::ostream& out) const;

   // import to mesh dprofile from .p2d format (2d profile text format).
   // This implies repeated calls to add_loop(...). Recommend to start from clean/empty mesh
   void import_p2d(std::istream& in, bool move_to_1st_quadrant);

   // number of triangles
   size_t size() const           { return m_tri.size();  }

   // triangle traversal
   triangle_iterator begin()     { return m_tri.begin(); }
   triangle_iterator end()       { return m_tri.end();   }

   // edge traversal
   edge_iterator edge_begin()    { return m_edge.begin(); }
   edge_iterator edge_end()      { return m_edge.end();   }

   // vertex traversal
   size_t         vertex_size() const { return m_vert.size(); }
   const dvertex* get_vertex(size_t iv) const;

   // clear the complete mesh
   void clear() { clear_profile(); clear_triangles(); clear_edges();  clear_vertices(); }

   // remove seleced topology
   void clear_profile()   { m_profile.clear(); }
   void clear_triangles();
   void clear_edges();
   void clear_vertices();

   // return pointer to profile, requires prior call to compute_profile()
   const dprofile* get_profile() const { return &m_profile; }

protected:
   size_t         add_vertex(const dpos2d& pos);
   dedge*         get_create_edge(size_t iv1, size_t iv2);
   dtriangle*     add_triangle(size_t iv1, size_t iv2, size_t iv3);
   dtriangle*     add_triangle(size_t iv1, dedge* edge);

   // add supertriangle based on existing user verices
   dtriangle*     add_supertriangle();

   // remove_triangle returns the edges wth use count > 0 after removal of triangle
   std::vector<dedge*> remove_triangle(dtriangle* triangle, bool remove_unused_edges=true);

   // remove the specified edge. Throws if edge is in use
   void           remove_edge(dedge* edge);

   // remove all unused edges
   void           remove_unused_edges();

   // compute the orientation of triangle (iv1,iv2,iv3)
   double         cross(size_t iv1, size_t iv2, size_t iv3);

   // triangulate based on existing vertices
   bool           triangulate_vertices();

   // add point based on Bowyer Watson method, using a pre-created vertex
   void           bowyer_watson(size_t iv);

   // remove nonmaterial triangles next to loops.
   // This can be inside holes our outside outer loops.
   void           remove_nonmaterial_triangles();

   // remove unbounded triangles, i.e. triangles with at least one free edge (use_count==1)
   void           remove_unbounded_triangles();

   // enforce free loop edges by splitting them, return number of splits. Zero return means all recovered
   size_t         recover_loop_edges_split();

   // split the given edge and replace the neighbour elements
   size_t         split_edge(dedge* edge);

private:
   std::vector<dvertex*>             m_vert;     // user defined vertices
   std::unordered_map<size_t,dedge*> m_edge;     // map of edges key(v1,v2), allowing lookup of edge based on vertices
   std::unordered_set<dtriangle*>    m_tri;      // generated triangles

   dprofile                     m_profile;  // the mesh profile (optional)
};

#endif // DMESH_H
