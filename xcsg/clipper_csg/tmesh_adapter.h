#ifndef TMESH_ADAPTER_H
#define TMESH_ADAPTER_H

#include "polyset2d.h"
#include "polymesh2d.h"
struct TESStesselator;

// tmesh_adapter manages meshing of 2d polygons and stores the result in a polymesh2d

class tmesh_adapter {
public:
   tmesh_adapter();
   virtual ~tmesh_adapter();

   // tesselate all polygons in the polyset into the same mesh
   bool tesselate(std::shared_ptr<polyset2d> polyset);

   // tesselate a single polygon, add the resulting mesh to the polymesh2d
   bool tesselate(std::shared_ptr<polygon2d> poly);

   // return the contained mesh
   std::shared_ptr<polymesh2d> mesh() { return m_mesh; }

private:
   void create_tess();
   void delete_tess();
   TESStesselator* m_tess;

   std::shared_ptr<polymesh2d> m_mesh;
};

#endif // TMESH_ADAPTER_H
