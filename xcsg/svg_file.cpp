#include "svg_file.h"

#include <sstream>
#include <iomanip>
#include "clipper_csg/polyset2d.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include "cf_xmlTree.h"

svg_file::svg_file()
{
   //ctor
}

svg_file::~svg_file()
{
   //dtor
}

dpos2d svg_file::to_svg(const dpos2d& p, const dbox2d& box)
{
   // SVG has origin at top left
   // adjust x and y so all values are positive
   return dpos2d(p.x()-box.p1().x(),box.p2().y() - p.y());
}

std::string svg_file::write( std::shared_ptr<polyset2d> polyset, const std::string& file_path)
{
   boost::filesystem::path fullpath(file_path);
   boost::filesystem::path dxf_path = fullpath.parent_path() / fullpath.stem();
   std::string path = dxf_path.string() + ".svg";

   // fix inconsistent slashes to something that is consistent and works everytwhere
   std::replace(path.begin(),path.end(), '\\', '/');

   // get bounding box of this polyset
   dbox2d box = polyset->bounding_box();
   dpos2d p1  = box.p1();
   dpos2d p2  = box.p2();
   double dx  = p2.x() - p1.x();
   double dy  = p2.y() - p1.y();

   cf_xmlTree tree;
   if(tree.create_root("svg")) {
      cf_xmlNode root;
      if(tree.get_root(root)) {

         // create some margin space for the viewBox
         double mx = dx*0.03;
         double my = dy*0.03;

        // model bounding box, rounded up to nearest mm
        // By NOT including these properties, the model will autofit to the canvas, e.g. in a browser
        //
        //  root.add_property("width",std::to_string(int(dx+2*mx))+"mm");
        //  root.add_property("height",std::to_string(int(dy+2*my))+"mm");

         dpos2d p = to_svg(p1,box);
         std::ostringstream box_out;
         box_out << -mx  << ' ' << -my << ' ' << dx+mx  << ' ' << dy+my;
         root.add_property("viewBox",box_out.str());

         // bureaucracy
         root.add_property("xmlns","http://www.w3.org/2000/svg");
         root.add_property("version","1.1");

         // marketing
         cf_xmlNode title = root.add_child("title");
         title.put_value("xcsg: " +fullpath.stem().string());

         // Create the model data, written as a giant "path", containing several closed contours with "pen-ups" between them.
         // This loop creates the value of the "d" property
         std::string svg_path;
         for(auto i=polyset->begin(); i!=polyset->end(); i++) {
            std::shared_ptr<polygon2d> poly = *i;
            size_t nc = poly->size();
            for(size_t ic=0;ic<nc;ic++) {
               svg_path += write_contour(poly->get_contour(ic),box);
            }
         }

         // create the one and only path tag, with properties and model data.

         // set a stroke width that is adapted to the model size
         ostringstream stroke_out;
         stroke_out << std::setprecision(3) << (dx+dy)/1000;

         cf_xmlNode path_tag = root.add_child("path");

         // properties
         path_tag.add_property("stroke","black");
         path_tag.add_property("stroke-width",stroke_out.str());
         path_tag.add_property("fill","lightgray");

         // model data
         path_tag.add_property("d",svg_path);

         // write SVG to file
         tree.write_xml(path);
      }
   }

   return path;
}


std::string svg_file::write_contour(std::shared_ptr<contour2d> contour, const dbox2d& box)
{
   std::ostringstream out;
   for(size_t i=0; i<contour->size();i++) {
      const dpos2d& vtx = (*contour)[i];
      dpos2d p = to_svg(vtx,box);
      if(i==0) out << " M ";
      else     out << " L ";
      out << p.x() << ',' << p.y();
   }
   out << " z";
   return out.str();
}
