#include "carve_convex_tesselator.h"

static inline carve::geom::vector<3> vpos(carve::mesh::Face<3>::vertex_t* vertex)
{
   return carve::geom::VECTOR(vertex->v[0],vertex->v[1],vertex->v[2]);
}

carve_convex_tesselator::carve_convex_tesselator(std::vector<carve::mesh::Face<3>::vertex_t*> verts)
{
   face f;
   for(size_t i=0;i<verts.size();i++) {
      m_vert.push_back(pos3d(vpos(verts[i])));
      f.push_back(i);
   }

   // find face normal so we have something to compare with at vertices
   vec3d fnormal = face_normal(f);
   split_face(f,fnormal);
}

carve_convex_tesselator::~carve_convex_tesselator()
{}

std::vector<carve_convex_tesselator::pos3d> carve_convex_tesselator::face_coords(const face& f) const
{
   std::vector<carve_convex_tesselator::pos3d>  coords;
   coords.reserve(f.size());
   for(size_t i : f) {
      coords.push_back(m_vert[i]);
   }
   return coords;
}


// compute face normal
// also note: face_area = 0.5*normal.length();
carve_convex_tesselator::vec3d carve_convex_tesselator::face_normal(const face& f)
{
   vec3d normal = carve::geom::VECTOR(0.0,0.0,0.0);

   if(f.size() == 3) {

      // triangle
      vec3d x = m_vert[f[1]].vec() - m_vert[f[0]].vec();
      vec3d y = m_vert[f[2]].vec() - m_vert[f[0]].vec();
      normal  = carve::geom::cross(x,y);
   }
   else {

      // general polygon
      size_t n = f.size();
      vec3d b  = m_vert[f[n-2]].vec();
      vec3d c  = m_vert[f[n-1]].vec();
      for(size_t i=0; i<n; i++ ) {

         vec3d a = b;
         b       = c;
         c       =  m_vert[f[i]].vec();

         double dx = b.y * (c.z - a.z);
         double dy = b.z * (c.x - a.x);
         double dz = b.x * (c.y - a.y);

         normal += carve::geom::VECTOR(dx,dy,dz);
      }
   }
   return normal;
}

inline size_t next_index(const carve_convex_tesselator::face& f, size_t i)
{
   return ( ((i+1)<f.size())? i+1 : 0 );
}

void carve_convex_tesselator::split_face(const face& f, const vec3d& fnormal)
{
   if(f.size() == 3) {
      // triangle, end of recursion
      m_face.push_back(f);
      return;
   }

   // i_concave and i_convex are indices in f
   // the values in f are indices in m_vert

   // find a concave vertex in this face
   size_t istart=0;
   size_t i_concave=0;
   if(find_concave(f,istart,fnormal,i_concave)) {

      // found a concave face, look for a convex vertex
      // starting one beyond the concave vertex
      istart=next_index(f,i_concave);
      size_t i_convex=0;
      if(find_convex(f,istart,fnormal,i_convex)) {

         // we found a convex vertex also, split f into f1 and f2
         face f1;
         size_t i=i_concave;
         while(i != i_convex) {
            f1.push_back(f[i]);
            i = next_index(f,i);
         }
         f1.push_back(f[i_convex]);

         face f2;
         i=i_convex;
         while(i != i_concave) {
            f2.push_back(f[i]);
            i = next_index(f,i);
         }
         f2.push_back(f[i_concave]);

         // possibly split these faces again
         split_face(f1,fnormal);
         split_face(f2,fnormal);
         return;
      }
   }

   // face was convex
   m_face.push_back(f);
}


bool  carve_convex_tesselator::find_concave(const face& f, size_t istart, const vec3d& fnormal, size_t& i_concave)
{
   size_t i = istart;
   do {
      size_t i0 = i;
      size_t i1 = next_index(f,i0);
      size_t i2 = next_index(f,i1);

      const pos3d& p0  = m_vert[f[i0]];
      const pos3d& p1  = m_vert[f[i1]];
      const pos3d& p2  = m_vert[f[i2]];

      // normal vector in vertex p1
      vec3d v1     = p1.vec() - p0.vec();
      vec3d v2     = p2.vec() - p1.vec();
      vec3d vnorm  = carve::geom::cross(v1,v2);
      double dot = carve::geom::dot(vnorm,fnormal);
      if(dot < 0.0) {
         // i1 is concave
         i_concave = i1;
         return true;
      }

      i = i1;
   } while(i != istart);

   return false;
}

bool  carve_convex_tesselator::find_convex(const face& f, size_t istart, const vec3d& fnormal, size_t& i_convex)
{
   size_t i = istart;
   do {
      size_t i0 = i;
      size_t i1 = next_index(f,i0);
      size_t i2 = next_index(f,i1);

      const pos3d& p0  = m_vert[f[i0]];
      const pos3d& p1  = m_vert[f[i1]];
      const pos3d& p2  = m_vert[f[i2]];

      // normal vector in vertex p1
      vec3d v1     = p1.vec() - p0.vec();
      vec3d v2     = p2.vec() - p1.vec();
      vec3d vnorm  = carve::geom::cross(v1,v2);
      double dot = carve::geom::dot(vnorm,fnormal);
      if(dot > 0.0) {
         // i1 is convex
         i_convex = i1;
         return true;
      }
      i = i1;
   } while(i != istart);

   return false;
}
