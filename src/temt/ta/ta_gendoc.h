// Copyright, 1995-2005, Regents of the University of Colorado,
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
  static void 		GenDoc(TypeSpace* ths, fstream& strm);
  // main function -- output type info to stream

  // helper funs:
  static String_PArray* TypeDef_Get_Parents(TypeDef* td, String_PArray* bp);
  static bool 		TypeDef_Filter_Type(TypeDef* td, TypeSpace* ts);
  static bool 		MemberSpace_Filter_Member(MemberSpace* ths, MemberDef* md);
  static bool 		MethodSpace_Filter_Method(MethodSpace* ths, MethodDef* md);
};

#endif // ta_gendoc_h
