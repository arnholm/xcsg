#include "carve_mesh_thread.h"
#include <list>
#include "boolean_timer.h"

carve_mesh_thread::carve_mesh_thread(const carve::math::Matrix& t,
                                     std::shared_ptr<xsolid>    solid,
                                     safe_queue<MeshSet_ptr>&   mesh_queue,
                                     safe_queue<std::string>&   exception_queue)
: m_t(t)
, m_solid(solid)
, m_mesh_queue(mesh_queue)
, m_exception_queue(exception_queue)
{}

carve_mesh_thread::~carve_mesh_thread()
{}

void carve_mesh_thread::run()
{
   try {
      m_mesh_queue.enqueue(m_solid->create_carve_mesh(m_t));
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
   // the time runs only when an actual meshing is taking place
   safe_queue<std::string> exception_queue;
   std::list<boost::thread>  mesh_threads;
   for(auto i=objects.begin(); i!=objects.end(); i++) {
       mesh_threads.push_back(boost::thread(carve_mesh_thread(t,*i,mesh_queue,exception_queue)));
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
   // the time runs only when an actual meshing is taking place
   safe_queue<std::string> exception_queue;
   std::list<boost::thread>  mesh_threads;
   for(auto i=objects.begin(); i!=objects.end(); i++) {
       mesh_threads.push_back(boost::thread(carve_mesh_thread(t,*i,mesh_queue,exception_queue)));
   }

   // wait for the threads to finish
   for(auto ithread=mesh_threads.begin(); ithread!=mesh_threads.end(); ithread++) {
      ithread->join();
   }

   if(exception_queue.size() > 0) {
      throw std::logic_error(exception_queue.dequeue());
   }
}
