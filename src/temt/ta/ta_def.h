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
#else 
#define TA_API
#endif

// comment directives which are parsed 'internally' (add a # sign before each)
// "NO_TOKENS"
// "IGNORE"
// "REG_FUN"
// "INSTANCE"
// "NO_INSTANCE"
// "HIDDEN"
// "READ_ONLY"
// "NO_CSS"
// "NO_MEMBERS"

/*
  These are the basic pre-defined built in type definitions. Many of these are synonyms
  for each other (ex. int==signed==signed int== etc.)

  The indentation here indicates a DerivesFrom parentage hierarchy (for synonyms.)
  In any context where you want to treat all synonyms of a type the same way, you only
  need to check for "DerivesFrom(TA_xxx)" of the most primary type below.
  ex. if you check for TA_int, you'll get all the synonyms of it.


  'long' is the same size as 'int' on all our 32 and 64-bit platforms. We consider 'long'
  types deprecated.

  intptr_t is an int type whose size is the same as a pointer on your platform. It is not
  actually a basic type, but will be a synonym for either int or int64_t.

  'long long' is 64-bits, but we prefer to use the ansi designation 'int64_t' -- we
  provide a guarded typedef for this (for Windows) in the ta_stdefl.h header file.

  Note that our code is not well tested against any use of unsigned types, and apart from
  'byte' we suggest not using them, particularly in gui contexts.

  Note that the C standard specifies that 'char' 'unsigned char' and 'signed char' are
  distinct types. Most C's (including the ones we support) treat char as signed.
  In tacss/pdp, we use 'char' for its normal purpose (ansi character), and char* for
  C-style strings.
  We use 'byte' as a synonym for 'unsigned char' and treat it as an 8 bit unsigned int.
  This is principally used for color values and in network data patterns. We don't use
  'signed char' but if you use it in your code, it will be treated in the gui as a
  8-bit signed numeric type, not an ansi character.
*/
#ifndef __MAKETA__
extern TA_API TypeDef TA_void;
extern TA_API TypeDef TA_void_ptr;
extern TA_API TypeDef TA_char;
//extern TA_API TypeDef TA_int8_t; // on Windows
extern TA_API TypeDef TA_signed_char;
  extern TA_API TypeDef TA_int8_t;
extern TA_API TypeDef TA_unsigned_char;
  extern TA_API TypeDef TA_uint8_t;  // note: seemingly absent in MSVC
extern TA_API TypeDef TA_short;
  extern TA_API TypeDef TA_signed_short;
  extern TA_API TypeDef TA_short_int;
  extern TA_API TypeDef TA_signed_short_int;
  extern TA_API TypeDef TA_int16_t;
extern TA_API TypeDef TA_unsigned_short;
  extern TA_API TypeDef TA_unsigned_short_int;
  extern TA_API TypeDef TA_uint16_t; // note: seemingly absent in MSVC
extern TA_API TypeDef TA_int;
  extern TA_API TypeDef TA_signed;
  extern TA_API TypeDef TA_signed_int;
  extern TA_API TypeDef TA_int32_t;
//extern TA_API TypeDef TA_intptr_t; // on 32-bit systems
//extern TA_API TypeDef TA_long; // where long is 32-bit
extern TA_API TypeDef TA_unsigned_int;
  extern TA_API TypeDef TA_unsigned;
  extern TA_API TypeDef TA_uint;
//extern TA_API TypeDef TA_unsigned_long; // where long is 32-bit
  extern TA_API TypeDef TA_uint32_t; // note: seemingly absent in MSVC
extern TA_API TypeDef TA_long;
  extern TA_API TypeDef TA_signed_long;
  extern TA_API TypeDef TA_long_int;
  extern TA_API TypeDef TA_signed_long_int;
extern TA_API TypeDef TA_unsigned_long;
  extern TA_API TypeDef TA_unsigned_long_int;
  extern TA_API TypeDef TA_ulong;
extern TA_API TypeDef TA_int64_t;
  extern TA_API TypeDef TA_long_long;
  extern TA_API TypeDef TA_signed_long_long;
//extern TA_API TypeDef TA_intptr_t; // on 64-bit systems
//extern TA_API TypeDef TA_long; // where long is 64-bit
extern TA_API TypeDef TA_uint64_t; // note: seemingly absent in MSVC
  extern TA_API TypeDef TA_unsigned_long_long;
//extern TA_API TypeDef TA_unsigned_long; // where long is 64-bit
extern TA_API TypeDef TA_intptr_t; //NOTE: synonym, will either be 'int' or 'int64_t'
extern TA_API TypeDef TA_uintptr_t;
extern TA_API TypeDef TA_float;
extern TA_API TypeDef TA_double;
extern TA_API TypeDef TA_bool;
extern TA_API TypeDef TA_const; // const is not formal...
extern TA_API TypeDef TA_enum;          // par_formal
extern TA_API TypeDef TA_struct;        // par_formal
extern TA_API TypeDef TA_union; // par_formal
extern TA_API TypeDef TA_class; // par_formal
extern TA_API TypeDef TA_template;      // par_formal
extern TA_API TypeDef TA_templ_inst;    // par_formal template instantiation
extern TA_API TypeDef TA_ta_array;      // par_formal (indicates "arrayness")
extern TA_API TypeDef TA_taBase;
extern TA_API TypeDef TA_taRegFun;      // registered functions
extern TA_API TypeDef TA_TypeItem;      // give these to the user...
extern TA_API TypeDef   TA_TypeDef;
extern TA_API TypeDef   TA_EnumDef;
extern TA_API TypeDef   TA_MemberDefBase;
extern TA_API TypeDef     TA_MemberDef;
extern TA_API TypeDef     TA_PropertyDef;
extern TA_API TypeDef   TA_MethodDef;
extern TA_API TypeDef TA_ta_Globals;
extern TA_API TypeDef TA_taString;
extern TA_API TypeDef TA_Variant;
extern TA_API TypeDef TA_QAtomicInt;
#endif

// for css support
class cssEl;                    // #IGNORE

typedef cssEl* (*css_fun_stub_ptr)(void*, int, cssEl**);
typedef void (*ta_void_fun)();

class TA_API ta_memb_ptr_class {
public:
  virtual ~ta_memb_ptr_class()  { }; // make sure it has a vtable..
};

typedef int ta_memb_ptr_class::* ta_memb_ptr; //

#endif // ta_def_h
