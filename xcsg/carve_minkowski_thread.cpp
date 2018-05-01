#include "carve_minkowski_thread.h"
#include "carve_minkowski_hull.h"
#include "carve_boolean.h"
#include "carve_triangulate.h"
#include "carve_boolean_thread.h"
#include "xpolyhedron.h"

#include <map>

carve_minkowski_thread::carve_minkowski_thread()
{}

carve_minkowski_thread::~carve_minkowski_thread()
{}

void carve_minkowski_thread::add_faces(std::shared_ptr<carve::poly::Polyhedron> poly, MeshSet_ptr meshB, safe_queue<hull_pair>& hull_queue)
{
   size_t nfaces = poly->faces.size();
   std::vector<carve::poly::Geometry<3>::vertex_t>& vertices = poly->vertices;

   // one hull pair per face
   for(size_t iface=0; iface< nfaces; iface++) {

      hull_pair hp;

      // the face should always be triangular at this point
      size_t nv = poly->faces[iface].nVertices();
      std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
      poly->faces[iface].getVertexLoop(vloop);
      for(size_t ivert=0; ivert<nv; ivert++) {
         size_t index = poly->vertexToIndex(vloop[ivert]);
         carve::poly::Geometry<3>::vertex_t& vtx = vertices[index];
         hp.first.push_back(carve::geom::VECTOR(vtx.v[0],vtx.v[1],vtx.v[2]));
      }

      hp.second = meshB;
      hull_queue.enqueue(hp);
   }
}

void carve_minkowski_thread::create_mesh_queue(const carve::math::Matrix& t,
                                               std::list<std::shared_ptr<xsolid>> objects,
                                               safe_queue<MeshSet_ptr>& mesh_queue)
{
   if(objects.size() != 2) {
      throw logic_error("carve_minkowski_thread: expected 2 object parameters, but got " + objects.size());
   }

   // first create meshes for for A and B, stored in "objects"
   // we do this synchronously here, so that the order of the parameters are guaranteed
   auto i = objects.begin();
   MeshSet_ptr meshA = (*i++)->create_carve_mesh(t);
   MeshSet_ptr meshB = (*i++)->create_carve_mesh(t);

   // meshA goes straight into the mesh queue as it will be unioned
   // with the hull meshes
   mesh_queue.enqueue(meshA);

   // extract coordinates for all faces in A and build the hull queue.
   // The hull queue contains the B mesh pluss perturbation coordinates
   // for computing a hull mesh, based on A faces
   safe_queue<hull_pair> hull_queue;

   // triangulate meshA so we are sure it contains only triangular (i.e. convex) faces
   // then add all faces to the hull_queue
   auto polyA = triangulate(meshA);
   for(size_t ipoly=0; ipoly<polyA->size(); ipoly++) {
      std::shared_ptr<carve::poly::Polyhedron> poly = (*polyA)[ipoly];
      add_faces(poly,meshB,hull_queue);
   }

   // compute the hull meshes and store them in the mesh queue
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

   // the mesh queue is now complete
}

std::shared_ptr<std::vector<std::shared_ptr<carve::poly::Polyhedron>>> carve_minkowski_thread::triangulate(MeshSet_ptr mesh_set)
{
   // the triangulator needs a carve polyhedron.
   // we can convert to such polyhedra using carve_boolean
   carve_boolean csg;
   csg.compute(mesh_set,carve::csg::CSG::OP::UNION);

   carve_triangulate triangulator;

   size_t nmani = csg.size();
   for(size_t imani=0; imani<nmani; imani++) {
      std::shared_ptr<xpolyhedron> poly = csg.create_manifold(imani);

      // check if there are any non-triangular faces
      bool must_triangulate = false;
      size_t nfaces = poly->f_size();
      for(size_t iface=0;iface<nfaces; iface++) {
         const xface& face = poly->f_get(iface);
         if(face.size() > 3) {
            must_triangulate = true;
            break;
         }
      }

      if(must_triangulate) {
         bool improve = false;
         bool canonicalize = false;
         bool degen_check = false;
         triangulator.compute(poly->create_carve_polyhedron(),improve,canonicalize,degen_check);
      }
      else {
         // triangulation not required
         triangulator.add(poly->create_carve_polyhedron());
      }
   }
   return triangulator.carve_polyset();
}


