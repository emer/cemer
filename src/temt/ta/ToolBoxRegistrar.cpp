// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "ToolBoxRegistrar.h"
#include <ToolBoxRegistrar_PtrList>

ToolBoxRegistrar_PtrList* ToolBoxRegistrar::m_instances = NULL;

ToolBoxRegistrar_PtrList* ToolBoxRegistrar::instances() {
  if (!m_instances)
    m_instances = new ToolBoxRegistrar_PtrList;
  return m_instances;
}

ToolBoxRegistrar::ToolBoxRegistrar(ToolBoxProc proc_)
: proc(proc_)
{
  instances()->Add(this);
}

