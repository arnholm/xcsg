#ifndef CARVE_CONVEX_TESSELATOR_H
#define CARVE_CONVEX_TESSELATOR_H

#include <vector>
#include <list>
#include <carve/mesh.hpp>

// carve carve_convex_tesselator takes a carve face and
// tesselates it into a number of convex faces if required.
// The resulting faces may contain more than 4 vertices as long as they are convex.
// The tesselator is used in carve_minkowski_thread

class carve_convex_tesselator {
public:
   typedef std::vector<size_t>   face;
   typedef std::list<face>       flist;
   typedef flist::const_iterator flist_iterator;

   typedef carve::geom::vector<3> vec3d;
   class pos3d  {
   public:
      pos3d(const vec3d& v) : x(v[0]),y(v[1]),z(v[2]) {}
      vec3d vec() const { return carve::geom::VECTOR(x,y,z); }
      double x,y,z;
   };

   // construct from carve mesh face
   carve_convex_tesselator(std::vector<carve::mesh::Face<3>::vertex_t*> verts);
   virtual ~carve_convex_tesselator();

   // traverse convex faces
   flist_iterator begin() const { return m_face.begin(); }
   flist_iterator end() const   { return m_face.end(); }

   // extract coordinate vector of convex face
   std::vector<pos3d> face_coords(const face& f) const;

protected:

   // compute face normal
   vec3d face_normal(const face& f);
   void  split_face(const face& f, const vec3d& fnormal);

   // find concave/convex vertex in face. istart/i_concave/i_convex are indices in face f
   bool find_concave(const face& f, size_t istart, const vec3d& fnormal, size_t& i_concave);
   bool find_convex(const face& f, size_t istart, const vec3d& fnormal, size_t& i_convex);

private:
   std::list<face>    m_face;   // convex faces
   std::vector<pos3d> m_vert;   // original face vertices
};

#endif // CARVE_CONVEX_TESSELATOR_H
