#include "tmesh_adapter.h"
#include <cstdlib>
#include <memory>
#include <map>
#include <cmath>
#include "tmesh/libtess2/Include/tesselator.h"

static void *stdAlloc(void* userData, unsigned int size) {
	TESS_NOTUSED(userData);
	return malloc(size);
}

static void stdFree(void* userData, void* ptr) {
	TESS_NOTUSED(userData);
	free(ptr);
}

tmesh_adapter::tmesh_adapter()
: m_tess(0)
, m_mesh(new polymesh2d())
{}

tmesh_adapter::~tmesh_adapter()
{
   delete_tess();
}

void tmesh_adapter::create_tess()
{
   delete_tess();
   TESSalloc ma;
   memset(&ma, 0, sizeof(ma));
   ma.memalloc = stdAlloc;
   ma.memfree = stdFree;
   ma.extraVertices = 256; // realloc not provided, allow 256 extra vertices.
   m_tess = tessNewTess(&ma);
}

void tmesh_adapter::delete_tess()
{
   if(m_tess)tessDeleteTess(m_tess);
   m_tess = 0;
}

bool tmesh_adapter::tesselate(std::shared_ptr<polyset2d> polyset)
{
   // build a sorted map of contours, largest areas first
   std::multimap<double,std::shared_ptr<contour2d>> sorted_contours;
   for(auto i=polyset->begin(); i!=polyset->end(); i++) {
      std::shared_ptr<polygon2d> poly = *i;
      size_t nc = poly->size();
      for(size_t ic=0;ic<nc;ic++) {
         std::shared_ptr<contour2d> contour = poly->get_contour(ic);
         double area = fabs(contour->signed_area());
         if(area > 0) {
            sorted_contours.insert(std::make_pair(area,contour));
         }
      }
   }

   // then tesselate the contours
   return tesselate_contours(sorted_contours);

}

bool tmesh_adapter::tesselate_contours(ContourMap& contours)
{
   const int polySize   = 3; // defines maximum vertices per polygon (i.e. triangle)
   const int vertexSize = 2; // defines the number of coordinates in tesselation result vertex, must be 2 or 3.

   // make sure tess is initialised
   create_tess();

   // number of vertices along polygon contours
   int n_input_vertices = 0;

   // traverse all contours and add them to the tesselator
   for(auto& cp : contours) {

      // add the contour to the mesh, this will add contour vertices also in the mesh
      std::shared_ptr<contour2d> contour = cp.second;
      m_mesh->add_contour(contour);

      // count the vertices so far
      n_input_vertices += contour->size();

      // add the contour vertices to libtess2, coordinates in libtess2 format
      std::vector<TESSreal> coords;
      coords.reserve(contour->size()*vertexSize);
      for(size_t i=0; i<contour->size();i++) {
         const dpos2d& vtx = (*contour)[i];
         coords.push_back(static_cast<TESSreal>(vtx.x()));
         coords.push_back(static_cast<TESSreal>(vtx.y()));
      }

      // add the contour vertices to the tesselator
      tessAddContour(m_tess,2,&coords[0],sizeof(TESSreal)*vertexSize,static_cast<int>(contour->size()));
   }

   // compute the Constrained Delaunay mesh for the whole profile
   TESSreal* normalvec = 0; // normal automatically calculated
   bool success = (1 == tessTesselate(m_tess,TESS_WINDING_ODD,TESS_CONSTRAINED_DELAUNAY_TRIANGLES, polySize, vertexSize, normalvec));
   if(!success) return false;

   // get the tesselation results
   const TESSreal* verts = tessGetVertices(m_tess);
   const int* elems      = tessGetElements(m_tess);
   const int nelems      = tessGetElementCount(m_tess);

   const int nverts      = tessGetVertexCount(m_tess);
   const int* vinds      = tessGetVertexIndices(m_tess);

   if(n_input_vertices < nverts) {
      // extra vertices have been created
      // this should not happen when we have proper input with no new intersections
      throw std::logic_error("tmesh_adapter:: extra vertices unaccounted for");
   }

   // Here, we rely on the fact that no new vertices will be added by TESS, and the vertices
   // have already been added to the output mesh using "mesh->add_contour(contour)"
   // We therefore ignore the mesh coordinates in the tesselator output (they are single precision anyway).
   // However, we must use the "vinds" lookup table as the order of the vertices have been changed by the tesselator

   // traverse the tesselator faces and add them to the mesh
   for(int iiel=0; iiel<nelems; iiel++) {

      polymesh2d::index_vector face;
      face.reserve(polySize);

      // p = pointer to polygon triangle, each polygon uses polySize*1 indices for TESS_CONSTRAINED_DELAUNAY_TRIANGLES
      const int* p = &elems[iiel*polySize];
      for(int iv=0; iv<polySize; iv++) {

         // ivert is the index in the TESS reorganised vertex sequence
         int ivert = p[iv];

         if(ivert == TESS_UNDEF) break;
         if(ivert >  nverts) break;

         // Extract/adjust the face vertex index.
         // Using the vinds[ivert] lookup, we get index in the input vertex sequence.
         // This way the faces will be referring to the original vertices in m_mesh
         face.push_back(vinds[ivert]);
      }

      // add the completed face to the mesh
      if(face.size() == polySize)m_mesh->add_face(face);
   }

   // tess data structure no longer needed
   delete_tess();

   return true;
}

