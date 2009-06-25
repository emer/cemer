// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.


#include "safeptr_so.h"
#ifdef TA_USE_INVENTOR

#include <Inventor/misc/SoBase.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>


//////////////////////////
// 	SoPtr_impl	//
//////////////////////////

SoPtr_impl::~SoPtr_impl() {
  set(NULL);
}

void SoPtr_impl::set(SoBase* src) {
  if (m_ptr == src) return; // must detect identity assignment, otherwise could unref() and then get deleted!
  if (m_ptr) m_ptr->unref();
  m_ptr = src;
  if (m_ptr) m_ptr->ref();
}

#endif

