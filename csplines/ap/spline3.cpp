/*************************************************************************
Copyright (c) 2007, Sergey Bochkanov (ALGLIB project).

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

//#include <stdafx.h>
#include "spline3.h"

void heapsortpoints(ap::real_1d_array& x, ap::real_1d_array& y, int n);
void heapsortdpoints(ap::real_1d_array& x,
     ap::real_1d_array& y,
     ap::real_1d_array& d,
     int n);
void solvetridiagonal(ap::real_1d_array a,
     ap::real_1d_array b,
     ap::real_1d_array c,
     ap::real_1d_array d,
     int n,
     ap::real_1d_array& x);
double diffthreepoint(double t,
     double x0,
     double f0,
     double x1,
     double f1,
     double x2,
     double f2);

/*************************************************************************
This subroutine builds linear spline coefficients table.

Input parameters:
    X   -   spline nodes, array[0..N-1]
    Y   -   function values, array[0..N-1]
    N   -   points count, N>=2

Output parameters:
    C   -   coefficients table.  Used  by  SplineInterpolation  and  other
            subroutines from this file.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildlinearspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     ap::real_1d_array& c)
{
    int i;
    int tblsize;

    ap::ap_error::make_assertion(n>=2, "BuildLinearSpline: N<2!");

    //
    // Sort points
    //
    heapsortpoints(x, y, n);

    //
    // Fill C:
    //  C[0]            -   length(C)
    //  C[1]            -   type(C):
    //                      3 - general cubic spline
    //  C[2]            -   N
    //  C[3]...C[3+N-1] -   x[i], i = 0...N-1
    //  C[3+N]...C[3+N+(N-1)*4-1] - coefficients table
    //
    tblsize = 3+n+(n-1)*4;
    c.setbounds(0, tblsize-1);
    c(0) = tblsize;
    c(1) = 3;
    c(2) = n;
    for(i = 0; i <= n-1; i++)
    {
        c(3+i) = x(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        c(3+n+4*i+0) = y(i);
        c(3+n+4*i+1) = (y(i+1)-y(i))/(x(i+1)-x(i));
        c(3+n+4*i+2) = 0;
        c(3+n+4*i+3) = 0;
    }
}


/*************************************************************************
This subroutine builds cubic spline coefficients table.

Input parameters:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    N           -   points count, N>=2
    BoundLType  -   boundary condition type for the left boundary
    BoundL      -   left boundary condition (first or second derivative,
                    depending on the BoundLType)
    BoundRType  -   boundary condition type for the right boundary
    BoundR      -   right boundary condition (first or second derivative,
                    depending on the BoundRType)

Output parameters:
    C           -   coefficients table.  Used  by  SplineInterpolation and
                    other subroutines from this file.

The BoundLType/BoundRType parameters can have the following values:
    * 0,   which  corresponds  to  the  parabolically   terminated  spline
      (BoundL/BoundR are ignored).
    * 1, which corresponds to the first derivative boundary condition
    * 2, which corresponds to the second derivative boundary condition

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildcubicspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     int boundltype,
     double boundl,
     int boundrtype,
     double boundr,
     ap::real_1d_array& c)
{
    ap::real_1d_array a1;
    ap::real_1d_array a2;
    ap::real_1d_array a3;
    ap::real_1d_array b;
    ap::real_1d_array d;
    int i;
//    int tblsize;
//    double delta;
//    double delta2;
//    double delta3;

    ap::ap_error::make_assertion(n>=2, "BuildCubicSpline: N<2!");
    ap::ap_error::make_assertion(boundltype==0||boundltype==1||boundltype==2, "BuildCubicSpline: incorrect BoundLType!");
    ap::ap_error::make_assertion(boundrtype==0||boundrtype==1||boundrtype==2, "BuildCubicSpline: incorrect BoundRType!");
    a1.setbounds(0, n-1);
    a2.setbounds(0, n-1);
    a3.setbounds(0, n-1);
    b.setbounds(0, n-1);

    //
    // Special case:
    // * N=2
    // * parabolic terminated boundary condition on both ends
    //
    if( n==2&&boundltype==0&&boundrtype==0 )
    {

        //
        // Change task type
        //
        boundltype = 2;
        boundl = 0;
        boundrtype = 2;
        boundr = 0;
    }

    //
    //
    // Sort points
    //
    heapsortpoints(x, y, n);

    //
    // Left boundary conditions
    //
    if( boundltype==0 )
    {
        a1(0) = 0;
        a2(0) = 1;
        a3(0) = 1;
        b(0) = 2*(y(1)-y(0))/(x(1)-x(0));
    }
    if( boundltype==1 )
    {
        a1(0) = 0;
        a2(0) = 1;
        a3(0) = 0;
        b(0) = boundl;
    }
    if( boundltype==2 )
    {
        a1(0) = 0;
        a2(0) = 2;
        a3(0) = 1;
        b(0) = 3*(y(1)-y(0))/(x(1)-x(0))-0.5*boundl*(x(1)-x(0));
    }

    //
    // Central conditions
    //
    for(i = 1; i <= n-2; i++)
    {
        a1(i) = x(i+1)-x(i);
        a2(i) = 2*(x(i+1)-x(i-1));
        a3(i) = x(i)-x(i-1);
        b(i) = 3*(y(i)-y(i-1))/(x(i)-x(i-1))*(x(i+1)-x(i))+3*(y(i+1)-y(i))/(x(i+1)-x(i))*(x(i)-x(i-1));
    }

    //
    // Right boundary conditions
    //
    if( boundrtype==0 )
    {
        a1(n-1) = 1;
        a2(n-1) = 1;
        a3(n-1) = 0;
        b(n-1) = 2*(y(n-1)-y(n-2))/(x(n-1)-x(n-2));
    }
    if( boundrtype==1 )
    {
        a1(n-1) = 0;
        a2(n-1) = 1;
        a3(n-1) = 0;
        b(n-1) = boundr;
    }
    if( boundrtype==2 )
    {
        a1(n-1) = 1;
        a2(n-1) = 2;
        a3(n-1) = 0;
        b(n-1) = 3*(y(n-1)-y(n-2))/(x(n-1)-x(n-2))+0.5*boundr*(x(n-1)-x(n-2));
    }

    //
    // Solve
    //
    solvetridiagonal(a1, a2, a3, b, n, d);

    //
    // Now problem is reduced to the cubic Hermite spline
    //
    buildhermitespline(x, y, d, n, c);
}


/*************************************************************************
This subroutine builds cubic Hermite spline coefficients table.

Input parameters:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    D           -   derivatives, array[0..N-1]
    N           -   points count, N>=2

Output parameters:
    C           -   coefficients table.  Used  by  SplineInterpolation and
                    other subroutines from this file.

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildhermitespline(ap::real_1d_array x,
     ap::real_1d_array y,
     ap::real_1d_array d,
     int n,
     ap::real_1d_array& c)
{
    int i;
    int tblsize;
    double delta;
    double delta2;
    double delta3;

    ap::ap_error::make_assertion(n>=2, "BuildHermiteSpline: N<2!");

    //
    // Sort points
    //
    heapsortdpoints(x, y, d, n);

    //
    // Fill C:
    //  C[0]            -   length(C)
    //  C[1]            -   type(C):
    //                      3 - general cubic spline
    //  C[2]            -   N
    //  C[3]...C[3+N-1] -   x[i], i = 0...N-1
    //  C[3+N]...C[3+N+(N-1)*4-1] - coefficients table
    //
    tblsize = 3+n+(n-1)*4;
    c.setbounds(0, tblsize-1);
    c(0) = tblsize;
    c(1) = 3;
    c(2) = n;
    for(i = 0; i <= n-1; i++)
    {
        c(3+i) = x(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        delta = x(i+1)-x(i);
        delta2 = ap::sqr(delta);
        delta3 = delta*delta2;
        c(3+n+4*i+0) = y(i);
        c(3+n+4*i+1) = d(i);
        c(3+n+4*i+2) = (3*(y(i+1)-y(i))-2*d(i)*delta-d(i+1)*delta)/delta2;
        c(3+n+4*i+3) = (2*(y(i)-y(i+1))+d(i)*delta+d(i+1)*delta)/delta3;
    }
}


/*************************************************************************
This subroutine builds Akima spline coefficients table.

Input parameters:
    X           -   spline nodes, array[0..N-1]
    Y           -   function values, array[0..N-1]
    N           -   points count, N>=5

Output parameters:
    C           -   coefficients table.  Used  by  SplineInterpolation and
                    other subroutines from this file.

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void buildakimaspline(ap::real_1d_array x,
     ap::real_1d_array y,
     int n,
     ap::real_1d_array& c)
{
    int i;
    ap::real_1d_array d;
    ap::real_1d_array w;
    ap::real_1d_array diff;

    ap::ap_error::make_assertion(n>=5, "BuildAkimaSpline: N<5!");

    //
    // Sort points
    //
    heapsortpoints(x, y, n);

    //
    // Prepare W (weights), Diff (divided differences)
    //
    w.setbounds(1, n-2);
    diff.setbounds(0, n-2);
    for(i = 0; i <= n-2; i++)
    {
        diff(i) = (y(i+1)-y(i))/(x(i+1)-x(i));
    }
    for(i = 1; i <= n-2; i++)
    {
        w(i) = fabs(diff(i)-diff(i-1));
    }

    //
    // Prepare Hermite interpolation scheme
    //
    d.setbounds(0, n-1);
    for(i = 2; i <= n-3; i++)
    {
        if( fabs(w(i-1))+fabs(w(i+1))!=0 )
        {
            d(i) = (w(i+1)*diff(i-1)+w(i-1)*diff(i))/(w(i+1)+w(i-1));
        }
        else
        {
            d(i) = ((x(i+1)-x(i))*diff(i-1)+(x(i)-x(i-1))*diff(i))/(x(i+1)-x(i-1));
        }
    }
    d(0) = diffthreepoint(x(0), x(0), y(0), x(1), y(1), x(2), y(2));
    d(1) = diffthreepoint(x(1), x(0), y(0), x(1), y(1), x(2), y(2));
    d(n-2) = diffthreepoint(x(n-2), x(n-3), y(n-3), x(n-2), y(n-2), x(n-1), y(n-1));
    d(n-1) = diffthreepoint(x(n-1), x(n-3), y(n-3), x(n-2), y(n-2), x(n-1), y(n-1));

    //
    // Build Akima spline using Hermite interpolation scheme
    //
    buildhermitespline(x, y, d, n, c);
}


/*************************************************************************
This subroutine calculates the value of the spline at the given point X.

Input parameters:
    C           -   coefficients table. Built by BuildLinearSpline,
                    BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    X           -   point

Result:
    S(x)

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double splineinterpolation(const ap::real_1d_array& c, double x)
{
    double result;
    int n;
    int l;
    int r;
    int m;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineInterpolation: incorrect C!");
    n = ap::round(c(2));

    //
    // Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
    //
    l = 3;
    r = 3+n-2+1;
    while(l!=r-1)
    {
        m = (l+r)/2;
        if( c(m)>=x )
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }

    //
    // Interpolation
    //
    x = x-c(l);
    m = 3+n+4*(l-3);
    result = c(m)+x*(c(m+1)+x*(c(m+2)+x*c(m+3)));
    return result;
}


/*************************************************************************
This subroutine differentiates the spline.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    X   -   point

Result:
    S   -   S(x)
    DS  -   S'(x)
    D2S -   S''(x)

  -- ALGLIB PROJECT --
     Copyright 24.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinedifferentiation(const ap::real_1d_array& c,
     double x,
     double& s,
     double& ds,
     double& d2s)
{
    int n;
    int l;
    int r;
    int m;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineInterpolation: incorrect C!");
    n = ap::round(c(2));

    //
    // Binary search
    //
    l = 3;
    r = 3+n-2+1;
    while(l!=r-1)
    {
        m = (l+r)/2;
        if( c(m)>=x )
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }

    //
    // Differentiation
    //
    x = x-c(l);
    m = 3+n+4*(l-3);
    s = c(m)+x*(c(m+1)+x*(c(m+2)+x*c(m+3)));
    ds = c(m+1)+2*x*c(m+2)+3*ap::sqr(x)*c(m+3);
    d2s = 2*c(m+2)+6*x*c(m+3);
}


/*************************************************************************
This subroutine makes the copy of the spline.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.

Result:
    CC  -   spline copy

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinecopy(const ap::real_1d_array& c, ap::real_1d_array& cc)
{
    int s;

    s = ap::round(c(0));
    cc.setbounds(0, s-1);
    ap::vmove(&cc(0), &c(0), ap::vlen(0,s-1));
}


/*************************************************************************
This subroutine unpacks the spline into the coefficients table.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    X   -   point

Result:
    Tbl -   coefficients table, unpacked format, array[0..N-2, 0..5].
            For I = 0...N-2:
                Tbl[I,0] = X[i]
                Tbl[I,1] = X[i+1]
                Tbl[I,2] = C0
                Tbl[I,3] = C1
                Tbl[I,4] = C2
                Tbl[I,5] = C3
            On [x[i], x[i+1]] spline is equals to:
                S(x) = C0 + C1*t + C2*t^2 + C3*t^3
                t = x-x[i]

  -- ALGLIB PROJECT --
     Copyright 29.06.2007 by Bochkanov Sergey
*************************************************************************/
void splineunpack(const ap::real_1d_array& c, int& n, ap::real_2d_array& tbl)
{
    int i;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineUnpack: incorrect C!");
    n = ap::round(c(2));
    tbl.setbounds(0, n-2, 0, 5);

    //
    // Fill
    //
    for(i = 0; i <= n-2; i++)
    {
        tbl(i,0) = c(3+i);
        tbl(i,1) = c(3+i+1);
        tbl(i,2) = c(3+n+4*i);
        tbl(i,3) = c(3+n+4*i+1);
        tbl(i,4) = c(3+n+4*i+2);
        tbl(i,5) = c(3+n+4*i+3);
    }
}


/*************************************************************************
This subroutine performs linear transformation of the spline argument.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    A, B-   transformation coefficients: x = A*t + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinelintransx(ap::real_1d_array& c, double a, double b)
{
    int i;
    int n;
    double v;
    double dv;
    double d2v;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array d;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineLinTransX: incorrect C!");
    n = ap::round(c(2));

    //
    // Special case: A=0
    //
    if( a==0 )
    {
        v = splineinterpolation(c, b);
        for(i = 0; i <= n-2; i++)
        {
            c(3+n+4*i) = v;
            c(3+n+4*i+1) = 0;
            c(3+n+4*i+2) = 0;
            c(3+n+4*i+3) = 0;
        }
        return;
    }

    //
    // General case: A<>0.
    // Unpack, X, Y, dY/dX.
    // Scale and pack again.
    //
    x.setbounds(0, n-1);
    y.setbounds(0, n-1);
    d.setbounds(0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        x(i) = c(3+i);
        splinedifferentiation(c, x(i), v, dv, d2v);
        x(i) = (x(i)-b)/a;
        y(i) = v;
        d(i) = a*dv;
    }
    buildhermitespline(x, y, d, n, c);
}


/*************************************************************************
This subroutine performs linear transformation of the spline.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    A, B-   transformation coefficients: S2(x) = A*S(x) + B
Result:
    C   -   transformed spline

  -- ALGLIB PROJECT --
     Copyright 30.06.2007 by Bochkanov Sergey
*************************************************************************/
void splinelintransy(ap::real_1d_array& c, double a, double b)
{
    int i;
    int n;
//    double v;
//    double dv;
//    double d2v;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array d;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineLinTransX: incorrect C!");
    n = ap::round(c(2));

    //
    // Special case: A=0
    //
    for(i = 0; i <= n-2; i++)
    {
        c(3+n+4*i) = a*c(3+n+4*i)+b;
        c(3+n+4*i+1) = a*c(3+n+4*i+1);
        c(3+n+4*i+2) = a*c(3+n+4*i+2);
        c(3+n+4*i+3) = a*c(3+n+4*i+3);
    }
}


/*************************************************************************
This subroutine integrates the spline.

Input parameters:
    C   -   coefficients table. Built by BuildLinearSpline,
            BuildHermiteSpline, BuildCubicSpline, BuildAkimaSpline.
    X   -   right bound of the integration interval [a, x]
Result:
    integral(S(t)dt,a,x)

  -- ALGLIB PROJECT --
     Copyright 23.06.2007 by Bochkanov Sergey
*************************************************************************/
double splineintegration(const ap::real_1d_array& c, double x)
{
    double result;
    int n;
    int i;
    int l;
    int r;
    int m;
    double w;

    ap::ap_error::make_assertion(ap::round(c(1))==3, "SplineIntegration: incorrect C!");
    n = ap::round(c(2));

    //
    // Binary search in the [ x[0], ..., x[n-2] ] (x[n-1] is not included)
    //
    l = 3;
    r = 3+n-2+1;
    while(l!=r-1)
    {
        m = (l+r)/2;
        if( c(m)>=x )
        {
            r = m;
        }
        else
        {
            l = m;
        }
    }

    //
    // Integration
    //
    result = 0;
    for(i = 3; i <= l-1; i++)
    {
        w = c(i+1)-c(i);
        m = 3+n+4*(i-3);
        result = result+c(m)*w;
        result = result+c(m+1)*ap::sqr(w)/2;
        result = result+c(m+2)*ap::sqr(w)*w/3;
        result = result+c(m+3)*ap::sqr(ap::sqr(w))/4;
    }
    w = x-c(l);
    m = 3+n+4*(l-3);
    result = result+c(m)*w;
    result = result+c(m+1)*ap::sqr(w)/2;
    result = result+c(m+2)*ap::sqr(w)*w/3;
    result = result+c(m+3)*ap::sqr(ap::sqr(w))/4;
    return result;
}


/*************************************************************************
Obsolete subroutine, left for backward compatibility.
*************************************************************************/
void spline3buildtable(int n,
     const int& diffn,
     ap::real_1d_array x,
     ap::real_1d_array y,
     const double& boundl,
     const double& boundr,
     ap::real_2d_array& ctbl)
{
    bool c;
//    int e;
    int g;
    double tmp;
    int nxm1;
    int i;
    int j;
    double dx;
    double dxj;
    double dyj;
    double dxjp1;
    double dyjp1;
    double dxp;
//    double dyp;
    double yppa;
    double yppb;
    double pj;
    double b1;
    double b2;
    double b3;
    double b4;

    n = n-1;
    g = (n+1)/2;
    do
    {
        i = g;
        do
        {
            j = i-g;
            c = true;
            do
            {
                if( x(j)<=x(j+g) )
                {
                    c = false;
                }
                else
                {
                    tmp = x(j);
                    x(j) = x(j+g);
                    x(j+g) = tmp;
                    tmp = y(j);
                    y(j) = y(j+g);
                    y(j+g) = tmp;
                }
                j = j-1;
            }
            while(j>=0&&c);
            i = i+1;
        }
        while(i<=n);
        g = g/2;
    }
    while(g>0);
    ctbl.setbounds(0, 4, 0, n);
    n = n+1;
    if( diffn==1 )
    {
        b1 = 1;
        b2 = 6/(x(1)-x(0))*((y(1)-y(0))/(x(1)-x(0))-boundl);
        b3 = 1;
        b4 = 6/(x(n-1)-x(n-2))*(boundr-(y(n-1)-y(n-2))/(x(n-1)-x(n-2)));
    }
    else
    {
        b1 = 0;
        b2 = 2*boundl;
        b3 = 0;
        b4 = 2*boundr;
    }
    nxm1 = n-1;
    if( n>=2 )
    {
        if( n>2 )
        {
            dxj = x(1)-x(0);
            dyj = y(1)-y(0);
            j = 2;
            while(j<=nxm1)
            {
                dxjp1 = x(j)-x(j-1);
                dyjp1 = y(j)-y(j-1);
                dxp = dxj+dxjp1;
                ctbl(1,j-1) = dxjp1/dxp;
                ctbl(2,j-1) = 1-ctbl(1,j-1);
                ctbl(3,j-1) = 6*(dyjp1/dxjp1-dyj/dxj)/dxp;
                dxj = dxjp1;
                dyj = dyjp1;
                j = j+1;
            }
        }
        ctbl(1,0) = -b1/2;
        ctbl(2,0) = b2/2;
        if( n!=2 )
        {
            j = 2;
            while(j<=nxm1)
            {
                pj = ctbl(2,j-1)*ctbl(1,j-2)+2;
                ctbl(1,j-1) = -ctbl(1,j-1)/pj;
                ctbl(2,j-1) = (ctbl(3,j-1)-ctbl(2,j-1)*ctbl(2,j-2))/pj;
                j = j+1;
            }
        }
        yppb = (b4-b3*ctbl(2,nxm1-1))/(b3*ctbl(1,nxm1-1)+2);
        i = 1;
        while(i<=nxm1)
        {
            j = n-i;
            yppa = ctbl(1,j-1)*yppb+ctbl(2,j-1);
            dx = x(j)-x(j-1);
            ctbl(3,j-1) = (yppb-yppa)/dx/6;
            ctbl(2,j-1) = yppa/2;
            ctbl(1,j-1) = (y(j)-y(j-1))/dx-(ctbl(2,j-1)+ctbl(3,j-1)*dx)*dx;
            yppb = yppa;
            i = i+1;
        }
        for(i = 1; i <= n; i++)
        {
            ctbl(0,i-1) = y(i-1);
            ctbl(4,i-1) = x(i-1);
        }
    }
}


/*************************************************************************
Obsolete subroutine, left for backward compatibility.
*************************************************************************/
double spline3interpolate(int n, const ap::real_2d_array& c, const double& x)
{
    double result;
    int i;
    int l;
    int half;
    int first;
    int middle;

    n = n-1;
    l = n;
    first = 0;
    while(l>0)
    {
        half = l/2;
        middle = first+half;
        if( c(4,middle)<x )
        {
            first = middle+1;
            l = l-half-1;
        }
        else
        {
            l = half;
        }
    }
    i = first-1;
    if( i<0 )
    {
        i = 0;
    }
    result = c(0,i)+(x-c(4,i))*(c(1,i)+(x-c(4,i))*(c(2,i)+c(3,i)*(x-c(4,i))));
    return result;
}


/*************************************************************************
Internal subroutine. Heap sort.
*************************************************************************/
void heapsortpoints(ap::real_1d_array& x, ap::real_1d_array& y, int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    bool isascending;
    bool isdescending;


    //
    // Test for already sorted set
    //
    isascending = true;
    isdescending = true;
    for(i = 1; i <= n-1; i++)
    {
        isascending = isascending&&x(i)>x(i-1);
        isdescending = isdescending&&x(i)<x(i-1);
    }
    if( isascending )
    {
        return;
    }
    if( isdescending )
    {
        for(i = 0; i <= n-1; i++)
        {
            j = n-1-i;
            if( j<=i )
            {
                break;
            }
            tmp = x(i);
            x(i) = x(j);
            x(j) = tmp;
            tmp = y(i);
            y(i) = y(j);
            y(j) = tmp;
        }
        return;
    }

    //
    // Special case: N=1
    //
    if( n==1 )
    {
        return;
    }

    //
    // General case
    //
    i = 2;
    do
    {
        t = i;
        while(t!=1)
        {
            k = t/2;
            if( x(k-1)>=x(t-1) )
            {
                t = 1;
            }
            else
            {
                tmp = x(k-1);
                x(k-1) = x(t-1);
                x(t-1) = tmp;
                tmp = y(k-1);
                y(k-1) = y(t-1);
                y(t-1) = tmp;
                t = k;
            }
        }
        i = i+1;
    }
    while(i<=n);
    i = n-1;
    do
    {
        tmp = x(i);
        x(i) = x(0);
        x(0) = tmp;
        tmp = y(i);
        y(i) = y(0);
        y(0) = tmp;
        t = 1;
        while(t!=0)
        {
            k = 2*t;
            if( k>i )
            {
                t = 0;
            }
            else
            {
                if( k<i )
                {
                    if( x(k)>x(k-1) )
                    {
                        k = k+1;
                    }
                }
                if( x(t-1)>=x(k-1) )
                {
                    t = 0;
                }
                else
                {
                    tmp = x(k-1);
                    x(k-1) = x(t-1);
                    x(t-1) = tmp;
                    tmp = y(k-1);
                    y(k-1) = y(t-1);
                    y(t-1) = tmp;
                    t = k;
                }
            }
        }
        i = i-1;
    }
    while(i>=1);
}


/*************************************************************************
Internal subroutine. Heap sort.
*************************************************************************/
void heapsortdpoints(ap::real_1d_array& x,
     ap::real_1d_array& y,
     ap::real_1d_array& d,
     int n)
{
    int i;
    int j;
    int k;
    int t;
    double tmp;
    bool isascending;
    bool isdescending;


    //
    // Test for already sorted set
    //
    isascending = true;
    isdescending = true;
    for(i = 1; i <= n-1; i++)
    {
        isascending = isascending&&x(i)>x(i-1);
        isdescending = isdescending&&x(i)<x(i-1);
    }
    if( isascending )
    {
        return;
    }
    if( isdescending )
    {
        for(i = 0; i <= n-1; i++)
        {
            j = n-1-i;
            if( j<=i )
            {
                break;
            }
            tmp = x(i);
            x(i) = x(j);
            x(j) = tmp;
            tmp = y(i);
            y(i) = y(j);
            y(j) = tmp;
            tmp = d(i);
            d(i) = d(j);
            d(j) = tmp;
        }
        return;
    }

    //
    // Special case: N=1
    //
    if( n==1 )
    {
        return;
    }

    //
    // General case
    //
    i = 2;
    do
    {
        t = i;
        while(t!=1)
        {
            k = t/2;
            if( x(k-1)>=x(t-1) )
            {
                t = 1;
            }
            else
            {
                tmp = x(k-1);
                x(k-1) = x(t-1);
                x(t-1) = tmp;
                tmp = y(k-1);
                y(k-1) = y(t-1);
                y(t-1) = tmp;
                tmp = d(k-1);
                d(k-1) = d(t-1);
                d(t-1) = tmp;
                t = k;
            }
        }
        i = i+1;
    }
    while(i<=n);
    i = n-1;
    do
    {
        tmp = x(i);
        x(i) = x(0);
        x(0) = tmp;
        tmp = y(i);
        y(i) = y(0);
        y(0) = tmp;
        tmp = d(i);
        d(i) = d(0);
        d(0) = tmp;
        t = 1;
        while(t!=0)
        {
            k = 2*t;
            if( k>i )
            {
                t = 0;
            }
            else
            {
                if( k<i )
                {
                    if( x(k)>x(k-1) )
                    {
                        k = k+1;
                    }
                }
                if( x(t-1)>=x(k-1) )
                {
                    t = 0;
                }
                else
                {
                    tmp = x(k-1);
                    x(k-1) = x(t-1);
                    x(t-1) = tmp;
                    tmp = y(k-1);
                    y(k-1) = y(t-1);
                    y(t-1) = tmp;
                    tmp = d(k-1);
                    d(k-1) = d(t-1);
                    d(t-1) = tmp;
                    t = k;
                }
            }
        }
        i = i-1;
    }
    while(i>=1);
}


/*************************************************************************
Internal subroutine. Tridiagonal solver.
*************************************************************************/
void solvetridiagonal(ap::real_1d_array a,
     ap::real_1d_array b,
     ap::real_1d_array c,
     ap::real_1d_array d,
     int n,
     ap::real_1d_array& x)
{
    int k;
    double t;

    x.setbounds(0, n-1);
    a(0) = 0;
    c(n-1) = 0;
    for(k = 1; k <= n-1; k++)
    {
        t = a(k)/b(k-1);
        b(k) = b(k)-t*c(k-1);
        d(k) = d(k)-t*d(k-1);
    }
    x(n-1) = d(n-1)/b(n-1);
    for(k = n-2; k >= 0; k--)
    {
        x(k) = (d(k)-c(k)*x(k+1))/b(k);
    }
}


/*************************************************************************
Internal subroutine. Three-point differentiation
*************************************************************************/
double diffthreepoint(double t,
     double x0,
     double f0,
     double x1,
     double f1,
     double x2,
     double f2)
{
    double result;
    double a;
    double b;

    t = t-x0;
    x1 = x1-x0;
    x2 = x2-x0;
    a = (f2-f0-x2/x1*(f1-f0))/(ap::sqr(x2)-x1*x2);
    b = (f1-f0-a*ap::sqr(x1))/x1;
    result = 2*a*t+b;
    return result;
}



