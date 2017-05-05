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
   
#include "dtriangle.h"
#include "dcoedge.h"
#include "dedge.h"
#include "dmesh.h"
#include "dvertex.h"
#include <algorithm>
#include "dloop.h"
#include "dvec2d.h"

#include <unordered_set>

dtriangle::dtriangle(dmesh* mesh,size_t iv1,size_t iv2,size_t iv3)
: dentity(mesh)
, m_circle(dpos2d(),-1.0)
{
   // create_coedge will generate the underlying dedge as required

   m_coedges.reserve(3);
   m_coedges.push_back(create_coedge(iv1,iv2));
   m_coedges.push_back(create_coedge(iv2,iv3));
   m_coedges.push_back(create_coedge(iv3,iv1));

   const dpos2d& p1 = mesh->get_vertex(m_coedges[0]->vertex1())->pos();
   const dpos2d& p2 = mesh->get_vertex(m_coedges[1]->vertex1())->pos();
   const dpos2d& p3 = mesh->get_vertex(m_coedges[2]->vertex1())->pos();

   m_circle = dcircle(p1,p2,p3);
}

void dtriangle::super()
{
   m_coedges[0]->edge()->addref(0);
   m_coedges[1]->edge()->addref(0);
   m_coedges[2]->edge()->addref(0);
}

dtriangle::~dtriangle()
{
   clear();
}

bool dtriangle::in_circumcircle(const dpos2d& pos)
{
   return m_circle.pos_inside(pos);
}

dcoedge* dtriangle::create_coedge(size_t iv1, size_t iv2)
{
   // get the underlying edge
   dedge* edge = get_mesh()->get_create_edge(iv1,iv2);
   bool fwd = (iv1 == edge->vertex1());
   return new dcoedge(this,edge,fwd);
}

void dtriangle::clear()
{
   // the coedges are owned here
   for(auto coedge : m_coedges) {
      delete coedge;
   }
   m_coedges.clear();
}


std::vector<dedge*> dtriangle::get_edges() const
{
   std::vector<dedge*> edges(3);
   edges[0] = m_coedges[0]->edge();
   edges[1] = m_coedges[1]->edge();
   edges[2] = m_coedges[2]->edge();
   return std::move(edges);
}

const dcoedge* dtriangle::coedge(size_t i) const
{
   if(i>2) throw std::logic_error("dtriangle::coedge: index > 2");
   return m_coedges[i];
}

dcoedge* dtriangle::coedge(size_t i)
{
   if(i>2) throw std::logic_error("dtriangle::coedge: index > 2");
   return m_coedges[i];
}


double dtriangle::signed_area() const
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

   // we reverse the sign here as positive areas correspond to counterclockwise loops
   double s_area = -0.5*sum;
   return s_area;

}


dpos2d dtriangle::centroid() const
{
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(vertex1())->pos();
   const dpos2d& p2 = mesh->get_vertex(vertex2())->pos();
   const dpos2d& p3 = mesh->get_vertex(vertex3())->pos();

   double x = (p1.x() + p2.x() + p3.x())/3.0;
   double y = (p1.y() + p2.y() + p3.y())/3.0;
   return dpos2d(x,y);
}


bool dtriangle::in_triangle(const dpos2d& pos)
{
   // ref method 3 , http://totologic.blogspot.no/2014/01/accurate-point-in-triangle-test.html
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(vertex1())->pos();
   const dpos2d& p2 = mesh->get_vertex(vertex2())->pos();
   const dpos2d& p3 = mesh->get_vertex(vertex3())->pos();

   dvec2d v1(p1,p2);
   dvec2d v2(p2,p3);
   dvec2d v3(p3,p1);

   dvec2d v1p(p1,pos);
   dvec2d v2p(p2,pos);
   dvec2d v3p(p3,pos);

   double dot1 = v1.dot(v1p);
   double dot2 = v2.dot(v2p);
   double dot3 = v3.dot(v3p);

   return ((dot1>=0.0) && (dot2>=0) && (dot3>=0));
}

double dtriangle::aspect_ratio() const
{
   const dmesh* mesh = get_mesh();
   const dpos2d& p1 = mesh->get_vertex(vertex1())->pos();
   const dpos2d& p2 = mesh->get_vertex(vertex2())->pos();
   const dpos2d& p3 = mesh->get_vertex(vertex3())->pos();

   double a = p1.dist(p2);
   double b = p2.dist(p3);
   double c = p3.dist(p1);

   double ab_min = std::min(a,b);
   double ac_min = std::min(a,c);
   double s_min  = std::min(ab_min,ac_min);

   double ab_max = std::max(a,b);
   double ac_max = std::max(a,c);
   double s_max  = std::max(ab_max,ac_max);

   return s_min/s_max;
}


const dcoedge* dtriangle::free_coedge() const
{
   for(auto coedge : m_coedges) {
      if(coedge->edge()->use_count() < 2)return coedge;
   }
   return 0;
}

const dcoedge*  dtriangle::reversed_loop() const
{
   for(auto coedge : m_coedges) {
      dedge* edge = coedge->edge();
      // traverse the neighbouring coedges
      for(auto other_coedge : *edge) {
         if(dloop* loop = dynamic_cast<dloop*>(other_coedge->parent())) {
            if(coedge->fwd() != other_coedge->fwd()) return coedge;
         }
      }
   }
   return 0;
}

size_t dtriangle::oppsite_vertex(const dedge* edge) const
{
   std::unordered_set<size_t> vt;
   vt.insert(vertex1());
   vt.insert(vertex2());
   vt.insert(vertex3());
   for(auto coedge : m_coedges) {
      dedge* this_edge = coedge->edge();
      if(this_edge == edge) {
         vt.erase(edge->vertex1());
         vt.erase(edge->vertex2());
         return *vt.begin();
      }
   }

   throw std::logic_error("dtriangle::oppsite_vertex: given edge not referenced by triangle");
}
