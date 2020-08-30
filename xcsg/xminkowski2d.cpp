#include "xminkowski2d.h"
#include "csg_parser/cf_xmlNode.h"
#include "xshape2d_collector.h"

#include "clipper_boolean.h"
#include "boolean_timer.h"

xminkowski2d::xminkowski2d()
{}

xminkowski2d::xminkowski2d(const cf_xmlNode& node)
{
   if(node.tag() != "minkowski2d")throw logic_error("Expected xml tag minkowski2d, but found " + node.tag());
   set_transform(node);
   xshape2d_collector::collect_children(node,m_incl);

   if(m_incl.size() != 2) throw logic_error("Expected 2 parameters for minkowski2d, but got " + std::to_string(m_incl.size()));
}


xminkowski2d::~xminkowski2d()
{}

size_t xminkowski2d::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}


std::shared_ptr<clipper_profile> xminkowski2d::create_clipper_profile(const carve::math::Matrix& t) const
{
   std::shared_ptr<clipper_profile> a       = m_incl[0]->create_clipper_profile(t*get_transform());
   std::shared_ptr<clipper_profile> b_brush = m_incl[1]->create_clipper_profile(t*get_transform());

   clipper_boolean csg;
   csg.minkowski_sum(a,b_brush);
   return csg.profile();
}

std::shared_ptr<carve::mesh::MeshSet<3>> xminkowski2d::create_carve_mesh(const carve::math::Matrix& t) const
{
   std::shared_ptr<carve::mesh::MeshSet<3>> mesh;

   return mesh;
}
