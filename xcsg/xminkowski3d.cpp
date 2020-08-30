#include "xminkowski3d.h"
#include "csg_parser/cf_xmlNode.h"
#include "xsolid_collector.h"

#include "carve_boolean.h"
#include "carve_boolean_thread.h"
#include "carve_minkowski_thread.h"

#include "boolean_timer.h"

xminkowski3d::xminkowski3d()
{
   //ctor
}

xminkowski3d::xminkowski3d(const cf_xmlNode& node)
{
   if(node.tag() != "minkowski3d")throw logic_error("Expected xml tag minkowski3d, but found " + node.tag());
   set_transform(node);
   xsolid_collector::collect_children(node,m_incl);

   if(m_incl.size() != 2) throw logic_error("Expected 2 parameters for minkowski3d, but got " + std::to_string(m_incl.size()));
}


xminkowski3d::~xminkowski3d()
{}

size_t xminkowski3d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xsolid> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}


std::shared_ptr<carve::mesh::MeshSet<3>> xminkowski3d::create_carve_mesh(const carve::math::Matrix& t) const
{
   // first fill the mesh queue with objects to union
   safe_queue<std::string> exception_queue;
   safe_queue<carve_boolean_thread::MeshSet_ptr> mesh_queue;
   carve_minkowski_thread::create_mesh_queue(t*get_transform(),m_incl,mesh_queue);

   // improve the timer estimate now that the number of booleans is known for this operation
   boolean_timer::singleton().add_nbool(mesh_queue.size());

   // union the resulting meshes
   const size_t nthreads = carve_boolean_thread::default_nthreads();
   list<boost::thread>  csg_threads;
   for(size_t i=0; i<nthreads; i++) {
      csg_threads.push_back(boost::thread(carve_boolean_thread(mesh_queue,carve::csg::CSG::UNION,exception_queue)));
   }

   // wait for the threads to finish
   for(auto ithread=csg_threads.begin(); ithread!=csg_threads.end(); ithread++) {
      ithread->join();
   }
   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }

   return mesh_queue.dequeue();
}
