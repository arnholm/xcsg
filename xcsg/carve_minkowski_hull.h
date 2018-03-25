#ifndef CARVE_MINKOWSKI_HULL_H
#define CARVE_MINKOWSKI_HULL_H

#include <memory>
#include <string>
#include <vector>
#include <boost/thread.hpp>
#include "safe_queue.h"
#include "xshape.h"

// carve_minkowski_hull translates the "hull_queue" into the "mesh_queue"
// by computing convex hull meshes for all entries in the hull queue
// Each hull_pair contains a mesh and associated perturbation coordinates for computing a hull

class carve_minkowski_hull {
public:
   typedef std::shared_ptr<carve::mesh::MeshSet<3>> MeshSet_ptr;
   typedef std::pair<std::vector<xvertex>,MeshSet_ptr>  hull_pair;

   carve_minkowski_hull(safe_queue<hull_pair>&   hull_queue,
                        safe_queue<MeshSet_ptr>& mesh_queue,
                        safe_queue<std::string>& exception_queue);

   virtual ~carve_minkowski_hull();

   // allow this class to run as a boost::thread
   void operator()() { run(); }

protected:
   void run();

   MeshSet_ptr compute_hull(hull_pair& hp);

private:
   safe_queue<hull_pair>&   m_hull_queue;
   safe_queue<MeshSet_ptr>& m_mesh_queue;
   safe_queue<std::string>& m_exception_queue;
};

#endif // CARVE_MINKOWSKI_HULL_H
