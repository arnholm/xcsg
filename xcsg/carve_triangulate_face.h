#ifndef CARVE_TRIANGULATE_FACE_H
#define CARVE_TRIANGULATE_FACE_H

#include <carve/csg.hpp>
struct TESStesselator;

class carve_triangulate_face {
public:
   carve_triangulate_face();
   virtual ~carve_triangulate_face();

   // returns a vector of triangulated faces in the form of vertex indices
   // the vertex indices returned will be one of the input indices only
   std::vector<std::vector<size_t>> compute(const std::vector<size_t>&    vind,           // input face vertex indices (into polyhedron vertex vector)
                                            const std::vector<carve::geom2d::P2>& vxy);   // computed 2d vertex coordinates

protected:
   void create_tess();
   void delete_tess();

private:
   TESStesselator* m_tess;
};

#endif // CARVE_TRIANGULATE_FACE_H
