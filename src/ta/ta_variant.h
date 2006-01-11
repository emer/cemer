// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// ta_variant.h -- variant datatype

#ifndef TA_VARIANT_H
#define TA_VARIANT_H

#include "ta_string.h"
#include "ta_stdef.h"

// externals
class taBase;
class taMatrix_impl;

// note: taVariant is based in part on QVariant implementation in Qt
// Variant is always 12 bytes long on both 32/64 platforms

class Variant { // flexible value type that can hold many different types of content
public:

  enum VarType {
    T_Invalid,

    T_Bool,
    T_Int,
    T_UInt,
    T_Int64,
    T_UInt64,
    T_Float,
    T_Double,
    T_Char,
    
    T_Ptr, // void*
    
    T_String,
    
    T_TypeDef, // 
    T_Base, // taBase ref counted
    T_Matrix, // taMatrix ref counted
  };

  Variant& 	operator =(const Variant& cp);
  
  Variant(); // default is null/invalid
  Variant(const Variant &cp);
  Variant(bool val);
  Variant(int val);
  Variant(uint val);
  Variant(int64_t val);
  Variant(uint64_t val);
  Variant(float val);
  Variant(double val);
  Variant(char val);
  Variant(void* val);
  Variant(const String& val);
  Variant(const char* val);
  ~Variant();
protected:
#ifdef __MAKETA__
  unsigned char d[12];
#else
  union Data // sizes are given for 32/64 sys
  {
      bool b; // 8
      int i; // 32
      uint u; // 32
      int64_t i64; // 64
      uint64_t u64; // 64
      float f; // 32
      double d; // 64
      char c;
      intptr_t iptr; // 32/64
      intptr_t str; // 32/64 note: this is an in-place taString, NOT a pointer
      void* ptr; // 32/64
      TypeDef* typ; // 32/64
      taBase* tab; // 32/64 note: properly ref counted; also used for matrix
  } d;
  uint type : 31;
  uint is_null : 1;
#endif

  void			releaseType(); // handles undoing of specials
  //note: following ops don't affect is_null -- context must determine that
  void			setString(const String& cp); // handles setting of a string 
  void			setBase(taBase* cp); // handles setting of a taBase
  void			setMatrix(taMatrix_impl* cp); // handles setting of a matrix
  
  //note: following gets ONLY valid when type is known to be of correct type
  const String& 	getString() const { return *((String*)(&d.str));}
  String& 		getString() { return *((String*)(&d.str));}
  taMatrix_impl*	getMatrix() { return (taMatrix_impl*)(d.tab);} // only if type=T_Matrix
  taMatrix_impl*	getMatrix() const { return (taMatrix_impl*)(d.tab);} // only if type=T_Matrix
};

inline Variant::Variant():type(T_Invalid), is_null(true) { d.i64 = 0; } // default is null/invalid
inline Variant::Variant(bool val):type(T_Bool), is_null(false) {d.b = val;}
inline Variant::Variant(int val):type(T_Int), is_null(false) {d.i = val;}
inline Variant::Variant(uint val):type(T_UInt), is_null(false) {d.u = val;}
inline Variant::Variant(int64_t val):type(T_Int64), is_null(false) {d.i64 = val;}
inline Variant::Variant(uint64_t val):type(T_UInt64), is_null(false) {d.u64 = val;}
inline Variant::Variant(float val):type(T_Float), is_null(false) {d.f = val;}
inline Variant::Variant(double val):type(T_Double), is_null(false) {d.d = val;}
inline Variant::Variant(char val):type(T_Char), is_null(false) {d.c = val;}
inline Variant::Variant(void* val):type(T_Ptr), is_null(false) {d.ptr = val;}
inline Variant::Variant(const String& val):type(T_String), is_null(false) {new(&d.str)String(val);}
inline Variant::Variant(const char* val):type(T_String), is_null(false) 
  {if (val == NULL) {is_null = true; new(&d.str)String();} 
   else {is_null = false; new(&d.str)String(val);}}


#endif

