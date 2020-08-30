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

#include "dmesh.h"
#include "dvertex.h"
#include "dedge.h"
#include "dcoedge.h"
#include "dtriangle.h"

#include "dpos2d.h"
#include "dvec2d.h"
#include <map>
#include <string>
#include <cmath>

#include <iomanip>

dmesh::dmesh(double epspnt)
: m_epspnt(epspnt)
, m_profile(this)
{}

dmesh::~dmesh()
{}

bool dmesh::triangulate_point_cloud(const std::vector<dpos2d>& points)
{
   // delete everything existing
   clear();

   // use reserve_vertices that will manage 3 initial supervertices
   reserve_vertices( points.size());

   // create vertices of all the points
   for(auto p : points) {
     // create the new vertex
     size_t iv = add_vertex(p);
   }

   // then run triangulation based on the new vertices
   return triangulate_vertices();
}

void dmesh::reserve_vertices(size_t nv)
{
   // the 3 first vertices are supervertices, will be assigned positions by triangulate_vertices();
   m_vert.reserve(3 + nv);

   if(m_vert.size() == 0) {
      // allocate the supervertices, here with dummy coordinates
      add_vertex(dpos2d(0,0));
      add_vertex(dpos2d(0,0));
      add_vertex(dpos2d(0,0));
   }
}

void dmesh::add_loop(const std::vector<dpos2d>& points)
{
   m_profile.add_loop(points);
}

void  dmesh::remove_unused_edges()
{
   // clear any unused edges
   std::unordered_set<dedge*> bad_edges;
   for(auto p : m_edge) {
      dedge* edge = p.second;
      if(edge->use_count() < 1) {
         bad_edges.insert(edge);
      }
   }
   for(auto edge : bad_edges) {
      remove_edge(edge);
   }
}

bool dmesh::triangulate_profile(bool rmv_nonmat, bool split_loops, bool rmv_unbound)
{
   // Sort profile loops
   m_profile.sort_loops();

   // run triangulation based on loop vertices
   bool repeat = true;
   while(repeat) {

      // remove any existing triangles, we will perform a remesh based on vertices and edges
      clear_triangles();
      remove_unused_edges();

      repeat = false;
      triangulate_vertices();

      // remove extra triangles
      if(rmv_nonmat) {
         remove_nonmaterial_triangles();
      }

      if(split_loops) {
         repeat = (recover_loop_edges_split() > 0);
      }

      if(rmv_unbound) {
         remove_unbounded_triangles();
      }
   }

   return true;
}

bool dmesh::triangulate_refine_area(double area_limit, double param, bool rmv_nonmat, bool split_loops, bool rmv_unbound)
{
   size_t nref = 0;
   do {
      nref = 0;
      typedef std::multimap<double,dtriangle*>         area_mmap;
      area_mmap    area;
      for(auto triangle : m_tri) {
         area.insert(std::make_pair(triangle->signed_area(),triangle));
      }

      for(auto p : area) {
         if(p.first > area_limit) {
            dtriangle* triangle = p.second;
            dpos2d p1 = triangle->centroid();
            dpos2d p2 = triangle->circle().center();
            dpos2d p  = p1 + param*(p2-p1);
            if(triangle->in_triangle(p)) {
               bowyer_watson(add_vertex(p));
               nref++;
               break;
            }
         }
      }
   }
   while(nref > 0);

   size_t nsplit = 0;
   do {
      remove_nonmaterial_triangles();
      nsplit = recover_loop_edges_split();
   }
   while(nsplit > 0);

   triangulate_profile(rmv_nonmat,split_loops,rmv_unbound);
//   remove_unbounded_triangles();


   return true;
}


dtriangle* dmesh::add_supertriangle()
{
   if(m_vert.size()<6)throw std::logic_error("add_supertriangle: not enough vertices exist");

   // compute the extent of the point cloud
   const dpos2d& p0 = m_vert[0]->pos();
   double xmin = p0.x();
   double xmax = xmin;
   double ymin = p0.y();
   double ymax = ymin;

   // skip the supervertices in this loop
   for(size_t iv=3; iv<m_vert.size(); iv++) {
      const dpos2d& p = m_vert[iv]->pos();
      double x = p.x();
      double y = p.y();
      xmax = (x > xmax)? x : xmax;
      xmin = (x < xmin)? x : xmin;

      ymax = (y > ymax)? y : ymax;
      ymin = (y < ymin)? y : ymin;
   }

   double dx   =  xmax - xmin;
   double dy   =  ymax - ymin;
   double dmax = (dx > dy)? dx : dy;
   double xmid = (xmax + xmin) / 2.0;
   double ymid = (ymax + ymin) / 2.0;

   m_vert[0]->set_pos(dpos2d(xmid-2*dmax, ymid-dmax   ));
   m_vert[1]->set_pos(dpos2d(xmid+2*dmax, ymid-dmax   ));
   m_vert[2]->set_pos(dpos2d(xmid       , ymid+2*dmax ));

   // supertiangle
   dtriangle* triangle = add_triangle(0,1,2);
   // mark its edges so they don't immediately disappear when deleting the supertriangle
   triangle->super();

   return triangle;
}

bool dmesh::triangulate_vertices()
{
   if(m_vert.size()<6)throw std::logic_error("triangulate_vertices: not enough vertices exist");

   // clear any existing triangles, this also removes edges
   clear_triangles();

   // create supertriangle
   dtriangle* triangle = add_supertriangle();

   // obtain supertiangle vertices
   size_t isv1 = triangle->vertex1();
   size_t isv2 = triangle->vertex2();
   size_t isv3 = triangle->vertex3();

   // perform meshing by adding user points (skipping the super vertices)
   for(size_t iv=3; iv<m_vert.size(); iv++) {
      bowyer_watson(iv);
   }

   // remove triangles referring to supervertices
   std::unordered_set<dtriangle*> bad_triangles;
   for(auto triangle : m_tri) {
      size_t iv1 = triangle->coedge(0)->vertex1();
      size_t iv2 = triangle->coedge(1)->vertex1();
      size_t iv3 = triangle->coedge(2)->vertex1();

      bool do_remove =     (iv1==isv1 || iv1==isv2 || iv1==isv3)
                        || (iv2==isv1 || iv2==isv2 || iv2==isv3)
                        || (iv3==isv1 || iv3==isv2 || iv3==isv3);

      if(do_remove) {
         bad_triangles.insert(triangle);
      }
   }

   // erase the zero-references in edges, introduced by supertriangle
   for(auto p : m_edge) {
      dedge* edge = p.second;
      edge->release(0);

      if(edge->use_count() == 0) {
         remove_edge(edge);
      }
   }

   // remove the bad triangles with any loose edges
   for(auto triangle : bad_triangles) {
      remove_triangle(triangle,true);
   }

   return true;
}

void dmesh::bowyer_watson(size_t iv)
{
   // get the position of the vertex
   const dpos2d& p = m_vert[iv]->pos();

   // this will contain the triangles to be removed because of p
   std::unordered_set<dtriangle*> bad_triangles;

   for(auto triangle : m_tri) {
      if(triangle->in_circumcircle(p,m_epspnt)) {
         bad_triangles.insert(triangle);
      }
   }

   // set of edges representing hole boundary after removing bad triangles
   std::unordered_set<dedge*> poly_edges;

   // add all edges of the bad triangles to poly_edges,
   // then delete the triangles to dereferenc the edges
   for(auto triangle : bad_triangles) {
      poly_edges.insert(triangle->coedge(0)->edge());
      poly_edges.insert(triangle->coedge(1)->edge());
      poly_edges.insert(triangle->coedge(2)->edge());

      // don't delete the unused edges here, as we want to check poly_edges
      remove_triangle(triangle,false);
   }

   // build the set of edges representing the hole contour
   std::unordered_set<dedge*> poly_contour;
   for(auto edge : poly_edges) {
      if(edge->use_count() > 0) {
         poly_contour.insert(edge);
      }
      else {
         remove_edge(edge);
      }
   }
   poly_edges.clear();

   // finally create the new triangles to fill the hole
   for(auto edge : poly_contour) {
      add_triangle(iv,edge);
   }
}

bool dmesh::compute_profile()
{
   m_profile.compute(this);
   return true;
}

size_t dmesh::add_vertex(const dpos2d& pos)
{
   size_t iv = m_vert.size();
   m_vert.push_back(new dvertex(pos));
   return iv;
}

const dvertex* dmesh::get_vertex(size_t iv) const
{
   if(iv >= m_vert.size())throw std::logic_error("get_vertex: no such vertex");
   return m_vert[iv];
}

dedge* dmesh::get_create_edge(size_t iv1, size_t iv2)
{
   // get edge key
   size_t key = dedge::key(iv1,iv2);

   // look it up or create a new
   auto iedge = m_edge.find(key);
   if(iedge == m_edge.end()) {
      auto p = m_edge.insert(std::make_pair(key,new dedge(this,iv1,iv2)));
      iedge = p.first;
   }
   return iedge->second;
}

double dmesh::cross(size_t iv1, size_t iv2, size_t iv3)
{
   const dvertex* v1 = get_vertex(iv1);
   const dvertex* v2 = get_vertex(iv2);
   const dvertex* v3 = get_vertex(iv3);

   dvec2d vec1(v1->pos(),v2->pos());
   dvec2d vec2(v1->pos(),v3->pos());
   return vec1.cross(vec2);
}

dtriangle* dmesh::add_triangle(size_t iv1, dedge* edge)
{
   return add_triangle(iv1,edge->vertex1(),edge->vertex2());
}

dtriangle* dmesh::add_triangle(size_t iv1, size_t iv2,size_t iv3)
{
   dtriangle* triangle = 0;
   if(cross(iv1,iv2,iv3) > 0) {
      triangle = new dtriangle(this,iv1,iv2,iv3);
   }
   else {
      triangle = new dtriangle(this,iv1,iv3,iv2);
   }
   m_tri.insert(triangle);

   return triangle;
}

std::vector<dedge*> dmesh::remove_triangle(dtriangle* triangle, bool remove_unused_edges)
{
   //get the triangle edges and delete the triange itself
   // some of the edges may have use_count=0 after deleting triangle
   std::vector<dedge*> edges = triangle->get_edges();
   m_tri.erase(triangle);
   delete triangle;

   if(remove_unused_edges) {
      std::vector<dedge*> used_edges;
      for(auto edge : edges) {

         // get edge key
         if(edge->use_count() == 0) {
            size_t key = dedge::key(edge->vertex1(),edge->vertex2());
            m_edge.erase(key);
            delete edge;
         }
         else {
            // this edge is still in use after deleting triangle
            used_edges.push_back(edge);
         }
      }
      return std::move(used_edges);
   }
   return std::move(edges);
}


void dmesh::remove_edge(dedge* edge)
{
   if(!edge->use_count() == 0) throw std::logic_error("remove_edge: trying to remove edge with use_count>0!");
   size_t key = dedge::key(edge->vertex1(),edge->vertex2());
   m_edge.erase(key);
   delete edge;
}

void dmesh::clear_triangles()
{
   auto i=m_tri.begin();
   while(i != m_tri.end()) {
      remove_triangle(*i,true);
      i=m_tri.begin();
   }
   m_tri.clear();
}

void dmesh::clear_edges()
{
   if(m_tri.size() > 0) throw std::logic_error("clear_edges: error, triangles exist!");

   auto i=m_edge.begin();
   while(i != m_edge.end()) {
      remove_edge(i->second);
      i=m_edge.begin();
   }
   m_edge.clear();
}

void dmesh::clear_vertices()
{
   if(m_tri.size() > 0) throw std::logic_error("clear_vertices: error, triangles exist!");
   if(m_edge.size() > 0) throw std::logic_error("clear_vertices: error, edges exist!");

   for(auto v : m_vert) {
      delete v;
   }
   m_vert.clear();
}

void dmesh::prune_radius(double r)
{
   auto i=m_tri.begin();
   while(i != m_tri.end()) {
      dtriangle* triangle = *i;
      if(triangle->circle().radius() > r) {
        remove_triangle(*i,true);
        i=m_tri.begin();
      }
      else {
         i++;
      }
   }
}

void  dmesh::remove_nonmaterial_triangles()
{
   // traverse the profile loops and check the triangles next to them
   std::unordered_set<dtriangle*> nonmaterial_triangles;
   for(auto loop : m_profile) {
      // traverse the coedges in the loop
      for(auto loop_coedge : *loop) {
         // get the edge referenced
         dedge* edge = loop_coedge->edge();
         // traverse the neighbouring coedges
         for(auto edge_coedge : *edge) {
            // disregard the loop coedge
            if(edge_coedge != loop_coedge) {
               // is the neighbour coedge owned by a triangle?
               if(dtriangle* triangle = dynamic_cast<dtriangle*>(edge_coedge->parent())) {
                  // yes, this coedge is referenced from a triangle
                  // compare the coedge directions
                  if(loop_coedge->fwd() != edge_coedge->fwd()) {
                     // the 2 coedges are opposite, which means the triangle is non-material
                     // mark the triangle for deletion
                     nonmaterial_triangles.insert(triangle);
                  }
               }
            }
         }
      }
   }

  // now delete the triangles and any edges that fall out of use
  for(auto triangle : nonmaterial_triangles) {
      remove_triangle(triangle,true);
  }

}

void dmesh::remove_unbounded_triangles()
{
   // each deletion may uncover new unbounded elements
   // therefore the process must be repeated until no more unbounded elements are found

   size_t ndel = 0;
   do {
      std::unordered_set<dtriangle*> unbounded_triangles;
      for(auto triangle : m_tri) {
         std::vector<dedge*> edges = triangle->get_edges();
         for(auto edge : edges) {
            if(edge->use_count() == 1) {
               unbounded_triangles.insert(triangle);
               break;
            }
         }
      }

     // now delete the triangles and any edges that fall out of use
     for(auto triangle : unbounded_triangles) {
        remove_triangle(triangle,true);
     }

     // number of deleted triangles in this iteration
     ndel = unbounded_triangles.size();

   } while (ndel > 0);

}

void dmesh::remove_overlapping_triangles()
{
   std::unordered_set<dtriangle*> overlapping;
   for(auto& loop : m_profile ) {

      for(auto& e : m_edge) {
         dedge* edge = e.second;
         loop->collect_overlapping_triangles(edge,overlapping);
      }
   }

  // now delete the triangles and any edges that fall out of use
  for(auto triangle : overlapping) {
     remove_triangle(triangle,true);
  }
}


void dmesh::export_p2d(std::ostream& out) const
{
   m_profile.export_p2d(out);
}

void  dmesh::import_p2d(std::istream& in, bool move_to_1st_quadrant)
{
   m_profile.import_p2d(in,move_to_1st_quadrant);
}


size_t dmesh::recover_loop_edges_split()
{
   size_t nsplit=0;

   // traverse the profile loops and check the triangles next to them
   for(auto loop : m_profile) {

      // bad_coedges contain the coedges that must be split in this loop
      std::list<dcoedge*> bad_coedges;

      // traverse the coedges in the loop
      for(auto coedge : *loop) {
         if(coedge->edge()->use_count() == 1) {
            // this edge must be split
            bad_coedges.push_back(coedge);
         }
      }

      // perform the actual splitting
      for(auto coedge : bad_coedges) {
         size_t ivm = loop->split_coedge(coedge);
         bowyer_watson(ivm);
         nsplit++;
      }
   }

   return nsplit;
}

size_t dmesh::split_long_edges(double length)
{
   size_t nsplit = 0;
   size_t iter = 0;
   size_t maxiter = 5000;

   while(true) {

      // collect the candidate edges
      std::multimap<double,dedge*> edges;
      for(auto p : m_edge) {
         dedge* edge = p.second;

         // check that this edge is not referenced by a loop
         if(!edge->is_referenced_from<dloop>()) {
            double edge_len = edge->length();

            // check that the length is longer than limit
            if(edge_len > length){
               edges.insert(std::make_pair(edge_len,edge));
            }
         }
      }

      if(edges.size() == 0  || ++iter>maxiter) break;

      // split longest first
      dedge* edge = edges.rbegin()->second;
      nsplit += split_edge(edge);
   }

   return nsplit;
}

size_t dmesh::split_edge(dedge* edge)
{
   std::unordered_set<dtriangle*> tri = edge->triangles();
   if(tri.size() == 2) {
      auto i = tri.begin();
      dtriangle* triangle1 = *i++;
      dtriangle* triangle2 = *i;

      // get the relevant existing vertices
      size_t v1 = edge->vertex1();
      size_t v2 = edge->vertex2();
      size_t v3 = triangle1->oppsite_vertex(edge);
      size_t v4 = triangle2->oppsite_vertex(edge);

      const dpos2d& p3 = m_vert[v3]->pos();
      const dpos2d& p4 = m_vert[v4]->pos();

      if(edge->length() > p3.dist(p4)) {
         // we prefer an edge flip instead of splitting the edge

         remove_triangle(triangle1);
         remove_triangle(triangle2);

         add_triangle(v1,v4,v3);
         add_triangle(v3,v4,v2);

         return 1;
      }
      else {

         // split this edge

         double area1 = fabs(triangle1->signed_area());
         double area2 = fabs(triangle2->signed_area());

         // relative weight of area1
         double par = area1/(area1+area2);

         // compute new vertex as mean
         const dpos2d& p1 = m_vert[v1]->pos();
         const dpos2d& p2 = m_vert[v2]->pos();

         // weighted position between v3 and v4, moving the pos in direction of largest area
         dpos2d pmid = p4 + par*(p3-p4);

         // add the new vertex
         size_t v5 = add_vertex(0.2*(p1+p2+3*pmid));

         // remove the original triangles,
         // input edge to be split will also be removed by this
         remove_triangle(triangle1);
         remove_triangle(triangle2);

         // add the new triangles
         add_triangle(v1,v4,v5);
         add_triangle(v4,v2,v5);
         add_triangle(v2,v3,v5);
         add_triangle(v3,v1,v5);

         return 1;
      }
   }
   return 0;
}


void dmesh::debug_mesh_report(std::ostream& out) const
{
   using namespace std;

   size_t ivert = 0;
   for(auto& v : m_vert) {
      const dpos2d& p = v->pos();
      out << "Vertex: "
          << setw(5)  << 'v'+std::to_string(ivert++)
          << setw(12) << p.x()
          << setw(12) << p.y()
          << endl;
   }

   out << endl;
   map<const dedge*,string> edge_names;
   size_t iedge=0;
   for(auto& e : m_edge) {
      const dedge* edge = e.second;
      string edge_name = 'e'+std::to_string(iedge++);
      edge_names[edge] = edge_name;
      out << "Edge: "
          << setw(5)  << edge_name
          << setw(5)  << 'v'+std::to_string(edge->vertex1())
          << setw(5)  << 'v'+std::to_string(edge->vertex2())
          << ", uc="<< setw(4)  << edge->use_count()
          << endl;
   }

   out << endl;
   size_t itri=0;
   dpos2d p11(100.0/3,100);
   for(auto& t : m_tri) {
      const dcircle& c = t->circle();
      const dpos2d& p  = c.center();
      out << "Triangle: "
          << setw(5)  << 't'+std::to_string(itri++)
          << "   :v "
          << setw(4)  << 'v'+std::to_string(t->vertex1())
          << setw(4)  << 'v'+std::to_string(t->vertex2())
          << setw(4)  << 'v'+std::to_string(t->vertex3())
          << "   :e "
          << setw(4)  << edge_names[t->coedge(0)->edge()]
          << setw(4)  << edge_names[t->coedge(1)->edge()]
          << setw(4)  << edge_names[t->coedge(2)->edge()]
          << "   :u "
          << setw(2)  << t->coedge(0)->edge()->use_count()
          << setw(2)  << t->coedge(1)->edge()->use_count()
          << setw(2)  << t->coedge(2)->edge()->use_count()
          << "   :c "
          << setw(8)  << setprecision(5) << p.x()
          << setw(8)  << setprecision(5) << p.y()
          << "   :r "
          << setw(20)  << setprecision(16) << c.radius()
          << setw(20)  << setprecision(16) << p.dist(p11)
          << setw(5)  << ( (t->in_circumcircle(p11,m_epspnt)) ? " T " : " " )
          << endl;
   }

}

void dmesh::debug_stl(std::ostream& out) const
{
   out << "solid dmesh-debug " << std::endl;
   for(auto& t : m_tri) {
      out << "facet normal 0 0 1" << std::endl;
      out << "\touter loop" << std::endl;

      std::vector<dpos2d> p = { m_vert[t->vertex1()]->pos(), m_vert[t->vertex2()]->pos(),m_vert[t->vertex3()]->pos() };

      for(size_t iv=0; iv<p.size(); iv++ ) {
         out << "\t\tvertex "<< std::setprecision(16) << p[iv].x() <<
                         ' ' << std::setprecision(16) << p[iv].y() <<
                         ' ' << "0.0" << std::endl;
      }
      out << "\tendloop" << std::endl;
      out << "endfacet" << std::endl;
   }
}
