// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
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

#include "xhull2d.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "carve_boolean.h"
#include "xpolygon.h"
#include "clipper_boolean.h"
#include "qhull/qhull2d.h"
#include "qhull/qhull3d.h"
#include "xshape2d_collector.h"

xhull2d::xhull2d()
{}

xhull2d::xhull2d(const cf_xmlNode& node)
{
   if(node.tag() != "hull2d")throw logic_error("Expected xml tag hull2d, but found " + node.tag());
   set_transform(node);
   xshape2d_collector::collect_children(node,m_incl);
}


xhull2d::~xhull2d()
{}

size_t xhull2d::nbool()
{
   return 1;
}

std::shared_ptr<clipper_profile> xhull2d::create_clipper_profile(const carve::math::Matrix& t) const
{
   std::shared_ptr<clipper_profile> profile(new clipper_profile);

   qhull2d qhull;

   // accumulate vertices of underlying objects
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {

      std::shared_ptr<clipper_profile> profile = (*i)->create_clipper_profile(t*get_transform());
      std::shared_ptr<polyset2d> polyset = profile->polyset();

      for(auto ipoly=polyset->begin(); ipoly!=polyset->end(); ipoly++) {
         std::shared_ptr<polygon2d> poly = *ipoly;
         for(size_t ic=0; ic<poly->size();ic++) {
            std::shared_ptr<const contour2d> contour = (*poly)[ic];
            for(size_t iv=0; iv<contour->size();iv++) {
                const dpos2d& vtx = (*contour)[iv];
                qhull.push_back(qhull2d::xy(vtx.x(),vtx.y()));
            }
         }
      }
   }

   // then compute the convex hull for the vertex set
   if(qhull.compute()) {

      // create a polygon representing the computed hull
      size_t nvert = qhull.vertex_size();
      std::shared_ptr<xpolygon> hull_polygon(new xpolygon);
      hull_polygon->v_reserve(nvert);
      for(auto iv=qhull.vertex_begin(); iv!=qhull.vertex_end(); iv++) {
         qhull2d::xy xy = *iv;
         hull_polygon->v_add(xy.x,xy.y);
      }

      // return the clipper_profile so we can perform further booleans
      profile = hull_polygon->create_clipper_profile();
   }
   else {
      throw std::logic_error("hull2d computation failed");
   }

   return profile;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xhull2d::create_carve_mesh(const carve::math::Matrix& t) const
{
   qhull3d qhull;

   // accumulate vertices of underlying objects
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<carve::mesh::MeshSet<3>> meshset = (*i)->create_carve_mesh(t*get_transform());
      size_t nvert =  meshset->vertex_storage.size();
      qhull.reserve(qhull.nvertices()+nvert);
      for(size_t i=0;i<nvert;i++) {
         carve::mesh::MeshSet<3>::vertex_t& vertex = meshset->vertex_storage[i];
         qhull.push_back(vertex.v[0],vertex.v[1],vertex.v[2]);
      }
   }

   carve_boolean csg;
   csg.compute(qhull);
   return csg.mesh_set();
}


