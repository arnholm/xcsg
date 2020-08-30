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

#include "amf_file.h"
#include "csg_parser/cf_xmlTree.h"
#include <carve/poly.hpp>
#include <ctime>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

amf_file::amf_file()
{
   //ctor
}

amf_file::~amf_file()
{
   //dtor
}

std::string amf_file::write(std::shared_ptr<poly_vector> polyset, const std::string& file_path)
{
   // ISO8601 date and time string of current time
   time_t now = time(0);
   const size_t blen = 80;
   char buffer[blen];
   strftime(buffer,blen,"%Y-%m-%dT%H:%M:%S",gmtime(&now));
   string iso8601(buffer);

   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path amf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = amf_path.string() + ".amf";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   cf_xmlTree tree;
   if(tree.create_root("amf")) {
      cf_xmlNode root;
      if(tree.get_root(root)) {
         root.add_property("unit","millimeter");

         // add some metadata
         cf_xmlNode name      = root.add_child("metadata"); name.add_property("type","name");         name.put_value(fullpath.stem().string());
         cf_xmlNode date_time = root.add_child("metadata"); date_time.add_property("type","created"); date_time.put_value(iso8601);
         cf_xmlNode software  = root.add_child("metadata"); software.add_property("type","software"); software.put_value("xcsg");

         // write one amf "object" per polyhedron
         for(size_t ipoly=0; ipoly<polyset->size(); ipoly++) {
            to_amf_object(root,polyset,ipoly);
         }

         tree.write_xml(path);
      }
   }

   return path;
}


bool amf_file::to_amf_object(cf_xmlNode& parent, std::shared_ptr<poly_vector> polyset, size_t index)
{
   std::shared_ptr<carve::poly::Polyhedron> poly = (*polyset)[index];

   cf_xmlNode object = parent.add_child("object");
   object.add_property("id",index);
   cf_xmlNode mesh = object.add_child("mesh");

   cf_xmlNode vertices = mesh.add_child("vertices");
   for(size_t ivert=0; ivert<poly->vertices.size(); ivert++) {
      carve::poly::Geometry<3>::vertex_t& vtx = poly->vertices[ivert];

      cf_xmlNode coordinates = vertices.add_child("vertex").add_child("coordinates");
      coordinates.add_child("x").put_value(vtx.v[0]);
      coordinates.add_child("y").put_value(vtx.v[1]);
      coordinates.add_child("z").put_value(vtx.v[2]);
   }

   cf_xmlNode volume = mesh.add_child("volume");

   const string vtags[] = { "v1", "v2", "v3" } ;

   for(size_t iface = 0; iface<poly->faces.size(); ++iface) {
      carve::poly::Face<3>& face = poly->faces[iface];

      size_t nvert = face.nVertices();
      if(nvert != 3)throw std::logic_error("amf_file::to_xml, non-triangular face encountered!");

      cf_xmlNode triangle = volume.add_child("triangle");
      std::vector<const carve::poly::Polyhedron::vertex_t *> vloop;
      poly->faces[iface].getVertexLoop(vloop);
      for(size_t ivert=0; ivert<nvert; ivert++) {
         size_t index = poly->vertexToIndex(vloop[ivert]);
         triangle.add_child(vtags[ivert]).put_value(index);
      }
   }

   return true;
}
