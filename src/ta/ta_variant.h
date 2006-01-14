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
class taMatrix;

// note: taVariant is based in part on QVariant implementation in Qt
// Variant is always 12 bytes long on both 32/64 platforms

class Variant { // flexible value m_type that can hold many different types of content
public:

  // note: VarType values must never change, they are used in persistence, add new ones only
  enum VarType {
    T_Invalid = 0,

    T_Bool = 1,
    T_Int = 2,
    T_UInt = 3,
    T_Int64 = 4,
    T_UInt64 = 5,
    T_Double = 6,
    T_Char = 7,
    T_String = 9,
    
    T_Ptr = 10, // void*
    
    T_Base = 11, // taBase ref counted
    T_Matrix = 12 // taMatrix ref counted
#ifndef __MAKETA__    
    ,T_MaxType = T_Matrix
    ,T_LastType = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
#endif
  };

  bool			isNull() const {return m_is_null;} // 'true' if the value is null
  VarType		type() const {return (VarType)m_type;} //
  
  void			save(ostream& s) const;
  void			load(istream& s);
  
  // the "<type> toXxx()" return a result of requested type, leaving current value as is
  bool toBool() const;
  int toInt() const;
  uint toUInt() const;
  int64_t toInt64() const;
  uint64_t toUInt64() const; //
//  float toFloat() const;
  double toDouble() const;
  char toChar() const;
  void* toPtr() const; // must be a void*, Base, or Matrix, otherwise returns NULL
  String toString() const;
  taBase* toBase() const; // must be a Base or Matrix, otherwise returns NULL
  taMatrix* toMatrix() const; // must be a Matrix, otherwise returns NULL
  
  // following are the automatic operators for C++ casting
  operator bool() const {return toBool();}
  operator int() const {return toInt();}
  operator uint() const {return toUInt();}
  operator int64_t() const {return toInt64();}
  operator uint64_t() const {return toUInt64();} //
//  operator float() const {return toFloat();}
  operator float() const {return (float)toDouble();}
  operator double() const {return toDouble();}
  operator char() const {return toChar();}
  operator void*() const {return toPtr();}
  operator String() const {return toString();}
  operator taBase*() const {return toBase();}
  operator taMatrix*() const {return toMatrix();} //
  
  
//TODO  bool			canCast(VarType new_type);
    // returns 'true' if current type can be successfully cast to requested type
  // assignment operators
  Variant& 	operator=(const Variant& cp);
  Variant& 	operator=(bool val);
  Variant& 	operator=(int val);
  Variant& 	operator=(uint val);
  Variant& 	operator=(int64_t val);
  Variant& 	operator=(uint64_t val);
  Variant& 	operator=(float val);
  Variant& 	operator=(double val);
  Variant& 	operator=(char val);
  Variant& 	operator=(void* val);
  Variant& 	operator=(const String& val);
  Variant& 	operator=(taBase* val);
  Variant& 	operator=(taMatrix* val);
  
#ifdef __MAKETA__
  friend ostream&   operator<<(ostream& s, const Variant& x);
  friend istream&   operator>>(istream& s, Variant& x);
#else
  friend std::ostream&   operator<<(std::ostream& s, const Variant& x); // streams type code, then value
  friend std::istream&   operator>>(std::istream& s, Variant& x);  // expects: type code then value
#endif
  
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
  Variant(taBase* val);
  Variant(taMatrix* val);
  
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
      double d; // 64
      char c;
      intptr_t str; // 32/64 note: this is an in-place taString, NOT a pointer
      void* ptr; // 32/64
      taBase* tab; // 32/64 note: properly ref counted; also used for matrix
  } d;
  uint m_type : 31;
  uint m_is_null : 1;
#endif

  void			releaseType(); // #IGNORE handles undoing of specials
  //note: following ops don't affect m_is_null -- context must determine that
  void			setString(const String& cp); // #IGNORE handles setting of a string 
  void			setBase(taBase* cp); // #IGNORE handles setting of a taBase
  void			setMatrix(taMatrix* cp); // #IGNORE handles setting of a matrix
  
  //note: following gets ONLY valid when m_type is known to be of correct type
  const String& 	getString() const { return *((String*)(&d.str));} // #IGNORE
  String& 		getString() { return *((String*)(&d.str));} // #IGNORE
  taMatrix*	getMatrix() { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
  taMatrix*	getMatrix() const { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
};

inline Variant::Variant():m_type(T_Invalid), m_is_null(true) { d.i64 = 0; } // default is null/invalid
inline Variant::Variant(bool val):m_type(T_Bool), m_is_null(false) {d.b = val;}
inline Variant::Variant(int val):m_type(T_Int), m_is_null(false) {d.i = val;}
inline Variant::Variant(uint val):m_type(T_UInt), m_is_null(false) {d.u = val;}
inline Variant::Variant(int64_t val):m_type(T_Int64), m_is_null(false) {d.i64 = val;}
inline Variant::Variant(uint64_t val):m_type(T_UInt64), m_is_null(false) {d.u64 = val;}
inline Variant::Variant(float val):m_type(T_Double), m_is_null(false) {d.d = val;}
inline Variant::Variant(double val):m_type(T_Double), m_is_null(false) {d.d = val;}
inline Variant::Variant(char val):m_type(T_Char), m_is_null(false) {d.c = val;}
inline Variant::Variant(void* val):m_type(T_Ptr) {m_is_null = (val == NULL); d.ptr = val;}
inline Variant::Variant(const String& val):m_type(T_String), m_is_null(false) {new(&d.str)String(val);}
inline Variant::Variant(const char* val):m_type(T_String), m_is_null(false) 
  {if (val == NULL) {m_is_null = true; new(&d.str)String();} 
   else {m_is_null = false; new(&d.str)String(val);}}


#endif

