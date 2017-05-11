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

#include "xcsg_main.h"

#include <sstream>
#include <stdexcept>
using namespace std;
#include "cf_xmlTree.h"

#include "xshape2d.h"

#include "clipper_boolean.h"
#include "carve_boolean.h"
#include "carve_triangulate.h"
#include "mesh_utils.h"
#include "xpolyhedron.h"
#include "xcsg_factory.h"
#include <boost/timer.hpp>
#include "boolean_timer.h"

#include "openscad_csg.h"
#include "out_triangles.h"
#include "amf_file.h"

xcsg_main::xcsg_main(const boost_command_line& cmd)
: m_cmd(cmd)
{}

xcsg_main::~xcsg_main()
{}

bool xcsg_main::run()
{
   if(!m_cmd.parsed_ok())return false;
   if(m_cmd.count("xcsg-file")==0) {
      cout << endl << "Error, missing required input parameter <xcsg-file>" << endl;
      return false;
   }

   std::string xcsg_file;
   try {
      xcsg_file = m_cmd.get<std::string>("xcsg-file");
      std::replace(xcsg_file.begin(),xcsg_file.end(), '\\', '/');
   }
   catch(std::exception& ex) {
      ostringstream sout;
      sout << "xcsg  command line processing error: " << ex.what() << ", please report. "<< endl;
      throw std::logic_error(sout.str());
   }

   cf_xmlTree tree;
   if(tree.read_xml(xcsg_file)) {

      cout << "xcsg processing " << xcsg_file << endl;

      cf_xmlNode root;
      if(tree.get_root(root)) {
         if("xcsg" == root.tag()) {

            // set the global secant tolerance,
            mesh_utils::set_secant_tolerance(root.get_property("secant_tolerance",mesh_utils::secant_tolerance()));

            size_t icount = 0;
            for(auto i=root.begin(); i!=root.end(); i++) {
               cf_xmlNode child(i);
               if(!child.is_attribute_node()) {
                  if(xcsg_factory::singleton().is_solid(child)) {
                     run_xsolid(child,xcsg_file);
                     icount++;
                  }
                  else if(xcsg_factory::singleton().is_shape2d(child)) {
                     run_xshape2d(child,xcsg_file);
                     icount++;
                  }
               }
               if(icount > 0)break;
            }
         }
      }
   }
   else {
      cout << "error: xcsg input file not found: " << xcsg_file << endl;
   }
   return true;
}


bool xcsg_main::run_xsolid(cf_xmlNode& node,const std::string& xcsg_file)
{
   cout << "processing solid: " << node.tag() << endl;
   std::shared_ptr<xsolid> obj = xcsg_factory::singleton().make_solid(node);
   if(obj.get()) {

      int nbool = static_cast<int>(obj->nbool());
      cout << "...completed CSG tree: " <<  nbool << " boolean operations to process." << endl;

      cout << "...starting boolean operations" << endl;

      carve_boolean csg;
      try {
         boost::timer timer;
         boolean_timer::singleton().init(nbool);
         csg.compute(obj->create_carve_mesh(),carve::csg::CSG::OP::UNION);

         double elapsed_sec = timer.elapsed();
         cout << "...completed boolean operations in " <<  setprecision(5) << elapsed_sec<< " [sec] "<< endl;
      }
      catch(carve::exception& ex ) {

         // rethrow as std::exception
         string msg("(carve error): ");
         msg += ex.str();
         cout << "WARNING: " << msg << endl;
//         throw std::exception(msg.c_str());
      }

      size_t nmani = csg.size();
      cout << "...result model contains " << nmani << ((nmani==1)? " lump.": " lumps.") << endl;

      // we export only triangles
      carve_triangulate triangulate;
      for(size_t imani=0; imani<nmani; imani++) {

         // create & check lump
         std::shared_ptr<xpolyhedron> poly = csg.create_manifold(imani);
         cout << "...lump " << imani+1 << ": " <<poly->v_size() << " vertices, " << poly->f_size() << " polygon faces." << endl;

         size_t num_non_tri = 0;
         poly->check_polyhedron(cout,num_non_tri);

         if(num_non_tri > 0) {
            bool improve = true;
            bool canonicalize = true;
            bool degen_check = true;
            cout << "...Triangulating lump ... " << std::endl;
            cout << "...Triangulation completed with " << triangulate.compute(poly->create_carve_polyhedron(),improve,canonicalize,degen_check)<< " triangle faces" << endl;
         }
         else {
            // triangulation not required
            triangulate.add(poly->create_carve_polyhedron());
         }
      }

      cout <<    "...Exporting results " << endl;

      // create object for file export
      out_triangles exporter(triangulate.carve_polyset());

      amf_file amf;
      if(m_cmd.count("csg")>0)       cout << "Created OpenSCAD file: " << exporter.write_csg(xcsg_file) << endl;
      if(m_cmd.count("stl")>0)       cout << "Created STL file     : " << exporter.write_stl(xcsg_file,true) << endl;
      else if(m_cmd.count("astl")>0) cout << "Created STL file     : " << exporter.write_stl(xcsg_file,false) << endl;
      if(m_cmd.count("amf")>0)       cout << "Created AMF file     : " << amf.write(triangulate.carve_polyset(),xcsg_file) << endl;
      if(m_cmd.count("obj")>0)       cout << "Created OBJ file     : " << exporter.write_obj(xcsg_file) << endl;
      if(m_cmd.count("off")>0)       cout << "Created OFF file(s)  : " << exporter.write_off(xcsg_file) << endl;

   }
   else {
      throw logic_error("xcsg tree contains no data. ");
   }
   return true;
}


bool xcsg_main::run_xshape2d(cf_xmlNode& node,const std::string& xcsg_file)
{
   cout << "processing shape2d: " << node.tag() << endl;
   std::shared_ptr<xshape2d> obj = xcsg_factory::singleton().make_shape2d(node);
   if(obj.get()) {

      int nbool = static_cast<int>(obj->nbool());
      cout << "...completed CSG tree: " <<  nbool << " boolean operations to process." << endl;

      cout << "...starting boolean operations" << endl;
      clipper_boolean csg;
      csg.compute(obj->create_clipper_profile(),ClipperLib::ctUnion);

      std::shared_ptr<polyset2d> polyset = csg.profile()->polyset();
      size_t nmani = polyset->size();
      cout << "...result model contains " << nmani << ((nmani==1)? " lump.": " lumps.") << endl;

      openscad_csg openscad(xcsg_file);
      size_t imani = 0;
      for(auto i=polyset->begin(); i!=polyset->end(); i++) {
         std::shared_ptr<polygon2d> poly = *i;
         cout << "...lump " << ++imani << ": " <<poly->size() << " contours " << endl;
         openscad.write_polygon(poly);
      }
      cout << "Created OpenSCAD file: " << openscad.path() << endl;
   }
   else {
      throw logic_error("xcsg tree contains no data. ");
   }
   return true;

}
