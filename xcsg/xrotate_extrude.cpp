// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017 Carsten Arnholm
// All rights reserved
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation and appearing in the files LICENSE.GPL2
// and LICENSE.GPL3 included in the packaging of this file.
// 
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE. ALL COPIES OF THIS FILE MUST INCLUDE THIS LICENSE.
// EndLicense:
   
#include "xrotate_extrude.h"
#include "carve_boolean.h"
#include "mesh_utils.h"
#include "cf_xmlNode.h"
#include "xcsg_factory.h"
#include "extrude_mesh.h"
#include "clipper_boolean.h"
#include "xshape2d_collector.h"

static const double pi = 4.0*atan(1.0);

xrotate_extrude::xrotate_extrude(double angle)
: m_angle(angle)
{}

xrotate_extrude::~xrotate_extrude()
{}

xrotate_extrude::xrotate_extrude(const cf_xmlNode& node)
: m_angle(1.0)
, m_pitch(0.0)
{
   if(node.tag() != "rotate_extrude")throw logic_error("Expected xml tag 'rotate_extrude', but found " + node.tag());
   set_transform(node);

   // apply negative angle to achieve CCW around Y  (in XZ plane)
   m_angle  = -fabs(node.get_property("angle",m_angle));
   m_pitch  = node.get_property("pitch",m_pitch);

   xshape2d_collector::collect_children(node,m_incl);
}

size_t xrotate_extrude::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }
   return nbool-1;
}

std::shared_ptr<carve::mesh::MeshSet<3>> xrotate_extrude::create_carve_mesh(const carve::math::Matrix& t) const
{
   clipper_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      // create profile in native 2d system
      csg.compute((*i)->create_clipper_profile(carve::math::Matrix()),ClipperLib::ctUnion);
   }

   // apply 3d transformation when creating 3d mesh
   return extrude_mesh::rotate_extrude(csg.profile(),m_angle,m_pitch,t*get_transform());
}


/*

size_t xrotate_extrude::nbool()
{
   size_t nbool = 0;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      std::shared_ptr<xshape2d> obj = *i;
      nbool += (obj->nbool()+1);
   }

   // this is just a guesstimate
   double ang_deg = 180.0*fabs(m_angle)/pi;
   nbool += int(ang_deg*1.5);

   return nbool;
}


std::shared_ptr<carve::mesh::MeshSet<3>> xrotate_extrude::create_carve_mesh(const carve::math::Matrix& t) const
{
   carve_boolean csg;
   for(auto i=m_incl.begin(); i!=m_incl.end(); i++) {
      csg.compute((*i)->create_carve_mesh(t*get_transform()),carve::csg::CSG::UNION);
   }

   // do not allow extrusion to self-intersect
   bool full_circle = false;
   double extrude_angle = std::max(m_angle, -2.0*pi + 0.001);
   if(fabs(m_angle) > fabs(extrude_angle)) {
      full_circle = true;
      // extrude exactly half circle, and mirror it afterwards
      extrude_angle = -pi;
   }

   double r = extrude_mesh::evaluate_max_x(csg.mesh_set());
   size_t nseg = 1;
   double alpha = extrude_angle/nseg;
   while(r*(1.0-cos(0.5*alpha)) >  mesh_utils::secant_tolerance()) {
      nseg += 1;
      alpha = extrude_angle/nseg;
   }

   const int z_incr = 1 << 16;

   // produce nseg layers, with tops z_incr units apart
   carve_boolean csg_rotate;
   for(size_t iseg=0; iseg<nseg; iseg++) {
      double dz = double(z_incr*(iseg+1));
      std::shared_ptr<carve::mesh::MeshSet<3>> mesh_segment = extrude_mesh::clone_extrude(csg.mesh_set(),carve::math::Matrix::TRANS(0.0,0.0,dz));
      csg_rotate.compute(mesh_segment,carve::csg::CSG::UNION);
   }

   // topologically correct mesh, wrong coordinates
   std::shared_ptr<carve::mesh::MeshSet<3>> meshset = csg_rotate.mesh_set();

   // fix the coordinates
   size_t nvert =  meshset->vertex_storage.size();
   for(size_t i=0;i<nvert;i++) {
      carve::mesh::MeshSet<3>::vertex_t& vertex = meshset->vertex_storage[i];

      int iseg = int(vertex.v[2]/z_incr);
      if(iseg>0) {
         double angle = extrude_angle*double(iseg)/double(nseg);
         carve::math::Matrix rot_y = carve::math::Matrix();
         rot_y.m[0][0] =  cos(angle);
         rot_y.m[2][0] =  sin(angle);
         rot_y.m[0][2] = -sin(angle);
         rot_y.m[2][2] =  cos(angle);
         vertex = rot_y * carve::geom::VECTOR(vertex.v[0],vertex.v[1],0.0);
      }
   }

   // traverse the meshset and detect open meshes,
   // i.e. with edges that are referenced only once
   size_t nmani = meshset->meshes.size();
   vector<carve::mesh::Mesh<3>* > keep_mesh,delete_mesh;
   for(size_t imani=0; imani<nmani; imani++) {
      carve::mesh::Mesh<3>* mesh = meshset->meshes[imani];
      if(mesh->isClosed()) {

         // this was a closed mesh, so keep it
         keep_mesh.push_back(mesh);
      }
      else {

         // this was an open mesh, so delete
         delete_mesh.push_back(mesh);
      }
   }

   // swap the keepers into the mesh set, and delete the others
   std::swap(keep_mesh, meshset->meshes);
   for(auto mesh : delete_mesh) {
      delete mesh;
   }

   if(full_circle) {

      // apply the current mesh to the morror boolean
      carve_boolean csg_mirror;
      csg_mirror.compute(meshset,carve::csg::CSG::UNION);

      // mirror-clone the mesh
      // also translate a tiny bit up to get the two to become one lump
      carve::math::Matrix& tmirr = carve::math::Matrix::IDENT();
      tmirr.m[2][2] = -1.0;
      carve::math::Matrix& trans = carve::math::Matrix::TRANS(0.0,0.0,1E-11);
      std::shared_ptr<carve::mesh::MeshSet<3>> mesh_mirror = extrude_mesh::clone_transform(csg_mirror.mesh_set(),trans*tmirr);

      // make sure faces are oriented properly in the mirrored version
      size_t nmani = mesh_mirror->meshes.size();
      for(size_t imani=0; imani<nmani; imani++) {
         carve::mesh::Mesh<3>* mesh = mesh_mirror->meshes[imani];
         mesh->invert();
         mesh->recalc();
      }

      // union the two
      csg_mirror.compute(mesh_mirror,carve::csg::CSG::UNION);
      meshset = csg_mirror.mesh_set();
   }

   return meshset;
}

*/
