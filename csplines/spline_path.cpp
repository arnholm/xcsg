#include "spline_path.h"
#include "ap/spline3.h"
#include <algorithm>
#include <cmath>

namespace csplines {

   spline_path::spline_path()
   : m_length(0.0)
   {}

   spline_path::spline_path(const std::vector<cpoint>& points)
   : m_length(0.0)
   {
      compute_spline(points);
   }

   spline_path::~spline_path()
   {}

   size_t  spline_path::size() const
   {
      return m_points.size();
   }

   bool spline_path::compute_spline(const std::vector<cpoint>& points)
   {
      m_points = points;

      // estimate parameter values for each of the points as the distance travelled along the curve
      int n = (int) points.size();

      // allocate the arrays
      ap::real_1d_array t,px,py,pz,vx,vy,vz;
      t.setbounds(0,n-1);
      px.setbounds(0,n-1);
      py.setbounds(0,n-1);
      pz.setbounds(0,n-1);
      vx.setbounds(0,n-1);
      vy.setbounds(0,n-1);
      vz.setbounds(0,n-1);

      // assign all array values
      double tsum = 0.0;
      t(0) = tsum;
      px(0) = points[0].px;
      py(0) = points[0].py;
      pz(0) = points[0].pz;
      vx(0) = points[0].vx;
      vy(0) = points[0].vy;
      vz(0) = points[0].vz;
      for(int i=1; i<n; i++) {
         tsum += points[i].dist(points[i-1]);
         t(i) = tsum;
         px(i) = points[i].px;
         py(i) = points[i].py;
         pz(i) = points[i].pz;
         vx(i) = points[i].vx;
         vy(i) = points[i].vy;
         vz(i) = points[i].vz;
      }

      // normalise the parameters [0,1]
      double scale = 1.0/tsum;
      for(int i=1; i<n; i++) t(i) *= scale;
      m_length = tsum;

      buildcubicspline(t,px,n,0,0.0,0,0.0,m_cpx);
      buildcubicspline(t,py,n,0,0.0,0,0.0,m_cpy);
      buildcubicspline(t,pz,n,0,0.0,0,0.0,m_cpz);

      buildcubicspline(t,vx,n,0,0.0,0,0.0,m_cvx);
      buildcubicspline(t,vy,n,0,0.0,0,0.0,m_cvy);
      buildcubicspline(t,vz,n,0,0.0,0,0.0,m_cvz);
      return true;
   }

   cpoint spline_path::pos(double t) const
   {
      double px = splineinterpolation(m_cpx,t);
      double py = splineinterpolation(m_cpy,t);
      double pz = splineinterpolation(m_cpz,t);

      double vx = splineinterpolation(m_cvx,t);
      double vy = splineinterpolation(m_cvy,t);
      double vz = splineinterpolation(m_cvz,t);

      return cpoint(px,py,pz, vx,vy,vz);
   }

   cpoint spline_path::dir(double t) const
   {
      double px,dpx,d2px;
      double py,dpy,d2py;
      double pz,dpz,d2pz;
      splinedifferentiation(m_cpx,t,px,dpx,d2px);
      splinedifferentiation(m_cpy,t,py,dpy,d2py);
      splinedifferentiation(m_cpz,t,pz,dpz,d2pz);

      double vx = splineinterpolation(m_cvx,t);
      double vy = splineinterpolation(m_cvy,t);
      double vz = splineinterpolation(m_cvz,t);

      return cpoint(dpx,dpy,dpz, vx,vy,vz);
   }

   double spline_path::curvature(double t) const
   {
      // https://math.stackexchange.com/questions/1786495/estimating-the-curvature-of-a-discretized-curve-in-3d-with-cubic-splines

      double px,dpx,d2px;
      double py,dpy,d2py;
      double pz,dpz,d2pz;
      splinedifferentiation(m_cpx,t,px,dpx,d2px);
      splinedifferentiation(m_cpy,t,py,dpy,d2py);
      splinedifferentiation(m_cpz,t,pz,dpz,d2pz);

      double v1 =pow((d2pz*dpy-d2py*dpz),2.0);
      double v2 =pow((d2px*dpz-d2pz*dpx),2.0);
      double v3 =pow((d2py*dpx-d2px*dpy),2.0);

      double denom = pow((dpx*dpx+dpy*dpy+dpz*dpz),-1.5);
      return sqrt(v1+v2+v3)*denom;
   }

   double spline_path::max_curvature(int nseg) const
   {
      // curvature corresponds to 2nd derivative
      double dt = 1.0/nseg;
      double c  = 0.0;
      int    np = nseg+1;
      for(int ip=0; ip<np; ip++) {
         double t = ip*dt;
         c = std::max(c,curvature(t));
      }
      return c;
   }


   double spline_path::length() const
   {
      return m_length;
   }

   double spline_path::scaling_range() const
   {
      double smin = 0.0;
      double smax = 0.0;
      for(size_t i=0; i<m_points.size(); i++) {
         const cpoint& p = m_points[i];
         double l = p.length();
         smin = (i==0)? l : std::min(l,smin);
         smax = (i==0)? l : std::max(l,smin);
      }

      return (smax-smin);
   }

   std::vector<cpoint>  spline_path::summed_points() const
   {
      std::vector<cpoint> points2;
      points2.reserve(m_points.size());
      for(auto& p : m_points) points2.push_back(p.summed_point());
      return points2;
   }

   std::shared_ptr<csplines::spline_path> spline_path::summed_spline() const
   {
      return std::make_shared<spline_path>(summed_points());
   }
}
