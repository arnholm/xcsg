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

#include "xunion3d.h"
#include "carve_boolean.h"
#include "csg_parser/cf_xmlNode.h"
#include "xcsg_factory.h"
#include "xsolid_collector.h"

#include "carve_boolean_thread.h"
#include "carve_mesh_thread.h"

xunion3d::xunion3d()
{}

xunion3d::xunion3d(const cf_xmlNode& node)
{
   if(node.tag() != "union3d")throw logic_error("Expected xml tag union3d, but found " + node.tag());
   set_transform(node);
   xsolid_collector::collect_children(node,m_incl);
}

xunion3d::~xunion3d()
{}

size_t xunion3d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xunion3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   // run booleans in threads

   safe_queue<std::string> exception_queue;
   safe_queue<carve_boolean_thread::MeshSet_ptr> mesh_queue;
   carve_mesh_thread::create_mesh_queue(t*get_transform(),m_incl,mesh_queue);

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
