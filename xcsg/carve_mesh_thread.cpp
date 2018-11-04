#include "carve_mesh_thread.h"
#include <list>
#include "boolean_timer.h"

carve_mesh_thread::carve_mesh_thread(const carve::math::Matrix& t,
                                     const std::unordered_set<std::shared_ptr<xsolid>>& solids,
                                     safe_queue<MeshSet_ptr>&   mesh_queue,
                                     safe_queue<std::string>&   exception_queue)
: m_t(t)
, m_solids(solids)
, m_mesh_queue(mesh_queue)
, m_exception_queue(exception_queue)
{}

carve_mesh_thread::~carve_mesh_thread()
{}

void carve_mesh_thread::run()
{
   try {
      for(auto& solid : m_solids) {
         m_mesh_queue.enqueue(solid->create_carve_mesh(m_t));
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

void carve_mesh_thread::create_mesh_queue(const carve::math::Matrix& t, std::unordered_set<std::shared_ptr<xsolid>> objects, safe_queue<MeshSet_ptr>& mesh_queue)
{
   safe_queue<std::string> exception_queue;
   std::list<boost::thread>  mesh_threads;

   size_t max_threads = 20;
   size_t num_threads = std::min(objects.size(),max_threads);

   size_t num_obj_thread = 1 + objects.size()/num_threads;
   while(objects.size() > 0) {
      std::unordered_set<std::shared_ptr<xsolid>> thread_objects;
      size_t num_obj =std::min(num_obj_thread,objects.size());
      for(size_t iobj=0; iobj<num_obj; iobj++) {
         if(objects.size()==0)break;
         auto i = objects.begin();
         thread_objects.insert(*i);
         objects.erase(i);
      }
      mesh_threads.push_back(boost::thread(carve_mesh_thread(t,thread_objects,mesh_queue,exception_queue)));
   }

   // wait for the threads to finish
   for(auto ithread=mesh_threads.begin(); ithread!=mesh_threads.end(); ithread++) {
      ithread->join();
   }

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }
}

void carve_mesh_thread::create_mesh_queue(const carve::math::Matrix& t, std::list<std::shared_ptr<xsolid>> objects, safe_queue<MeshSet_ptr>& mesh_queue)
{
   std::unordered_set<std::shared_ptr<xsolid>> objects_set;
   std::copy(objects.begin(),objects.end(),std::inserter(objects_set,objects_set.begin()));
   create_mesh_queue(t,objects_set,mesh_queue);
}
