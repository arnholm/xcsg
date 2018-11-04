#ifndef CARVE_TRIANGULATE_FACE_H
#define CARVE_TRIANGULATE_FACE_H

#include <carve/csg.hpp>
#include <memory>
struct TESStesselator;

class carve_triangulate_face {
public:
   struct spec {
      std::vector<size_t>            vind; // input face vertex indices (into polyhedron vertex vector)
      std::vector<carve::geom2d::P2> vxy;  // projected 2d vertex coordinates
   };

   carve_triangulate_face(std::shared_ptr<spec>  spec);
   virtual ~carve_triangulate_face();

   void compute();

   // move_triangles is a destructive read
   std::vector<std::vector<size_t>> move_triangles() { return std::move(m_tri); }

protected:

   // returns a vector of triangulated faces in the form of vertex indices
   // the vertex indices returned will be one of the input indices only
   std::vector<std::vector<size_t>> compute2d(const std::vector<size_t>&    vind,
                                              const std::vector<carve::geom2d::P2>& vxy);

protected:
   void create_tess();
   void delete_tess();

private:
   TESStesselator*                   m_tess;
   std::shared_ptr<spec>             m_spec;
   std::vector<std::vector<size_t>>  m_tri;  // vector of triangulated faces in the form of vertex indices
};

#endif // CARVE_TRIANGULATE_FACE_H
