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


// basic types for css

#ifndef CSS_BASIC_TYPES_H
#define CSS_BASIC_TYPES_H

#include "css_machine.h"

class CSS_API cssInt : public cssEl {
  // an integer value
public:
  Int		val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Int; }
  const char*	GetTypeName() const	{ return "(Int)"; }
  bool		IsNumericTypeStrict() const   { return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" " + name + " = " + String(val); }
  String	PrintFStr() const { return String(val); }

  // constructors
  void 		Constr()		{ val = 0; }
  void		Copy(const cssInt& cp)	{ cssEl::Copy(cp); val = cp.val; }

  cssInt()				{ Constr(); }
  cssInt(Int vl)			{ Constr(); val = vl; }
  cssInt(Int vl, const String& nm) 	{ Constr(); name = nm;  val = vl; }
  cssInt(const cssInt& cp)		{ Copy(cp); name = cp.name; }
  cssInt(const cssInt& cp, const String& nm)  { Copy(cp); name = nm; }

  cssCloneFuns(cssInt, 0);

  // converters
  String GetStr() const		{ return String(val); }
  Variant GetVar() const	{ return Variant(val); }
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return val; }
  operator bool() const	 	{ return val; }

  void* 	NullPtrCvt(const String& typ_nm)	const { if(val != 0) CvtErr(typ_nm); return NULL; }

  operator void*() const	{ return NullPtrCvt("(void*)"); }
  operator void**() const	{ return (void**)NullPtrCvt("(void**)"); }

  operator int*() const		{ return (int*)NullPtrCvt("(int*)"); }
  operator short*() const	{ return (short*)NullPtrCvt("(short*)"); }
  operator long*() const	{ return (long*)NullPtrCvt("(long*)"); }
  operator double*() const	{ return (double*)NullPtrCvt("(double*)"); }
  operator float*() const	{ return (float*)NullPtrCvt("(float*)"); }
  operator String*() const	{ return (String*)NullPtrCvt("(String*)"); }
  operator Variant*() const	{ return (Variant*)NullPtrCvt("(Variant*)"); }
  operator bool*() const	{ return (bool*)NullPtrCvt("(bool*)"); }

  operator int**() const	{ return (int**)NullPtrCvt("(int**)"); }
  operator short**() const	{ return (short**)NullPtrCvt("(short**)"); }
  operator long**() const	{ return (long**)NullPtrCvt("(long**)"); }
  operator double**() const	{ return (double**)NullPtrCvt("(double**)"); }
  operator float**() const	{ return (float**)NullPtrCvt("(float**)"); }
  operator String**() const	{ return (String**)NullPtrCvt("(String**)"); }
  operator Variant**() const	{ return (Variant**)NullPtrCvt("(Variant**)"); }
  operator bool**() const	{ return (bool**)NullPtrCvt("(bool**)"); }

  operator ostream*() const	{ return (ostream*)NullPtrCvt("(ostream*)"); }
  operator istream*() const	{ return (istream*)NullPtrCvt("(istream*)"); }
  operator iostream*() const	{ return (iostream*)NullPtrCvt("(iostream*)"); }
  operator fstream*() const	{ return (fstream*)NullPtrCvt("(fstream*)"); }
  operator stringstream*() const { return (stringstream*)NullPtrCvt("(stringstream*)"); }

  operator ostream**() const	{ return (ostream**)NullPtrCvt("(ostream**)"); }
  operator istream**() const	{ return (istream**)NullPtrCvt("(istream**)"); }
  operator iostream**()	const	{ return (iostream**)NullPtrCvt("(iostream**)"); }
  operator fstream**() const	{ return (fstream**)NullPtrCvt("(fstream**)"); }
  operator stringstream**() const { return (stringstream**)NullPtrCvt("(stringstream**)"); }

  operator taBase*() const	{ return (taBase*)NullPtrCvt("(taBase*)"); }
  operator taBase**() const 	{ return (taBase**)NullPtrCvt("(taBase**)"); }
  operator TypeDef*() const	{ return (TypeDef*)NullPtrCvt("(TypeDef*)"); }
  operator MemberDef*() const	{ return (MemberDef*)NullPtrCvt("(MemberDef*)"); }
  operator MethodDef*() const	{ return (MethodDef*)NullPtrCvt("(MethodDef*)"); }

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = (int)strtol((const char*)cp, NULL, 0); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s) { val = (Int)s; }

  cssEl* operator+(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssEl::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl& t)
  { cssInt* r = new cssInt(*this,""); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt* r = new cssInt(*this,""); r->val = -val; return r; }
  cssEl* operator~()
  { cssInt* r = new cssInt(*this,""); r->val = ~val; return r; }

  void operator+=(cssEl& t) 	{ val += (Int)t; }
  void operator-=(cssEl& t) 	{ val -= (Int)t; }
  void operator*=(cssEl& t) 	{ val *= (Int)t; }
  void operator/=(cssEl& t) 	{ val /= (Int)t; }
  void operator%=(cssEl& t) 	{ val %= (Int)t; }
  void operator<<=(cssEl& t) 	{ val <<= (Int)t; }
  void operator>>=(cssEl& t) 	{ val >>= (Int)t; }
  void operator&=(cssEl& t) 	{ val &= (Int)t; }
  void operator^=(cssEl& t) 	{ val ^= (Int)t; }
  void operator|=(cssEl& t) 	{ val |= (Int)t; }

  cssEl* operator< (cssEl& s) 	{ return new cssBool(val < (Int)s); }
  cssEl* operator> (cssEl& s) 	{ return new cssBool(val > (Int)s); }
  cssEl* operator<=(cssEl& s) 	{ return new cssBool(val <= (Int)s); }
  cssEl* operator>=(cssEl& s) 	{ return new cssBool(val >= (Int)s); }
  cssEl* operator==(cssEl& s) 	{ return new cssBool(val == (Int)s); }
  cssEl* operator!=(cssEl& s) 	{ return new cssBool(val != (Int)s); }
};

#define cssInt_inst(l,n,x)          l .Push(new cssInt((int) n,(const char *) #x))
#define cssInt_inst_nm(l,n,s)       l .Push(new cssInt(n, s))
#define cssInt_inst_ptr(l,n,x)      l .Push(cssBI::x = new cssInt(n, #x))
#define cssInt_inst_ptr_nm(l,n,x,s) l .Push(cssBI::x = new cssInt(n, s))

class CSS_API cssConstInt : public cssInt {
  // a constant integer value -- cannot be changed through std operators -- must assign directly to value in initialization
public:

  cssConstInt()				{ Constr(); }
  cssConstInt(Int vl)			{ Constr(); val = vl; }
  cssConstInt(Int vl, const String& nm) 	{ Constr(); name = nm;  val = vl; }
  cssConstInt(const cssConstInt& cp)		{ Copy(cp); name = cp.name; }
  cssConstInt(const cssConstInt& cp, const String& nm)  { Copy(cp); name = nm; }

  cssCloneFuns(cssConstInt, 0);

  void operator=(Real cp) 		{ NopErr("const ="); }
  void operator=(Int cp)		{ NopErr("const ="); }
  void operator=(const String& cp)	{ NopErr("const ="); }

  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s) { NopErr("const ="); }

  void operator+=(cssEl& t) 	{ NopErr("const +="); }
  void operator-=(cssEl& t) 	{ NopErr("const -="); }
  void operator*=(cssEl& t) 	{ NopErr("const *="); }
  void operator/=(cssEl& t) 	{ NopErr("const /="); }
  void operator%=(cssEl& t) 	{ NopErr("const %="); }
  void operator<<=(cssEl& t) 	{ NopErr("const <<="); }
  void operator>>=(cssEl& t) 	{ NopErr("const >>="); }
  void operator&=(cssEl& t) 	{ NopErr("const &="); }
  void operator^=(cssEl& t) 	{ NopErr("const ^="); }
  void operator|=(cssEl& t) 	{ NopErr("const |="); }
};

#define cssConstInt_inst(l,n,x)          l .Push(new cssConstInt((int) n,(const char *) #x))
#define cssConstInt_inst_nm(l,n,s)       l .Push(new cssConstInt(n, s))
#define cssConstInt_inst_ptr(l,n,x)      l .Push(cssBI::x = new cssConstInt(n, #x))
#define cssConstInt_inst_ptr_nm(l,n,x,s) l .Push(cssBI::x = new cssConstInt(n, s))

class CSS_API cssInt64 : public cssEl {
  // a 64-bt integer value
public:
  ta_int64_t		val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Int64; }
  const char*	GetTypeName() const	{ return "(Int64)"; }
  bool		IsNumericTypeStrict() const   { return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" " + name + " = " + String(val); }
  String	PrintFStr() const { return String(val); }

  // constructors
  void 		Constr()		{ val = 0LL; }
  void		Copy(const cssInt64& cp)	{ cssEl::Copy(cp); val = cp.val; }
  cssInt64()				{ Constr(); }
  cssInt64(Int vl)			{ Constr(); val = vl; }
  cssInt64(Int vl, const String& nm) 	{ Constr(); name = nm;  val = vl; }
  cssInt64(const cssInt64& cp)		{ Copy(cp); name = cp.name; }
  cssInt64(const cssInt64& cp, const String& nm)  { Copy(cp); name = nm; }

  cssCloneFuns(cssInt64, 0);

  // converters
  String GetStr() const	{ return String(val); }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return (Int)val; }
  operator bool() const	 	{ return val; }
  operator float() const 		{ return (float)val; }
  operator unsigned int() const 	{ return (unsigned int)val; }
  operator unsigned char() const 	{ return (unsigned char)val; }
  operator unsigned short() const	{ return (unsigned short)val; }
  operator unsigned long() const	{ return (unsigned long)val; }
  operator ta_int64_t() const	 	{ return val; }
  operator ta_uint64_t() const	 	{ return (ta_uint64_t)val; }

//  operator void*() const; //TODO64 -- Int64 should hold pointers in 64-bit version
//  operator void**() const;

  void operator=(Real cp) 		{ val = (ta_int64_t)cp; }
  void operator=(Int cp)		{ val = (int)cp; }
  void operator=(ta_int64_t cp)		{ val = cp; }
  void operator=(ta_uint64_t cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = cp.toInt64(); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s) { val = (ta_int64_t)s; }

  cssEl* operator+(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val += (ta_int64_t)t; return r; }
  cssEl* operator-(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val -= (ta_int64_t)t; return r; }
  cssEl* operator*()		{ return cssEl::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val *= (ta_int64_t)t; return r; }
  cssEl* operator/(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val /= (ta_int64_t)t; return r; }
  cssEl* operator%(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val %= (ta_int64_t)t; return r; }
  cssEl* operator<<(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val <<= (ta_int64_t)t; return r; }
  cssEl* operator>>(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val >>= (ta_int64_t)t; return r; }
  cssEl* operator&(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val &= (ta_int64_t)t; return r; }
  cssEl* operator^(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val ^= (ta_int64_t)t; return r; }
  cssEl* operator|(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val |= (ta_int64_t)t; return r; }

  cssEl* operator-()
  { cssInt64* r = new cssInt64(*this,""); r->val = -val; return r; }
  cssEl* operator~()
  { cssInt64* r = new cssInt64(*this,""); r->val = ~val; return r; }

  void operator+=(cssEl& t) 	{ val += (ta_int64_t)t; }
  void operator-=(cssEl& t) 	{ val -= (ta_int64_t)t; }
  void operator*=(cssEl& t) 	{ val *= (ta_int64_t)t; }
  void operator/=(cssEl& t) 	{ val /= (ta_int64_t)t; }
  void operator%=(cssEl& t) 	{ val %= (ta_int64_t)t; }
  void operator<<=(cssEl& t) 	{ val <<= (ta_int64_t)t; }
  void operator>>=(cssEl& t) 	{ val >>= (ta_int64_t)t; }
  void operator&=(cssEl& t) 	{ val &= (ta_int64_t)t; }
  void operator^=(cssEl& t) 	{ val ^= (ta_int64_t)t; }
  void operator|=(cssEl& t) 	{ val |= (ta_int64_t)t; }

  cssEl* operator< (cssEl& s) 	{ return new cssBool(val < (ta_int64_t)s); }
  cssEl* operator> (cssEl& s) 	{ return new cssBool(val > (ta_int64_t)s); }
  cssEl* operator<=(cssEl& s) 	{ return new cssBool(val <= (ta_int64_t)s); }
  cssEl* operator>=(cssEl& s) 	{ return new cssBool(val >= (ta_int64_t)s); }
  cssEl* operator==(cssEl& s) 	{ return new cssBool(val == (ta_int64_t)s); }
  cssEl* operator!=(cssEl& s) 	{ return new cssBool(val != (ta_int64_t)s); }
};

#define cssInt64_inst(l,n,x)          l .Push(new cssInt64((ta_int64_t) n,(const char *) #x))
#define cssInt64_inst_nm(l,n,s)       l .Push(new cssInt64(n, s))
#define cssInt64_inst_ptr(l,n,x)      l .Push(cssBI::x = new cssInt64(n, #x))
#define cssInt64_inst_ptr_nm(l,n,x,s) l .Push(cssBI::x = new cssInt64(n, s))

class CSS_API cssChar : public cssInt {
  // a character (for string conversions)
public:
  const char*	GetTypeName() const { return "(char)"; }
  bool		IsStringType() const   	{ return true; }
  bool		IsNumericTypeStrict() const   { return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" " + name + " = '" + (char)val + "'"; }
  String	PrintFStr() const { return (char)val; }

  cssChar()                      : cssInt()		{ };
  cssChar(Int vl)                : cssInt(vl)		{ };
  cssChar(Int vl, const String& nm)      : cssInt(vl, nm)	{ };
  cssChar(const cssChar& cp)           : cssInt(cp)		{ };
  cssChar(const cssChar& cp, const String& nm) : cssInt(cp, nm) 	{ };
  cssCloneFuns(cssChar, *this);

  String GetStr() const	{ return (String)(char)val; }
  Variant GetVar() const { return Variant((char)val); }

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp)	{ if(!cp.empty()) val = cp[0]; }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssInt::operator=)

  // operators
  void operator=(const cssEl& s);
};

class CSS_API cssReal : public cssEl {
  // a real (floating point) value
public:
  Real		val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Real; }
  const char*	GetTypeName() const 	{ return "(Real)"; }
  bool		IsNumericTypeStrict() const   { return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" "+ name + " = " + String(val); }
  String	PrintFStr() const { return String(val); }

  // constructors
  void 		Constr()		{ val = 0; }
  void		Copy(const cssReal& cp)	{ cssEl::Copy(cp); val = cp.val; }
  cssReal()				{ Constr(); }
  cssReal(Real vl)			{ Constr(); val = vl; }
  cssReal(Real vl, const String& nm)	{ Constr(); val = vl; name = nm; }
  cssReal(const cssReal& cp)		{ Copy(cp); name = cp.name; }
  cssReal(const cssReal& cp, const String& nm){ Copy(cp); name = nm; }

  cssCloneFuns(cssReal, 0.0);

  // converters
  String GetStr() const	{ return String(val); }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return val; }
  operator Int() const	 	{ return (int)val; }
  operator bool() const	 	{ return val; }

  void operator=(Real cp) 		{ val = cp; }
  void operator=(Int cp)		{ val = (Real)cp; }
  void operator=(ta_int64_t cp)		{ operator=((Real)cp); }
  void operator=(ta_uint64_t cp)		{ operator=((Real)cp); }
  void operator=(const String& cp)	{ val = atof((const char*)cp); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s)	{ val = (Real)s; }

  cssEl* operator+(cssEl& t)
  { cssReal* r = new cssReal(*this,""); r->val += (Real)t; return r; }
  cssEl* operator-(cssEl& t)
  { cssReal* r = new cssReal(*this,""); r->val -= (Real)t; return r; }
  cssEl* operator*()		{ return cssEl::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssReal* r = new cssReal(*this,""); r->val *= (Real)t; return r; }
  cssEl* operator/(cssEl& t)
  { cssReal* r = new cssReal(*this,""); r->val /= (Real)t; return r; }

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl& t)
  { cssReal* r = new cssReal(); r->val = pow(val, (Real)t); return r; }

  cssEl* operator-()
  { cssReal* r = new cssReal(*this,""); r->val = -val; return r; }

  void operator+=(cssEl& t) 	{ val += (Real)t; }
  void operator-=(cssEl& t) 	{ val -= (Real)t; }
  void operator*=(cssEl& t) 	{ val *= (Real)t; }
  void operator/=(cssEl& t) 	{ val /= (Real)t; }

  cssEl* operator< (cssEl& s) { return new cssBool(val < (Real)s); }
  cssEl* operator> (cssEl& s) { return new cssBool(val > (Real)s); }
  cssEl* operator<=(cssEl& s) { return new cssBool(val <= (Real)s); }
  cssEl* operator>=(cssEl& s) { return new cssBool(val >= (Real)s); }
  cssEl* operator==(cssEl& s) { return new cssBool(val == (Real)s); }
  cssEl* operator!=(cssEl& s) { return new cssBool(val != (Real)s); }
};

#define cssReal_inst(l,n,x)		l .Push(new cssReal(n, #x))
#define cssReal_inst_nm(l,n,s)	l .Push(new cssReal(n, s))
#define cssReal_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssReal(n, #x))
#define cssReal_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssReal(n, s))

// for making stubs automatically, for real functions, having n args
#define cssRealFun_stub0(x) cssEl* cssRealFun_ ## x ## _stub(int, cssEl**)\
  { return new cssReal((Real)x ()); }
#define cssRealFun_stub1(x) cssEl* cssRealFun_ ## x ## _stub(int, cssEl* arg[])\
  { return new cssReal((Real)x ((double)*(arg[1]))); }
#define cssRealFun_stub2(x) cssEl* cssRealFun_ ## x ## _stub(int, cssEl* arg[])\
  { return new cssReal((Real)x ((double)*(arg[1]), (double)*(arg[2]))); }
#define cssRealFun_stub3(x) cssEl* cssRealFun_ ## x ## _stub(int, cssEl* arg[])\
  { return new cssReal((Real)x ((double)*(arg[1]), (double)*(arg[2]), (double)*(arg[3]))); }
#define cssRealFun_stub4(x) cssEl* cssRealFun_ ## x ## _stub(int, cssEl* arg[])\
  { return new cssReal((Real)x ((double)*(arg[1]), (double)*(arg[2]), (double)*(arg[3]), (double)*(arg[4]))); }

#define cssRealFun_inst(l,x,n,hst) l .Push(new cssElCFun(n, cssRealFun_ ## x ## _stub, #x, CSS_FUN, hst))
#define cssRealFun_inst_mtx(l,x,n,hst,mtxarg) l .Push(new cssElCFun(n, cssRealFun_ ## x ## _stub, #x, CSS_FUN, hst, cssElFun::FUN_ITR_MATRIX, mtxarg))


class CSS_API cssConstReal : public cssReal {
  // a constant real value -- cannot be changed through std operators -- must assign directly to value in initialization
public:

  cssConstReal()				{ Constr(); }
  cssConstReal(Real vl)			{ Constr(); val = vl; }
  cssConstReal(Real vl, const String& nm)	{ Constr(); val = vl; name = nm; }
  cssConstReal(const cssConstReal& cp)		{ Copy(cp); name = cp.name; }
  cssConstReal(const cssConstReal& cp, const String& nm){ Copy(cp); name = nm; }

  cssCloneFuns(cssConstReal, 0.0);

  void operator=(Real cp) 		{ NopErr("const ="); }
  void operator=(Int cp)		{ NopErr("const ="); }
  void operator=(const String& cp)	{ NopErr("const ="); }

  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s) { NopErr("const ="); }

  void operator+=(cssEl& t) 	{ NopErr("const +="); }
  void operator-=(cssEl& t) 	{ NopErr("const -="); }
  void operator*=(cssEl& t) 	{ NopErr("const *="); }
  void operator/=(cssEl& t) 	{ NopErr("const /="); }
  void operator%=(cssEl& t) 	{ NopErr("const %="); }
  void operator<<=(cssEl& t) 	{ NopErr("const <<="); }
  void operator>>=(cssEl& t) 	{ NopErr("const >>="); }
  void operator&=(cssEl& t) 	{ NopErr("const &="); }
  void operator^=(cssEl& t) 	{ NopErr("const ^="); }
  void operator|=(cssEl& t) 	{ NopErr("const |="); }
};

#define cssConstReal_inst(l,n,x) l .Push(new cssConstReal(n, #x))
#define cssConstReal_inst_nm(l,n,s)	l .Push(new cssConstReal(n, s))
#define cssConstReal_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssConstReal(n, #x))
#define cssConstReal_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssConstReal(n, s))

class taFiler;

class CSS_API cssString : public cssEl {
  // a character-string value
public:
  String	val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_String; }
  const char*	GetTypeName() const	{ return "(String)"; }
  bool		IsStringType() const   	{ return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" "+name + " = " + String(val); }
  String	PrintFStr() const	{ return String(val); }
  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  void		Save(ostream& strm = cout);
  void		Load(istream& strm = cin);

  // special functions
  virtual String GetVal() 	 { return val; }

  // constructors
  void		Constr() { };
  void		Copy(const cssString& cp)	 { cssEl::Copy(cp); val = cp.val; }
  cssString()					 { Constr(); }
  cssString(const String& vl)			 { Constr(); val = vl; }
  cssString(const String& vl, const String& nm)	 { Constr(); name = nm; val = vl; }
  cssString(const cssString& cp)	 	 { Copy(cp); name = cp.name; }
  cssString(const cssString& cp, const String& nm) { Copy(cp); name = nm; }
  ~cssString() { };

  cssCloneFuns(cssString, "");

  // converters
  String GetStr() const	  	{ return val; }
  Variant GetVar() const 	{ return Variant(val); }
  operator Real() const	 	{ Real r = atof((const char*)val); return r; }
  operator Int() const		{ Int r = (int)strtol((const char*)val, NULL, 0); return r; }
  operator bool() const;

  operator taBase*() const;	// lookup as a path..
  operator TypeDef*() const;	// lookup as name
  operator MemberDef*() const;
  operator MethodDef*() const;  

  void operator=(Real cp) 		{ val = String(cp); }
  void operator=(Int cp)		{ val = String(cp); }
  void operator=(ta_int64_t cp)		{ val = String(cp); }
  void operator=(ta_uint64_t cp)		{ val = String(cp); }
  void operator=(const String& cp)	{ val = cp; }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s)	{ val = s.GetStr(); }

  cssEl* operator+(cssEl& t)
  { cssString* r = new cssString(*this,""); r->val += t.GetStr(); return r; }
  cssEl* operator-(cssEl&)	{ NopErr("-"); return &cssMisc::Void; }
  cssEl* operator/(cssEl&)	{ NopErr("/"); return &cssMisc::Void; }
  cssEl* operator%(cssEl&)	{ NopErr("%"); return &cssMisc::Void; }
  cssEl* operator-()    	{ NopErr("-"); return &cssMisc::Void; }

  void operator+=(cssEl& t) 	{ val += t.GetStr(); }

  cssEl* operator< (cssEl& s) { return new cssBool(val < s.GetStr()); }
  cssEl* operator> (cssEl& s) { return new cssBool(val > s.GetStr()); }
  cssEl* operator<=(cssEl& s) { return new cssBool(val <= s.GetStr()); }
  cssEl* operator>=(cssEl& s) { return new cssBool(val >= s.GetStr()); }
  cssEl* operator==(cssEl& s) { return new cssBool(val == s.GetStr()); }
  cssEl* operator!=(cssEl& s) { return new cssBool(val != s.GetStr()); }

  // these use the TA info to perform actions
  cssEl* operator[](const Variant& idx) const;
  int	 GetMethodNo(const String& method) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetMethodFmName(const String& memb) const;
  cssEl* GetScoped(const String& nm) const;
};

#define cssString_inst(l,n,x)		l .Push(new cssString(n, #x))
#define cssString_inst_nm(l,n,s)	l .Push(new cssString(n, s))
#define cssString_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssString(n, #x))
#define cssString_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssString(n, s))


class CSS_API cssConstBool : public cssBool {
  // a constant boolean value -- cannot be changed through std operators -- must assign directly to value in initialization
public:

  cssConstBool()					{ Constr(); }
  cssConstBool(bool vl)					{ Constr(); val = vl; }
  cssConstBool(bool vl, const String& nm) 		{ Constr(); name = nm;  val = vl; }
  cssConstBool(const cssConstBool& cp)			{ Copy(cp); name = cp.name; }
  cssConstBool(const cssConstBool& cp, const String& nm)  { Copy(cp); name = nm; }

  cssCloneFuns(cssConstBool, false);

  void operator=(Real cp) 		{ NopErr("const ="); }
  void operator=(Int cp)		{ NopErr("const ="); }
  void operator=(const String& cp)	{ NopErr("const ="); }

  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s) { NopErr("const ="); }

  void operator&=(cssEl& t) 	{ NopErr("const &="); }
  void operator^=(cssEl& t) 	{ NopErr("const ^="); }
  void operator|=(cssEl& t) 	{ NopErr("const |="); }
};

class CSS_API cssVariant: public cssEl {
  // css wrapper for a variant object
INHERITED(cssEl)
public:
  Variant	val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Variant; }
  cssTypes 	GetPtrType() const;
  const char*	GetTypeName() const 	{ return "(Variant)"; }

  String 	PrintStr() const;
  String	PrintFStr() const { return val.toString(); }

  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  // constructors
  void 		Constr()		{ } // default is Invalid
  void		Copy(const cssVariant& cp) { cssEl::Copy(cp); val = cp.val; }
  cssVariant()				{ Constr(); }
  cssVariant(const Variant& vl)		{ Constr(); val = vl; }
  cssVariant(const Variant& vl, const String& nm) { Constr(); val = vl; name = nm; }
  cssVariant(const cssVariant& cp)		{ Copy(cp); name = cp.name; }
  cssVariant(const cssVariant& cp, const String& nm){ Copy(cp); name = nm; }

  cssCloneFuns(cssVariant, _nilVariant);

  // converters -- note, be as permissive as possible, because strings can easily arise
  // unexpectedly in most other atomic contexts (esp numeric)
  String GetStr() const	{ return val.toString(); }
  Variant GetVar() const { return val; }
  operator Real() const	 { return val.toDouble();}
  operator float() const { return val.toFloat();}
  operator Int() const	 { return val.toInt();}
  operator ta_int64_t() const { return val.toInt64(); }
  operator ta_uint64_t() const { return val.toUInt64(); }
  operator taBase*() const;
  operator TypeDef*() const;
  operator MemberDef*() const;
  operator MethodDef*() const;
  operator bool() const	{ return val.toBool(); }
  
  void operator=(Real cp) 		{ val = cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(ta_int64_t cp)		{ val = cp; }
  void operator=(ta_uint64_t cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = cp; }
  void operator=(const Variant& cp)	{ val = cp;}

  void operator=(void* ptr)	 	{ val = ptr; }
  void operator=(void**)		{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s)	{ val = s.GetVar(); }

  cssEl* operator+(cssEl& t);
  cssEl* operator-(cssEl& t);
  cssEl* operator*();
  cssEl* operator*(cssEl& t);
  cssEl* operator/(cssEl& t);

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl& t);

  cssEl* operator-();
  cssEl* operator~();

  void operator+=(cssEl& s)	{ val += s.GetVar(); }
  void operator-=(cssEl& s)	{ val -= s.GetVar(); }
  void operator*=(cssEl& s)	{ val *= s.GetVar(); }
  void operator/=(cssEl& s)	{ val /= s.GetVar(); }

  cssEl* operator< (cssEl& s)  { return new cssBool(val < s.GetVar()); } 
  cssEl* operator> (cssEl& s)  { return new cssBool(val > s.GetVar()); } 
  cssEl* operator<=(cssEl& s)  { return new cssBool(val <= s.GetVar()); }
  cssEl* operator>=(cssEl& s)  { return new cssBool(val >= s.GetVar()); }
  cssEl* operator==(cssEl& s)  { return new cssBool(val == s.GetVar()); }
  cssEl* operator!=(cssEl& s)  { return new cssBool(val != s.GetVar()); }
  
  // these delegate to the string, base or variant
  cssEl* operator[](const Variant& idx) const; // only valid for Matrixes, gets flat el
  bool	 MembersDynamic()	{ return true; }
  int	 GetMemberNo(const String& memb) const { return -1; } // don't do any advance lookup: always dynamic
  cssEl* GetMemberFmNo(int memb) const;
  cssEl* GetMemberFmName(const String& memb) const;
  int	 GetMethodNo(const String& meth) const { return -1; } // keep it dynamic
  cssEl* GetMethodFmNo(int meth) const;
  cssEl* GetMethodFmName(const String& meth) const;
  cssEl* GetScoped(const String& nm) const;
};

#define cssVariant_inst(l,n,x)		l .Push(new cssVariant(n, #x))
#define cssVariant_inst_nm(l,n,s)	l .Push(new cssVariant(n, s))
#define cssVariant_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssVariant(n, #x))
#define cssVariant_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssVariant(n, s))


class CSS_API cssPtr : public cssEl {
  // this is an el for pointing to other el's
public:
  cssElPtr	ptr;
  cssEl*	el_type;	// what type of element do we point to?

  int		GetParse() const	{ return CSS_PTR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Ptr; }
  cssTypes	GetPtrType() const	{ return el_type->GetType(); }
  const char*	GetTypeName() const	{ return el_type->GetTypeName(); }
  cssEl*	GetTypeObject() const	{ return el_type; }
  bool		IsStringType() const   	{ cssEl* el = ((cssPtr*)this)->GetActualObj();
    if (el) return el->IsStringType(); else return false;}
  bool		IsNumericTypeStrict() const { cssEl* el = ((cssPtr*)this)->GetActualObj();
    if (el) return el->IsNumericTypeStrict(); else return false;}

  cssEl*	GetActualObj() const	{ return ptr.El()->GetActualObj(); }
  cssEl*	GetNonPtrObj() const 	{ return ptr.El(); }

  virtual cssEl* GetNonPtrTypeObj() const;
  virtual cssTypes GetNonPtrType() const { return GetNonPtrTypeObj()->GetType(); }

  String 	PrintStr() const;
  String	PrintFStr() const 	{ return ptr.El()->PrintFStr(); }

  void		SetPtr(const cssElPtr& pt) { cssEl::SetRefElPtr(ptr, pt); }
  void		SetPtr(cssEl* it)	{ cssElPtr pt; pt.SetDirect(it); SetPtr(pt); }

  void		SetElType(cssEl* typ) 	{ cssEl::SetRefPointer(&el_type, typ); }

  // constructors
  void		Constr()		{ el_type = &cssMisc::Void; }

  void		Copy(const cssPtr& cp) 	{ cssEl::Copy(cp); SetPtr(cp.ptr); SetElType(cp.el_type); }
  void		CopyType(const cssPtr& cp) { cssEl::CopyType(cp); SetElType(cp.el_type); }

  cssPtr();
  cssPtr(const cssElPtr& it);	// this sets the pointer
  cssPtr(cssEl* typ);		// this sets the type
  cssPtr(cssEl* typ, const String& nm); // sets type
  cssPtr(cssEl* typ, const cssElPtr& it, const String& nm);
  cssPtr(const cssPtr& cp);
  cssPtr(const cssPtr& cp, const String& nm);
  ~cssPtr();

  cssCloneOnly(cssPtr);
  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssPtr(el_type, arg[1]->GetStr()); }

  bool	 MembersDynamic()	{ return true; }
  int    GetMemberNo(const String& s) const; // check type even if ptr is null
  cssEl* GetMemberFmNo(int s) const  		{ return ptr.El()->GetMemberFmNo(s); }
  cssEl* GetMemberFmName(const String& s) const  	{ return ptr.El()->GetMemberFmName(s); }
  int	 GetMethodNo(const String& s) const; // check type even if ptr is null
  cssEl* GetMethodFmNo(int s) const; // check for virtual funs
  cssEl* GetMethodFmName(const String& s) const;
  cssEl* GetScoped(const String& s) const  	{ return ptr.El()->GetScoped(s); }

  // converters
  Int		GetIntVal() const	// get integer value of pointer
  { Int rval = (Int)(long)(ptr.El()); if(ptr.El() == &cssMisc::Void) rval = 0;
    return rval; }

  String	GetStr() const;
  Variant	GetVar() const { return Variant((void*)ptr.El()); } // raw pointer
  operator 	Real() const	{ return (Real)GetIntVal(); }
  operator 	Int() const	{ return GetIntVal(); }
  operator 	bool() const	{ return GetIntVal(); }

  void	DelOpr()		{ SetPtr(cssMisc::VoidElPtr); }
  // delete is done by unrefing thing we point to, seting ptr to null..

//  void operator=(Real)	 	{ CvtErr("(Real)"); }
//  void operator=(Int)		{ CvtErr("(Int)"); }
//  void operator=(const String&)	{ CvtErr("(String)"); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssElPtr& s)	{ SetPtr(s); }
  void operator=(const cssEl& s);
  void operator=(cssEl* s)              { SetPtr(s->GetAddr()); }

  virtual cssElPtr& GetOprPtr() const;
  // get the pointer for the purposes of following operators
  // if points to an array, this is the pointer of the array, not our pointer

  cssEl* operator+(cssEl& t)
  { cssElPtr r = GetOprPtr(); r += (Int)t; return new cssPtr(el_type, r, ""); }
  cssEl* operator-()		{ return cssEl::operator-(); }
  cssEl* operator-(cssEl& t)
  { cssElPtr r = GetOprPtr(); r -= (Int)t; return new cssPtr(el_type, r, ""); }
  cssEl* operator*(cssEl&)	{ NopErr("*"); return &cssMisc::Void; }

  cssEl* operator*()	     	{ return GetOprPtr().El(); } // unary de-ptr
  cssEl* operator[](const Variant& idx) const;

  void operator+=(cssEl& t)
  { cssElPtr r = GetOprPtr(); r += (Int)t; SetPtr(r); }
  void operator-=(cssEl& t)
  { cssElPtr r = GetOprPtr(); r -= (Int)t; SetPtr(r); }

  cssEl* operator< (cssEl& s) { return new cssBool(GetIntVal() < (Int)s); }
  cssEl* operator> (cssEl& s) { return new cssBool(GetIntVal() > (Int)s); }
  cssEl* operator<=(cssEl& s) { return new cssBool(GetIntVal() <= (Int)s); }
  cssEl* operator>=(cssEl& s) { return new cssBool(GetIntVal() >= (Int)s); }
  cssEl* operator==(cssEl& s) { return new cssBool(GetIntVal() == (Int)s); }
  cssEl* operator!=(cssEl& s) { return new cssBool(GetIntVal() != (Int)s); }
};

class CSS_API cssArray : public cssPtr {
public:
  cssSpace*	items;		// what's in the array

  int		GetParse() const	{ return CSS_PTR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Array; }

  String	PrintStr() const;
  String	PrintFStr() const;
  void		TypeInfo(ostream& fh = cout) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  void		Constr();
  void 		Constr(int no);
  void		Copy(const cssArray& cp);

  virtual void  Fill(cssEl* it);                // fill space with this type of thing
  virtual int   Alloc(int no);                 // reallocate this array
  virtual int   AllocAll(int na, cssEl* arg[]); // reallocate this and sub-arrays based on args
  cssArray();
  cssArray(int no);
  cssArray(int no, cssEl* it);
  cssArray(int no, const String& nm);
  cssArray(int no, cssEl* it, const String& nm);
  cssArray(const cssArray& cp);
  cssArray(const cssArray& cp, const String& nm);
  cssArray(const cssArrayType& cp, const String& nm);
  ~cssArray();

  cssCloneOnly(cssArray);
  cssEl*	MakeToken_stub(int na, cssEl* arg[]);

  RunStat	Do(cssProg* prg);
  // set prog of the el_type also

  // converters
  String	GetStr() const;
  Variant	GetVar() const 	{ CvtErr("(Variant)"); return _nilVariant; }
  operator 	Real() const	{ CvtErr("(Real)"); return 0; }
  operator 	Int() const	{ CvtErr("(Int)"); return 0; }
  operator 	bool() const	{ CvtErr("(bool)"); return 0; }

  void operator=(Real)	 	{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssPtr::operator=)

  // operators
  cssElPtr& GetOprPtr()	const 	{ return (cssElPtr&)ptr; } // our ptr is always ours!

  void NoAssgn() 	{ cssMisc::Error(prog,"re-assigning base of array ptr is illegal"); }
  void operator=(const cssElPtr&)	{ NoAssgn(); }
  void operator=(cssEl*)		{ NoAssgn(); }
  void operator=(const cssEl&);
};


class CSS_API cssArrayType : public cssArray {
  // an array definition, no actual allocation
public:
  int           size;           // defined size of array

  int           GetParse() const        { return CSS_PTR; }
  uint          GetSize() const         { return sizeof(*this); }
  cssTypes      GetType() const         { return T_ArrayType; }

  void          SetSize(int no) { size = no; }

  String        PrintStr() const;
  String        PrintFStr() const;
  void          TypeInfo(ostream& fh = cout) const;

  void          Constr();
  void          Copy(const cssArrayType& cp);
  void          CopyType(const cssArrayType& cp);

  virtual void  Fill(cssEl*) {};             // nop
  virtual int   Alloc(int no) { size = no; return true; }   // just set size
  virtual int   AllocAll(int na, cssEl* arg[]); // reallocate this and sub-arrays based on args

  cssArrayType();
  cssArrayType(int no, cssEl* it);
  cssArrayType(int no, cssEl* it, const String& nm);
  cssArrayType(const cssArrayType& cp);
  cssArrayType(const cssArrayType& cp, const String& nm);
  ~cssArrayType();

  cssCloneOnly(cssArrayType);
  cssEl*        MakeToken_stub(int na, cssEl* arg[]);

  // converters
  void operator=(Real)          { CvtErr("(Real)"); }
  void operator=(Int)           { CvtErr("(Int)"); }
  void operator=(const String&) { CvtErr("(String)"); }
  void operator=(void*)         { CvtErr("(void*)"); }
  void operator=(void**)        { CvtErr("(void**)"); }
  USING(cssArray::operator=)

  // operators
  cssEl* operator+(cssEl&)      { NopErr("+"); return &cssMisc::Void; }
  cssEl* operator-()            { NopErr("-"); return &cssMisc::Void; }
  cssEl* operator-(cssEl&)      { NopErr("-"); return &cssMisc::Void; }
  cssEl* operator*(cssEl&)      { NopErr("*"); return &cssMisc::Void; }
  cssEl* operator*()            { NopErr("*"); return &cssMisc::Void; }
  cssEl* operator[](const Variant& idx) const
  { NopErr("[]"); return &cssMisc::Void; }
  void operator+=(cssEl&)       { NopErr("+="); }
  void operator-=(cssEl&)       { NopErr("-="); }
  cssEl* operator< (cssEl&)       { NopErr("<"); return &cssMisc::Void; }
  cssEl* operator> (cssEl&)       { NopErr(">"); return &cssMisc::Void; }
  cssEl* operator<=(cssEl&)       { NopErr("<="); return &cssMisc::Void; }
  cssEl* operator>=(cssEl&)       { NopErr(">="); return &cssMisc::Void; }
  cssEl* operator==(cssEl&)       { NopErr("=="); return &cssMisc::Void; }
  cssEl* operator!=(cssEl&)       { NopErr("!="); return &cssMisc::Void; }

  void operator=(const cssElPtr&)       { NopErr("="); }
  void operator=(cssEl*)                { NopErr("="); }
  void operator=(const cssEl&)          { NopErr("="); }
};


class CSS_API cssRef : public cssEl {
  // reference to an el
public:
  cssElPtr	ptr;

  int		GetParse() const	{ return CSS_PTR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return ptr.El()->GetType(); }
  cssTypes	GetPtrType() const	{ return ptr.El()->GetPtrType(); }
  const char*	GetTypeName() const	{ return ptr.El()->GetTypeName(); }
  cssEl*	GetTypeObject() const	{ return ptr.El()->GetTypeObject(); }
  int		IsRef()	const		{ return true; }
  bool		IsStringType() const   	{ return ptr.El()->IsStringType(); }
  bool		IsNumericTypeStrict() const { return ptr.El()->IsNumericTypeStrict(); }
  cssElPtr	GetAddr() const;

  cssEl*	GetActualObj() const	{ return ptr.El()->GetActualObj(); }
  cssEl*	GetNonRefObj() const	{ return ptr.El(); }

  String 	PrintStr() const
  { String rv = ptr.El()->PrintStr(); rv.prepend("&"); return rv; }
  String	PrintFStr() const 	{ return ptr.El()->PrintFStr(); }

  void		PrintR(ostream& fh = cout) const	{ ptr.El()->PrintR(fh); }
  int		Edit(bool wait=false)			{ return ptr.El()->Edit(wait); }

  void  	TypeInfo(ostream& fh = cout) const	{ ptr.El()->TypeInfo(fh); }
  void		InheritInfo(ostream& fh = cout) const	{ ptr.El()->InheritInfo(fh); }

  // saving and loading objects to/from files (special format)
  void		Save(ostream& fh = cout)	{ ptr.El()->Save(fh); }
  void		Load(istream& fh = cin)		{ ptr.El()->Load(fh); }

  // token information about a certain type
  void		TokenInfo(ostream& fh = cout) const	{ ptr.El()->TokenInfo(fh); }
  cssEl*	GetToken(int idx) const			{ return ptr.El()->GetToken(idx); }

  // constructors
  void		Constr()			{ }
  void		Copy(const cssRef& cp) 		{ cssEl::Copy(cp); ptr = cp.ptr; }
  cssRef()					{ Constr(); }
  cssRef(const cssElPtr& it)			{ Constr(); ptr = it; }
  cssRef(cssEl* it)				{ Constr(); ptr.SetDirect(it); }
  cssRef(cssEl* it, const String& nm)		{ Constr(); name = nm; ptr.SetDirect(it); }
  cssRef(const cssElPtr& it, const String& nm)	{ Constr(); name = nm; ptr = it; }
  cssRef(const cssRef& cp)			{ Copy(cp); name = cp.name; }
  cssRef(const cssRef& cp, const String& nm)	{ Copy(cp); name = nm; }

  cssCloneOnly(cssRef);
  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssRef(ptr, arg[1]->GetStr()); }

  // converters
  String GetStr() const 	{ return ptr.El()->GetStr(); }
  Variant GetVar() const 	{ return ptr.El()->GetVar(); }
  operator Real() const	 	{ return (Real)*(ptr.El()); }
  operator Int() const	 	{ return (Int)*(ptr.El()); }
  operator bool() const	 	{ return (bool)*(ptr.El()); }
  operator void*() const	{ return (void*)*(ptr.El()); }
  operator void**() const	{ return (void**)*(ptr.El()); }

  void* GetVoidPtrOfType(TypeDef* td) const
  { return ptr.El()->GetVoidPtrOfType(td); }
  void* GetVoidPtrOfType(const String& td) const
  { return ptr.El()->GetVoidPtrOfType(td); }

  operator int*() const	 	{ return (int*)*(ptr.El()); }
  operator short*() const	{ return (short*)*(ptr.El()); }
  operator long*() const	{ return (long*)*(ptr.El()); }
  operator double*() const	{ return (double*)*(ptr.El()); }
  operator float*() const	{ return (float*)*(ptr.El()); }
  operator String*() const	{ return (String*)*(ptr.El()); }
  operator bool*() const	{ return (bool*)*(ptr.El()); }

  operator int**() const	{ return (int**)*(ptr.El()); }
  operator short**() const	{ return (short**)*(ptr.El()); }
  operator long**() const	{ return (long**)*(ptr.El()); }
  operator double**() const	{ return (double**)*(ptr.El()); }
  operator float**() const	{ return (float**)*(ptr.El()); }
  operator String**() const	{ return (String**)*(ptr.El()); }
  operator bool**() const	{ return (bool**)*(ptr.El()); }

  operator ostream*() const	{ return (ostream*)*(ptr.El()); }
  operator istream*() const	{ return (istream*)*(ptr.El()); }
  operator iostream*() const	{ return (iostream*)*(ptr.El()); }
  operator fstream*() const	{ return (fstream*)*(ptr.El()); }
  operator stringstream*() const { return (stringstream*)*(ptr.El()); }

  operator ostream**() const	{ return (ostream**)*(ptr.El()); }
  operator istream**() const	{ return (istream**)*(ptr.El()); }
  operator iostream**() const	{ return (iostream**)*(ptr.El()); }
  operator fstream**() const	{ return (fstream**)*(ptr.El()); }
  operator stringstream**() const	{ return (stringstream**)*(ptr.El()); }

  // support for external types
  operator taBase*() const	{ return (taBase*)*(ptr.El()); }
  operator taBase**() const	{ return (taBase**)*(ptr.El()); }
  operator TypeDef*() const	{ return (TypeDef*)*(ptr.El()); }
  operator MemberDef*() const	{ return (MemberDef*)*(ptr.El()); }
  operator MethodDef*() const	{ return (MethodDef*)*(ptr.El()); }

  // assign from types
  void operator=(Real cp) 		{ ptr.El()->operator=(cp); }
  void operator=(Int cp)		{ ptr.El()->operator=(cp); }
  void operator=(const String& cp)	{ ptr.El()->operator=(cp); }

  void operator=(void* cp) 	{ ptr.El()->operator=(cp); }
  void operator=(void** cp)	{ ptr.El()->operator=(cp); }
  USING(cssEl::operator=)

  void AssignFromType(TypeDef* td, void* bs)  	{ ptr.El()->AssignFromType(td, bs); }
  void AssignFromType(const String& td, void* bs) { ptr.El()->AssignFromType(td, bs); }

  // operators
  void operator=(const cssEl& cp);
  void CastFm(const cssEl& cp)	{ InitAssign(cp); }
  void InitAssign(const cssEl& cp);
  void UpdateAfterEdit()	{ ptr.El()->UpdateAfterEdit(); }

  cssEl* operator+(cssEl& s) 	{ return ptr.El()->operator+(s); }
  cssEl* operator-(cssEl& s) 	{ return ptr.El()->operator-(s); }
  cssEl* operator*(cssEl& s) 	{ return ptr.El()->operator*(s); }
  cssEl* operator/(cssEl& s) 	{ return ptr.El()->operator/(s); }
  cssEl* operator%(cssEl& s) 	{ return ptr.El()->operator%(s); }
  cssEl* operator<<(cssEl& s)	{ return ptr.El()->operator<<(s); }
  cssEl* operator>>(cssEl& s)	{ return ptr.El()->operator>>(s); }
  cssEl* operator&(cssEl& s)	{ return ptr.El()->operator&(s); }
  cssEl* operator^(cssEl& s)	{ return ptr.El()->operator^(s); }
  cssEl* operator|(cssEl& s)	{ return ptr.El()->operator|(s); }
  cssEl* operator-()       	{ return ptr.El()->operator-(); }
  cssEl* operator~()       	{ return ptr.El()->operator~(); }
  cssEl* operator*()	   	{ return ptr.El()->operator*(); }
  cssEl* operator[](const Variant& idx) const	{ return ptr.El()->operator[](idx); }

  bool	 MembersDynamic()	{ return true; }
  int    GetMemberNo(const String& s) const; // emit errorless -1 for void
  cssEl* GetMemberFmNo(int s) const  		{ return ptr.El()->GetMemberFmNo(s); }
  cssEl* GetMemberFmName(const String& s) const { return ptr.El()->GetMemberFmName(s); }
  int	 GetMethodNo(const String& s) const; // emit errorless -1 for void
  cssEl* GetMethodFmNo(int s) const		{ return ptr.El()->GetMethodFmNo(s); }
  cssEl* GetMethodFmName(const String& s) const	{ return ptr.El()->GetMethodFmName(s); }
  cssEl* GetScoped(const String& s) const  	{ return ptr.El()->GetScoped(s); }
  cssEl* NewOpr()   				{ return ptr.El()->NewOpr(); }
  void	 DelOpr() 				{ ptr.El()->DelOpr(); }

  cssEl* operator< (cssEl& s) 	{ return ptr.El()->operator<(s); }
  cssEl* operator> (cssEl& s) 	{ return ptr.El()->operator>(s); }
  cssEl* operator<=(cssEl& s) 	{ return ptr.El()->operator<=(s); }
  cssEl* operator>=(cssEl& s) 	{ return ptr.El()->operator>=(s); }
  cssEl* operator==(cssEl& s) 	{ return ptr.El()->operator==(s); }
  cssEl* operator!=(cssEl& s) 	{ return ptr.El()->operator!=(s); }

  void operator+=(cssEl& s) 	{ ptr.El()->operator+=(s); }
  void operator-=(cssEl& s) 	{ ptr.El()->operator-=(s); }
  void operator*=(cssEl& s) 	{ ptr.El()->operator*=(s); }
  void operator/=(cssEl& s) 	{ ptr.El()->operator/=(s); }
  void operator%=(cssEl& s) 	{ ptr.El()->operator%=(s); }
  void operator<<=(cssEl& s) 	{ ptr.El()->operator<<=(s); }
  void operator>>=(cssEl& s) 	{ ptr.El()->operator>>=(s); }
  void operator&=(cssEl& s) 	{ ptr.El()->operator&=(s); }
  void operator^=(cssEl& s) 	{ ptr.El()->operator^=(s); }
  void operator|=(cssEl& s) 	{ ptr.El()->operator|=(s); }
};


//////////////////////////
//	Enums		//
//////////////////////////

class CSS_API cssEnumType : public cssEl {
  // this is a class that defines a type (collection) of enums
public:
  String	type_name;
  String	desc;		// description of class (from comment during definition)
  int		enum_cnt;	// last enum value assigned (for use during parsing..)
  cssSpace*	enums;		// the actual enums themselves (duplicated elsewhere..)

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_EnumType; }
  const char*	GetTypeName() const	{ return (const char*)type_name; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }

  virtual void	SetTypeName(const String& nm);

  void		TypeInfo(ostream& fh = cout) const;

  // constructors
  void 		Constr();
  void		Copy(const cssEnumType& cp);
  void		CopyType(const cssEnumType& cp);
  USING(cssEl::operator=)

  cssEnumType()				{ Constr(); }
  cssEnumType(const String& nm)		{ Constr(); SetTypeName(nm); }
  cssEnumType(const cssEnumType& cp);
  cssEnumType(const cssEnumType& cp, const String& nm);
  ~cssEnumType();

  cssCloneOnly(cssEnumType);
  cssEl* 	MakeToken_stub(int, cssEl* arg[]); // make an instance instead

  cssEl* 	GetScoped(const String& memb) const;
  cssEl*	FindValue(int val) const;
};

class CSS_API cssEnum : public cssInt {
  // an instance of an enum type (ie. a particular enum value)
public:
  cssEnumType*	type_def;	// definition of the enum type

  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Enum; }
  const char*	GetTypeName() const;
  cssEl*	GetTypeObject() const	{ return type_def; }
  String 	PrintStr() const;
  String	PrintFStr() const;

  void		SetEnumType(cssEnumType* et)
  { if(type_def != &cssMisc::VoidEnumType)  cssEl::unRefDone(type_def);
    type_def = et; if(type_def != &cssMisc::VoidEnumType)  cssEl::Ref(type_def); }

  void		Constr();
  void		Copy(const cssEnum& cp);
  void		CopyType(const cssEnum& cp);

  cssEnum()                      : cssInt()		{ Constr(); }
  cssEnum(Int vl)                : cssInt(vl)		{ Constr(); }
  cssEnum(Int vl, const String& nm ) : cssInt(vl, nm)	{ Constr(); }
  cssEnum(cssEnumType* cp, Int vl, const String& nm);
  // this is how it should be created..
  cssEnum(const cssEnum& cp);
  cssEnum(const cssEnum& cp, const String& nm);
  ~cssEnum();
  cssCloneFuns(cssEnum, *this);

  // converters
  String GetStr() const; //
  // Variant GetVar() const; same as base

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp);

  void operator=(void*)	 		{ CvtErr("(void*)"); }
  void operator=(void**)		{ CvtErr("(void**)"); }
  USING(cssInt::operator=)

  // operators
  void operator=(const cssEl& s);

  cssEl* operator==(cssEl& s);
  cssEl* operator!=(cssEl& s);
};

#define cssEnum_inst_nm(l,n,s)		l .Push(new cssEnum(n, s))
#define cssEnum_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssEnum(n, s))


//////////////////////////
//	Classes		//
//////////////////////////

class CSS_API cssClassMember : public cssEl {
  // contains class members (name is member name, points to type object)
public:
  cssEl*	mbr_type;	// type of this member

  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_ClassMbr; }
  cssEl*	GetTypeObject() const	{ return mbr_type; }
  const char*	GetTypeName() const	{ return mbr_type->GetTypeName(); }
  String 	PrintStr() const;
  String	PrintFStr() const;

  void		SetMbrType(cssEl* mbtp) { cssEl::SetRefPointer(&mbr_type, mbtp); }

  void		Constr();
  void		Copy(const cssClassMember& cp);
  void		CopyType(const cssClassMember& cp);
  USING(cssEl::operator=)
  
  cssClassMember()  : cssEl()		{ Constr(); }
  cssClassMember(cssEl* mbtp);
  cssClassMember(cssEl* mbtp, const String& mbnm);
  cssClassMember(const cssClassMember& cp);
  cssClassMember(const cssClassMember& cp, const String& nm);
  ~cssClassMember();
  cssCloneFuns(cssClassMember, *this);

  int    GetMemberNo(const String& s) const	{ return mbr_type->GetMemberNo(s); }
  cssEl* GetMemberFmNo(int s) const  		{ return mbr_type->GetMemberFmNo(s); }
  cssEl* GetMemberFmName(const String& s) const  	{ return mbr_type->GetMemberFmName(s); }
  int	 GetMethodNo(const String& s) const	{ return mbr_type->GetMethodNo(s); }
  cssEl* GetMethodFmNo(int s) const		{ return mbr_type->GetMethodFmNo(s); }
  cssEl* GetMethodFmName(const String& s) const	{ return mbr_type->GetMethodFmName(s); }
  cssEl* GetScoped(const String& s) const  	{ return mbr_type->GetScoped(s); }
};

class CSS_API cssClassType : public cssEl {
  // this is a class that defines the css class type
public:
  String	type_name;
  String	desc;		// description of class (from comment during definition)
  String	opts;
  String_Array	member_desc;	// description strings for members
  String_Array	member_opts;	// option strings for members
  cssSpace*	parents;
  cssSpace*	members;
  cssSpace*	methods;
  cssSpace*	types;
  cssProgSpace*	last_top;	// last top-level space that this object was defined in
  bool		multi_space;	// this class has been defined across multiple spaces

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_ClassType; }
  const char*	GetTypeName() const	{ return (const char*)type_name; }
  cssEl*	GetTypeObject() const	{ return (cssEl*)this; }

  void		SetTypeName(const String& nm);
  void  	AddParent(cssClassType* par);

  String 	PrintStr() const;
  String	PrintFStr() const;

  void		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  bool          InheritsFrom(const cssClassType* cp);

  // builtin stubs
  static cssEl* InheritsFrom_stub(void*, int na, cssEl* arg[]);
  static cssEl* Load_stub(void*, int na, cssEl* arg[]);
  static cssEl* Save_stub(void*, int na, cssEl* arg[]);

  // constructors
  void 		Constr();
  void		AddBuiltins();			// add builtin members/methods

  void		Copy(const cssClassType& cp);
  void		CopyType(const cssClassType& cp);
  USING(cssEl::operator=)

  cssClassType()			{ Constr(); AddBuiltins(); }
  cssClassType(const String& nm)		{ Constr(); SetTypeName(nm); AddBuiltins(); }
  cssClassType(const cssClassType& cp);
  cssClassType(const cssClassType& cp, const String& nm);
  ~cssClassType();
  cssCloneOnly(cssClassType);

  cssEl* 	MakeToken_stub(int, cssEl* arg[]); // make an instance instead

  void	CallVoidMethod(cssClassInst* tok, const String& meth_nm);	// call a method taking no args & returning nothing..

  void	ConstructToken(cssClassInst* tok);	// call all constructors on this class
  void	DestructToken(cssClassInst* tok);	// call all destructors on this class

  void	ConstructToken_impl(cssClassInst* tok); // call constructor on this class
  void	DestructToken_impl(cssClassInst* tok); // call destructor on this class

  void	UpdateAfterEdit_impl(cssClassInst* tok);
  // call updateafteredit function on token if it is defined

  void		SetDesc_impl(String& dsc, String& opt, const String& des);
  // set given description and opts strings based on des input string
  String	OptionAfter_impl(const String& optns, const String& opt) const;

  void		SetDesc(const String& des)	   { SetDesc_impl(desc, opts, des); }
  String	OptionAfter(const String& opt) const { return OptionAfter_impl(opts, opt); }
  bool		HasOption(const String& opt) const   { return opts.contains(opt); }

  void		MbrSetDesc(int mbr, const String& des);
  String	MbrOptionAfter(int mbr, const String& opt) const
  { return OptionAfter_impl(member_opts.SafeEl(mbr), opt); }
  bool		MbrHasOption(int mbr, const String& opt) const
  { return member_opts.SafeEl(mbr).contains(opt); }

  void          GetComments(cssEl* obj, cssElPtr cmt);
  // extract info for options and desc for class object from comment

  // converters
  String	GetStr() const;
  Variant	GetVar() const 	{ CvtErr("(Variant)"); return _nilVariant; }
  operator 	Real() const		{ CvtErr("(Real)"); return 0; }
  operator 	Int() const		{ CvtErr("(Int)"); return 0; }

  int 	 GetMemberNo(const String& memb) const;
  cssEl* GetMemberFmNo(int memb) const;
  cssEl* GetMemberFmName(const String& memb) const;
  int 	 GetMethodNo(const String& memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetMethodFmName(const String& memb) const;
  cssEl* GetScoped(const String& memb) const;
  cssEl* NewOpr();
};

class CSS_API cssClassInst : public cssEl {
  // this is a class that defines the css class
public:
  cssClassType*	type_def;	// defines the type of this class
  cssSpace*	members;	// members that actually belong to this one

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Class; }
  const char*	GetTypeName() const;
  cssEl*	GetTypeObject() const	{ return type_def; }

  void		SetClassType(cssClassType* ct)
  { if(type_def != &cssMisc::VoidClassType)  cssEl::unRefDone(type_def);
    type_def = ct; if(type_def != &cssMisc::VoidClassType)  cssEl::Ref(type_def); }
#ifdef TA_GUI
  int		Edit(bool wait=false);	// uses stuff in css_iv to edit classes...
#endif
  String 	PrintStr() const;
  String	PrintFStr() const;

  void		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  // saving and loading objects to/from files (special format)
  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  // constructors
  void 		Constr();
  void		Copy(const cssClassInst& cp);
  void		CopyType(const cssClassInst& cp);

  cssClassInst()			{ Constr(); }
  cssClassInst(const String& nm)		{ Constr(); name = nm; }
  cssClassInst(cssClassType* cp, const String& nm);
  // use this to create from type
  cssClassInst(const cssClassInst& cp);
  cssClassInst(const cssClassInst& cp, const String& nm);
  ~cssClassInst();

  cssCloneFuns(cssClassInst, *this);

  void	 ConstructToken();	// call constructor on this class
  void	 DestructToken();	// call destructor on this class

  // converters
  String	GetStr() const;
  Variant	GetVar() const 	{ CvtErr("(Variant)"); return _nilVariant; }
  operator 	Real() const		{ CvtErr("(Real)"); return 0; }
  operator 	Int() const		{ CvtErr("(Int)"); return 0; }
  operator 	bool() const		{ CvtErr("(bool)"); return 0; }

  void operator=(Real) 	 	{ CvtErr("(Real)"); }
  void operator=(Int) 		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void*)		{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }
  USING(cssEl::operator=)

  // operators
  void operator=(const cssEl& s);

  void	UpdateAfterEdit();

  int 	 GetMemberNo(const String& memb) const;
  cssEl* GetMemberFmNo(int memb) const;
  cssEl* GetMemberFmName(const String& memb) const;
  int 	 GetMethodNo(const String& memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetMethodFmName(const String& memb) const;
  cssEl* GetScoped(const String& memb) const;
};

class CSS_API cssSubShell : public cssEl {
  // contains a separate prog space
public:
  cssProgSpace		prog_space;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_SubShell; }
  const char*	GetTypeName() const	{ return "(SubShell)"; }

  String	PrintStr() const;
  String	PrintFStr() const;

  USING(cssEl::operator=)

  cssSubShell();
  cssSubShell(const String& nm);
  cssSubShell(const cssSubShell& cp);
  cssSubShell(const cssSubShell& cp, const String& nm);

  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssSubShell(arg[1]->GetStr()); }

  cssCloneOnly(cssSubShell);
};

#endif // CSS_BASIC_TYPES_H
