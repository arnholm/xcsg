#include "carve_triangulate_face.h"
#include <cstdlib>
#include <memory>
#include <cmath>
#include <cstring> // for memset
#include "tmesh/libtess2/Include/tesselator.h"

static void *stdAlloc(void* userData, unsigned int size) {
	TESS_NOTUSED(userData);
	return malloc(size);
}

static void stdFree(void* userData, void* ptr) {
	TESS_NOTUSED(userData);
	free(ptr);
}

carve_triangulate_face::carve_triangulate_face()
: m_tess(0)
{}

carve_triangulate_face::~carve_triangulate_face()
{
   delete_tess();
}

void carve_triangulate_face::create_tess()
{
   delete_tess();
   TESSalloc ma;
   memset(&ma, 0, sizeof(ma));
   ma.memalloc = stdAlloc;
   ma.memfree = stdFree;
   ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.
   m_tess = tessNewTess(&ma);
}

void carve_triangulate_face::delete_tess()
{
   if(m_tess)tessDeleteTess(m_tess);
   m_tess = 0;
}



std::vector<std::vector<size_t>> carve_triangulate_face::compute(const std::vector<size_t>&    vind,   // input face vertex indices (into polyhedron vertex vector)
                                                         const std::vector<carve::geom2d::P2>& vxy)    // computed 2d vertex coordinates
{
   const int polySize   = 3; // defines maximum vertices per polygon (i.e. triangle)
   const int vertexSize = 2; // defines the number of coordinates in tesselation result vertex, must be 2 or 3.

   create_tess();

   std::vector<std::vector<size_t>> tri_faces;

   // add the face vertices to libtess2, coordinates in libtess2 format
   std::vector<TESSreal> coords;
   coords.reserve(vxy.size()*vertexSize);
   for(size_t i=0; i<vxy.size();i++) {
      coords.push_back(static_cast<TESSreal>(vxy[i][0]));
      coords.push_back(static_cast<TESSreal>(vxy[i][1]));
   }
   tessAddContour(m_tess,2,&coords[0],sizeof(TESSreal)*vertexSize,static_cast<int>(vxy.size()));

   // compute the Constrained Delaunay mesh for the face
   TESSreal* normalvec = 0; // normal automatically calculated
   bool success = (1 == tessTesselate(m_tess,TESS_WINDING_ODD,TESS_CONSTRAINED_DELAUNAY_TRIANGLES, polySize, vertexSize, normalvec));
   if(!success) throw std::logic_error("carve_triangulate_face failed");

   // get the tesselation results
   const TESSreal* verts = tessGetVertices(m_tess);
   const int* elems      = tessGetElements(m_tess);
   const int nelems      = tessGetElementCount(m_tess);

   const int nverts      = tessGetVertexCount(m_tess);
   const int* vinds      = tessGetVertexIndices(m_tess);

   // Here, we rely on the fact that no new vertices will be added by TESS
   // We therefore ignore the mesh coordinates in the tesselator output (they are single precision anyway).
   // However, we must use the "vinds" lookup table as the order of the vertices have been changed by the tesselator

   tri_faces.resize(nelems);

   // traverse the tesselator faces and add them to the mesh
   for(int iiel=0; iiel<nelems; iiel++) {

      auto& face_vinds = tri_faces[iiel];
      face_vinds.reserve(3);

      // p = pointer to polygon triangle, each polygon uses polySize*1 indices for TESS_CONSTRAINED_DELAUNAY_TRIANGLES
      const int* p = &elems[iiel*polySize];
      for(int iv=0; iv<polySize; iv++) {

         // ivert is the index in the TESS reorganised vertex sequence
         int ivert = p[iv];

         if(ivert == TESS_UNDEF) break;
         if(ivert >  nverts) break;

         // Extract/adjust the face vertex index.
         // Using the vinds[ivert] lookup, we get index in the coords vertex sequence.
         // using vind[vinds[ivert]], we get the original vertex indices of the polyhedron
         // This way the faces will be referring to the original vertices of the polyhedron
         size_t vertex_index = vind[vinds[ivert]];
         face_vinds.push_back(vertex_index);
      }

   }

   return std::move(tri_faces);
}
