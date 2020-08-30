// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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

#include "qhull2d.h"
#include "qvec3d.h"
#include <iostream>
#include <set>
using namespace std;

// ref http://stackoverflow.com/questions/19530731/qhull-library-c-interface

//#include "libqhullcpp/RboxPoints.h"
//#include "libqhullcpp/Coordinates.h"
//#include "libqhullcpp/QhullError.h"
#include "libqhullcpp/Qhull.h"
//#include "libqhullcpp/QhullQh.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullFacetSet.h"
//#include "libqhullcpp/QhullLinkedList.h"
#include "libqhullcpp/QhullVertex.h"
//#include "libqhullcpp/QhullSet.h"
#include "libqhullcpp/QhullVertexSet.h"

qhull2d::qhull2d()
{}

qhull2d::~qhull2d()
{}

void qhull2d::clear()
{
   m_in_vert.clear();
   m_contour.clear();
}

void qhull2d::reserve(size_t nvert)
{
   m_in_vert.reserve(3*nvert);
}

void qhull2d::push_back(const xy& pnt)
{
   m_in_vert.push_back(pnt.x);
   m_in_vert.push_back(pnt.y);
}

size_t qhull2d::vertex_size() const
{
   return m_contour.size();
}

// a hidden helper function to get to next vertex along contour using obfuscated code....
static orgQhull::QhullVertex next_contour_vertex(orgQhull::QhullVertex& vtx, const std::set<size_t>& visited)
{
   // get vertex neighbour facets, look for 1st unvisited vertex and return it
   // This causes us to traverse the contour in one direction
   orgQhull::QhullFacetSet fSet = vtx.neighborFacets();
   for(auto fit = fSet.begin(); fit != fSet.end(); ++fit)  {
      const orgQhull::QhullFacet& f = *fit;
      orgQhull::QhullVertexSet vSet = f.vertices();
      for (auto vIt = vSet.begin(); vIt != vSet.end(); ++vIt)  {
         orgQhull::QhullVertex v = *vIt;
         if(visited.find(v.id()) == visited.end()) return v;
      }
   }
   return vtx;
}

bool qhull2d::compute()
{
   // this is the 2d version
   const int pointDimension = 2;
   int pointCount = static_cast<int>(m_in_vert.size()/pointDimension);

   // actually run the hull algorithm
   orgQhull::Qhull qhull;
   qhull.runQhull("qhull2d",pointDimension, pointCount,&m_in_vert[0],"Qt");

   // the result will be somewhat chaotic, the following must be fixed in a few post-processing steps
   // - The "facets" returned are actually edges between hull contour vertices
   // - we need to look at neighbour facets at each vertex and then pick the next one we have not already seen
   // - once complete, we must check for CW vs. CCW contour sequence and reverse as required. we want CCW

   // compute the hull contour
   // call to defineVertexNeighborFacets() is required for vtx.neighborFacets() to work
   std::set<size_t> visited;
   qhull.defineVertexNeighborFacets();
   orgQhull::QhullVertex vtx =  qhull.firstVertex();
   int id_prev = vtx.id();
   do {
      // record the current vertex as seen
      id_prev = vtx.id();
      visited.insert(id_prev);

      // store the vertex coordinates in the contour vector
      orgQhull::QhullPoint p = vtx.point();
      double* coords = p.coordinates();
      m_contour.push_back(xy(coords[0],coords[1]));

      // get next vertex along contour
      // if no next found, the input vertex is returned
      vtx = next_contour_vertex(vtx,visited);
   }
   while(vtx.id() != id_prev);

   // At this point we have the complete contour, but it might be sorted the wrong way.
   // We want CCW, so we require a negative area
   if(signed_area() > 0) {
      // we need to flip
      std::reverse(m_contour.begin(),m_contour.end());
   }
   return (m_contour.size() > 2);
}

double qhull2d::signed_area() const
{
   // this function computes the area of the polygon described by m_contour.
   // it returns a positive area if the contour is CW

   size_t np  = m_contour.size();
   double sum = 0.0;

   for (size_t ip = 1; ip < np; ip++)    {
      const xy& prev = m_contour[ip-1];
      const xy& pcur = m_contour[ip];

      double x1 = prev.x;
      double y1 = prev.y;

      double x2 = pcur.x;
      double y2 = pcur.y;

      sum += (x2 - x1)*(y2 + y1);
   }

   // close the polygon: account for the final edge from last to first point
   const xy& prev = m_contour[np-1];
   const xy& pcur = m_contour[0];

   double x1 = prev.x;
   double y1 = prev.y;

   double x2 = pcur.x;
   double y2 = pcur.y;

   sum += (x2 - x1)*(y2 + y1);

   double s_area = 0.5*sum;
   return s_area;
}

qhull2d::vertex_iterator qhull2d::vertex_begin()
{
   return m_contour.begin();
}

qhull2d::vertex_iterator qhull2d::vertex_end()
{
   return m_contour.end();
}


