#ifndef CARVE_MESH_THREAD_H
#define CARVE_MESH_THREAD_H

#include <memory>
#include <string>
#include <boost/thread.hpp>
#include "safe_queue.h"

#include "xsolid.h"

class carve_mesh_thread {
public:
  typedef std::shared_ptr<carve::mesh::MeshSet<3>> MeshSet_ptr;

   carve_mesh_thread(const carve::math::Matrix& t,
                     std::shared_ptr<xsolid>    solid,
                     safe_queue<MeshSet_ptr>&   mesh_queue,
                     safe_queue<std::string>&   exception_queue);

   virtual ~carve_mesh_thread();

   // allow this class to run as a boost::thread
   void operator()() { run(); }

   // build the mesh queue in threads
   static void create_mesh_queue(const carve::math::Matrix& t,
                                 std::unordered_set<std::shared_ptr<xsolid>> objects,
                                 safe_queue<MeshSet_ptr>& mesh_queue);

protected:
   void run();

private:
   carve::math::Matrix       m_t;
   std::shared_ptr<xsolid>   m_solid;
   safe_queue<MeshSet_ptr>&  m_mesh_queue;
   safe_queue<std::string>&  m_exception_queue;
};

#endif // CARVE_MESH_THREAD_H
