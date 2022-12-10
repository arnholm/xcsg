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

#include "dloop_optimizer.h"
#include "dline2d.h"
#include <map>
#include <cstddef>
typedef std::map<size_t,dpos2d> pos_map;


dloop_optimizer::dloop_optimizer(double arrow_max, double dist_max)
: m_arrow_max(arrow_max)
, m_dist_max(dist_max)
{}

dloop_optimizer::~dloop_optimizer()
{}

static pos_map::iterator next_pos(pos_map& pmap, pos_map::iterator it)
{
   // next_pos finds the next position in the lopp, possibly beyond the vector end
   pos_map::iterator itnext = it;
   itnext++;
   if(itnext == pmap.end()) itnext = pmap.begin();
   return itnext;
}

std::vector<dpos2d> dloop_optimizer::optimize(const std::vector<dpos2d>& points) const
{
   std::vector<dpos2d> opt_points;
   if(points.size() > 0) {

      // first make a map to simplify erasing, the key is the original vector index
      // we need to use a map and not an unordered_map, as the sequence is significant
      pos_map pmap;
      for(size_t i=0; i<points.size(); i++) {
         pmap[i] = points[i];
      }

      pos_map::iterator iprev = pmap.begin();
      pos_map::iterator imid  = next_pos(pmap,iprev);
      pos_map::iterator inext = next_pos(pmap,imid);

      size_t icount  = 0;  // counts the number of tests, i.e. rounds of the while loop
      size_t ierased = 0;  // count when last erased happened
      bool   finished = false;
      while(!finished) {

         // minimal size of loop vector is 3
         if(pmap.size() < 4)break;

         icount++;

         dpos2d p1(iprev->second);
         dpos2d midpos(imid->second);
         dpos2d p2(inext->second);
         double dist = p1.dist(midpos);

         dline2d line(p1,p2);
         dpos2d  proj = line.interpolate(line.project(midpos));
         double  arrow = midpos.dist(proj);
         if((dist > m_dist_max) || (arrow > m_arrow_max)) {
            // keep midpos
             iprev = next_pos(pmap,iprev);
             imid  = next_pos(pmap,imid);
             inext = next_pos(pmap,inext);
         }
         else {
            // eliminate midpos
            pos_map::iterator ierase = imid;
            pmap.erase(ierase);
            ierased = icount;

            iprev = inext;
            imid  = next_pos(pmap,iprev);
            inext = next_pos(pmap,imid);
         }

         // if we have gone 2 rounds without any points erased, we call it quits
         finished = ((icount - ierased) > 2*pmap.size());
      }

      // return the points remaining
      opt_points.reserve(pmap.size());
      for(auto p : pmap) {
         opt_points.push_back(p.second);
      }
   }

   return std::move(opt_points);
}


double dloop_optimizer::signed_area(const std::vector<dpos2d>& points)
{

   double sum = 0.0;
   if(points.size() > 0) {
      for(size_t i=1;i<points.size(); i++) {

         const dpos2d& p1 = points[i-1];
         const dpos2d& p2 = points[i];

         double x1 = p1.x();
         double y1 = p1.y();

         double x2 = p2.x();
         double y2 = p2.y();

         sum += (x2 - x1)*(y2 + y1);
      }

      const dpos2d& p1 = points[points.size()-1];
      const dpos2d& p2 = points[0];

      double x1 = p1.x();
      double y1 = p1.y();

      double x2 = p2.x();
      double y2 = p2.y();

      sum += (x2 - x1)*(y2 + y1);
   }
   double s_area = -0.5*sum;
   return s_area;
}
