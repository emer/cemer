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


#ifndef SO_DEF_H
#define SO_DEF_H

#include "ta_global.h"
 
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if ((defined(SO_DLL)) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef SO_EXPORTS
  #define SO_API __declspec(dllexport)
  #else
  #define SO_API __declspec(dllimport)
  #endif
#else 
#define SO_API
#endif


#endif // SO_DEF_H
