#ifndef CARVE_MINKOWSKI_THREAD_H
#define CARVE_MINKOWSKI_THREAD_H

#include <memory>
#include <string>
#include <boost/thread.hpp>
#include "safe_queue.h"

#include "xsolid.h"

class carve_minkowski_thread {
public:
   typedef std::shared_ptr<carve::mesh::MeshSet<3>> MeshSet_ptr;

   carve_minkowski_thread();
   virtual ~carve_minkowski_thread();

   // build the mesh queue
   static void create_mesh_queue(const carve::math::Matrix& t,
                                 std::list<std::shared_ptr<xsolid>> objects,
                                 safe_queue<MeshSet_ptr>& mesh_queue);

protected:

private:
};

#endif // CARVE_MINKOWSKI_THREAD_H
