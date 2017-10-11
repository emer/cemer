// Copyright 2017, Regents of the University of Colorado,
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

// ALWAYS overwrite any exsiting def of this one -- we redefine in network State code
#ifdef INHERITED
#undef INHERITED
#endif
#ifdef __MAKETA__
# define INHERITED(c)
#else
# define INHERITED(c) typedef c inherited;
#endif

#ifndef TA_DEF_H
#define TA_DEF_H

#include "ta_stdef.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if (defined(TA_DLL) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef TA_EXPORTS
  #define TA_API __declspec(dllexport)
  #else
  #define TA_API __declspec(dllimport)
  #endif
  #define taTypeDef_Of(T) extern TA_API TypeDef TA_ ## T
  #define taTypeInst_Of(T) extern TA_API T* TAI_ ## T
#else 
#define TA_API
#define taTypeDef_Of(T) extern TypeDef TA_ ## T
#define taTypeInst_Of(T) extern T* TAI_ ## T
#endif

class TA_API ta_memb_ptr_class {
public:
  virtual ~ta_memb_ptr_class()  { }; // make sure it has a vtable..
};

typedef char ta_memb_ptr_class::* ta_memb_ptr; //

#endif // ta_def_h
