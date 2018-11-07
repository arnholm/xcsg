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

#include "carve_triangulate_thread.h"

carve_triangulate_thread::carve_triangulate_thread(safe_queue<std::shared_ptr<carve_triangulate_face::spec>>&      in_queue,        // input faces to be triangulated
                                                   safe_queue<std::shared_ptr<carve_triangulate_face::triangles>>& triangle_queue,  // computed triangle faces
                                                   safe_queue<std::string>&                                        exception_queue)
: m_in_queue(in_queue)
, m_triangle_queue(triangle_queue)
, m_exception_queue(exception_queue)
{}

carve_triangulate_thread::~carve_triangulate_thread()
{}

void carve_triangulate_thread::run()
{
   size_t icount = 0;
   try {
      while(m_in_queue.size() > 0) {
         std::shared_ptr<carve_triangulate_face::spec> spec;
         if(m_in_queue.try_dequeue(spec)) {
            // triangulate this face and store result in triangle queue
            carve_triangulate_face triangulator(spec);
            m_triangle_queue.enqueue(triangulator.compute());
            icount++;
         }
      }
   }
   catch(std::exception& ex) {
      m_exception_queue.enqueue(ex.what());
   }
}


void carve_triangulate_thread::run_threads(size_t num_threads,
                                          safe_queue<std::shared_ptr<carve_triangulate_face::spec>>&      in_queue,        // input faces to be triangulated
                                          safe_queue<std::shared_ptr<carve_triangulate_face::triangles>>& triangle_queue)  // computed triangle faces
{
    safe_queue<std::string> exception_queue;
    std::list<boost::thread>  triangulate_threads;

    for(size_t i=0; i<num_threads; i++) {
       triangulate_threads.push_back(boost::thread(carve_triangulate_thread(in_queue,triangle_queue,exception_queue)));
    }

   // wait for the threads to finish
   for(auto ithread=triangulate_threads.begin(); ithread!=triangulate_threads.end(); ithread++) {
      ithread->join();
   }

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }
}
