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

#include "primitives3d.h"
#include "geodesic_sphere.h"
using namespace std;

static const double pi = 4.0*atan(1.0);

std::shared_ptr<xpolyhedron>  primitives3d::make_cuboid(double dx, double dy, double dz,  bool center_xy, bool center_z, const carve::math::Matrix& t)
{
   // the cuboid is in 1st quadrant by default, unit length
   // if requested we must center it

   double tdx = (center_xy)? -0.5*dx : 0.0;
   double tdy = (center_xy)? -0.5*dy : 0.0;
   double tdz = (center_z)?  -0.5*dz : 0.0;

   carve::math::Matrix tloc = carve::math::Matrix::TRANS(tdx,tdy,tdz);

   // scale up the cuboid and apply the user transform
//   tloc = t * carve::math::Matrix::SCALE(dx,dy,dz) * tloc;
   tloc = t * tloc;

   bool reverse_face = mesh_utils::is_left_hand(tloc);

   std::shared_ptr<xpolyhedron> poly(new xpolyhedron());
   poly->v_reserve(8);
   poly->f_reserve(6);

   // bottom
   poly->v_add(tloc * carve::geom::VECTOR( 0.0,  0.0,  0.0));
   poly->v_add(tloc * carve::geom::VECTOR( +dx,  0.0,  0.0));
   poly->v_add(tloc * carve::geom::VECTOR( +dx,  +dy,  0.0));
   poly->v_add(tloc * carve::geom::VECTOR( 0.0,  +dy,  0.0));

   // top
   poly->v_add(tloc * carve::geom::VECTOR( 0.0,  0.0,  +dz));
   poly->v_add(tloc * carve::geom::VECTOR( +dx,  0.0,  +dz));
   poly->v_add(tloc * carve::geom::VECTOR( +dx,  +dy,  +dz));
   poly->v_add(tloc * carve::geom::VECTOR( 0.0,  +dy,  +dz));

   // sides
   poly->f_add( xface(0, 1, 5, 4),reverse_face );
   poly->f_add( xface(1, 2, 6, 5),reverse_face );
   poly->f_add( xface(2, 3, 7, 6),reverse_face );
   poly->f_add( xface(3, 0, 4, 7),reverse_face );

   // top
   poly->f_add( xface(4, 5, 6, 7),reverse_face );
   // bottom
   poly->f_add( xface(3 ,2 ,1 ,0),reverse_face );

   return poly;
}


std::shared_ptr<xpolyhedron>  primitives3d::make_cone(double r1, double r2, double height, bool center, int nseg, const carve::math::Matrix& t)
{
   if(nseg < 0) {
      double r = (r1 > r2)? r1 : r2;
      nseg = 12;
      double alpha = 2.0*pi/nseg;
      while(r*(1.0-cos(0.5*alpha)) > mesh_utils::secant_tolerance()) {
         nseg += 2;
         alpha = 2*pi/nseg;
      }
   }

   bool reverse_face = mesh_utils::is_left_hand(t);

   // the cone is created with correct size so we just have to apply the user transform

   // number of vertices along circumference
   size_t nvc = nseg;

   // total number of vertices
   size_t nvert = (nvc+1)*2;
   // total number of faces
   size_t nface = nvc*3;

   double radius[] = {r1,r2};
   double z1 = (center)? -0.5*height : 0.0;
   double z2 = (center)? +0.5*height : height;
   double z[] = {z1,z2};

   double dang = 2*pi/nseg;

   std::shared_ptr<xpolyhedron> poly(new xpolyhedron());
   poly->v_reserve(nvert);
   poly->f_reserve(nface);

   // vertices and faces at top & bottom
   for(size_t iz=0; iz<2; iz++) {
      // first vertex v0 is at center
      size_t v0 = poly->v_add(t * carve::geom::VECTOR(0.0,0.0,z[iz]));

      // then along circumfecence
      double r  = radius[iz];
      double ang = 0.0;
      for(size_t ivc=0; ivc<nvc; ivc++) {
         double x = r*cos(ang);
         double y = r*sin(ang);
         size_t v1 = poly->v_add(t * carve::geom::VECTOR(x,y,z[iz]));
         size_t v2 = (ivc==(nvc-1))? v0+1:v1+1;

         // reverse bottom faces
         if(iz==0) poly->f_add( xface(v2,v1,v0),reverse_face);
         else      poly->f_add( xface(v0,v1,v2),reverse_face);

         ang += dang;
      }
   }

   // side faces
   size_t vb0 = 1;
   for(size_t ivc=0; ivc<nvc; ivc++) {
      size_t v0 = vb0+ivc;
      size_t v1 = (ivc==(nvc-1))? vb0 : v0+1;
      size_t v2 = v1+1+nvc;
      size_t v3 = v0+1+nvc;
      poly->f_add( xface(v0,v1,v2,v3),reverse_face);
   }
   return poly;
}


std::shared_ptr<xpolyhedron>  primitives3d::make_sphere(double r, int nseg, const carve::math::Matrix& t)
{
   // the sphere is created with correct size so we just have to apply the user transform

   if(nseg < 0) {
      nseg = 4;
      double alpha = 2.0*pi/nseg;
      while(r*(1.0-cos(0.5*alpha)) >  mesh_utils::secant_tolerance()) {
         nseg += 2;
         alpha = 2*pi/nseg;
      }
   }
   bool reverse_face = mesh_utils::is_left_hand(t);

   double dang = 2*pi/nseg;

   // number of vertices in longitude
   size_t nlong = nseg;

   // number of latitude bands, north hemisphere
   size_t nlat  = size_t((0.5*pi)/dang) - 1;

   // total number of vertices, complete sphere
   size_t nband = 1 + 2*nlat;
   size_t nvert = nband*nlong + 2;
   size_t nface = nlong*(nlat+1)*2;

   std::shared_ptr<xpolyhedron>  poly(new xpolyhedron());
   poly->v_reserve(nvert);
   poly->f_reserve(nface);

   // first vertex v0 is at south pole
   poly->v_add(t * carve::geom::VECTOR(0.0,0.0,-r));

   double ang_lat = -0.5*pi + dang;
   while(ang_lat < 0.5*pi) {

      size_t v1st = poly->v_size();
      double z = r*sin(ang_lat);
      for(size_t ilong=0; ilong<nlong; ilong++) {

         // longitude angle
         double ang_lon = ilong*dang;
         double x = r*cos(ang_lat)*cos(ang_lon);
         double y = r*cos(ang_lat)*sin(ang_lon);
         size_t v0 = poly->v_add(t * carve::geom::VECTOR(x,y,z));

         if(ang_lat+dang < 0.5*pi) {
            // add faces only as lomg as there wil be another vertex row
            size_t v1 = (ilong==(nlong-1))? v1st : v0+1;
            size_t v2 = v1+nlong;
            size_t v3 = v0+nlong;
            size_t iface = poly->f_add( xface(v0,v1,v2,v3),reverse_face);
         }
      }
      ang_lat += dang;
   }

   // last vertex is at north pole
   size_t vnp =  poly->v_add(t * carve::geom::VECTOR(0.0,0.0,r));

   // pole faces
   for(size_t ilong=0; ilong<nlong; ilong++) {

      // south pole
      size_t v0 = 0;
      size_t v1 = ilong+1;
      size_t v2 = (ilong==(nlong-1))? 1 : v1+1;
      poly->f_add( xface(v2,v1,v0),reverse_face);

      // north pole, opposite orientation
      v0 = vnp;
      v1 = vnp-nlong+ilong;
      v2 = (ilong==(nlong-1))? v1-ilong : v1+1;
      poly->f_add( xface(v0,v1,v2),reverse_face);
   }

   return poly;
}

std::shared_ptr<xpolyhedron>  primitives3d::make_geodesic_sphere(double r, int nseg, const carve::math::Matrix& t)
{
   bool reverse_face = mesh_utils::is_left_hand(t);

   // scale up the sphere
   carve::math::Matrix tloc = t * carve::math::Matrix::SCALE(r,r,r);

   if(nseg < 0) {
      nseg = 6;
      double alpha = 2.0*pi/nseg;
      while(1.1*r*(1.0-cos(0.5*alpha)) >  mesh_utils::secant_tolerance()) {
         nseg *= 2;
         alpha = 2*pi/nseg;
      }
   }

   // approximate number of recursion levels from nseg
   size_t idepth = 0;

   if(nseg >  12)idepth = 1;
   if(nseg >  24)idepth = 2;
   if(nseg >  48)idepth = 3;
   if(nseg >  96)idepth = 4;
   if(nseg > 192)idepth = 5;


   geodesic_sphere gsphere(idepth);

   size_t nvert = gsphere.v_size();
   size_t nface = gsphere.f_size();

   std::shared_ptr<xpolyhedron>  poly(new xpolyhedron());
   poly->v_reserve(nvert);
   poly->f_reserve(nface);

   for(size_t ivert=0;ivert<nvert; ivert++) {
      poly->v_add(tloc* gsphere.v_get(ivert));
   }
   for(size_t iface=0;iface<nface; iface++) {
      poly->f_add(gsphere.f_get(iface),reverse_face);
   }

   return poly;
}

std::shared_ptr<xpolyhedron> primitives3d::make_polygon(const std::vector<xvertex>& vertices, double dz, const carve::math::Matrix& t)
{
   std::shared_ptr<xpolyhedron>  poly(new xpolyhedron());

   bool reverse_face = mesh_utils::is_left_hand(t);

   size_t nvert = vertices.size();

   poly->v_reserve(nvert*2);
   poly->f_reserve(nvert+2);

   // vertices in 2 layers
   for(size_t i=0; i<2; i++) {
      for(size_t iv=0; iv<nvert; iv++) {
         xvertex vert = (i==0)? vertices[iv] : vertices[iv] + carve::geom::VECTOR(0.0,0.0,dz);
         poly->v_add(t * vert);
      }
   }

   // top & bottom faces mentions all vertices in their relative layers, but in opposite orders
   std::vector<size_t> ind_bot(nvert);
   std::vector<size_t> ind_top(nvert);
   for(size_t i=0; i<nvert; i++) {
      ind_bot[i] = i;
      ind_top[i] = nvert + i;
   }

   // add the faces, bottom face is opposite the others, therefore !reverse_face
   poly->f_add(xface(ind_bot),!reverse_face);
   for(size_t i=0; i<nvert; i++) {
      if(i < (nvert-1)) {
         // normal faces
         poly->f_add(xface(ind_bot[i],ind_bot[i+1],ind_top[i+1],ind_top[i]),reverse_face);
      }
      else {
         // last face
         poly->f_add(xface(ind_bot[i],ind_bot[0]  ,ind_top[0]  ,ind_top[i]),reverse_face);
      }
   }
   poly->f_add(xface(ind_top),reverse_face);

   return poly;
}

