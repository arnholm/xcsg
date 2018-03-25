#include "carve_minkowski_hull.h"
#include "qhull/qhull3d.h"
#include "carve_boolean.h"
#include <carve/matrix.hpp>
#include "xshape.h"

carve_minkowski_hull::carve_minkowski_hull(safe_queue<hull_pair>&  hull_queue,
                                          safe_queue<MeshSet_ptr>& mesh_queue,
                                          safe_queue<std::string>& exception_queue)
: m_hull_queue(hull_queue)
, m_mesh_queue(mesh_queue)
, m_exception_queue(exception_queue)
{}

carve_minkowski_hull::~carve_minkowski_hull()
{}


void carve_minkowski_hull::run()
{
   // compute hull meshes as long as the hull queue is non-empty
   try {
      while(m_hull_queue.size() > 0) {
         hull_pair hp;
         if(m_hull_queue.try_dequeue(hp)) {
            m_mesh_queue.enqueue(compute_hull(hp));
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

carve_minkowski_hull::MeshSet_ptr carve_minkowski_hull::compute_hull(hull_pair& hp)
{
   std::vector<xvertex>& coord = hp.first;
   MeshSet_ptr meshB           = hp.second;

   qhull3d qhull;
   size_t nvert =  meshB->vertex_storage.size();
   qhull.reserve(nvert*coord.size());
   for(size_t i=0; i<coord.size(); i++) {
      // use perturbation point to establish translation matrix
      // and then translate all vertex coordinates in the B mesh
      carve::math::Matrix t;
      t.m[3][0] = coord[i].x;
      t.m[3][1] = coord[i].y;
      t.m[3][2] = coord[i].z;
      for(size_t i=0;i<nvert;i++) {
         carve::mesh::MeshSet<3>::vertex_t vertex = meshB->vertex_storage[i];
         xvertex pos = t*vertex.v;
         qhull.push_back(pos[0],pos[1],pos[2]);
      }
   }

   // compute the hull mesh and return it as carve mesh
   carve_boolean csg;
   csg.compute(qhull);
   return csg.mesh_set();
}
