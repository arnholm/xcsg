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


#include "carve_boolean_thread.h"
#include "carve_boolean.h"


carve_boolean_thread::carve_boolean_thread(safe_queue<MeshSet_ptr>& mesh_queue, carve::csg::CSG::OP op, safe_queue<std::string>& exception_queue)
: m_op(op)
, m_mesh_queue(mesh_queue)
, m_exception_queue(exception_queue)
{}

carve_boolean_thread::~carve_boolean_thread()
{}


void carve_boolean_thread::run()
{
   // pick work from mesh queue as long as there are at least 2 meshes

   try {
      while(m_mesh_queue.size() > 1) {

         MeshSet_ptr a,b;
         if(m_mesh_queue.try_dequeue(a)) {
            if(m_mesh_queue.try_dequeue(b)) {

               carve_boolean csg;
               csg.compute(a,m_op);
               csg.compute(b,m_op);
               m_mesh_queue.enqueue(csg.mesh_set());
            }
            else {
               m_mesh_queue.enqueue(a);
            }
         }
      }
   }
   catch(carve::exception& ex) {
      std::string msg("(carve error): ");
      msg += ex.str();
      m_exception_queue.enqueue(msg);
   }
   catch(std::exception& ex) {
      m_exception_queue.enqueue(ex.what());
   }
}

