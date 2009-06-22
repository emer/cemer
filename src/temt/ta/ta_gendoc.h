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

// ta_gendoc.h

#ifndef ta_gendoc_h
#define ta_gendoc_h

// documentation generation from scanned type information (maketa)

#include "ta_platform.h"
#include "ta_type.h"

class TA_API taGenDoc  {
  // #NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS collection of gendoc functions
public:
  static void 		GenDoc(TypeSpace* ths, int detail_level=1);
  // main function -- output type info to stream -- saves .html files in current location -- detail level: 0,1 = basic user information, 2 = + full programming details (expert mode)

  static bool 		TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts);
};

#endif // ta_gendoc_h
