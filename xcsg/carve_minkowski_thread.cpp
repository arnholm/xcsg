#include "carve_minkowski_thread.h"
#include "carve_minkowski_hull.h"
#include "carve_mesh_thread.h"
#include "carve_boolean_thread.h"

#include <iostream>

carve_minkowski_thread::carve_minkowski_thread()
{}

carve_minkowski_thread::~carve_minkowski_thread()
{}


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

   std::cout << "nfacesA=" << nfacesA << " nfacesB=" << nfacesB << std::endl;

   // meshA goes straight into the mesh queue as it will be unioned
   // with all the "face hull meshes"
 //  mesh_queue.enqueue(meshA);

   // extract coordinates for all faces in A and build the hull queue.
   // The hull queue contains the B mesh pluss perturbation coordinates
   // for computing a local hull mesh
   typedef std::pair<std::vector<xvertex>,MeshSet_ptr>  hull_pair;
   safe_queue<hull_pair> hull_queue;

   // meshA may in theory contain multiple independent meshes
   // so we traverse them
   size_t nmesh = meshA->meshes.size();

   std::cout << "solid@[] hulls = {"<< std::endl;
   for(size_t imesh=0; imesh<nmesh; imesh++) {
      carve::mesh::Mesh<3>* mesh = meshA->meshes[imesh];

      // traverse the faces in this mesh
      size_t nfaces = mesh->faces.size();
      for (size_t iface=0; iface<nfaces; iface++) {
         carve::mesh::Face<3>* face = mesh->faces[iface];
         std::vector<carve::mesh::Face<3>::vertex_t*> verts;
         face->getVertices(verts);

         // build the hull specification and put it on the hull queue
         hull_pair hp;
         size_t nv = verts.size();

         if(iface>0) std::cout << ',';
         std::cout << "hull3d(";
         for(size_t i=0;i<nv;i++) {
            carve::mesh::Face<3>::vertex_t* vertex = verts[i];
            double x = vertex->v[0];
            double y = vertex->v[1];
            double z = vertex->v[2];
            hp.first.push_back(vertex->v);
            if(i>0) std::cout << ',';
            std::cout << "translate("<<x<<','<<y<<','<<z<<")*sphere(10)";
         }
         std::cout << ") " << std::endl;
         /*
         typedef carve::geom::vector<3> vec3d;
         // triangle
         vec3d x = hp.first[1] - hp.first[0];
         vec3d y = hp.first[2] - hp.first[0];
         vec3d normal = carve::geom::cross(x,y);
         double area = 0.5*normal.length();
         std::cout << "area=" << area << std::endl;
         */

         hp.second = meshB;
         hull_queue.enqueue(hp);
      }
   }

   std::cout << "};"<< std::endl;
   size_t nhull = hull_queue.size();

   safe_queue<std::string>   exception_queue;
   carve_minkowski_hull  hulls(hull_queue,mesh_queue,exception_queue);
   hulls.run();

   size_t nhull_mesh = mesh_queue.size();


/*
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
*/

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }
}




