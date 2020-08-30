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


#ifndef CARVE_BOOLEAN_THREAD_H
#define CARVE_BOOLEAN_THREAD_H

#include <memory>
#include <string>
#include <carve/csg.hpp>
#include "safe_queue.h"

#include <boost/thread.hpp>

// carve_boolean_thread allows boolean operations to be performed in boost::threads
// meshes to be processed must be placed in mesh_queue before launching the threads.

class carve_boolean_thread {
public:

   // default hard coded until we have something more sophisticated
   static size_t default_nthreads() { return 12; }

   typedef std::shared_ptr<carve::mesh::MeshSet<3>> MeshSet_ptr;

   carve_boolean_thread(safe_queue<MeshSet_ptr>& mesh_queue, carve::csg::CSG::OP op, safe_queue<std::string>& exception_queue);
   virtual ~carve_boolean_thread();

   // allow this class to run as a boost::thread
   void operator()() { run(); }

protected:
   // run does the actual calculation work
   void run();

private:
   carve::csg::CSG::OP m_op;
   safe_queue<MeshSet_ptr>& m_mesh_queue;
   safe_queue<std::string>& m_exception_queue;
};

#endif // CARVE_BOOLEAN_THREAD_H
