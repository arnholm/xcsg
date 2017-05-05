// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
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
   
#include "vmap2d.h"
#include "contour2d.h"

vmap2d::vmap2d(const contour2d& c)
: m_contour(c)
, m_len(c.length())
{
   compute_map();
}

void vmap2d::compute_map()
{
   m_vmap.clear();
   m_vmap[0.0] = m_contour[0];
   double dist = 0.0;
   for(size_t i=1; i<m_contour.size(); i++) {
      dist += m_contour[i].dist(m_contour[i-1]);
      m_vmap[dist/m_len] = m_contour[i];
   }
}

void vmap2d::compute_contour()
{
   // update the contour
   m_contour.clear();
   m_contour.reserve(m_vmap.size());
   for(auto ip : m_vmap) {
      m_contour.push_back(ip.second);
   }
}

vmap2d::~vmap2d()
{}

void vmap2d::get_edges(EdgeVec& edges) const
{
   edges.clear();
   edges.reserve(m_contour.size());
   for(size_t i=0; i<m_contour.size()-1; i++) {
      edges.push_back(std::make_pair(i,i+1));
   }
   edges.push_back(std::make_pair(m_contour.size()-1,0));
}

void vmap2d::append_vlines(Vlines& lines) const
{
   dpos2d gcen = m_contour.geometric_center();
   for(size_t i=0; i<m_contour.size(); i++) {
      lines.push_back(dline2d(gcen,m_contour[i]));
   }
}

size_t  vmap2d::imprint(Vlines& lines, double epspnt)
{
   EdgeVec edges;
   get_edges(edges);

   // dist is the distance along contour before current edge
   double dist = 0.0;

   // traverse contour edges
   for(size_t iedge=0; iedge<edges.size(); iedge++) {
      auto& p = edges[iedge];
      const dpos2d& pos1 = m_contour[p.first];
      const dpos2d& pos2 = m_contour[p.second];
      dline2d edge(pos1,pos2);

      // traverse vertex lines
      for(size_t ivlin=0; ivlin<lines.size(); ivlin++) {
         const dline2d& vline = lines[ivlin];
         dpos2d xpos;
         double edge_param = -1;
         double vlin_param = -1;
         if(edge.intersect(vline,xpos,edge_param,vlin_param)) {
            if( (edge_param>0.0) && (edge_param<1.0)) {

               // distance from start of edge to intersection
               double dist1 = pos1.dist(xpos);
               double dist2 = xpos.dist(pos2);

               if((dist1>epspnt) && (dist2>epspnt)) {
                  // compute intersection parameter
                  double p = (dist + dist1)/m_len;
                  m_vmap[p] = xpos;
               }
            }
         }
      }

      // update dist
      dist += pos1.dist(pos2);
   }

   // update the contour
   compute_contour();
   return m_contour.size();
}


void vmap2d::add_vertices(size_t nv)
{
   if(nv > 0) {
      double dp = 1.0/nv;
      double p0 = dp/2.0;
      double par = p0;
      for(size_t i=0; i<nv; i++) {
         Vmap::const_iterator ip2 = m_vmap.upper_bound(par);
         Vmap::const_iterator ip1 = ip2; ip1--;
         double p = 0.5*(ip1->first + ip2->first);
         const dpos2d& p1 = ip1->second;
         const dpos2d& p2 = ip2->second;
         double x = 0.5*(p1.x() + p2.x());
         double y = 0.5*(p1.y() + p2.y());
         m_vmap[p] = dpos2d(x,y);
         par += dp;
      }

      // update the contour
      compute_contour();
   }
}


const contour2d& vmap2d::contour() const
{
   return m_contour;
}

bool vmap2d::force_first_vertex(const dline2d& vline, double epspnt)
{
   contour2d c2;
   c2.reserve(m_contour.size());

   size_t iv_1st = 0;
   // traverse contour edges
   EdgeVec edges;
   get_edges(edges);
   for(size_t iedge=0; iedge<edges.size(); iedge++) {
      auto& p = edges[iedge];
      const dpos2d& pos1 = m_contour[p.first];
      const dpos2d& pos2 = m_contour[p.second];
      dline2d edge(pos1,pos2);

      dpos2d xpos;
      double edge_param = -1;
      double vlin_param = -1;
      if(edge.intersect(vline,xpos,edge_param,vlin_param)) {
         if(vlin_param > 0) {
            double dist1 = pos1.dist(xpos);
            if((dist1<=epspnt)) {
               iv_1st = p.first;
               break;
            }
         }
      }
   }


   for(size_t iv=iv_1st;iv<m_contour.size(); iv++) {
      c2.push_back(m_contour[iv]);
   }
   for(size_t iv=0;iv<iv_1st; iv++) {
      c2.push_back(m_contour[iv]);
   }

   m_contour = c2;
   compute_map();

   return true;
}


/*
dpos2d vmap2d::interpolate(double par) const
{
   /// upper_bound returns an iterator pointing to the first element in the container
   /// whose key compares greater than x

   /// This means:
   /// - we get begin() returned when par is lower than lowest in container
   /// - we get end()   returned when par is higher than highest in container

   double par2 = -1.0;
   Vmap::const_iterator ip2 = m_vmap.upper_bound(par);
   Vmap::const_iterator ip1 = ip2;  ip1--;
   if(ip2==m_vmap.end()) {
      par2 = 1.0;
      ip2  = m_vmap.begin();
   }
   else {
      par2 = ip2->first;
   }
   double par1 = ip1->first;

   /// par will be between par1 and par2
   /// Compute the local parameter lpar for this segment
   double lpar = (par-par1)/(par2-par1);
   const dpos2d& v1 = ip1->second;
   const dpos2d& v2 = ip2->second;

   double x = v1.x() + lpar * ( v2.x() - v1.x() );
   double y = v1.y() + lpar * ( v2.y() - v1.y() );
   return dpos2d(x,y);
}
*/
