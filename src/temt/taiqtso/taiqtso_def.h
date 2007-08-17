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


#ifndef TAIQTSO_DEF_H
#define TAIQTSO_DEF_H

#include "ta_global.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TAIQTSO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TAIQTSO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if ((defined(TAIQTSO_DLL)) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef TAIQTSO_EXPORTS
  #define TAIQTSO_API __declspec(dllexport)
  #else
  #define TAIQTSO_API __declspec(dllimport)
  #endif
#else 
  #define TAIQTSO_API
#endif

// Qt3 semi-compatibility macro
#define SET_PALETTE_BACKGROUND_COLOR(w,c) { \
  QPalette pal = w->palette(); \
  pal.setColor(QPalette::Background,c); \
  w->setPalette(pal);} 

#define SET_PALETTE_COLOR(w,r,c) { \
  QPalette pal = w->palette(); \
  pal.setColor(r,c); \
  w->setPalette(pal);} 

#define COLOR_RO_BACKGROUND Qt::lightGray

// NOTE: these are instantiated in tai_qtso_ti.cc (for want of a better place)
// COLOR_BRIGHT_HILIGHT is for checkboxes, for better contrast
//#define COLOR_BRIGHT_HILIGHT qtsoMisc::color_bright_hilight
extern TAIQTSO_API QColor& COLOR_HILIGHT;
extern TAIQTSO_API QColor& COLOR_BRIGHT_HILIGHT;

#endif // TAIQTSO_DEF_H
