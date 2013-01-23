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

#ifndef ToolBoxRegistrar_h
#define ToolBoxRegistrar_h 1

// parent includes:
#include "ta_def.h"

// member includes:
#ifndef __MAKETA__
#include <iToolBoxDockViewer> // ToolBoxProc
#endif

// declare all other types mentioned but not required to include:
class ToolBoxRegistrar_PtrList; // 

TypeDef_Of(ToolBoxRegistrar);

class TA_API ToolBoxRegistrar {
  // static class used to manage toolbar procs; instances used as static globals to register
public:
  static ToolBoxRegistrar_PtrList* instances();
#ifndef __MAKETA__
  ToolBoxProc           proc;
  ToolBoxRegistrar(ToolBoxProc proc_);
#endif

protected:
  static ToolBoxRegistrar_PtrList* m_instances;

};

#endif // ToolBoxRegistrar_h
