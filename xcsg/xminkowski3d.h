#ifndef XMINKOWSKI3D_H
#define XMINKOWSKI3D_H

#include "xsolid.h"

class xminkowski3d : public xsolid {
public:
   xminkowski3d();
   xminkowski3d(const cf_xmlNode& node);
   virtual ~xminkowski3d();

   virtual size_t nbool();

   std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const;
protected:

private:
   std::list<std::shared_ptr<xsolid>> m_incl;
};

#endif // XMINKOWSKI3D_H
