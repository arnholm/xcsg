// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Gemoetry
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

#include "qhull3d.h"
#include "qvec3d.h"
#include <iostream>
using namespace std;

// ref http://stackoverflow.com/questions/19530731/qhull-library-c-interface

//#include "libqhullcpp/RboxPoints.h"
//#include "libqhullcpp/Coordinates.h"
//#include "libqhullcpp/QhullError.h"
#include "libqhullcpp/Qhull.h"
//#include "libqhullcpp/QhullQh.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
//#include "libqhullcpp/QhullLinkedList.h"
#include "libqhullcpp/QhullVertex.h"
//#include "libqhullcpp/QhullSet.h"
#include "libqhullcpp/QhullVertexSet.h"

class bbox {
public:
   bbox() : init(false) {}

   void add(qhull3d::xyz& xyz)
   {
      if(!init) {
         xyzmin.x = xyz.x;
         xyzmin.y = xyz.y;
         xyzmin.z = xyz.z;

         xyzmax.x = xyz.x;
         xyzmax.y = xyz.y;
         xyzmax.z = xyz.z;
         init = true;
      }
      else {
         xyzmin.x = std::min(xyzmin.x,xyz.x);
         xyzmin.y = std::min(xyzmin.y,xyz.y);
         xyzmin.z = std::min(xyzmin.z,xyz.z);

         xyzmax.x = std::max(xyzmax.x,xyz.x);
         xyzmax.y = std::max(xyzmax.y,xyz.y);
         xyzmax.z = std::max(xyzmax.z,xyz.z);
      }
   }

   qhull3d::xyz center()
   {
      double x = 0.5*( xyzmin.x + xyzmax.x);
      double y = 0.5*( xyzmin.y + xyzmax.y);
      double z = 0.5*( xyzmin.z + xyzmax.z);
      return qhull3d::xyz(x,y,z);
   }
private:
   bool init;
   qhull3d::xyz xyzmin;
   qhull3d::xyz xyzmax;
};

qhull3d::qhull3d()
{}

qhull3d::~qhull3d()
{}

void qhull3d::clear()
{
   m_in_vert.clear();
   m_faces.clear();
   m_vert.clear();
}

void qhull3d::reserve(size_t nvert)
{
   m_in_vert.reserve(3*nvert);
}

void qhull3d::push_back(double x, double y, double z)
{
 //  cout << "input vertex " << m_in_vert.size()/3 << " = " << x << " " << y << " " << z << endl;

   m_in_vert.push_back(x);
   m_in_vert.push_back(y);
   m_in_vert.push_back(z);
}

size_t qhull3d::nvertices() const
{
   return m_vert.size();
}

size_t qhull3d::nfaces() const
{
   return m_faces.size();
}

bool qhull3d::compute()
{
   const int pointDimension = 3;
   int pointCount = static_cast<int>(m_in_vert.size()/pointDimension);

   // actually run the hull algorithm
   orgQhull::Qhull qhull;
   qhull.runQhull("qhull3d",pointDimension, pointCount,&m_in_vert[0],"Qt");

   // the result will be somewhat chaotic, the following must be fixed in a few post-processing steps
   // - vertex id's are not sequential anymore, some may be missing
   // - some faces will have face normals pointing the wrong way, no consistency

   // <id,iinode> : vmap = mapping from qhull vertex_id to iinode (zero based and sequential)
   map<size_t,size_t> vmap;
   size_t iinode = 0;

   // compute mean hull coordinate, this will always be inside the convex hull
   xyz pmean;
   size_t nv = 0;

   // First collect vertices
   orgQhull::QhullVertexList  vertices = qhull.vertexList();
   for (orgQhull::QhullVertexList::iterator it = vertices.begin(); it != vertices.end(); ++it)  {

      // vertex and its coordinates
      orgQhull::QhullVertex& v = *it;
      orgQhull::QhullPoint p = v.point();
      double* coords = p.coordinates();

      // insert the new vertex with sequential number iinode, then keep mapping from id() to iinode
      xyz pnt(coords[0],coords[1],coords[2]);

      // add oint to boundary box
      pmean.add(pnt);
      nv++;

      // insert in vertex map
      m_vert.insert(std::make_pair(iinode, pnt ));

      // insert in id to iinode map
      vmap[v.id()] = iinode++;
   }

   // the "hull_center" is a point guaranteed to be inside the convex hull body
   pmean.scale(1.0/nv);
   xyz hull_center = pmean;

   int iface = 0;
   orgQhull::QhullFacetList facets = qhull.facetList();
   for (orgQhull::QhullFacetList::iterator it = facets.begin(); it != facets.end(); ++it)  {

      // check to see if the face is usable
      if(it->isGood()) {

         // get the face and it vertices
         orgQhull::QhullFacet& f = *it;
         orgQhull::QhullVertexSet vSet = f.vertices();

         // we want a continuous sequence of face id's as output, so just increment here
         face_vertices& fv = m_faces[iface++];

         // compute the face center
         xyz face_cen;
         for (orgQhull::QhullVertexSet::iterator vIt = vSet.begin(); vIt != vSet.end(); ++vIt)  {

            orgQhull::QhullVertex v = *vIt;
            size_t node_id = v.id();
            size_t iinode  = vmap[node_id];

            // Keep the converted reference for the face vertex in the face vertex vector
            fv.push_back(iinode);

            // accumulate face vertex coordinates
            xyz& fvc = m_vert[iinode];
            face_cen.x += fvc.x;
            face_cen.y += fvc.y;
            face_cen.z += fvc.z;
         }

         // divide by number of face vertices to get the face center estimate
         size_t nfv = fv.size();
         face_cen.x /= nfv;
         face_cen.y /= nfv;
         face_cen.z /= nfv;

         // compute the face normal and flip the face as needed
         check_flip(hull_center,face_cen,fv);
      }
   }

   return true;
}

void qhull3d::check_flip(xyz& cen, xyz& face_cen, face_vertices& fv)
{
   // vector from centre of hull to centre of face
   qvec3d vref(face_cen.x-cen.x,face_cen.y-cen.y,face_cen.z-cen.z);
   qvec3d vnorm;

   if(fv.size() < 4) {
      // a simple triangle, use the fastest method

      // face coordinates
      xyz&  p0 = m_vert[fv[0]];
      xyz&  p1 = m_vert[fv[1]];
      xyz&  p2 = m_vert[fv[2]];

      // face edge vectors
      qvec3d v1(p1.x-p0.x,p1.y-p0.y,p1.z-p0.z);
      qvec3d v2(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z);

      // face normal
      vnorm = v1.cross(v2);
   }
   else {
      // use Newell's method, more expensive but safer for general polygons
      vnorm = newell_face_normal(fv);
   }

   if(vref.dot(vnorm) < 0.0) {

      // normal was opposite the reference, so we must flip the face
      std::reverse(fv.begin(),fv.end());
   }
}

qvec3d qhull3d::newell_face_normal(face_vertices& fv)
{
   // https://www.opengl.org/wiki/Calculating_a_Surface_Normal
   // http://www.gamedev.net/topic/416131-calculating-a-polygon-normal/?p=3771628#entry3771628

   double nx(0.0),ny(0.0),nz(0.0);
   size_t nvert = fv.size();
   size_t ilast = nvert-1;
   for(size_t i=0; i<nvert; i++) {

      // determine next vertex
      size_t inext = (i<ilast)? (i+1) : 0;

      // obtain current and next vertex coorddinates
      const xyz& vc = m_vert[fv[i]];
      const xyz& vn = m_vert[fv[inext]];

      // compute for this vertex
      nx += (vc.y - vn.y) * (vc.z + vn.z);
      ny += (vc.z - vn.z) * (vc.x + vn.x);
      nz += (vc.x - vn.x) * (vc.y + vn.y);
   }

   // normalise vector
   qvec3d vnorm(nx,ny,nz);
   vnorm.normalise();
   return vnorm;
}

qhull3d::face_iterator qhull3d::face_begin()
{
   return m_faces.begin();
}

qhull3d::face_iterator qhull3d::face_end()
{
   return m_faces.end();
}

qhull3d::vertex_iterator qhull3d::vertex_begin()
{
   return m_vert.begin();
}

qhull3d::vertex_iterator qhull3d::vertex_end()
{
   return m_vert.end();
}

qhull3d::in_coords_iterator qhull3d::in_coords_begin()
{
   return m_in_vert.begin();
}

qhull3d::in_coords_iterator qhull3d::in_coords_end()
{
   return m_in_vert.end();
}

