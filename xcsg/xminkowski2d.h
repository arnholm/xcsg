#ifndef xminkowski2d_H
#define xminkowski2d_H

#include "xshape2d.h"

class xminkowski2d : public xshape2d {
public:
   xminkowski2d();
   xminkowski2d(const cf_xmlNode& node);
   virtual ~xminkowski2d();

   virtual size_t nbool();

   std::shared_ptr<clipper_profile> create_clipper_profile(const carve::math::Matrix& t = carve::math::Matrix()) const;
   std::shared_ptr<carve::mesh::MeshSet<3>> create_carve_mesh(const carve::math::Matrix& t = carve::math::Matrix()) const;
protected:

private:
   std::vector<std::shared_ptr<xshape2d>> m_incl;
};

#endif // xminkowski2d_H
