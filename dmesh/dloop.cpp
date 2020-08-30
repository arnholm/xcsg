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

#include "dloop.h"
#include "dvertex.h"
#include "dcoedge.h"
#include "dedge.h"
#include "dmesh.h"
#include <iterator>
#include <algorithm>

dloop::dloop(dmesh* mesh)
: dentity(mesh)
{}

dloop::~dloop()
{
   clear();
}

void dloop::clear()
{
   for(auto coedge : m_coedges) {
      delete coedge;
   }
   m_coedges.clear();
}

void dloop::push_back(dcoedge* coedge)
{
   m_coedges.push_back(coedge);
}

std::vector<dpos2d> dloop::get_pos(bool fwd) const
{
   const dmesh* mesh = get_mesh();

   // just get the points in the order stored
   std::vector<dpos2d> points;

   points.reserve(size());
   if(fwd) {
      // forward direction
      for(auto i=m_coedges.begin(); i!=m_coedges.end(); i++) {
         dcoedge* coedge = *i;
         points.push_back(mesh->get_vertex(coedge->vertex1())->pos());
      }
   }
   else {
      // reverse direction
      for(auto i=m_coedges.rbegin(); i!=m_coedges.rend(); i++) {
         dcoedge* coedge = *i;
         points.push_back(mesh->get_vertex(coedge->vertex1())->pos());
      }
   }

   return std::move(points);
}

double dloop::signed_area() const
{

   /*
     Method:

     ref http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
     Some of the suggested methods will fail in the case of a non-convex polygon,
     such as a crescent. Here's a simple one that will work with non-convex polygons
     (it'll even work with a self-intersecting polygon like a figure-eight, telling you whether it's mostly clockwise).

     Sum over the edges, (x2-x1)(y2+y1). If the result is positive the curve is clockwise,
     if it's negative the curve is counter-clockwise. (The result is twice the enclosed area, with a +/- convention.)

     point[0] = (5,0)   edge[0]: (6-5)(4+0) =   4
     point[1] = (6,4)   edge[1]: (4-6)(5+4) = -18
     point[2] = (4,5)   edge[2]: (1-4)(5+5) = -30
     point[3] = (1,5)   edge[3]: (1-1)(0+5) =   0
     point[4] = (1,0)   edge[4]: (5-1)(0+0) =   0
                                              ---
                                              -44  counter-clockwise
   */

   double sum = 0.0;
   if(m_coedges.size() > 0) {
      size_t ivcur = (*m_coedges.begin())->vertex1();
      const dmesh* mesh = get_mesh();
      dpos2d p1 = mesh->get_vertex(ivcur)->pos();
      for(auto coedge : m_coedges) {

         // edge vertex coordinates
         ivcur = coedge->vertex2();
         const dpos2d& p2 = mesh->get_vertex(ivcur)->pos();

         double x1 = p1.x();
         double y1 = p1.y();

         double x2 = p2.x();
         double y2 = p2.y();

         sum += (x2 - x1)*(y2 + y1);
         p1 = p2;
      }
   }

   // we reverse the sign here as positive areas correspond to counterclockwise loops
   double s_area = -0.5*sum;
   return s_area;

}


size_t dloop::split_coedge(dcoedge* coedge)
{

   // look up iterator of the coedge to be replaced
   auto icoedge = std::find(m_coedges.begin(),m_coedges.end(),coedge);
   if(icoedge != m_coedges.end()) {

      // found the coedge, get the 1st and second vertex in forward loop direction
      size_t iv1  = coedge->vertex1();
      size_t iv2  = coedge->vertex2();

      dmesh* mesh = get_mesh();
      const dpos2d& p1 = mesh->get_vertex(iv1)->pos();
      const dpos2d& p2 = mesh->get_vertex(iv2)->pos();
      dpos2d pm = 0.5*(p1+p2);

      // create the new vertex and the 2 new edges
      size_t ivm  = mesh->add_vertex(pm);
      dedge* e1   = mesh->get_create_edge(iv1,ivm);
      dedge* e2   = mesh->get_create_edge(ivm,iv2);

      // check if the new edges are directed the same way as the old (most likely not)
      // set the forward flag accordingly
      dedge* edge = coedge->edge();

      // compute the direction flags for the new coedges
      bool fwd1   = (edge->dir().dot(e1->dir()) > 0.0)? coedge->fwd() : !coedge->fwd();
      bool fwd2   = (edge->dir().dot(e2->dir()) > 0.0)? coedge->fwd() : !coedge->fwd();

      // erase the old coedge from the container and delete it
      auto ipos = m_coedges.erase(icoedge);
      delete coedge;

      // also request the old edge to be removed
      mesh->remove_edge(edge);

      // insert the new coedges in their proper loop positions
      ipos = m_coedges.insert(ipos,new dcoedge(this,e2,fwd2));
      ipos = m_coedges.insert(ipos,new dcoedge(this,e1,fwd1));

      // return the vertex index
      return ivm;
   }

   throw std::logic_error("dloop::split_coedge: coedge was not found");
}

void dloop::collect_overlapping_triangles(dedge* edge, std::unordered_set<dtriangle*>& overlapping)
{
   if(edge->is_referenced_from<dloop>())return;

   size_t iv1 = edge->vertex1();
   size_t iv2 = edge->vertex2();

   // get the line representation of the input edge
   dline2d edge_line = edge->line();

   for(auto icoedge=m_coedges.begin(); icoedge!=m_coedges.end(); icoedge++) {

      // get the line representation of the loop coedge
      dcoedge* coedge     = *icoedge;
      dedge* coedge_edge  = coedge->edge();

      size_t icv1 = coedge_edge->vertex1();
      size_t icv2 = coedge_edge->vertex2();

      bool common_vertex = (iv1==icv1 || iv1==icv2) || (iv2==icv1 || iv2==icv2);
      if( (coedge_edge != edge) && !common_vertex ) {

         dline2d coedge_line = coedge_edge->line();

         dpos2d pos;
         double edge_par   = -1.0;
         double coedge_par = -1.0;
         if(edge_line.intersect(coedge_line,pos,edge_par,coedge_par)) {
            bool on_line        = (edge_par  >0.0) && (edge_par  <1.0);
            bool on_coedge_line = (coedge_par>0.0) && (coedge_par<1.0);
            if(on_line && on_coedge_line) {
               // we have intersection
               std::unordered_set<dtriangle*> triangles = edge->triangles();
               for(auto& tri : triangles) {
                  overlapping.insert(tri);
               }
            }
         }
      }

   // next doedge in loop
   }
}
