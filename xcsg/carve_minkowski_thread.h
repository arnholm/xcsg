#ifndef CARVE_MINKOWSKI_THREAD_H
#define CARVE_MINKOWSKI_THREAD_H

#include <memory>
#include <string>
#include <boost/thread.hpp>
#include "safe_queue.h"
#include <carve/poly.hpp>
#include "xsolid.h"

// carve_minkowski_thread fills the mesh queue.
// it also does the main logic for minkowski

class carve_minkowski_thread {
public:
   typedef std::shared_ptr<carve::mesh::MeshSet<3>>     MeshSet_ptr;
   typedef std::pair<std::vector<xvertex>,MeshSet_ptr>  hull_pair;

   carve_minkowski_thread();
   virtual ~carve_minkowski_thread();

   // build the mesh queue
   static void create_mesh_queue(const carve::math::Matrix& t,
                                 std::list<std::shared_ptr<xsolid>> objects,
                                 safe_queue<MeshSet_ptr>& mesh_queue);

protected:
   static void add_faces(std::shared_ptr<carve::poly::Polyhedron> poly, MeshSet_ptr meshB, safe_queue<hull_pair>& hull_queue);

   // create triangulated polyhedra from mesh
   static std::shared_ptr<std::vector<std::shared_ptr<carve::poly::Polyhedron>>>  triangulate(MeshSet_ptr mesh);

private:
};

#endif // CARVE_MINKOWSKI_THREAD_H
