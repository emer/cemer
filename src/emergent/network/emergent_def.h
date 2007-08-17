// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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


#ifndef EMERGENT_DEF_H
#define EMERGENT_DEF_H

#include "ta_global.h"

 
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// EMERGENT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if ((defined(EMERGENT_DLL)) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef EMERGENT_EXPORTS
  #define EMERGENT_API __declspec(dllexport)
  #else
  #define EMERGENT_API __declspec(dllimport)
  #endif
#else 
#define EMERGENT_API
#endif


#endif // EMERGENT_DEF_H
