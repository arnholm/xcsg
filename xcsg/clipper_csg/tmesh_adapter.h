#ifndef TMESH_ADAPTER_H
#define TMESH_ADAPTER_H

#include <vector>
#include <map>
#include "polyset2d.h"
#include "polymesh2d.h"
struct TESStesselator;

// tmesh_adapter manages meshing of 2d polygons and stores the result in a polymesh2d
// This class uses libtess2 to create a constrained delaunay triangle mesh.
// The libtess2 version used here is the modified version found in https://github.com/openscad/openscad/

class tmesh_adapter {
public:
   tmesh_adapter();
   virtual ~tmesh_adapter();

   // tesselate all polygons in the polyset into the same mesh
   bool tesselate(std::shared_ptr<polyset2d> polyset);

   // return the contained mesh
   std::shared_ptr<polymesh2d> mesh() { return m_mesh; }

private:
   typedef std::multimap<double,std::shared_ptr<contour2d>>  ContourMap;
   bool tesselate_contours(ContourMap& contours);

private:
   void create_tess();
   void delete_tess();
   TESStesselator* m_tess;

   std::shared_ptr<polymesh2d> m_mesh;
};

#endif // TMESH_ADAPTER_H
