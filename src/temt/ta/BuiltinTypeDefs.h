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

#ifndef BuiltinTypeDefs_h
#define BuiltinTypeDefs_h 1

// parent includes:
#include <TypeDef>

// this is the list of builtin types, which are installed in the corresponding
// .cpp file, and should be avail by default to any maketa program

extern TypeDef TA_void;
extern TypeDef TA_bool;

extern TypeDef TA_int8_t;
extern TypeDef TA_uint8_t;
extern TypeDef TA_char;
extern TypeDef TA_signed_char;
extern TypeDef TA_unsigned_char;

extern TypeDef TA_int16_t;
extern TypeDef TA_uint16_t;
extern TypeDef TA_short;
extern TypeDef TA_signed_short;
extern TypeDef TA_short_int;
extern TypeDef TA_signed_short_int;
extern TypeDef TA_unsigned_short;
extern TypeDef TA_unsigned_short_int;

extern TypeDef TA_int32_t;
extern TypeDef TA_uint32_t;
extern TypeDef TA_int;
extern TypeDef TA_signed_int;
extern TypeDef TA_signed;
extern TypeDef TA_unsigned_int;
extern TypeDef TA_unsigned;
extern TypeDef TA_uint;

extern TypeDef TA_long;
extern TypeDef TA_signed_long;
extern TypeDef TA_long_int;
extern TypeDef TA_signed_long_int;
extern TypeDef TA_unsigned_long;
extern TypeDef TA_unsigned_long_int;
extern TypeDef TA_ulong;

extern TypeDef TA_int64_t;
extern TypeDef TA_uint64_t;
extern TypeDef TA_long_long;
extern TypeDef TA_signed_long_long;
extern TypeDef TA_unsigned_long_long;

extern TypeDef TA_intptr_t;
extern TypeDef TA_uintptr_t;

extern TypeDef TA_float;
extern TypeDef TA_double;

extern TypeDef TA_taString;
extern TypeDef TA_Variant;

// this function adds the builtin types to the taMisc::types list -- must be called
// at start of type initialization at program startup, prior to adding other types
extern TA_API void tac_AddBuiltinTypeDefs();

#endif // BuiltinTypeDefs_h
