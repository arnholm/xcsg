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

#include <numeric>

#include "xdifference3d.h"
#include "carve_boolean.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xsolid_collector.h"

#include "carve_boolean_thread.h"
#include "carve_mesh_thread.h"

xdifference3d::xdifference3d( )
{}

xdifference3d::~xdifference3d()
{}

std::shared_ptr<carve::mesh::MeshSet<3>> xdifference3d::compute_union(const carve::math::Matrix& t, std::unordered_set<std::shared_ptr<xsolid>>  objects) const
{
   safe_queue<std::string> exception_queue;
   safe_queue<carve_boolean_thread::MeshSet_ptr> mesh_queue;
   carve_mesh_thread::create_mesh_queue(t*get_transform(),objects,mesh_queue);

   const size_t nthreads = carve_boolean_thread::default_nthreads();
   list<boost::thread>  csg_threads;
   for(size_t i=0; i<nthreads; i++) {
      csg_threads.push_back(boost::thread(carve_boolean_thread(mesh_queue,carve::csg::CSG::UNION,exception_queue)));
   }

   // wait for the threads to finish
   for(auto ithread=csg_threads.begin(); ithread!=csg_threads.end(); ithread++) {
      ithread->join();
   }

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }

   return mesh_queue.dequeue();
}


std::shared_ptr<carve::mesh::MeshSet<3>> xdifference3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   // run booleans in threads

   std::shared_ptr<carve::mesh::MeshSet<3>>  a = compute_union(t,m_incl);
   std::shared_ptr<carve::mesh::MeshSet<3>>  b = compute_union(t,m_excl);

   carve_boolean csg;
   csg.compute(a,carve::csg::CSG::UNION);
   csg.compute(b,carve::csg::CSG::A_MINUS_B);
   csg.eliminate_short_edges();

   return csg.mesh_set();
}


size_t xdifference3d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   for(auto i=m_excl.begin(); i!=m_excl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

xdifference3d::xdifference3d(const cf_xmlNode& node)
{
   if(node.tag() != "difference3d")throw logic_error("Expected xml tag difference3d, but found " + node.tag());
   set_transform(node);

   xsolid_collector::collect_children(node,m_incl,1,m_excl);
}
