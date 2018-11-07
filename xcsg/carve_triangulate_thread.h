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

#ifndef CARVE_TRIANGULATE_THREAD_H
#define CARVE_TRIANGULATE_THREAD_H

#include <memory>
#include <string>
#include "safe_queue.h"
#include "carve_triangulate_face.h"

#include <boost/thread.hpp>

class carve_triangulate_thread {
public:
   // run triangulation using num_threads threads. in_queue must be prepared before
   static void run_threads(size_t num_threads,
                           safe_queue<std::shared_ptr<carve_triangulate_face::spec>>&      in_queue,        // input faces to be triangulated
                           safe_queue<std::shared_ptr<carve_triangulate_face::triangles>>& triangle_queue); // computed triangle faces

public:
   carve_triangulate_thread(safe_queue<std::shared_ptr<carve_triangulate_face::spec>>&      in_queue,        // input faces to be triangulated
                            safe_queue<std::shared_ptr<carve_triangulate_face::triangles>>& triangle_queue,  // computed triangle faces
                            safe_queue<std::string>& exception_queue);
   virtual ~carve_triangulate_thread();

   // allow this class to run as a boost::thread
   void operator()() { run(); }

protected:
   void run();

private:
   safe_queue<std::shared_ptr<carve_triangulate_face::spec>>&      m_in_queue;
   safe_queue<std::shared_ptr<carve_triangulate_face::triangles>>& m_triangle_queue;
   safe_queue<std::string>&                                        m_exception_queue;
};

#endif // CARVE_TRIANGULATE_THREAD_H
