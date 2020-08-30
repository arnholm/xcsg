// BeginLicense:
// Part of: xcsg - XML based Constructive Solid Geometry
// Copyright (C) 2017-2020 Carsten Arnholm
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

#ifndef XCSG_FACTORY_H
#define XCSG_FACTORY_H

#include <map>
#include <memory>

class xsolid;
class xshape2d;
class cf_xmlNode;

typedef  std::shared_ptr<xsolid> (*solid_factory)(const cf_xmlNode& node);
typedef  std::shared_ptr<xshape2d> (*shape2d_factory)(const cf_xmlNode& node);

class xcsg_factory {
public:
   static xcsg_factory& singleton()
   {
      static xcsg_factory instance;
      return instance;
   }

   // determine category of shape, given an XML node
   bool is_shape2d(const cf_xmlNode& node);
   bool is_solid(const cf_xmlNode& node);

   // factory functions for 2d and 3d objects, given XML node
   std::shared_ptr<xshape2d> make_shape2d(const cf_xmlNode& node);
   std::shared_ptr<xsolid>   make_solid(const cf_xmlNode& node);

protected:
   xcsg_factory();
   virtual ~xcsg_factory();

protected:

   // concrete 3d types
   static std::shared_ptr<xsolid> make_cone(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_cube(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_cuboid(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_cylinder(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_difference3d(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_intersection3d(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_polyhedron(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_sphere(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_tin_model(const cf_xmlNode& node);

   static std::shared_ptr<xsolid> make_union3d(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_hull3d(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_linear_extrude(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_rotate_extrude(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_transform_extrude(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_sweep(const cf_xmlNode& node);
   static std::shared_ptr<xsolid> make_minkowski3d(const cf_xmlNode& node);


   // concrete 2d types
   static std::shared_ptr<xshape2d> make_circle(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_polygon(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_rectangle(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_square(const cf_xmlNode& node);

   static std::shared_ptr<xshape2d> make_difference2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_intersection2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_union2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_hull2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_fill2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_offset2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_minkowski2d(const cf_xmlNode& node);
   static std::shared_ptr<xshape2d> make_projection2d(const cf_xmlNode& node);

private:
   typedef std::map<std::string,solid_factory> solid_factory_map;
   typedef std::map<std::string,shape2d_factory> shape2d_factory_map;

   solid_factory_map    m_solid_map;
   shape2d_factory_map  m_shape2d_map;
};

#endif // XCSG_FACTORY_H
