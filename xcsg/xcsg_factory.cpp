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

#include "cf_xmlNode.h"

#include "xcsg_factory.h"

#include "xcone.h"
#include "xcube.h"
#include "xcuboid.h"
#include "xcylinder.h"
#include "xdifference3d.h"
#include "xintersection3d.h"
#include "xpolyhedron.h"
#include "xsphere.h"
// #include "xtin_model.h"
#include "xunion3d.h"
#include "xhull3d.h"
#include "xlinear_extrude.h"
#include "xrotate_extrude.h"
#include "xtransform_extrude.h"
#include "xsweep.h"
#include "xminkowski3d.h"

#include "xcircle.h"
#include "xpolygon.h"
#include "xrectangle.h"
#include "xsquare.h"
#include "xdifference2d.h"
#include "xintersection2d.h"
#include "xunion2d.h"
#include "xhull2d.h"
#include "xfill2d.h"
#include "xoffset2d.h"

xcsg_factory::xcsg_factory()
{
   m_solid_map.insert(std::make_pair("cone",xcsg_factory::make_cone));
   m_solid_map.insert(std::make_pair("cube",xcsg_factory::make_cube));
   m_solid_map.insert(std::make_pair("cuboid",xcsg_factory::make_cuboid));
   m_solid_map.insert(std::make_pair("cylinder",xcsg_factory::make_cylinder));
   m_solid_map.insert(std::make_pair("difference3d",xcsg_factory::make_difference3d));
   m_solid_map.insert(std::make_pair("intersection3d",xcsg_factory::make_intersection3d));
   m_solid_map.insert(std::make_pair("polyhedron",xcsg_factory::make_polyhedron));
   m_solid_map.insert(std::make_pair("sphere",xcsg_factory::make_sphere));
 // experimental only  m_solid_map.insert(std::make_pair("tin_model",xcsg_factory::make_tin_model));
   m_solid_map.insert(std::make_pair("union3d",xcsg_factory::make_union3d));
   m_solid_map.insert(std::make_pair("hull3d",xcsg_factory::make_hull3d));
   m_solid_map.insert(std::make_pair("linear_extrude",xcsg_factory::make_linear_extrude));
   m_solid_map.insert(std::make_pair("rotate_extrude",xcsg_factory::make_rotate_extrude));
   m_solid_map.insert(std::make_pair("transform_extrude",xcsg_factory::make_transform_extrude));
   m_solid_map.insert(std::make_pair("sweep",xcsg_factory::make_sweep));
   m_solid_map.insert(std::make_pair("minkowski3d",xcsg_factory::make_minkowski3d));

   m_shape2d_map.insert(std::make_pair("circle",xcsg_factory::make_circle));
   m_shape2d_map.insert(std::make_pair("polygon",xcsg_factory::make_polygon));
   m_shape2d_map.insert(std::make_pair("rectangle",xcsg_factory::make_rectangle));
   m_shape2d_map.insert(std::make_pair("square",xcsg_factory::make_square));
   m_shape2d_map.insert(std::make_pair("difference2d",xcsg_factory::make_difference2d));
   m_shape2d_map.insert(std::make_pair("intersection2d",xcsg_factory::make_intersection2d));
   m_shape2d_map.insert(std::make_pair("union2d",xcsg_factory::make_union2d));
   m_shape2d_map.insert(std::make_pair("hull2d",xcsg_factory::make_hull2d));
   m_shape2d_map.insert(std::make_pair("fill2d",xcsg_factory::make_fill2d));
   m_shape2d_map.insert(std::make_pair("offset2d",xcsg_factory::make_offset2d));
}

xcsg_factory::~xcsg_factory()
{}

bool xcsg_factory::is_shape2d(const cf_xmlNode& node)
{
   return (m_shape2d_map.find(node.tag()) != m_shape2d_map.end());
}

bool xcsg_factory::is_solid(const cf_xmlNode& node)
{
   return (m_solid_map.find(node.tag()) != m_solid_map.end());
}

std::shared_ptr<xsolid> xcsg_factory::make_solid(const cf_xmlNode& node)
{
   std::string tag = node.tag();
   auto i=m_solid_map.find(tag);
   if(i != m_solid_map.end()) {
      solid_factory f = i->second;
      return f(node);
   }
   throw logic_error("make_solid: No factory function installed for XML tag " + tag);
   return 0;
}


std::shared_ptr<xsolid> xcsg_factory::make_cone(const cf_xmlNode& node)               { return std::shared_ptr<xsolid>(new xcone(node));           }
std::shared_ptr<xsolid> xcsg_factory::make_cube(const cf_xmlNode& node)               { return std::shared_ptr<xsolid>(new xcube(node));           }
std::shared_ptr<xsolid> xcsg_factory::make_cuboid(const cf_xmlNode& node)             { return std::shared_ptr<xsolid>(new xcuboid(node));         }
std::shared_ptr<xsolid> xcsg_factory::make_cylinder(const cf_xmlNode& node)           { return std::shared_ptr<xsolid>(new xcylinder(node));       }
std::shared_ptr<xsolid> xcsg_factory::make_difference3d(const cf_xmlNode& node)       { return std::shared_ptr<xsolid>(new xdifference3d(node));   }
std::shared_ptr<xsolid> xcsg_factory::make_intersection3d(const cf_xmlNode& node)     { return std::shared_ptr<xsolid>(new xintersection3d(node)); }
std::shared_ptr<xsolid> xcsg_factory::make_polyhedron(const cf_xmlNode& node)         { return std::shared_ptr<xsolid>(new xpolyhedron(node));     }
std::shared_ptr<xsolid> xcsg_factory::make_sphere(const cf_xmlNode& node)             { return std::shared_ptr<xsolid>(new xsphere(node));         }
// std::shared_ptr<xsolid> xcsg_factory::make_tin_model(const cf_xmlNode& node)          { return std::shared_ptr<xsolid>(new xtin_model(node));      }
std::shared_ptr<xsolid> xcsg_factory::make_union3d(const cf_xmlNode& node)            { return std::shared_ptr<xsolid>(new xunion3d(node));        }
std::shared_ptr<xsolid> xcsg_factory::make_hull3d(const cf_xmlNode& node)             { return std::shared_ptr<xsolid>(new xhull3d(node));         }
std::shared_ptr<xsolid> xcsg_factory::make_linear_extrude(const cf_xmlNode& node)     { return std::shared_ptr<xsolid>(new xlinear_extrude(node)); }
std::shared_ptr<xsolid> xcsg_factory::make_rotate_extrude(const cf_xmlNode& node)     { return std::shared_ptr<xsolid>(new xrotate_extrude(node)); }
std::shared_ptr<xsolid> xcsg_factory::make_transform_extrude(const cf_xmlNode& node)  { return std::shared_ptr<xsolid>(new xtransform_extrude(node)); }
std::shared_ptr<xsolid> xcsg_factory::make_sweep(const cf_xmlNode& node)              { return std::shared_ptr<xsolid>(new xsweep(node));          }
std::shared_ptr<xsolid> xcsg_factory::make_minkowski3d(const cf_xmlNode& node)        { return std::shared_ptr<xsolid>(new xminkowski3d(node));    }

std::shared_ptr<xshape2d>  xcsg_factory::make_shape2d(const cf_xmlNode& node)
{
   std::string tag = node.tag();
   auto i=m_shape2d_map.find(tag);
   if(i != m_shape2d_map.end()) {
      shape2d_factory f = i->second;
      return f(node);
   }
   throw logic_error("make_shape2d: No factory function installed for XML tag " + tag);
   return 0;
}

std::shared_ptr<xshape2d> xcsg_factory::make_circle(const cf_xmlNode& node)         { return std::shared_ptr<xshape2d>(new xcircle(node));         }
std::shared_ptr<xshape2d> xcsg_factory::make_polygon(const cf_xmlNode& node)        { return std::shared_ptr<xshape2d>(new xpolygon(node));        }
std::shared_ptr<xshape2d> xcsg_factory::make_rectangle(const cf_xmlNode& node)      { return std::shared_ptr<xshape2d>(new xrectangle(node));      }
std::shared_ptr<xshape2d> xcsg_factory::make_square(const cf_xmlNode& node)         { return std::shared_ptr<xshape2d>(new xsquare(node));         }
std::shared_ptr<xshape2d> xcsg_factory::make_difference2d(const cf_xmlNode& node)   { return std::shared_ptr<xshape2d>(new xdifference2d(node));   }
std::shared_ptr<xshape2d> xcsg_factory::make_intersection2d(const cf_xmlNode& node) { return std::shared_ptr<xshape2d>(new xintersection2d(node)); }
std::shared_ptr<xshape2d> xcsg_factory::make_union2d(const cf_xmlNode& node)        { return std::shared_ptr<xshape2d>(new xunion2d(node));        }
std::shared_ptr<xshape2d> xcsg_factory::make_hull2d(const cf_xmlNode& node)         { return std::shared_ptr<xshape2d>(new xhull2d(node));         }
std::shared_ptr<xshape2d> xcsg_factory::make_fill2d(const cf_xmlNode& node)         { return std::shared_ptr<xshape2d>(new xfill2d(node));         }
std::shared_ptr<xshape2d> xcsg_factory::make_offset2d(const cf_xmlNode& node)       { return std::shared_ptr<xshape2d>(new xoffset2d(node));       }
