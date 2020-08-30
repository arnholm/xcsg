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

#ifndef CSG_MATRIX_H
#define CSG_MATRIX_H
#include <cstddef>

template <size_t N, size_t M = N, class T = double>
class csg_matrix {
public:
   csg_matrix();                                  // construction, set to identity
   csg_matrix(const csg_matrix& other);           // copy construction
   csg_matrix(const T* values);                   // construct from array

   // assignment
   csg_matrix& operator=(const csg_matrix& other);  // matrix assignment
   csg_matrix& operator=(const T& value );          // constant value assignment

   // modification
   csg_matrix& operator+=(const csg_matrix& other); // add matrix to this
   csg_matrix& operator-=(const csg_matrix& other); // subtract matrix from this
   csg_matrix& operator*=(const T& value);          // multiply this matrix by constant

   // subscripting, indices are zero based
   // the second index is the "fastest" index
   T& operator()(size_t n, size_t m);               // read/write access
   const T&  operator()(size_t n, size_t m) const;  // read access

   T& operator()(size_t n);                         // linear read/write access
   const T&  operator()(size_t n) const;            // linear read access

   void set_diagonal(const T& value);               // set value on diagonal
   csg_matrix& transpose();                         // in-place transpose NxN matrix

   size_t dimension1() const {return N;}
   size_t dimension2() const {return M;}

   T* raw()  { return m_data; }
   const T* raw() const { return m_data; }
protected:
   T m_data[N*M];
};


template <size_t N, size_t M, class T>
csg_matrix<N,M,T>::csg_matrix()
{
   for(size_t i=0; i<N*M; i++)m_data[i]=0.0;
   set_diagonal(1.0);
}

template <size_t N,size_t M, class T>
csg_matrix<N,M,T>::csg_matrix(const T* values)
{
   for(size_t i=0; i<N*M; i++)m_data[i]=values[i];
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>::csg_matrix(const csg_matrix& other)
{
   for(size_t i=0; i<N*M; i++)m_data[i]=other.m_data[i];
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>&  csg_matrix<N,M,T>::operator=(const csg_matrix& other)
{
   for(size_t i=0; i<N*M; i++)m_data[i]=other.m_data[i];
   return *this;
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>&  csg_matrix<N,M,T>::operator+=(const csg_matrix& other)
{
   for(size_t i=0; i<N*M; i++)m_data[i]+=other.m_data[i];
   return *this;
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>&  csg_matrix<N,M,T>::operator-=(const csg_matrix& other)
{
   for(size_t i=0; i<N*M; i++)m_data[i]-=other.m_data[i];
   return *this;
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>&  csg_matrix<N,M,T>::operator*=(const T& value)
{
   for(size_t i=0; i<N*M; i++)m_data[i]*=value;
   return *this;
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>&  csg_matrix<N,M,T>::operator=(const T& value)
{
   for(size_t i=0; i<N*M; i++)m_data[i]=value;
   return *this;
}

template <size_t N, size_t M, class T>
T& csg_matrix<N,M,T>::operator()(size_t n, size_t m)
{
   return m_data[N*m + n];
}

template <size_t N, size_t M, class T>
const T& csg_matrix<N,M,T>::operator()(size_t n, size_t m) const
{
   return m_data[N*m + n];
}

template <size_t N, size_t M, class T>
T& csg_matrix<N,M,T>::operator()(size_t n)
{
   return m_data[n];
}

template <size_t N, size_t M, class T>
const T&  csg_matrix<N,M,T>::operator()(size_t n) const
{
   return m_data[n];
}


template <size_t N, size_t M, class T>
void csg_matrix<N,M,T>::set_diagonal(const T& value)
{
   size_t n=0,m=0;
   for(size_t i=0; i<N*M; i++) {
      if( (n>N-1) || (m>M-1))return;
      operator()(n,m) = value;
      n++;
      m++;
   }
}

template <size_t N, size_t M, class T>
csg_matrix<N,M,T>& csg_matrix<N,M,T>::transpose()
{
   if(N==M) {
      for(size_t i=0; i<N; i++) {
         for(size_t j=0; j<M; j++) {
            if(i < j) {
               T value = operator()(i,j);
               operator()(i,j) = operator()(j,i);
               operator()(j,i) = value;
            }
         }
      }
   }
   return *this;
}


/*
    Matrix multiplication function object, C = A*B

    Usage example:
       csg_matrix<3,3> a,b;
       ....
       csg_matrix<3,3> c = csg_matrix_mult<3,3,3>(a,b);
*/

template <size_t Nres, size_t Mres, size_t Na, class T = double >
class csg_matrix_mult {
public:
   typedef csg_matrix<Nres,Na,T>   A_type;
   typedef csg_matrix<Na,Mres,T>   B_type;
   typedef csg_matrix<Nres,Mres,T> value_type;

   csg_matrix_mult(const A_type& matrix_a, const B_type& matrix_b)
   {
      for(size_t k=0; k<Nres; k++) {
         for(size_t n=0; n<Mres; n++) {
            T sum = T();
            for(size_t m=0; m<Na; m++) {
               sum += matrix_a(k,m)*matrix_b(m,n);
            }
            m_res(k,n) = sum;
         }
      }
   }

   operator const value_type& () const { return m_res; }
private:
   value_type m_res;
};

#endif

