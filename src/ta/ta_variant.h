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
class TypeDef;
#ifndef NO_TA_BASE
class taBase;
class taMatrix;
#endif


// note: taVariant is based in part on QVariant implementation in Qt
// Variant is always 12 bytes long on both 32/64 platforms

class TA_API Variant { // flexible value m_type that can hold many different types of content
public:

  // note: VarType values must never change, they are used in persistence, add new ones only
  enum VarType {
    T_Invalid = 0, 	// #LABEL_Invalid

    T_Bool = 1, 	// #LABEL_Bool
    T_Int = 2, 		// #LABEL_Int
    T_UInt = 3, 	// #LABEL_UInt
    T_Int64 = 4, 	// #LABEL_Int64
    T_UInt64 = 5, 	// #LABEL_UInt64
    T_Double = 6, 	// #LABEL_Real
    T_Char = 7, 	// #LABEL_Char
    T_String = 9, 	// #LABEL_String
    
    T_Ptr = 10, 	// #LABEL_Ptr void*
    
    T_Base = 11, 	// #LABEL_taBase taBase ref counted
    T_Matrix = 12 	// #LABEL_taMatrix taMatrix ref counted
#ifndef __MAKETA__
    ,T_Atomic_Min = T_Bool,
    T_Atomic_Max = T_String
#endif
  };

  const void*		addrData() const {return &d;} // this is for low-level routines
  
  bool			isAtomic() const {return (m_type <= T_String);} 
    // 'true' for non-ptr types (includes Invalid)
  bool			isDefault() const; // returns 'true' if contains the default value for the type (null is ignored)
  bool			isInvalid() const {return (m_type == T_Invalid);} 
  bool			isNull() const; // 'true' if the value is null
  bool			isNumeric() const; 
    // 'true' if NumericStrict, or valid numeric string
  bool			isNumericStrict() const; // 'true' if an int-ish, float, or char type
  bool			isStringType() const {return ((m_type == T_String) || (m_type == T_Char));}
   // 'true' if a char or String
  bool			isPtrType() const {return ((m_type >= T_Ptr) && (m_type <= T_Matrix));} 
    // 'true' if the value is a void*, taBase*, or taMatrix*
  bool			isBaseType() const {return ((m_type == T_Base) || (m_type == T_Matrix));} 
    // 'true' if the value is a taBase* or taMatrix*
  bool			isMatrixType() const {return (m_type == T_Matrix);} 
    // 'true' if the value is a taMatrix*, BUT could be NULL
  VarType		type() const {return (VarType)m_type;} //
  void			setType(VarType value); // force it to be given type, if changed, set to default value
  String		getTypeAsString() const; // for debugging, get variant type as a string

  void			save(ostream& s) const; // streams out using << for the type
  void			load(istream& s); // streams in as a string, use toXxx if it is of another type
  
// following are ops to set to a specific type of value  
  void 			setInvalid(); // invalid/null
  void 			setVariant(const Variant& cp); // basically a copy
  void			setVariantData(const Variant& cp); // preserve target type, only copy data
  void 			setBool(bool val, bool null = false);
  void 			setByte(byte val, bool null = false) {setUInt(val, null);}
  void 			setInt(int val, bool null = false);
  void 			setUInt(uint val, bool null = false);
  void 			setInt64(int64_t val, bool null = false);
  void 			setUInt64(uint64_t val, bool null = false);
  void 			setIntPtr(intptr_t val, bool null = false)
    {if (sizeof(intptr_t) == sizeof(int)) setInt(val, null); else setInt64(val, null);}
  void 			setFloat(float val, bool null = false);
  void 			setDouble(double val, bool null = false);
  void 			setChar(char val, bool null = false);
  void 			setPtr(void* val);
  void			setString(const String& cp, bool null = false); // handles setting of a string 
  void			setCString(const char* val, bool null = false)
    {setString(String(val), null);}
#ifndef NO_TA_BASE
  void			setBase(taBase* cp); // handles setting of a taBase
  void			setMatrix(taMatrix* cp); // handles setting of a matrix
#endif  

  void			updateFromString(const String& val); // set value from string, but keep current type
  
  // the "<type> toXxx()" return a result of requested type, leaving current value as is
  bool 			toBool() const;
  byte 			toByte() const {return (byte)toUInt();}
  int 			toInt() const;
  uint 			toUInt() const;
  int64_t 		toInt64() const;
  uint64_t 		toUInt64() const; //
  float 		toFloat() const {return (float)toDouble();}
  double 		toDouble() const;
  char 			toChar() const;
  void* 		toPtr() const; // must be a void*, Base, or Matrix, otherwise returns NULL
  String 		toString() const;
#ifndef NO_TA_BASE
  taBase* 		toBase() const; // must be a Base or Matrix, otherwise returns NULL
  taMatrix* 		toMatrix() const; // must be a Matrix, otherwise returns NULL
#endif  
  const String		toCssLiteral() const; // to a form suitable for initializing a Css variable, ex. quoted strings, U suffix for unsigned, path for taBase variable, etc.
  // following are the operators for C++ casting -- note they are all explicit
  // because you can't mix having cast operators with having various math operators
/*can't have converters and ops
  operator bool() const {return toBool();}
  operator byte() const {return toByte();}
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
#ifndef NO_TA_BASE
  operator taBase*() const {return toBase();}
  operator taMatrix*() const {return toMatrix();} //
#endif  
*/
  // equality operations  
  bool			eqVariant(const Variant& val) const; // value equality, using fairly relaxed type rules; Invalid never == anything
  bool 			eqBool(bool val) const;
  bool 			eqByte(byte val) const {return eqUInt(val);}
  bool 			eqInt(int val) const;
  bool 			eqUInt(uint val) const;
  bool 			eqInt64(int64_t val) const;
  bool 			eqUInt64(uint64_t val) const;
  bool 			eqIntPtr(intptr_t val) const
    {if (sizeof(intptr_t) == sizeof(int)) return eqInt(val); else return eqInt64(val);}
  bool 			eqFloat(float val) const {return eqDouble(val);}
  bool 			eqDouble(double val) const;
  bool 			eqChar(char val) const;
  bool			eqString(const String& val) const; // handles eqting of a string 
  bool			eqCString(const char* val) const
    {return eqString(String(val));}
  bool 			eqPtr(const void* val) const;
#ifndef NO_TA_BASE
  bool			eqBase(const taBase* val) const {return eqPtr(val);} 
  bool			eqMatrix(const taMatrix* val) const {return eqPtr(val);} 
#endif

  // comparison operations, < -ve, == 0, > +ve
  int			cmpVariant(const Variant& val) const; // value compare, using fairly relaxed type rules; Invalid never == anything
  int 			cmpBool(bool val) const;
  int 			cmpByte(byte val) const {return cmpUInt(val);}
  int 			cmpInt(int val) const;
  int 			cmpUInt(uint val) const;
  int 			cmpInt64(int64_t val) const;
  int 			cmpUInt64(uint64_t val) const;
  int 			cmpIntPtr(intptr_t val) const
    {if (sizeof(intptr_t) == sizeof(int)) return cmpInt(val); else return cmpInt64(val);}
  int 			cmpFloat(float val) const {return cmpDouble(val);}
  int 			cmpDouble(double val) const;
  int 			cmpChar(char val) const;
  int			cmpString(const String& val) const; // handles cmpting of a string 
  int			cmpCString(const char* val) const
    {return cmpString(String(val));}
  int 			cmpPtr(const void* val) const;
#ifndef NO_TA_BASE
  int			cmpBase(const taBase* val) const {return cmpPtr(val);} 
  int			cmpMatrix(const taMatrix* val) const {return cmpPtr(val);} 
#endif

//TODO  bool			canCast(VarType new_type);
    // returns 'true' if current type can be successfully cast to requested type
  // assignment operators
  Variant& 	operator=(const Variant& val) {setVariant(val); return *this;}
  Variant& 	operator=(bool val) {setBool(val); return *this;}
  Variant& 	operator=(byte val) {setUInt(val); return *this;}
  Variant& 	operator=(int val) {setInt(val); return *this;}
  Variant& 	operator=(uint val) {setUInt(val); return *this;}
  Variant& 	operator=(int64_t val) {setInt64(val); return *this;}
  Variant& 	operator=(uint64_t val) {setUInt64(val); return *this;}
  Variant& 	operator=(float val) {setFloat(val); return *this;}
  Variant& 	operator=(double val) {setDouble(val); return *this;}
  Variant& 	operator=(char val) {setChar(val); return *this;}
  Variant& 	operator=(void* val) {setPtr(val); return *this;}
  Variant& 	operator=(const String& val) {setString(val); return *this;}
  Variant& 	operator=(const char* val) {setCString(val); return *this;} //
#ifndef NO_TA_BASE
  Variant& 	operator=(taBase* val) {setBase(val); return *this;}
  Variant& 	operator=(taMatrix* val) {setMatrix(val); return *this;} //
#endif

  Variant& 	operator+=(const String& rhs); 
    // concatenation, result is String if input is String or atomic

  //note: x= operators can cause type to change following C++ type promotion semantics:
  // if either is "double" result is "double"
  // else if equal-sized or longer op is "unsigned" other op is forced to "unsigned" 
  //   and size is forced to largest of two ops 
  Variant& 	operator+=(char rhs); // note: also does concat if we are a String
  Variant& 	operator+=(int rhs);
  Variant& 	operator+=(uint rhs);
  Variant& 	operator+=(int64_t rhs);
  Variant& 	operator+=(uint64_t rhs);
  Variant& 	operator+=(double rhs);
  Variant& 	operator+=(const Variant& rhs);
  
  Variant& 	operator-=(char rhs);
  Variant& 	operator-=(int rhs);
  Variant& 	operator-=(uint rhs);
  Variant& 	operator-=(int64_t rhs);
  Variant& 	operator-=(uint64_t rhs);
  Variant& 	operator-=(double rhs);
  Variant& 	operator-=(const Variant& rhs);
  
  Variant& 	operator*=(char rhs);
  Variant& 	operator*=(int rhs);
  Variant& 	operator*=(uint rhs);
  Variant& 	operator*=(int64_t rhs);
  Variant& 	operator*=(uint64_t rhs);
  Variant& 	operator*=(double rhs);
  Variant& 	operator*=(const Variant& rhs);
  
  Variant& 	operator/=(char rhs);
  Variant& 	operator/=(int rhs);
  Variant& 	operator/=(uint rhs);
  Variant& 	operator/=(int64_t rhs);
  Variant& 	operator/=(uint64_t rhs);
  Variant& 	operator/=(double rhs);
  Variant& 	operator/=(const Variant& rhs); //
  
  //note: %, <<, >>, and bitwise operator only valid for int types, not reals
  Variant& 	operator%=(char rhs);
  Variant& 	operator%=(int rhs);
  Variant& 	operator%=(uint rhs);
  Variant& 	operator%=(int64_t rhs);
  Variant& 	operator%=(uint64_t rhs);
  Variant& 	operator%=(const Variant& rhs); //

  Variant& 	operator<<=(char rhs);
  Variant& 	operator<<=(int rhs);
  Variant& 	operator<<=(uint rhs);
  Variant& 	operator<<=(int64_t rhs);
  Variant& 	operator<<=(uint64_t rhs);
  Variant& 	operator<<=(const Variant& rhs); //

  Variant& 	operator>>=(char rhs);
  Variant& 	operator>>=(int rhs);
  Variant& 	operator>>=(uint rhs);
  Variant& 	operator>>=(int64_t rhs);
  Variant& 	operator>>=(uint64_t rhs);
  Variant& 	operator>>=(const Variant& rhs); //

  Variant& 	operator&=(char rhs);
  Variant& 	operator&=(int rhs);
  Variant& 	operator&=(uint rhs);
  Variant& 	operator&=(int64_t rhs);
  Variant& 	operator&=(uint64_t rhs);
  Variant& 	operator&=(const Variant& rhs); //

  Variant& 	operator|=(char rhs);
  Variant& 	operator|=(int rhs);
  Variant& 	operator|=(uint rhs);
  Variant& 	operator|=(int64_t rhs);
  Variant& 	operator|=(uint64_t rhs);
  Variant& 	operator|=(const Variant& rhs); //

  Variant& 	operator^=(char rhs); // xor
  Variant& 	operator^=(int rhs);
  Variant& 	operator^=(uint rhs);
  Variant& 	operator^=(int64_t rhs);
  Variant& 	operator^=(uint64_t rhs);
  Variant& 	operator^=(const Variant& rhs); //

  Variant& 	operator++(); // prefix
  Variant 	operator++(int); // postfix (arg is dummy)
  Variant& 	operator--(); // prefix
  Variant 	operator--(int); // postfix (arg is dummy)
  
  Variant&	operator-(); // unary -
  Variant&	operator~(); // unary ~
  bool		operator!() {return !toBool();} 

#ifdef __MAKETA__
  friend ostream&   operator<<(ostream& s, const Variant& x);
  friend istream&   operator>>(istream& s, Variant& x);
#else
  friend std::ostream&   operator<<(std::ostream& s, const Variant& x); // streams type code, then value
  friend std::istream&   operator>>(std::istream& s, Variant& x);  // expects: type code then value
#endif 
  
#ifdef TA_USE_QT
  Variant(const QVariant &val);
  void 		setQVariant(const QVariant& cp); 
  Variant& 	operator=(const QVariant& val) {setQVariant(val); return *this;}
  QVariant 	toQVariant() const;
  operator QVariant() const {return toQVariant();}
  Variant(const QString& val);
  Variant&	operator = (const QString& y) {setCString(y.toLatin1()); return *this;}
  operator QString() const {return QString(toString().chars());}
#endif 

  Variant(); // default is null/invalid
  explicit Variant(VarType vt); // create with a specific type, of the default value of that type
  Variant(const Variant &cp);
  Variant(bool val);
  Variant(byte val);
  Variant(int val);
  Variant(uint val);
  Variant(long val);
  Variant(unsigned long val);
  Variant(int64_t val);
  Variant(uint64_t val);
  Variant(float val);
  Variant(double val);
  Variant(char val);
  Variant(void* val);
  Variant(const String& val);
  Variant(const char* val);
#ifndef NO_TA_BASE
  Variant(taBase* val);
  Variant(taMatrix* val);
#endif 
  ~Variant(); //

public: // following primarily for TypeDef usage, streaming, etc.
  void			GetRepInfo(TypeDef*& typ, void*& data); // current typedef, and pointer to the data
  void			UpdateAfterLoad(); // called after internal modifications, to reassert correctness of null etc.
  void			ForceType(VarType vt, bool null);
    // called by streaming system to force the type to be indicated kind
  void			Dump_Save_Type(ostream& strm) const; // dumps type and null 
  bool			Dump_Load_Type(istream& strm, int& last_char); 
    // loads type and null, using taMisc:: strm routines; calls ForceType; returns 'true' if type loaded 

protected:
#ifdef __MAKETA__
  unsigned char d[12];
#else
  union Data // sizes are given for 32/64 sys
  {
      bool b; // 8
      int i; // 32
      uint u; // 32 -- also for byte
      int64_t i64; // 64
      uint64_t u64; // 64
      double d; // 64
      char c;
      intptr_t str; // 32/64 note: this is an in-place taString, NOT a pointer
      void* ptr; // 32/64
#ifndef NO_TA_BASE
      taBase* tab; // 32/64 note: properly ref counted; also used for matrix
#endif
  } d;
  int m_type : 29;
  mutable int m_is_numeric : 1; // true when we've tested string for numeric
  mutable int m_is_numeric_valid : 1; // set when we've set is_numeric; clear on set of str
  int m_is_null : 1;
#endif

  inline void		init(int type_ = T_Invalid, bool is_numeric_ = 0, 
    bool is_numeric_valid_ = 0, bool is_null_ = 1) 
    { m_type = type_; m_is_numeric = is_numeric_; 
    m_is_numeric_valid = is_numeric_valid_;  m_is_null = is_null_;}
  void			releaseType(); // #IGNORE handles undoing of specials
  //note: following ops don't affect m_is_null -- context must determine that
  
  //note: following gets ONLY valid when m_type is known to be of correct type
  const String& 	getString() const { return *((String*)(&d.str));} // #IGNORE
  String& 		getString() { return *((String*)(&d.str));} // #IGNORE
#ifndef NO_TA_BASE
  taMatrix*		getMatrix() { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
  taMatrix*		getMatrix() const { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
#endif
  void			warn(const char* msg) const; // maybe output warning of invalid operation
};

// empty invalid variant
#define _nilVariant Variant()

inline Variant::Variant():m_type(T_Invalid), m_is_numeric(false), m_is_null(true) 
  { d.i64 = 0; } // default is null/invalid
inline Variant::Variant(bool val):m_type(T_Bool), m_is_null(false) {d.b = val;}
inline Variant::Variant(byte val):m_type(T_UInt), m_is_null(false) {d.u = val;}
inline Variant::Variant(int val):m_type(T_Int), m_is_null(false) {d.i = val;}
inline Variant::Variant(uint val):m_type(T_UInt), m_is_null(false) {d.u = val;}
//NOTE: long versions assume size(int)==size(long), s/b true for our platforms
inline Variant::Variant(long val):m_type(T_Int), m_is_null(false) {d.i = val;}
inline Variant::Variant(unsigned long val):m_type(T_UInt), m_is_null(false) {d.u = val;}
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

inline Variant operator+(const Variant& a, int b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, uint b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, int64_t b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, uint64_t b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, double b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, char b) {Variant r(a); r += b; return r;}
inline Variant operator+(const Variant& a, const String b) {Variant r(a); r += b; return r;}
inline Variant operator+(int a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(uint a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(int64_t a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(uint64_t a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(double a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(char a, const Variant& b) {Variant r(a); r += b; return r;}
inline Variant operator+(const String a, const Variant& b) {Variant r(a); r += b; return r;}

inline Variant operator-(const Variant& a, int b) {Variant r(a); r -= b; return r;}
inline Variant operator-(const Variant& a, uint b) {Variant r(a); r -= b; return r;}
inline Variant operator-(const Variant& a, int64_t b) {Variant r(a); r -= b; return r;}
inline Variant operator-(const Variant& a, uint64_t b) {Variant r(a); r -= b; return r;}
inline Variant operator-(const Variant& a, double b) {Variant r(a); r -= b; return r;}
inline Variant operator-(const Variant& a, char b) {Variant r(a); r -= b; return r;}
inline Variant operator-(int a, const Variant& b) {Variant r(a); r -= b; return r;}
inline Variant operator-(uint a, const Variant& b) {Variant r(a); r -= b; return r;}
inline Variant operator-(int64_t a, const Variant& b) {Variant r(a); r -= b; return r;}
inline Variant operator-(uint64_t a, const Variant& b) {Variant r(a); r -= b; return r;}
inline Variant operator-(double a, const Variant& b) {Variant r(a); r -= b; return r;}
inline Variant operator-(char a, const Variant& b) {Variant r(a); r -= b; return r;}

inline Variant operator*(const Variant& a, int b) {Variant r(a); r *= b; return r;}
inline Variant operator*(const Variant& a, uint b) {Variant r(a); r *= b; return r;}
inline Variant operator*(const Variant& a, int64_t b) {Variant r(a); r *= b; return r;}
inline Variant operator*(const Variant& a, uint64_t b) {Variant r(a); r *= b; return r;}
inline Variant operator*(const Variant& a, double b) {Variant r(a); r *= b; return r;}
inline Variant operator*(const Variant& a, char b) {Variant r(a); r *= b; return r;}
inline Variant operator*(int a, const Variant& b) {Variant r(a); r *= b; return r;}
inline Variant operator*(uint a, const Variant& b) {Variant r(a); r *= b; return r;}
inline Variant operator*(int64_t a, const Variant& b) {Variant r(a); r *= b; return r;}
inline Variant operator*(uint64_t a, const Variant& b) {Variant r(a); r *= b; return r;}
inline Variant operator*(double a, const Variant& b) {Variant r(a); r *= b; return r;}
inline Variant operator*(char a, const Variant& b) {Variant r(a); r *= b; return r;}

inline Variant operator/(const Variant& a, int b) {Variant r(a); r /= b; return r;}
inline Variant operator/(const Variant& a, uint b) {Variant r(a); r /= b; return r;}
inline Variant operator/(const Variant& a, int64_t b) {Variant r(a); r /= b; return r;}
inline Variant operator/(const Variant& a, uint64_t b) {Variant r(a); r /= b; return r;}
inline Variant operator/(const Variant& a, double b) {Variant r(a); r /= b; return r;}
inline Variant operator/(const Variant& a, char b) {Variant r(a); r /= b; return r;}
inline Variant operator/(int a, const Variant& b) {Variant r(a); r /= b; return r;}
inline Variant operator/(uint a, const Variant& b) {Variant r(a); r /= b; return r;}
inline Variant operator/(int64_t a, const Variant& b) {Variant r(a); r /= b; return r;}
inline Variant operator/(uint64_t a, const Variant& b) {Variant r(a); r /= b; return r;}
inline Variant operator/(double a, const Variant& b) {Variant r(a); r /= b; return r;}
inline Variant operator/(char a, const Variant& b) {Variant r(a); r /= b; return r;}

inline Variant operator%(const Variant& a, int b) {Variant r(a); r %= b; return r;}
inline Variant operator%(const Variant& a, uint b) {Variant r(a); r %= b; return r;}
inline Variant operator%(const Variant& a, int64_t b) {Variant r(a); r %= b; return r;}
inline Variant operator%(const Variant& a, uint64_t b) {Variant r(a); r %= b; return r;}
inline Variant operator%(const Variant& a, char b) {Variant r(a); r %= b; return r;}
inline Variant operator%(int a, const Variant& b) {Variant r(a); r %= b; return r;}
inline Variant operator%(uint a, const Variant& b) {Variant r(a); r %= b; return r;}
inline Variant operator%(int64_t a, const Variant& b) {Variant r(a); r %= b; return r;}
inline Variant operator%(uint64_t a, const Variant& b) {Variant r(a); r %= b; return r;}
inline Variant operator%(char a, const Variant& b) {Variant r(a); r %= b; return r;}

inline Variant operator<<(const Variant& a, int b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(const Variant& a, uint b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(const Variant& a, int64_t b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(const Variant& a, uint64_t b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(const Variant& a, char b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(int a, const Variant& b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(uint a, const Variant& b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(int64_t a, const Variant& b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(uint64_t a, const Variant& b) {Variant r(a); r <<= b; return r;}
inline Variant operator<<(char a, const Variant& b) {Variant r(a); r <<= b; return r;}

inline Variant operator>>(const Variant& a, int b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(const Variant& a, uint b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(const Variant& a, int64_t b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(const Variant& a, uint64_t b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(const Variant& a, char b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(int a, const Variant& b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(uint a, const Variant& b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(int64_t a, const Variant& b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(uint64_t a, const Variant& b) {Variant r(a); r >>= b; return r;}
inline Variant operator>>(char a, const Variant& b) {Variant r(a); r >>= b; return r;}

inline Variant operator&(const Variant& a, int b) {Variant r(a); r &= b; return r;}
inline Variant operator&(const Variant& a, uint b) {Variant r(a); r &= b; return r;}
inline Variant operator&(const Variant& a, int64_t b) {Variant r(a); r &= b; return r;}
inline Variant operator&(const Variant& a, uint64_t b) {Variant r(a); r &= b; return r;}
inline Variant operator&(const Variant& a, char b) {Variant r(a); r &= b; return r;}
inline Variant operator&(int a, const Variant& b) {Variant r(a); r &= b; return r;}
inline Variant operator&(uint a, const Variant& b) {Variant r(a); r &= b; return r;}
inline Variant operator&(int64_t a, const Variant& b) {Variant r(a); r &= b; return r;}
inline Variant operator&(uint64_t a, const Variant& b) {Variant r(a); r &= b; return r;}
inline Variant operator&(char a, const Variant& b) {Variant r(a); r &= b; return r;}

inline Variant operator|(const Variant& a, int b) {Variant r(a); r |= b; return r;}
inline Variant operator|(const Variant& a, uint b) {Variant r(a); r |= b; return r;}
inline Variant operator|(const Variant& a, int64_t b) {Variant r(a); r |= b; return r;}
inline Variant operator|(const Variant& a, uint64_t b) {Variant r(a); r |= b; return r;}
inline Variant operator|(const Variant& a, char b) {Variant r(a); r |= b; return r;}
inline Variant operator|(int a, const Variant& b) {Variant r(a); r |= b; return r;}
inline Variant operator|(uint a, const Variant& b) {Variant r(a); r |= b; return r;}
inline Variant operator|(int64_t a, const Variant& b) {Variant r(a); r |= b; return r;}
inline Variant operator|(uint64_t a, const Variant& b) {Variant r(a); r |= b; return r;}
inline Variant operator|(char a, const Variant& b) {Variant r(a); r |= b; return r;}

inline Variant operator^(const Variant& a, int b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(const Variant& a, uint b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(const Variant& a, int64_t b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(const Variant& a, uint64_t b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(const Variant& a, char b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(int a, const Variant& b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(uint a, const Variant& b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(int64_t a, const Variant& b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(uint64_t a, const Variant& b) {Variant r(a); r ^= b; return r;}
inline Variant operator^(char a, const Variant& b) {Variant r(a); r ^= b; return r;}

inline bool operator==(const Variant& a, const Variant& b) {return a.eqVariant(b);}
inline bool operator!=(const Variant& a, const Variant& b) {return !a.eqVariant(b);}
inline bool operator< (const Variant& a, const Variant& b) {return a.cmpVariant(b) < 0;}
inline bool operator<=(const Variant& a, const Variant& b) {return a.cmpVariant(b) <= 0;}
inline bool operator> (const Variant& a, const Variant& b) {return a.cmpVariant(b) > 0;}
inline bool operator>=(const Variant& a, const Variant& b) {return a.cmpVariant(b) >= 0;}

inline bool operator==(const Variant& a, bool b) {return a.eqBool(b);}
inline bool operator!=(const Variant& a, bool b) {return !a.eqBool(b);}
inline bool operator==(bool a, const Variant& b) {return b.eqBool(a);}
inline bool operator!=(bool a, const Variant& b) {return !b.eqBool(a);}
inline bool operator< (const Variant& a, bool b) {return a.cmpBool(b) < 0;}
inline bool operator<=(const Variant& a, bool b) {return a.cmpBool(b) <= 0;}
inline bool operator> (const Variant& a, bool b) {return a.cmpBool(b) > 0;}
inline bool operator>=(const Variant& a, bool b) {return a.cmpBool(b) >= 0;}
inline bool operator< (bool a, const Variant& b) {return b.cmpBool(a) >= 0;}
inline bool operator<=(bool a, const Variant& b) {return b.cmpBool(a) > 0;}
inline bool operator> (bool a, const Variant& b) {return b.cmpBool(a) <= 0;}
inline bool operator>=(bool a, const Variant& b) {return b.cmpBool(a) < 0;}

inline bool operator==(const Variant& a, byte b) {return a.eqByte(b);}
inline bool operator!=(const Variant& a, byte b) {return !a.eqByte(b);}
inline bool operator==(byte a, const Variant& b) {return b.eqByte(a);}
inline bool operator!=(byte a, const Variant& b) {return !b.eqByte(a);}
inline bool operator< (const Variant& a, byte b) {return a.cmpByte(b) < 0;}
inline bool operator<=(const Variant& a, byte b) {return a.cmpByte(b) <= 0;}
inline bool operator> (const Variant& a, byte b) {return a.cmpByte(b) > 0;}
inline bool operator>=(const Variant& a, byte b) {return a.cmpByte(b) >= 0;}
inline bool operator< (byte a, const Variant& b) {return b.cmpByte(a) >= 0;}
inline bool operator<=(byte a, const Variant& b) {return b.cmpByte(a) > 0;}
inline bool operator> (byte a, const Variant& b) {return b.cmpByte(a) <= 0;}
inline bool operator>=(byte a, const Variant& b) {return b.cmpByte(a) < 0;}

inline bool operator==(const Variant& a, int b) {return a.eqInt(b);}
inline bool operator!=(const Variant& a, int b) {return !a.eqInt(b);}
inline bool operator==(int a, const Variant& b) {return b.eqInt(a);}
inline bool operator!=(int a, const Variant& b) {return !b.eqInt(a);}
inline bool operator< (const Variant& a, int b) {return a.cmpInt(b) < 0;}
inline bool operator<=(const Variant& a, int b) {return a.cmpInt(b) <= 0;}
inline bool operator> (const Variant& a, int b) {return a.cmpInt(b) > 0;}
inline bool operator>=(const Variant& a, int b) {return a.cmpInt(b) >= 0;}
inline bool operator< (int a, const Variant& b) {return b.cmpInt(a) >= 0;}
inline bool operator<=(int a, const Variant& b) {return b.cmpInt(a) > 0;}
inline bool operator> (int a, const Variant& b) {return b.cmpInt(a) <= 0;}
inline bool operator>=(int a, const Variant& b) {return b.cmpInt(a) < 0;}

inline bool operator==(const Variant& a, uint b) {return a.eqUInt(b);}
inline bool operator!=(const Variant& a, uint b) {return !a.eqUInt(b);}
inline bool operator==(uint a, const Variant& b) {return b.eqUInt(a);}
inline bool operator!=(uint a, const Variant& b) {return !b.eqUInt(a);}
inline bool operator< (const Variant& a, uint b) {return a.cmpUInt(b) < 0;}
inline bool operator<=(const Variant& a, uint b) {return a.cmpUInt(b) <= 0;}
inline bool operator> (const Variant& a, uint b) {return a.cmpUInt(b) > 0;}
inline bool operator>=(const Variant& a, uint b) {return a.cmpUInt(b) >= 0;}
inline bool operator< (uint a, const Variant& b) {return b.cmpUInt(a) >= 0;}
inline bool operator<=(uint a, const Variant& b) {return b.cmpUInt(a) > 0;}
inline bool operator> (uint a, const Variant& b) {return b.cmpUInt(a) <= 0;}
inline bool operator>=(uint a, const Variant& b) {return b.cmpUInt(a) < 0;}

inline bool operator==(const Variant& a, int64_t b) {return a.eqInt64(b);}
inline bool operator!=(const Variant& a, int64_t b) {return !a.eqInt64(b);}
inline bool operator==(int64_t a, const Variant& b) {return b.eqInt64(a);}
inline bool operator!=(int64_t a, const Variant& b) {return !b.eqInt64(a);}
inline bool operator< (const Variant& a, int64_t b) {return a.cmpInt64(b) < 0;}
inline bool operator<=(const Variant& a, int64_t b) {return a.cmpInt64(b) <= 0;}
inline bool operator> (const Variant& a, int64_t b) {return a.cmpInt64(b) > 0;}
inline bool operator>=(const Variant& a, int64_t b) {return a.cmpInt64(b) >= 0;}
inline bool operator< (int64_t a, const Variant& b) {return b.cmpInt64(a) >= 0;}
inline bool operator<=(int64_t a, const Variant& b) {return b.cmpInt64(a) > 0;}
inline bool operator> (int64_t a, const Variant& b) {return b.cmpInt64(a) <= 0;}
inline bool operator>=(int64_t a, const Variant& b) {return b.cmpInt64(a) < 0;}

inline bool operator==(const Variant& a, uint64_t b) {return a.eqUInt64(b);}
inline bool operator!=(const Variant& a, uint64_t b) {return !a.eqUInt64(b);}
inline bool operator==(uint64_t a, const Variant& b) {return b.eqUInt64(a);}
inline bool operator!=(uint64_t a, const Variant& b) {return !b.eqUInt64(a);}
inline bool operator< (const Variant& a, uint64_t b) {return a.cmpUInt64(b) < 0;}
inline bool operator<=(const Variant& a, uint64_t b) {return a.cmpUInt64(b) <= 0;}
inline bool operator> (const Variant& a, uint64_t b) {return a.cmpUInt64(b) > 0;}
inline bool operator>=(const Variant& a, uint64_t b) {return a.cmpUInt64(b) >= 0;}
inline bool operator< (uint64_t a, const Variant& b) {return b.cmpUInt64(a) >= 0;}
inline bool operator<=(uint64_t a, const Variant& b) {return b.cmpUInt64(a) > 0;}
inline bool operator> (uint64_t a, const Variant& b) {return b.cmpUInt64(a) <= 0;}
inline bool operator>=(uint64_t a, const Variant& b) {return b.cmpUInt64(a) < 0;}

inline bool operator==(const Variant& a, float b) {return a.eqFloat(b);}
inline bool operator!=(const Variant& a, float b) {return !a.eqFloat(b);}
inline bool operator==(float a, const Variant& b) {return b.eqFloat(a);}
inline bool operator!=(float a, const Variant& b) {return !b.eqFloat(a);}
inline bool operator< (const Variant& a, float b) {return a.cmpFloat(b) < 0;}
inline bool operator<=(const Variant& a, float b) {return a.cmpFloat(b) <= 0;}
inline bool operator> (const Variant& a, float b) {return a.cmpFloat(b) > 0;}
inline bool operator>=(const Variant& a, float b) {return a.cmpFloat(b) >= 0;}
inline bool operator< (float a, const Variant& b) {return b.cmpFloat(a) >= 0;}
inline bool operator<=(float a, const Variant& b) {return b.cmpFloat(a) > 0;}
inline bool operator> (float a, const Variant& b) {return b.cmpFloat(a) <= 0;}
inline bool operator>=(float a, const Variant& b) {return b.cmpFloat(a) < 0;}

inline bool operator==(const Variant& a, double b) {return a.eqDouble(b);}
inline bool operator!=(const Variant& a, double b) {return !a.eqDouble(b);}
inline bool operator==(double a, const Variant& b) {return b.eqDouble(a);}
inline bool operator!=(double a, const Variant& b) {return !b.eqDouble(a);}
inline bool operator< (const Variant& a, double b) {return a.cmpDouble(b) < 0;}
inline bool operator<=(const Variant& a, double b) {return a.cmpDouble(b) <= 0;}
inline bool operator> (const Variant& a, double b) {return a.cmpDouble(b) > 0;}
inline bool operator>=(const Variant& a, double b) {return a.cmpDouble(b) >= 0;}
inline bool operator< (double a, const Variant& b) {return b.cmpDouble(a) >= 0;}
inline bool operator<=(double a, const Variant& b) {return b.cmpDouble(a) > 0;}
inline bool operator> (double a, const Variant& b) {return b.cmpDouble(a) <= 0;}
inline bool operator>=(double a, const Variant& b) {return b.cmpDouble(a) < 0;}

inline bool operator==(const Variant& a, const String& b) {return a.eqString(b);}
inline bool operator!=(const Variant& a, const String& b) {return !a.eqString(b);}
inline bool operator==(const String& a, const Variant& b) {return b.eqString(a);}
inline bool operator!=(const String& a, const Variant& b) {return !b.eqString(a);}
inline bool operator< (const Variant& a, const String& b) {return a.cmpString(b) < 0;}
inline bool operator<=(const Variant& a, const String& b) {return a.cmpString(b) <= 0;}
inline bool operator> (const Variant& a, const String& b) {return a.cmpString(b) > 0;}
inline bool operator>=(const Variant& a, const String& b) {return a.cmpString(b) >= 0;}
inline bool operator< (const String& a, const Variant& b) {return b.cmpString(a) >= 0;}
inline bool operator<=(const String& a, const Variant& b) {return b.cmpString(a) > 0;}
inline bool operator> (const String& a, const Variant& b) {return b.cmpString(a) <= 0;}
inline bool operator>=(const String& a, const Variant& b) {return b.cmpString(a) < 0;}

inline bool operator==(const Variant& a, const char* b) {return a.eqCString(b);}
inline bool operator!=(const Variant& a, const char* b) {return !a.eqCString(b);}
inline bool operator==(const char* a, const Variant& b) {return b.eqCString(a);}
inline bool operator!=(const char* a, const Variant& b) {return !b.eqCString(a);}
inline bool operator< (const Variant& a, const char* b) {return a.cmpCString(b) < 0;}
inline bool operator<=(const Variant& a, const char* b) {return a.cmpCString(b) <= 0;}
inline bool operator> (const Variant& a, const char* b) {return a.cmpCString(b) > 0;}
inline bool operator>=(const Variant& a, const char* b) {return a.cmpCString(b) >= 0;}
inline bool operator< (const char* a, const Variant& b) {return b.cmpCString(a) >= 0;}
inline bool operator<=(const char* a, const Variant& b) {return b.cmpCString(a) > 0;}
inline bool operator> (const char* a, const Variant& b) {return b.cmpCString(a) <= 0;}
inline bool operator>=(const char* a, const Variant& b) {return b.cmpCString(a) < 0;}

inline bool operator==(const Variant& a, char b) {return a.eqChar(b);}
inline bool operator!=(const Variant& a, char b) {return !a.eqChar(b);}
inline bool operator==(char a, const Variant& b) {return b.eqChar(a);}
inline bool operator!=(char a, const Variant& b) {return !b.eqChar(a);}
inline bool operator< (const Variant& a, char b) {return a.cmpChar(b) < 0;}
inline bool operator<=(const Variant& a, char b) {return a.cmpChar(b) <= 0;}
inline bool operator> (const Variant& a, char b) {return a.cmpChar(b) > 0;}
inline bool operator>=(const Variant& a, char b) {return a.cmpChar(b) >= 0;}
inline bool operator< (char a, const Variant& b) {return b.cmpChar(a) >= 0;}
inline bool operator<=(char a, const Variant& b) {return b.cmpChar(a) > 0;}
inline bool operator> (char a, const Variant& b) {return b.cmpChar(a) <= 0;}
inline bool operator>=(char a, const Variant& b) {return b.cmpChar(a) < 0;}

inline bool operator==(const Variant& a, const void* b) {return a.eqPtr(b);}
inline bool operator!=(const Variant& a, const void* b) {return !a.eqPtr(b);}
inline bool operator==(const void* a, const Variant& b) {return b.eqPtr(a);}
inline bool operator!=(const void* a, const Variant& b) {return !b.eqPtr(a);}
inline bool operator< (const Variant& a, const void* b) {return a.cmpPtr(b) < 0;}
inline bool operator<=(const Variant& a, const void* b) {return a.cmpPtr(b) <= 0;}
inline bool operator> (const Variant& a, const void* b) {return a.cmpPtr(b) > 0;}
inline bool operator>=(const Variant& a, const void* b) {return a.cmpPtr(b) >= 0;}
inline bool operator< (const void* a, const Variant& b) {return b.cmpPtr(a) >= 0;}
inline bool operator<=(const void* a, const Variant& b) {return b.cmpPtr(a) > 0;}
inline bool operator> (const void* a, const Variant& b) {return b.cmpPtr(a) <= 0;}
inline bool operator>=(const void* a, const Variant& b) {return b.cmpPtr(a) < 0;}

#ifndef NO_TA_BASE
inline bool operator==(const Variant& a, const taBase* b) {return a.eqBase(b);}
inline bool operator!=(const Variant& a, const taBase* b) {return !a.eqBase(b);}
inline bool operator==(const taBase* a, const Variant& b) {return b.eqBase(a);}
inline bool operator!=(const taBase* a, const Variant& b) {return !b.eqBase(a);}

inline bool operator==(const Variant& a, const taMatrix* b) {return a.eqMatrix(b);}
inline bool operator!=(const Variant& a, const taMatrix* b) {return !a.eqMatrix(b);}
inline bool operator==(const taMatrix* a, const Variant& b) {return b.eqMatrix(a);}
inline bool operator!=(const taMatrix* a, const Variant& b) {return !b.eqMatrix(a);}
#endif


class TA_API NameVar {
  // a name-value representation, using a String and a Variant
public:
  String	name;		// the name
  Variant	value;		// the value

  String	GetStr() const	{ return name + "=" + value.toString(); }
  void		SetFmStr(const String& val);

  bool	operator>(const NameVar& cm) { return value > cm.value; }
  bool	operator<(const NameVar& cm) { return value < cm.value; }
  bool	operator>=(const NameVar& cm) { return value >= cm.value; }
  bool	operator<=(const NameVar& cm) { return value <= cm.value; }
  bool	operator==(const NameVar& cm) { return value == cm.value; }

  NameVar(const NameVar& cp) { name = cp.name; value = cp.value; }
  NameVar()	{ };
  ~NameVar()	{ };
};

#endif

