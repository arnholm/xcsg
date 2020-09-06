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

#include <iostream>
#include <stdio.h>
#include <stdexcept>
#include <cmath>

#include "csg_node.h"
#include "csg_scalar.h"
#include "csg_vector.h"

static const double pi = 4.0*atan(1.0);


inline void tokenize(const std::string& input,
                     const std::string& delimiters,
                     std::vector<std::string>& tokens)
{
   using namespace std;
   string::size_type last_pos = 0;
   string::size_type pos = 0;
   while(true) {
      pos = input.find_first_of(delimiters, last_pos);
      if( pos == string::npos ) {
         if(input.length()-last_pos > 0)tokens.push_back(input.substr(last_pos));
         break;
      }
      else {
         if(pos-last_pos > 0)tokens.push_back(input.substr(last_pos, pos - last_pos));
         last_pos = pos + 1;
      }
   }
}


csg_node::xmap csg_node::m_xmap;

void csg_node::configure_xmap()
{
   if(m_xmap.size() == 0) {

      //    openscad              xcsg
      m_xmap["cube"]           = "cuboid";
      m_xmap["cylinder"]       = "cone";
      m_xmap["polyhedron"]     = "polyhedron";
      m_xmap["sphere"]         = "sphere";

//      m_xmap["linear_extrude"] = "linear_extrude";
      m_xmap["linear_extrude"] = "sweep";
      m_xmap["rotate_extrude"] = "rotate_extrude";
      m_xmap["group"]          = "union*";
      m_xmap["union"]          = "union*";
      m_xmap["color"]          = "union*";
      m_xmap["multmatrix"]     = "union*";
      m_xmap["render"]         = "union*";
      m_xmap["difference"]     = "difference*";
      m_xmap["intersection"]   = "intersection*";
      m_xmap["hull"]           = "hull*";
      m_xmap["minkowski"]      = "minkowski*";

      m_xmap["circle"]         = "circle";
      m_xmap["polygon"]        = "polygon";
      m_xmap["square"]         = "rectangle";
      m_xmap["offset"]         = "offset2d";
      m_xmap["projection"]     = "projection2d";

      // This will generate suitable "not implemented" error message
      m_xmap["import"]         = "N/A";
      m_xmap["surface"]        = "N/A";
      m_xmap["text"]           = "N/A";
      m_xmap["resize"]         = "N/A";

   }
}

csg_node::csg_node()
: m_level(-1)
, m_line_no(0)
, m_func("root()")
, m_has_matrix(false)
{}

csg_node::csg_node(size_t level, size_t line_no, const std::string& func)
: m_level(int(level))
, m_line_no(line_no)
, m_func(func)
, m_has_matrix(false)
{
   parse_params();
}

csg_node::~csg_node()
{}

std::string csg_node::tag() const
{
   size_t index = m_func.find_first_of('(');
   return m_func.substr(0,index);
}

std::string csg_node::par() const
{
   size_t index = m_func.find_first_of('(');
   return m_func.substr(index);
}

std::string csg_node::par_name(size_t ipos)
{
   const size_t len=32;
   char buffer[len];
   sprintf(buffer,"_p%03d", static_cast<int>(ipos));
   return std::string(buffer);
}

void csg_node::push_back(std::shared_ptr<csg_node> child)
{
   m_children.push_back(child);
}

void csg_node::build_tree(const std::vector<func_data>& func, size_t& index)
{
   while(index < func.size()) {
      const func_data& f = func[index];
      auto& p = f.second;
      int level = int(p.first);
      size_t line_no = p.second;
      if(level == m_level+1){
         std::shared_ptr<csg_node> child = std::make_shared<csg_node>(level,line_no,f.first);
         m_children.push_back(child);
         child->build_tree(func,++index);
      }
      else break;
   }
}

void csg_node::parse_params()
{
   std::string type   = tag();
   std::string params = par();

   // first get rid of the enclosing parentheses
   std::vector<std::string> tokens;
   tokenize(params,"()",tokens);
   if(tokens.size() > 0) {

      size_t iparam = 0; // parameter counter
      params = tokens[0];

      // we now have  name1=value1,name2=name2,... where values can be (nested) vectors
      // but note that in some few cases, the name is missing (multmatrix)
      while(params.size() > 0) {
         size_t ieq = params.find_first_of('=');
         std::string name;
         if(ieq==std::string::npos) {
            name = par_name(iparam);  // nameless parameter
            ieq = 0;
         }
         else {
            // increment ieq to eat equal sign
            name = params.substr(0,ieq++);
         }

         // extract value, parse it and assign it to parameter map
         std::string value_str = par_value(params,ieq);
         std::shared_ptr<csg_value> value = csg_value::parse(value_str,m_line_no);
         if(value.get()) m_par[name] = value;

         // truncate the parameter list from left
         size_t len = name.length()+1 + value_str.length()+1;
         params = (len <= params.size()) ? params.substr(len) : "";
      }
   }
}

// find the next value substring in the parameter list
// Make sure to account for (nested) vectors using [] characters
// we essentially search for end of vector ']', next comma or end of string
// if we find a comma, we replace it with a blank
std::string csg_node::par_value(std::string& param, size_t istart)
{
   std::string value;
   size_t inside = 0;
   for(size_t i=istart; i<param.size(); i++) {
      char c = param[i];
      if(c == '[') inside++;        // vector begins
      if(c == ',' && inside==0) {   // next parameter
         param[i] = ' ';
         break;
      }
      if(c == ']') {                // vector ends
         inside--;
         if(inside==0) {            // outer vector ends
            value += c;
            break;
         }
      }
      value += c;
   }
   return value;
}

void  csg_node::dump()
{
   for(int i=0; i<m_level; i++) std::cout << ' ';
   std::cout << tag();
   for(auto& p : m_par) {
      std::cout << ' ' << p.first << '=';
      std::shared_ptr<csg_value> value = p.second;
      size_t n = value->size();
      if( n == 1)  std::cout << value->to_string() << ' ';
      else {
         for(size_t i=0; i<n; i++) std::cout << value->get(i)->to_string() << ' ';
      }
   }
   std::cout << std::endl;

   for(auto& c : m_children) c->dump();
}

std::string  csg_node::get_scalar(const std::string& name)
{
   auto i = m_par.find(name);
   if(i !=m_par.end()) {
      return i->second->to_string();
   }
   std::string line_no = ".csg file line " + std::to_string(m_line_no);
   throw std::runtime_error("csg_node::get_scalar(), "+line_no +" parameter '" + name + "' not found for " + tag());
}

std::shared_ptr<csg_value> csg_node::get_value(const std::string& name)
{
   auto i = m_par.find(name);
   if(i !=m_par.end()) {
      return i->second;
   }
   std::string line_no = ".csg file line " + std::to_string(m_line_no);
   throw std::runtime_error("csg_node::get_value(), "+line_no +" parameter '" + name + "' not found for " + tag());
}


void csg_node::assign_matrix()
{
   std::shared_ptr<csg_value> matrix = m_par[par_name(0)];

   if(matrix->size() != 4) throw std::runtime_error("csg_node::assign_matrix(), multimatrix size != 4 ");

   for(size_t i=0; i<4; i++) {
      auto row = matrix->get(i);
      if(row->size() != 4) throw std::runtime_error("csg_node::assign_matrix(), multimatrix row size != 4 ");
      for(size_t j=0; j<4; j++) {
         m_matrix(i,j) = row->get(j)->to_double();
      }
   }

   m_has_matrix = true;
}

void csg_node::to_xcsg(cf_xmlNode& target, csg_matrix<4,4>& matrix)
{
   // assign transformation to this xml object
   cf_xmlNode xml_this = target.add_child("tmatrix");
   for(size_t irow=0; irow<4; irow++) {
      cf_xmlNode xml_row = xml_this.add_child("trow");
      for(size_t icol=0; icol<4; icol++) {
         ostringstream out;
         out << 'c' << icol;
         xml_row.add_property(out.str(),matrix(irow,icol));
      }
   }
}

size_t csg_node::dimension()
{
   // figure out if this node generates a 2d or 3d object
   // by inspecting child nodes

   if(m_children.size()==0) return 0;

   size_t dim = 0;

   for(auto& c : m_children) {

      if(c->tag() == "circle")dim=2;
      else if(c->tag() == "square")dim=2;
      else if(c->tag() == "polygon")dim=2;
      else if(c->tag() == "projection")dim=2;

      else if(c->tag() == "sphere")dim=3;
      else if(c->tag() == "cylinder")dim=3;
      else if(c->tag() == "cube")dim=3;
      else if(c->tag() == "polyhedron")dim=3;
      else if(c->tag() == "linear_extrude")dim=3;
      else if(c->tag() == "rotate_extrude")dim=3;

      else if(c->tag() == "group")dim= c->dimension();
      else if(c->tag() == "color")dim= c->dimension();
      else if(c->tag() == "multmatrix")dim= c->dimension();
      else if(c->tag().substr(0,4) == "unio")dim= c->dimension();
      else if(c->tag().substr(0,4) == "diff")dim= c->dimension();
      else if(c->tag().substr(0,4) == "inte")dim= c->dimension();
      else if(c->tag().substr(0,4) == "mink")dim= c->dimension();
      else if(c->tag().substr(0,4) == "offs")dim= c->dimension();
      else if(c->tag().substr(0,4) == "rend")dim= c->dimension();
      else if(c->tag().substr(0,4) == "hull")dim= c->dimension();

      else if(c->tag() == "text") throw  std::runtime_error("OpenSCAD csg: 'text' is not supported ");
      else if(c->tag() == "surface") throw  std::runtime_error("OpenSCAD csg: 'surface' is not supported ");
      else if(c->tag() == "import") throw  std::runtime_error("OpenSCAD csg: 'import' is not supported ");
      else if(c->tag() == "resize") throw  std::runtime_error("OpenSCAD csg: 'resize' is not supported ");

      if(dim>0)return dim;
   }

   return dim;
}

std::string  csg_node::fix_tag(const std::string& tag)
{
   size_t index = tag.find_first_of("*");
   if(index != std::string::npos) {
      std::string newtag = tag.substr(0,tag.length()-1);
      switch(dimension()) {
         case 2: { newtag += "2d"; break; }
         case 3: { newtag += "3d"; break; }
         default: { newtag = tag; }
      }
      return newtag;
   }
   return tag;
}

cf_xmlNode csg_node::to_xcsg(cf_xmlNode& parent)
{
   cf_xmlNode xml_this;
   if(m_level == -1) {

      // always add root as union, since csg files can sometimes have multiple roots
      std::string root_tag = fix_tag("union*");
      xml_this = parent.add_child(root_tag);

      for(auto& c : m_children) c->to_xcsg(xml_this);
   }
   else {

      std::string line_no = ".csg file line " + std::to_string(m_line_no);

      // get the openscad tag
      std::string openscad_tag = tag();
      if(openscad_tag == "group" && dimension()==0) return xml_this;

      // first check for special cases
      if(openscad_tag == "multmatrix") {
         assign_matrix();
      }

      auto it = m_xmap.find(openscad_tag);
      if(it != m_xmap.end()) {

         std::string xcsg_tag = fix_tag(it->second);
         if(xcsg_tag.find('*') == std::string::npos) {

            // we have determined the xcsg tag, so create the xcsg node
            xml_this = parent.add_child(xcsg_tag);

            if(xcsg_tag=="circle") {
               // == 2d circle
               xml_this.add_property("r",get_scalar("r"));
            }
            else if(xcsg_tag=="rectangle") {
               // == 2d square/rectangle

               // size can be scalar or vector
               auto siz = get_value("size");
               if(siz->size() > 1) {
                  xml_this.add_property("dx",siz->get(0)->to_string());
                  xml_this.add_property("dy",siz->get(1)->to_string());
               }
               else {
                  xml_this.add_property("dx",siz->to_string());
                  xml_this.add_property("dy",siz->to_string());
               }
               xml_this.add_property("center",get_scalar("center") );
            }
            else if(xcsg_tag=="polygon") {
               // == 2d polygon

               auto points = get_value("points");
               size_t np = points->size();
               vector<size_t> path(np);
               for(size_t ip=0; ip<np; ip++) path[ip]=ip;

               auto paths = get_value("paths");
               if(paths.get()) {
                  if(paths->is_vector()) {
                     // we allow max one specified path (=outer path)
                     if(paths->size()==1) {
                        auto outer_path = paths->get(0);
                        path.clear();
                        np = outer_path->size();
                        path.resize(np);
                        for(size_t ip=0; ip<np; ip++) path[ip]=outer_path->get(ip)->to_int();
                     }
                     else {
                        throw std::runtime_error(line_no +": polygon with internal hole(s) is not supported: " + m_func);
                     }
                  }
               }

               cf_xmlNode xml_vertices = xml_this.add_child("vertices");
               for(size_t ip=0; ip<np; ip++) {
                  auto point = points->get(path[ip]);
                  cf_xmlNode xml_vertex = xml_vertices.add_child("vertex");
                  xml_vertex.add_property("x",point->get(0)->to_string());
                  xml_vertex.add_property("y",point->get(1)->to_string());
               }
            }
            else if(xcsg_tag=="offset2d") {
               // == 2d offset

               auto ir  = m_par.find("r");
               auto id  = m_par.find("d");
               auto ich = m_par.find("chamfer");
               // translate the offset parameters to xcg
               double delta        = (ir  != m_par.end())? ir->second->to_double() : id->second->to_double();
               std::string round   = (ir  != m_par.end())? "true" : "false";
               std::string chamfer = (ich != m_par.end())? ich->second->to_string() : "false";
               xml_this.add_property("delta",delta);
               xml_this.add_property("round",round);
               xml_this.add_property("chamfer",chamfer);
            }
            else if(xcsg_tag=="cone") {
               // == 3d cylinder/cone

               xml_this.add_property("h",get_scalar("h"));
               xml_this.add_property("r1",get_scalar("r1"));
               xml_this.add_property("r2",get_scalar("r2"));
               xml_this.add_property("center",get_scalar("center") );
            }
            else if(xcsg_tag=="sphere") {
               // ==3d sphere

               xml_this.add_property("r",get_scalar("r"));
            }
            else if(xcsg_tag=="cuboid") {
               // == 3d cube/cuboid

               auto siz = get_value("size");
               if(siz->size() > 1) {
                  xml_this.add_property("dx",siz->get(0)->to_string());
                  xml_this.add_property("dy",siz->get(1)->to_string());
                  xml_this.add_property("dz",siz->get(2)->to_string());
               }
               else {
                  xml_this.add_property("dx",siz->to_string());
                  xml_this.add_property("dy",siz->to_string());
                  xml_this.add_property("dz",siz->to_string());
               }
               xml_this.add_property("center",get_scalar("center") );
            }
            else if(xcsg_tag=="linear_extrude") {
               // == 3d linear extrude

               double twist = 0.0;
               auto itw = m_par.find("twist");
               if(itw !=m_par.end()) twist = itw->second->to_double();
               if(fabs(twist) > 0.0)  throw std::runtime_error(line_no +": linear_extrude with non-zero twist is not supported: " + m_func);

               xml_this.add_property("dz",get_scalar("height"));
               xml_this.add_property("center",get_scalar("center") );
            }
            else if(xcsg_tag=="sweep") {
               // == linear extrude translated to sweep, here non-zero twist is supported

               double dz = get_value("height")->to_double();
               auto itwi = m_par.find("twist");
               double tw = (itwi != m_par.end())? -itwi->second->to_double()*pi/180. : 0.0;
               auto icen = m_par.find("center");
               std::string center = (icen != m_par.end())? icen->second->to_string() : "false";

               // check if scale is specified, i.e. top surface scaling relative to bottom
               double scx=1.0,scy=1.0;
               auto sc_value = get_value("scale");
               if(sc_value.get()) {
                  if(sc_value->is_vector()) {
                     scx = sc_value->get(0)->to_double();
                     scy = sc_value->get(1)->to_double();
                  }
                  else {
                     scx = sc_value->to_double();
                     scy = scx;
                  }
               }

               // bottom control point
               double x=0.0,y=0.0,z=0.0, vx0=0.0,vy0=1.0,vz0=0.0;

               // drop z point if center=true
               if(center == "true") z = -dz*0.5;

               // compute number of required segments for the sweep
               // with no twist we use only one segment.
               // with non-zero twist we compute number of spline control points from the twist angle
               size_t nseg = 1;
               if(fabs(tw) > 0) nseg = static_cast<size_t>(36*fabs(tw)/(2*pi));
               dz         = dz/nseg;
               double da  = tw/nseg;

               // delta scaling
               double dscx = (scx-1.0)/nseg;
               double dscy = (scy-1.0)/nseg;
               scx = 1.0;
               scy = 1.0;

               cf_xmlNode xml_sweep_path = xml_this.add_child("spline_path");

               // bottom control point
               double angle = 0.0;  // bottom angle
               cf_xmlNode xml_p0 = xml_sweep_path.add_child("cpoint");
               xml_p0.add_property("x",x);
               xml_p0.add_property("y",y);
               xml_p0.add_property("z",z);
               xml_p0.add_property("vx",vx0);
               xml_p0.add_property("vy",vy0);
               xml_p0.add_property("vz",vz0);

               // other control points
               for(size_t iseg=0; iseg<nseg; iseg++) {

                  z     += dz;
                  angle += da;
                  scx   += dscx;
                  scy   += dscy;
                  double sa  = sin(angle);
                  double ca  = cos(angle);
                  double vx1 = ca*vx0 - sa*vy0;
                  double vy1 = sa*vx0 + ca*vy0;

                  cf_xmlNode xml_p = xml_sweep_path.add_child("cpoint");
                  xml_p.add_property("x",x);
                  xml_p.add_property("y",y);
                  xml_p.add_property("z",z);
                  xml_p.add_property("vx",vx1*scx);
                  xml_p.add_property("vy",vy1*scy);
                  xml_p.add_property("vz",vz0);
               }
            }
            else if(xcsg_tag=="rotate_extrude") {
               // == 3d rotate_extrude

               auto iangle = m_par.find("angle");
               xml_this.add_property("angle",iangle->second->to_double()*pi/180);

               // special +90 deg rotate around x applied here since
               // openscad's rotate_extrude implies +90 deg rotate around x after extrusion
               csg_matrix<4,4> rotx;
               rotx(1,1)=0;
               rotx(1,2)=-1;
               rotx(2,1)=1;
               rotx(2,2)=0;
               if(m_has_matrix) m_matrix = csg_matrix_mult<4,4,4>(rotx,m_matrix);
               else             m_matrix = rotx;
               m_has_matrix = true;

            }
            else if(xcsg_tag=="polyhedron") {
               // 3d polyhedron

               auto points = get_value("points");
               size_t np    = points->size();
               if(np<4)  throw std::runtime_error(line_no + ": polyhedron with too few points: "+ par());
               cf_xmlNode xml_vertices = xml_this.add_child("vertices");
               for(size_t ip=0; ip<np; ip++) {
                  cf_xmlNode xml_vertex = xml_vertices.add_child("vertex");
                  auto point = points->get(ip);
                  if(point->size()<3)  throw std::runtime_error(line_no +": polyhedron points must have 3 values: "+ par());
                  xml_vertex.add_property("x",point->get(0)->to_string());
                  xml_vertex.add_property("y",point->get(1)->to_string());
                  xml_vertex.add_property("z",point->get(2)->to_string());
               }

               // Handle face list with variable number of vertices
               auto faces  = get_value("faces");
               if(faces.get()) {
                  cf_xmlNode xml_faces = xml_this.add_child("faces");
                  size_t nf = faces->size();
                  for(size_t iface=0; iface<nf; iface++) {
                     auto face = faces->get(iface);
                     cf_xmlNode xml_face = xml_faces.add_child("face");
                     size_t nfv=face->size();
                     if(nfv<3)  throw std::runtime_error(line_no +": polyhedron face must have 3 or more values: "+ par());
                     for(size_t ifv=0; ifv<nfv; ifv++) {
                        cf_xmlNode xml_fv = xml_face.add_child("fv");
                        // openscad face vertex order is reversed, so fix it
                        xml_fv.add_property("index",face->get(nfv-ifv-1)->to_string());
                     }
                  }
               }
            }
            else if(xcsg_tag.substr(0,4)=="unio" ||
                    xcsg_tag.substr(0,4)=="diff" ||
                    xcsg_tag.substr(0,4)=="inte" ||
                    xcsg_tag.substr(0,4)=="proj" ||
                    xcsg_tag.substr(0,4)=="mink" ||
                    xcsg_tag.substr(0,4)=="hull"
                    )
            {
               // NO-OP . These operations take no parameters
            }
            else {
               throw std::runtime_error(line_no +": Not supported : '" + openscad_tag + "' --> " + xcsg_tag + ": " + m_func);
            }

            // apply transform
            if(m_has_matrix) to_xcsg(xml_this,m_matrix);
         }
         else {
             throw std::runtime_error(line_no+": OpenSCAD node dimension could not be determined:" + openscad_tag + " --> " + xcsg_tag + ": " + m_func);
         }
      }

      // proceed to children
      for(auto& c : m_children) {
         cf_xmlNode xml_child = c->to_xcsg(xml_this);
      }
   }

   return xml_this;
}
