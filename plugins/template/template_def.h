#ifndef TEMPLATE_DEF_H
#define TEMPLATE_DEF_H

// TODO: add any copyright information or license information here --
//  (The GPL license is included by default below.)

//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include <ta_def.h>

// The following ifdef block is the standard way of creating macros for exporting from
// a Windows dll. You must decorate your classes and static functions with the
// TEMPLATE_API macro so that your plugin will compile on Windows. Published plugins
// MUST be compileable on all three platforms (Windows/Mac/Linux) unless they are 
// specifically created for features of a specific platform.
// All files within this DLL are compiled with the TEMPLATE_EXPORTS pre-processor symbol
// defined. This symbol should then not be defined on any other plugin that dynamically
// links to this plugin. This way any other project whose source files include this file see 
// TEMPLATE_API functions as being imported from a dll, whereas this dll sees symbols
// defined with this macro as being exported.
#if ((defined(EMERGENT_DLL)) && (defined(TA_OS_WIN) && defined(_MSC_VER)) && (!defined(__MAKETA__)))
  #ifdef template_EXPORTS
  #define TEMPLATE_API __declspec(dllexport)
  #else
  #define TEMPLATE_API __declspec(dllimport)
  #endif
  #define pTypeDef_Of(T) extern TEMPLATE_API TypeDef TA_ ## T
  #define pTypeInst_Of(T) extern TEMPLATE_API T* TAI_ ## T
#else 
#define TEMPLATE_API
#define pTypeDef_Of(T) extern TypeDef TA_ ## T
#define pTypeInst_Of(T) extern T* TAI_ ## T
#endif

#undef QT_SHARED                             // Already defined in config.h.

#endif // TEMPLATE_DEF_H
