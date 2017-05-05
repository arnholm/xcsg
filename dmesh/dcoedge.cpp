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
   
#include "dcoedge.h"
#include "dedge.h"

dcoedge::dcoedge(dentity* parent, dedge* edge, bool fwd)
: m_parent(parent)
, m_edge(edge)
, m_fwd(fwd)
{
   m_edge->addref(this);
}

dcoedge::~dcoedge()
{
   m_edge->release(this);
}

dcoedge* dcoedge::clone(dentity* parent)
{
   return new dcoedge(parent,m_edge,m_fwd);
}


size_t dcoedge::vertex1() const
{
   return (m_fwd)? m_edge->vertex1() : m_edge->vertex2();
}

size_t dcoedge::vertex2() const
{
   return (m_fwd)? m_edge->vertex2() : m_edge->vertex1();
}
