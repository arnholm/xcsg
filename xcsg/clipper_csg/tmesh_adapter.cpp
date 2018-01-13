#include "tmesh_adapter.h"
#include <cstdlib>
#include <memory>
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
   for(auto i=polyset->begin(); i!=polyset->end(); i++) {
      std::shared_ptr<polygon2d> poly = *i;
      if(!tesselate(poly))return false;
   }
   return true;
}


bool tmesh_adapter::tesselate(std::shared_ptr<polygon2d> poly)
{
   int polySize        = 3; // defines maximum vertices per polygons if output is polygons.
   int vertexSize      = 2; // defines the number of coordinates in tesselation result vertex, must be 2 or 3.

   // make sure tess is initialised
   create_tess();

   // number of vertices in mesh before this tesselation.
   // this will be used as offset
   size_t v_offset = m_mesh->nvertices();

   // number of vertices along polygon contours
   size_t n_input_vertices = 0;

   // the following loop defines the vertex sequence
   // N1 vertices from first contour
   // N2 vertices from second contour ... etc
   // the mesh vertices will be remapped to match this sequence

   size_t ncontour = poly->size();
   for(size_t icontour=0; icontour<ncontour; icontour++) {

      // add the contour to the mesh, this will add vertices also in the mesh
      std::shared_ptr<const contour2d> contour = (*poly)[icontour];
      m_mesh->add_contour(contour);

      // count the vertices for this contour
      n_input_vertices += contour->size();

      // contour coordinates in libtess2 format
      std::vector<TESSreal> coords;
      coords.reserve(contour->size()*vertexSize);
      for(size_t i=0; i<contour->size();i++) {
         const dpos2d& vtx = (*contour)[i];
         coords.push_back(static_cast<TESSreal>(vtx.x()));
         coords.push_back(static_cast<TESSreal>(vtx.y()));
      }

      // add the contour to the tesselator
      tessAddContour(m_tess,2,&coords[0],sizeof(TESSreal)*vertexSize,static_cast<int>(contour->size()));
   }

   TESSreal* normalvec = 0; // normal automatically calculated
   bool success = (1 == tessTesselate(m_tess,TESS_WINDING_ODD,TESS_CONSTRAINED_DELAUNAY_TRIANGLES, polySize, vertexSize, normalvec));
   if(!success) return false;

   // get the tesselation results
   const TESSreal* verts = tessGetVertices(m_tess);
   const int* elems      = tessGetElements(m_tess);
   const int nelems      = tessGetElementCount(m_tess);

   const int nverts      = tessGetVertexCount(m_tess);
   const int* vinds      = tessGetVertexIndices(m_tess);

   if(n_input_vertices != nverts) {
      // this should not happen when we hae proper input with no new intersections
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

         // Extract/adjust the face vertex indicies.
         // using the vinds[ivert] lookup, we get indicies in the input vertex sequence for the current polygon.
         // We must also offset for previously existing verticies
         face.push_back(v_offset + vinds[ivert]);
      }

      // add the completed face to the mesh
      if(face.size() == 3)m_mesh->add_face(face);
   }

   // tess data structure no longer needed
   delete_tess();

   return true;
}
