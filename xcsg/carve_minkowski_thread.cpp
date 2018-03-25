#include "carve_minkowski_thread.h"
#include "carve_minkowski_hull.h"
#include "carve_mesh_thread.h"
#include "carve_boolean_thread.h"
#include "carve_convex_tesselator.h"
#include <map>
//#include <iostream>

carve_minkowski_thread::carve_minkowski_thread()
{}

carve_minkowski_thread::~carve_minkowski_thread()
{}

void carve_minkowski_thread::add_face(carve::mesh::Face<3>* face, MeshSet_ptr meshB, safe_queue<hull_pair>& hull_queue)
{
   std::vector<carve::mesh::Face<3>::vertex_t*> verts;
   face->getVertices(verts);
//   std::cout << "add_face, size=" << verts.size() << std::endl;

   carve_convex_tesselator cfaces(verts);
   for(auto& i : cfaces) {
      std::vector<carve_convex_tesselator::pos3d>  coords = cfaces.face_coords(i);

      hull_pair hp;
//      std::cout << "---"  << std::endl;
      for(auto& p : coords ) {
         hp.first.push_back(p.vec());
//         std::cout << std::setw(8) << p.x <<  std::setw(8) <<  p.y << std::setw(8) << p.z  << std::endl;
      }
      hp.second = meshB;
      hull_queue.enqueue(hp);
   }
}


void carve_minkowski_thread::create_mesh_queue(const carve::math::Matrix& t,
                                               std::list<std::shared_ptr<xsolid>> objects,
                                               safe_queue<MeshSet_ptr>& mesh_queue)
{
   // first create meshes for A and B
   safe_queue<MeshSet_ptr> ab_mesh_queue;
   carve_mesh_thread::create_mesh_queue(t,objects,ab_mesh_queue);

   // extract the meshes
   MeshSet_ptr meshA,meshB;
   while(!ab_mesh_queue.try_dequeue(meshA)) {}
   while(!ab_mesh_queue.try_dequeue(meshB)) {}

   size_t nfacesA = meshA->meshes[0]->faces.size();
   size_t nfacesB = meshB->meshes[0]->faces.size();

   // meshA goes straight into the mesh queue as it will be unioned
   // with all the "face hull meshes"
   mesh_queue.enqueue(meshA);

   // extract coordinates for all faces in A and build the hull queue.
   // The hull queue contains the B mesh pluss perturbation coordinates
   // for computing a local hull mesh, based on A faces

   safe_queue<hull_pair> hull_queue;

   // meshA may in theory contain multiple independent meshes
   // so we traverse them
   size_t nmesh = meshA->meshes.size();

   for(size_t imesh=0; imesh<nmesh; imesh++) {
      carve::mesh::Mesh<3>* mesh = meshA->meshes[imesh];

      // traverse the faces in this mesh
      // if a face is concave, subdivide
      size_t nfaces = mesh->faces.size();
      for (size_t iface=0; iface<nfaces; iface++) {
         carve::mesh::Face<3>* face = mesh->faces[iface];
         add_face(face,meshB,hull_queue);
      }
   }

   // compute the local hull meshes and store them in the mesh queue
   const size_t nthreads = std::min(carve_boolean_thread::default_nthreads(),hull_queue.size());
   safe_queue<std::string>   exception_queue;
   std::list<boost::thread>  hull_threads;
   for(size_t i=0; i<nthreads; i++) {
      hull_threads.push_back(boost::thread(carve_minkowski_hull(hull_queue,mesh_queue,exception_queue)));
   }

   // wait for the threads to finish
   for(auto ithread=hull_threads.begin(); ithread!=hull_threads.end(); ithread++) {
      ithread->join();
   }

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }
}



