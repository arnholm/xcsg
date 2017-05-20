#include "spline3d.h"
#include "ap/spline3.h"

namespace csplines {

   spline3d::spline3d()
   {}

   spline3d::spline3d(const std::vector<cpoint>& points)
   {
      compute_spline(points);
   }

   spline3d::~spline3d()
   {}

   bool spline3d::compute_spline(const std::vector<cpoint>& points)
   {
      // estimate parameter values for each of the points as the distance travelled along the curve
      int n = (int) points.size();

      // allocate the arrays
      ap::real_1d_array t,x,y,z,xn,yn,zn;
      t.setbounds(0,n-1);
      x.setbounds(0,n-1);
      y.setbounds(0,n-1);
      z.setbounds(0,n-1);
      xn.setbounds(0,n-1);
      yn.setbounds(0,n-1);
      zn.setbounds(0,n-1);

      // assign all array values
      double tsum = 0.0;
      t(0) = tsum;
      x(0) = points[0].x;
      y(0) = points[0].y;
      z(0) = points[0].z;
      xn(0) = points[0].xn;
      yn(0) = points[0].yn;
      zn(0) = points[0].zn;
      for(int i=1; i<n; i++) {
         tsum += points[i].dist(points[i-1]);
         t(i) = tsum;
         x(i) = points[i].x;
         y(i) = points[i].y;
         z(i) = points[i].z;
         xn(i) = points[i].xn;
         yn(i) = points[i].yn;
         zn(i) = points[i].zn;
      }

      // normalise the parameters [0,1]
      double scale = 1.0/tsum;
      for(int i=1; i<n; i++) t(i) *= scale;

      buildcubicspline(t,x,n,0,0.0,0,0.0,m_cx);
      buildcubicspline(t,y,n,0,0.0,0,0.0,m_cy);
      buildcubicspline(t,z,n,0,0.0,0,0.0,m_cz);

      buildcubicspline(t,xn,n,0,0.0,0,0.0,m_cxn);
      buildcubicspline(t,yn,n,0,0.0,0,0.0,m_cyn);
      buildcubicspline(t,zn,n,0,0.0,0,0.0,m_czn);
      return true;
   }

   cpoint spline3d::pos(double t) const
   {
      double x = splineinterpolation(m_cx,t);
      double y = splineinterpolation(m_cy,t);
      double z = splineinterpolation(m_cz,t);

      double xn = splineinterpolation(m_cxn,t);
      double yn = splineinterpolation(m_cyn,t);
      double zn = splineinterpolation(m_czn,t);

      return cpoint(x,y,z, xn,yn,zn);
   }

   cpoint spline3d::deriv1(double t) const
   {
      double x,dx,d2x,dxn;
      double y,dy,d2y,dyn;
      double z,dz,d2z,dzn;
      splinedifferentiation(m_cx,t,x,dx,d2x);
      splinedifferentiation(m_cy,t,y,dy,d2y);
      splinedifferentiation(m_cz,t,z,dz,d2z);

      splinedifferentiation(m_cxn,t,x,dxn,d2x);
      splinedifferentiation(m_cyn,t,y,dyn,d2y);
      splinedifferentiation(m_czn,t,z,dzn,d2z);

      return cpoint(dx,dy,dz, dxn,dyn,dzn);
   }


   double spline3d::max_curvature(int nseg) const
   {
      // curvature corresponds to 2nd derivative

      double dt = 1.0/nseg;
      double c  = 0.0;
      int    np = nseg+1;
      for(int ip=0; ip<np; ip++) {
         double t = ip*dt;

         double x,dx,d2x;
         double y,dy,d2y;
         double z,dz,d2z;
         splinedifferentiation(m_cx,t,x,dx,d2x);
         splinedifferentiation(m_cy,t,y,dy,d2y);
         splinedifferentiation(m_cz,t,z,dz,d2z);

         c = std::max(c,fabs(d2x));
         c = std::max(c,fabs(d2y));
         c = std::max(c,fabs(d2z));
      }
      return c;

   }

}
