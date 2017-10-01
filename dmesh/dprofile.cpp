// BeginLicense:
// Part of: dmesh - Delaunay mesh library
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

#include "dprofile.h"
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <cmath>
using namespace std;

#include "dmesh.h"
#include "dedge.h"
#include "dcoedge.h"
#include "dvertex.h"
#include "dbox2d.h"

// key = vertex id, value = set of edges referring to the vertex
typedef unordered_map<size_t,unordered_set<dedge*>>  vertex_edge_map;

dprofile::dprofile(dmesh* mesh)
: dentity(mesh)
{}

dprofile::~dprofile()
{
   clear();
}

void dprofile::clear()
{
   for(auto loop : m_loops) {
      delete loop;
   }
   m_loops.clear();
}

size_t dprofile::vertex_size() const
{
   size_t nv = 0;
   for(auto loop : m_loops) {
      // number of vertices = number of coedges in a closed loop
      nv += loop->size();
   }
   return nv;
}

static dedge* get_next_edge(vertex_edge_map& ve_map, dedge* cur_edge, size_t iv_cur)
{
   auto i = ve_map.find(iv_cur);
   unordered_set<dedge*>& edges = i->second;
   if(edges.size() != 2) {
      dmesh* mesh = cur_edge->get_mesh();
      const dpos2d& p = mesh->get_vertex(iv_cur)->pos();
      throw logic_error("dprofile::get_next_edge: vertex at (" + std::to_string(p.x()) +','+ std::to_string(p.y()) + ") refers to " + std::to_string(edges.size()) + " edges.");
   }
   for(auto edge : edges) {
      if(edge != cur_edge) return edge;
   }
   throw logic_error("dprofile::get_next_edge: no usable edge found");
}

void dprofile::compute(dmesh* mesh)
{
   clear();

   // first collect all edges with use-count=1 into edge_set
   unordered_set<dedge*> edge_set;

   // the vertex map keeps track of which edges are referring to the current vertex index
   vertex_edge_map ve_map;

   // observe that even though we only consider edges with use count==1, we may still end up with some vertices referring to more than 2 edges
   // This happens when 2 loops share only a single vertex, but no shared edges
   // in theory we could resolve this by starting from the problematic vertex, but if there are multiple cases we still end up in trouble
   for(auto iedge=mesh->edge_begin(); iedge!=mesh->edge_end(); iedge++) {
      dedge* edge = iedge->second;
      if(edge->use_count() == 1) {
         edge_set.insert(edge);
         ve_map[edge->vertex1()].insert(edge);
         ve_map[edge->vertex2()].insert(edge);
      }
   }

   // select an edge and get its 1st vertex
   // traverse the topology until the same vertex is seen again
   while(edge_set.size() > 0) {

      // pick the edge from the set
      auto iedge       = edge_set.begin();
      dedge* edge      = *iedge;
      // keep the start edge
      dedge* edge_begin = edge;

      // get the one and only coedge
      dcoedge* coedge = (*edge->begin());

      size_t ivbegin = coedge->vertex1();
      size_t ivcur   = coedge->vertex2();

      // create and initialise loop
      dloop* loop = new dloop(mesh);
      m_loops.push_back(loop);

      // handle first edge
      edge_set.erase(edge);

      // clone the edge coedge for the loop
      loop->push_back(coedge->clone(loop));

      // traverse connected edges until the first vertex is seen again
      // push each edge visited edge to the back of the loop
      while(edge = get_next_edge(ve_map,edge,ivcur)) {

         // basic runaway prevention
         if(loop->size()>= 32767) {
            throw logic_error("dprofile:: loop is not closed! size=" + std::to_string(loop->size()) );
            break;
         }

         // if we somehow managed to miss the vertex
         if(edge == edge_begin) {
            break;
         }

         // remove current edge from edge set
         edge_set.erase(edge);

         // clone the edge's single coedge for the loop
         dcoedge* coedge = (*edge->begin());
         loop->push_back(coedge->clone(loop));
         ivcur   = coedge->vertex2();

         if(ivcur == ivbegin) {
            break;
         }

      }

      // verify that the loop is closed by comparing vertex1 on the first edge with vertex2 on the last edge
      size_t iv_first = (*loop->begin())->vertex1();
      size_t iv_last  = (*loop->rbegin())->vertex2();
      if(iv_first != iv_last) {
         throw logic_error("dprofile:: loop is not closed! first=" + std::to_string(iv_first) + " last=" + std::to_string(iv_last) );
      }
   }

   // we now have a number of loops each containing proper sequenced and directed coedges
   // we can simply evaluate the signed area of the loops to determine whether they represent
   // material or holes

}

void dprofile::add_loop(const vector<dpos2d>& points)
{
   // make sure supervertioces are properly assigned before proceeding
   size_t nv = get_mesh()->vertex_size();
   if(nv == 0) {
      get_mesh()->reserve_vertices(nv*100);
   }

   // create and initialise new loop in this mesh
   dloop* loop = new dloop(get_mesh());
   m_loops.push_back(loop);

   // create vertices for the loop + maintain a local vertex index vector for the loop
   vector<size_t> vind;
   vind.reserve(points.size());
   for(size_t ipos=0; ipos<points.size(); ipos++) {
      vind.push_back(get_mesh()->add_vertex(points[ipos]));
   }

   // create edges and coedges in same order as vertices, so the coedges are all forward here
   for(size_t i=1; i<vind.size(); i++) {
      size_t iv1 = vind[i-1];
      size_t iv2 = vind[i];
      dedge* edge = get_mesh()->get_create_edge(iv1,iv2);
      loop->push_back(new dcoedge(loop,edge,true));
   }

   // close the loop, this coedge is reversed since it goes from last to first vertex
   size_t iv1 = vind[vind.size()-1];
   size_t iv2 = vind[0];
   dedge* edge = get_mesh()->get_create_edge(iv1,iv2);
   loop->push_back(new dcoedge(loop,edge,false));
}

void dprofile::sort_loops()
{
   // make sure the loop with the largest absolute area comes first in the list of loops

   std::multimap<double,dloop*> sorted_loops;
   for(auto& loop : m_loops) {
      double area = fabs(loop->signed_area());
      if(area > 0) {
         sorted_loops.insert(std::make_pair(area,loop));
      }
   }
   m_loops.clear();
   for(auto& p : sorted_loops) {
      m_loops.push_back(p.second);
   }
}


void  dprofile::export_p2d(ostream& out) const
{
   /*

      2d profile file format *.p2d
      ============================
      p2d <nc> <xlow> <ylow> <xhigh> <yhigh>   <--- number of contour paths + bounding box of profile
      p2d_path <np> <signed_area>              <--- number of points + signed area of path
      x y
      x y
      x y
      p2d_path <np> <signed_area>
      x y
      x y
      x y

   */

   dbox2d box = bounding_box();
   const dpos2d& plo = box.p1();
   const dpos2d& phi = box.p2();

   out << "p2d " << m_loops.size() << ' ' << plo.x() << ' ' << plo.y() << ' ' << phi.x() << ' ' << phi.y() << endl;
   for(auto loop : m_loops) {
      out << "p2d_path " << loop->size() << ' ' << loop->signed_area() << endl;
      vector<dpos2d> points = loop->get_pos(true);
      for(auto p : points) {
         out << p.x() << ' ' << p.y() << endl;
      }
   }
}


void dprofile::import_p2d(istream& in, bool move_to_1st_quadrant)
{
   string line,entity;
   if(!std::getline(in,line)) std::logic_error("dmesh::import_p2d(), error reading p2d header line ");

   istringstream in0(line);
   size_t nc = 0;
   double xlo=0.0, ylo=0.0, xhi=0.0, yhi=0.0;
   in0 >> entity >> nc >> xlo >> ylo >> xhi >> yhi;
   if(entity == "p2d") {
      for(size_t ic=0; ic<nc; ic++) {
         size_t np=0;
         double area = 0.0;

         if(!std::getline(in,line)) std::logic_error("dmesh::import_p2d(), error reading p2d_path header line ");
         istringstream in1(line);
         in1 >> entity >> np >> area;
         if(entity == "p2d_path") {
            vector<dpos2d> points;
            points.reserve(np);
            for(size_t ip=0; ip<np; ip++) {
               if(!std::getline(in,line)) std::logic_error("dmesh::import_p2d(), error reading x,y line ");
               double x,y;
               istringstream inxy(line);
               inxy >> x >> y;
               if(move_to_1st_quadrant) {
                  x += -xlo;
                  y += -ylo;
               }
               points.push_back(dpos2d(x,y));
            }
            add_loop(points);
         }
         else std::logic_error("dmesh::import_p2d(), expected 'p2d_path' but got " + entity);
      }

      return;
   }
   throw std::logic_error("dmesh::import_p2d(), expected 'p2d' but got " + entity);
}

dbox2d dprofile::bounding_box() const
{
   dbox2d box;
   for(auto loop : m_loops) {
      vector<dpos2d> points = loop->get_pos(true);
      for(auto p : points) {
         box.enclose(p);
      }
   }
   return box;
}

