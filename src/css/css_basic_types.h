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
  void 		Constr()		{ Register(); val = 0; }
  void		Copy(const cssInt& cp)	{ cssEl::Copy(cp); val = cp.val; }
  cssInt()				{ Constr(); }
  cssInt(Int vl)			{ Constr(); val = vl; }
  cssInt(Int vl, const char* nm) 	{ Constr(); name = nm;  val = vl; }
  cssInt(const cssInt& cp)		{ Constr(); Copy(cp); }
  cssInt(const cssInt& cp, const char* nm)  { Constr(); Copy(cp); name = nm; }

  cssCloneFuns(cssInt, 0);

  // converters
  String GetStr() const	{ return String(val); }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return val; }

  operator void*() const;
  operator void**() const;

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = (int)strtol((const char*)cp, NULL, 0); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

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

  bool operator< (cssEl& s) 	{ return (val < (Int)s); }
  bool operator> (cssEl& s) 	{ return (val > (Int)s); }
  bool operator! () 	    	{ return ( ! val); }
  bool operator<=(cssEl& s) 	{ return (val <= (Int)s); }
  bool operator>=(cssEl& s) 	{ return (val >= (Int)s); }
  bool operator==(cssEl& s) 	{ return (val == (Int)s); }
  bool operator!=(cssEl& s) 	{ return (val != (Int)s); }
  bool operator&&(cssEl& s) 	{ return (val && (Int)s); }
  bool operator||(cssEl& s) 	{ return (val || (Int)s); }
};

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
  cssChar(Int vl, const char* nm)      : cssInt(vl, nm)	{ };
  cssChar(const cssChar& cp)           : cssInt(cp)		{ };
  cssChar(const cssChar& cp, const char* nm) : cssInt(cp, nm) 	{ };
  cssCloneFuns(cssChar, *this);

  String GetStr() const	{ return (String)val; }
  Variant GetVar() const { return Variant((char)val); }

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp)	{ if(!cp.empty()) val = cp[0]; }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

  // operators
  void operator=(const cssEl& s);
};

#define cssInt_inst(l,n,x)          l .Push(new cssInt((int) n,(const char *) #x))
#define cssInt_inst_nm(l,n,s)       l .Push(new cssInt(n, s))
#define cssInt_inst_ptr(l,n,x)      l .Push(cssBI::x = new cssInt(n, #x))
#define cssInt_inst_ptr_nm(l,n,x,s) l .Push(cssBI::x = new cssInt(n, s))

class CSS_API cssInt64 : public cssEl {
  // a 64-bt integer value
public:
  int64_t		val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Int64; }
  const char*	GetTypeName() const	{ return "(Int64)"; }
  bool		IsNumericTypeStrict() const   { return true; }

  String 	PrintStr() const
  { return String(GetTypeName())+" " + name + " = " + String(val); }
  String	PrintFStr() const { return String(val); }

  // constructors
  void 		Constr()		{ Register(); val = 0LL; }
  void		Copy(const cssInt64& cp)	{ cssEl::Copy(cp); val = cp.val; }
  cssInt64()				{ Constr(); }
  cssInt64(Int vl)			{ Constr(); val = vl; }
  cssInt64(Int vl, const char* nm) 	{ Constr(); name = nm;  val = vl; }
  cssInt64(const cssInt64& cp)		{ Constr(); Copy(cp); }
  cssInt64(const cssInt64& cp, const char* nm)  { Constr(); Copy(cp); name = nm; }

  cssCloneFuns(cssInt64, 0);

  // converters
  String GetStr() const	{ return String(val); }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return (Int)val; }
  operator float() const 		{ return (float)val; }
  operator unsigned int() const 	{ return (unsigned int)val; }
  operator unsigned char() const 	{ return (unsigned char)val; }
  operator unsigned short() const	{ return (unsigned short)val; }
  operator unsigned long() const	{ return (unsigned long)val; }
  operator int64_t() const	 	{ return val; }
  operator uint64_t() const	 	{ return (uint64_t)val; }

//  operator void*() const; //TODO64 -- Int64 should hold pointers in 64-bit version
//  operator void**() const;

  void operator=(Real cp) 		{ val = (int64_t)cp; }
  void operator=(Int cp)		{ val = (int)cp; }
  void operator=(int64_t cp)		{ val = cp; }
  void operator=(uint64_t cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = cp.toInt64(); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

  // operators
  void operator=(const cssEl& s) { val = (int64_t)s; }

  cssEl* operator+(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val += (int64_t)t; return r; }
  cssEl* operator-(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val -= (int64_t)t; return r; }
  cssEl* operator*()		{ return cssEl::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val *= (int64_t)t; return r; }
  cssEl* operator/(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val /= (int64_t)t; return r; }
  cssEl* operator%(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val %= (int64_t)t; return r; }
  cssEl* operator<<(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val <<= (int64_t)t; return r; }
  cssEl* operator>>(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val >>= (int64_t)t; return r; }
  cssEl* operator&(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val &= (int64_t)t; return r; }
  cssEl* operator^(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val ^= (int64_t)t; return r; }
  cssEl* operator|(cssEl& t)
  { cssInt64* r = new cssInt64(*this,""); r->val |= (int64_t)t; return r; }

  cssEl* operator-()
  { cssInt64* r = new cssInt64(*this,""); r->val = -val; return r; }

  void operator+=(cssEl& t) 	{ val += (int64_t)t; }
  void operator-=(cssEl& t) 	{ val -= (int64_t)t; }
  void operator*=(cssEl& t) 	{ val *= (int64_t)t; }
  void operator/=(cssEl& t) 	{ val /= (int64_t)t; }
  void operator%=(cssEl& t) 	{ val %= (int64_t)t; }
  void operator<<=(cssEl& t) 	{ val <<= (int64_t)t; }
  void operator>>=(cssEl& t) 	{ val >>= (int64_t)t; }
  void operator&=(cssEl& t) 	{ val &= (int64_t)t; }
  void operator^=(cssEl& t) 	{ val ^= (int64_t)t; }
  void operator|=(cssEl& t) 	{ val |= (int64_t)t; }

  bool operator< (cssEl& s) 	{ return (val < (int64_t)s); }
  bool operator> (cssEl& s) 	{ return (val > (int64_t)s); }
  bool operator! () 	    	{ return ( ! val); }
  bool operator<=(cssEl& s) 	{ return (val <= (int64_t)s); }
  bool operator>=(cssEl& s) 	{ return (val >= (int64_t)s); }
  bool operator==(cssEl& s) 	{ return (val == (int64_t)s); }
  bool operator!=(cssEl& s) 	{ return (val != (int64_t)s); }
  bool operator&&(cssEl& s) 	{ return (val && (int64_t)s); }
  bool operator||(cssEl& s) 	{ return (val || (int64_t)s); }
};

#define cssInt64_inst(l,n,x)          l .Push(new cssInt64((int64_t) n,(const char *) #x))
#define cssInt64_inst_nm(l,n,s)       l .Push(new cssInt64(n, s))
#define cssInt64_inst_ptr(l,n,x)      l .Push(cssBI::x = new cssInt64(n, #x))
#define cssInt64_inst_ptr_nm(l,n,x,s) l .Push(cssBI::x = new cssInt64(n, s))


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
  void 		Constr()		{ Register(); val = 0; }
  void		Copy(const cssReal& cp)	{ cssEl::Copy(cp); val = cp.val; }
  cssReal()				{ Constr(); }
  cssReal(Real vl)			{ Constr(); val = vl; }
  cssReal(Real vl, const char* nm)	{ Constr(); val = vl; name = nm; }
  cssReal(const cssReal& cp)		{ Constr(); Copy(cp); }
  cssReal(const cssReal& cp, const char* nm){ Constr(); Copy(cp); name = nm; }

  cssCloneFuns(cssReal, 0.0);

  // converters
  String GetStr() const	{ return String(val); }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return val; }
  operator Int() const	 	{ return (int)val; }

  void operator=(Real cp) 		{ val = cp; }
  void operator=(Int cp)		{ val = (Real)cp; }
  void operator=(int64_t cp)		{ operator=((Real)cp); }
  void operator=(uint64_t cp)		{ operator=((Real)cp); }
  void operator=(const String& cp)	{ val = atof((const char*)cp); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

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

  bool operator< (cssEl& s) { return (val < (Real)s); }
  bool operator> (cssEl& s) { return (val > (Real)s); }
  bool operator! () 	    { return ( ! val); }
  bool operator<=(cssEl& s) { return (val <= (Real)s); }
  bool operator>=(cssEl& s) { return (val >= (Real)s); }
  bool operator==(cssEl& s) { return (val == (Real)s); }
  bool operator!=(cssEl& s) { return (val != (Real)s); }
  bool operator&&(cssEl& s) { return (val && (Real)s); }
  bool operator||(cssEl& s) { return (val || (Real)s); }
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

class taFiler;

class CSS_API cssString : public cssEl {
  // a character-string value
public:
  String	val;
  taFiler*	gf;		// this allows strings to be converted into filenames

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
  void		Constr();
  void		Copy(const cssString& cp)	 { cssEl::Copy(cp); val = cp.val; }
  cssString()					 { Constr(); }
  cssString(String vl)				 { Constr(); val = vl; }
  cssString(const char* vl)			 { Constr(); val = vl; }
  cssString(String vl, const char* nm)		 { Constr(); name = nm; val = vl; }
  cssString(const char* vl, const char* nm)	 { Constr(); name = nm; val = vl; }
  cssString(const cssString& cp)	 	 { Constr(); Copy(cp); }
  cssString(const cssString& cp, const char* nm) { Constr(); Copy(cp); name = nm; }
  ~cssString();

  cssCloneFuns(cssString, "");

  // converters
  String GetStr() const	  { return val; }
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ Real r = atof((const char*)val); return r; }
  operator Int() const		{ Int r = (int)strtol((const char*)val, NULL, 0); return r; }
  operator String() const	{ return val; }
  operator void*() const	{ return (void*)&val; }
  // convert to stream as file-name of a file
  operator ostream*() const;
  operator istream*() const;

  void operator=(Real cp) 		{ val = String(cp); }
  void operator=(Int cp)		{ val = String(cp); }
  void operator=(int64_t cp)		{ val = String(cp); }
  void operator=(uint64_t cp)		{ val = String(cp); }
  void operator=(const String& cp)	{ val = cp; }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

  // operators
  void operator=(const cssEl& s)	{ val = s.GetStr(); }

  cssEl* operator+(cssEl& t)
  { cssString* r = new cssString(*this,""); r->val += t.GetStr(); return r; }
  cssEl* operator-(cssEl&)	{ NopErr("-"); return &cssMisc::Void; }
  cssEl* operator/(cssEl&)	{ NopErr("/"); return &cssMisc::Void; }
  cssEl* operator%(cssEl&)	{ NopErr("%"); return &cssMisc::Void; }
  cssEl* operator-()    	{ NopErr("-"); return &cssMisc::Void; }

  void operator+=(cssEl& t) 	{ val += t.GetStr(); }

  bool operator< (cssEl& s) { return (val < s.GetStr()); }
  bool operator> (cssEl& s) { return (val > s.GetStr()); }
  bool operator! () 	    { return val.length(); }
  bool operator<=(cssEl& s) { return (val <= s.GetStr()); }
  bool operator>=(cssEl& s) { return (val >= s.GetStr()); }
  bool operator==(cssEl& s) { return (val == s.GetStr()); }
  bool operator!=(cssEl& s) { return (val != s.GetStr()); }
  bool operator&&(cssEl& s) { return (val.length() && (Int)s); }
  bool operator||(cssEl& s) { return (val.length() || (Int)s); }

  // these use the TA info to perform actions
  cssEl* operator[](int idx) const;
  int	 GetMemberFunNo(const char*) const;
  cssEl* GetMemberFun(int memb) const;
  cssEl* GetScoped(const char*) const;
};

#define cssString_inst(l,n,x)		l .Push(new cssString(n, #x))
#define cssString_inst_nm(l,n,s)	l .Push(new cssString(n, s))
#define cssString_inst_ptr(l,n,x)	l .Push(cssBI::x = new cssString(n, #x))
#define cssString_inst_ptr_nm(l,n,x,s)	l .Push(cssBI::x = new cssString(n, s))

class CSS_API cssBool : public cssEl {
  // a boolean value
public:
  bool		val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Bool; }
  const char*	GetTypeName() const	{ return "(Bool)"; }

  String 	PrintStr() const
  { return String(GetTypeName())+" " + name + " = " + GetStr(); }
  String	PrintFStr() const { return GetStr(); }

  // constructors
  void 		Constr()			{ Register(); val = false; }
  void		Copy(const cssBool& cp)		{ cssEl::Copy(cp); val = cp.val; }
  cssBool()					{ Constr(); }
  cssBool(bool vl)				{ Constr(); val = vl; }
  cssBool(bool vl, const char* nm) 		{ Constr(); name = nm;  val = vl; }
  cssBool(const cssBool& cp)			{ Constr(); Copy(cp); }
  cssBool(const cssBool& cp, const char* nm)  	{ Constr(); Copy(cp); name = nm; }

  cssCloneFuns(cssBool, false);

  // converters
  String GetStr() const;
  Variant GetVar() const { return Variant(val); }
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return val; }

  void operator=(Real cp) 		{ val = (bool)cp; }
  void operator=(Int cp)		{ val = (bool)cp; }
  void operator=(const String& cp);

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

  // operators
  void operator=(const cssEl& s);

  bool operator! () 	    	{ return ( ! val); }
  bool operator==(cssEl& s) 	{ return (val?1:0 == ((Int)s)?1:0); }
  bool operator!=(cssEl& s) 	{ return (val?1:0 != (Int)s)?1:0; }
  bool operator&&(cssEl& s) 	{ return (val && (Int)s); }
  bool operator||(cssEl& s) 	{ return (val || (Int)s); }
};


class CSS_API cssVariant: public cssEl {
INHERITED(cssEl)
public:
  Variant	val;

  int		GetParse() const	{ return CSS_VAR; }
  uint		GetSize() const		{ return sizeof(*this); }
  cssTypes 	GetType() const		{ return T_Variant; }
  const char*	GetTypeName() const 	{ return "(Variant)"; }

  String 	PrintStr() const;
  String	PrintFStr() const { return val.toString(); }

  // constructors
  void 		Constr()		{ Register(); } // default is Invalid
  void		Copy(const cssVariant& cp) { cssEl::Copy(cp); val = cp.val; }
  cssVariant()				{ Constr(); }
  cssVariant(const Variant& vl)		{ Constr(); val = vl; }
  cssVariant(const Variant& vl, const char* nm) { Constr(); val = vl; name = nm; }
  cssVariant(const cssVariant& cp)		{ Constr(); Copy(cp); }
  cssVariant(const cssVariant& cp, const char* nm){ Constr(); Copy(cp); name = nm; }

  cssCloneFuns(cssVariant, _nilVariant);

  // converters -- note, be as permissive as possible, because strings can easily arise
  // unexpectedly in most other atomic contexts (esp numeric)
  String GetStr() const	{ return val.toString(); }
  Variant GetVar() const { return val; }
  operator Real() const	 { return val.toDouble();}
  operator float() const { return val.toFloat();}
  operator Int() const	 { return val.toInt();}
  operator int64_t() const { return val.toInt64(); }
  operator uint64_t() const { return val.toUInt64(); }
  operator TAPtr() const;
#ifndef NO_BUILTIN_BOOL
  operator bool() const	{ return val.toBool(); }
#endif
  
  void operator=(Real cp) 		{ val = cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(int64_t cp)		{ val = cp; }
  void operator=(uint64_t cp)		{ val = cp; }
  void operator=(const String& cp)	{ val = cp; }
  void operator=(const Variant& cp)	{ val = cp;}

  void operator=(void* ptr)	 	{ val = ptr; }
  void operator=(void**)	{ CvtErr("(void**)"); }

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

  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);

  bool operator< (cssEl& s);
  bool operator> (cssEl& s);
  bool operator! ();
  bool operator<=(cssEl& s);
  bool operator>=(cssEl& s);
  bool operator==(cssEl& s);
  bool operator!=(cssEl& s);
  bool operator&&(cssEl& s);
  bool operator||(cssEl& s);
  
  // these delegate to the string or taBase, otherwise non for atomics
  cssEl* operator[](int idx) const; // only valid for Matrixes, gets flat el
  int	 GetMemberFunNo(const char*) const;
  cssEl* GetMemberFun(int memb) const; // numbers only refer to intrinsic Variant funcs
  cssEl* GetMemberFun(const char* nm) const;
  int	 GetMemberNo(const char* memb) const {return -1;} // no statics
  cssEl* GetMember(const char* nm) const;
  cssEl* GetScoped(const char*) const;
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
  cssTypes	GetPtrType() 		{ return el_type->GetType(); }
  const char*	GetTypeName() const	{ return el_type->GetTypeName(); }
  cssEl*	GetTypeObject() const	{ return el_type; }
  bool		IsStringType() const   	{ cssEl* el = ((cssPtr*)this)->GetActualObj();
    if (el) return el->IsStringType(); else return false;}
  bool		IsNumericTypeStrict() const { cssEl* el = ((cssPtr*)this)->GetActualObj();
    if (el) return el->IsNumericTypeStrict(); else return false;}

  cssEl*	GetActualObj() 		{ return ptr.El()->GetActualObj(); }

  virtual cssEl* GetNonPtrTypeObj() const;
  virtual cssTypes GetNonPtrType() const { return GetNonPtrTypeObj()->GetType(); }

  String 	PrintStr() const;
  String	PrintFStr() const 	{ return ptr.El()->PrintFStr(); }

  void		SetPtr(const cssElPtr& pt) { cssEl::SetRefElPtr(ptr, pt); }
  void		SetPtr(cssEl* it)	{ cssElPtr pt; pt.SetDirect(it); SetPtr(pt); }

  void		SetElType(cssEl* typ) 	{ cssEl::SetRefPointer(&el_type, typ); }

  // constructors
  void		Constr();
  void		Copy(const cssPtr& cp);
  cssPtr();
  cssPtr(const cssElPtr& it);	// this sets the pointer
  cssPtr(cssEl* typ);		// this sets the type
  cssPtr(cssEl* typ, const char* nm); // sets type
  cssPtr(cssEl* typ, const cssElPtr& it, const char* nm);
  cssPtr(const cssPtr& cp);
  cssPtr(const cssPtr& cp, const char* nm);
  ~cssPtr();

  cssCloneOnly(cssPtr);
  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssPtr(el_type, (const char*)*(arg[1])); }

  int    GetMemberNo(const char* s) const;
  cssEl* GetMember(const char* s) const  	{ return ptr.El()->GetMember(s); }
  cssEl* GetMember(int s) const  		{ return ptr.El()->GetMember(s); }
  int	 GetMemberFunNo(const char* s) const;
  cssEl* GetMemberFun(const char* s) const;
  cssEl* GetMemberFun(int s) const;
  cssEl* GetScoped(const char* s) const  	{ return ptr.El()->GetScoped(s); }

  // converters
  Int		GetIntVal() const	// get integer value of pointer
  { Int rval = (Int)(long)(ptr.El()); if(ptr.El() == &cssMisc::Void) rval = 0;
    return rval; }

  String	GetStr() const;
  Variant	GetVar() const { return Variant((void*)ptr.El()); } // raw pointer
  operator 	Real() const	{ return (Real)GetIntVal(); }
  operator 	Int() const	{ return GetIntVal(); }

  void	DelOpr()		{ SetPtr(cssMisc::VoidElPtr); }
  // delete is done by unrefing thing we point to, seting ptr to null..

//  void operator=(Real)	 	{ CvtErr("(Real)"); }
//  void operator=(Int)		{ CvtErr("(Int)"); }
//  void operator=(const String&)	{ CvtErr("(String)"); }

  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

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
  cssEl* operator[](int i) const
  { cssElPtr tmp = GetOprPtr(); tmp += i; return tmp.El(); };

  void operator+=(cssEl& t)
  { cssElPtr r = GetOprPtr(); r += (Int)t; SetPtr(r); }
  void operator-=(cssEl& t)
  { cssElPtr r = GetOprPtr(); r -= (Int)t; SetPtr(r); }

  bool operator< (cssEl& s) { return (GetIntVal() < (Int)s); }
  bool operator> (cssEl& s) { return (GetIntVal() > (Int)s); }
  bool operator! () 	    { return (GetIntVal() == 0); }
  bool operator<=(cssEl& s) { return (GetIntVal() <= (Int)s); }
  bool operator>=(cssEl& s) { return (GetIntVal() >= (Int)s); }
  bool operator==(cssEl& s) { return (GetIntVal() == (Int)s); }
  bool operator!=(cssEl& s) { return (GetIntVal() != (Int)s); }
  bool operator&&(cssEl& s) { return (GetIntVal() && (Int)s); }
  bool operator||(cssEl& s) { return (GetIntVal() || (Int)s); }
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
  cssArray(int no, const char* nm);
  cssArray(int no, cssEl* it, const char* nm);
  cssArray(const cssArray& cp);
  cssArray(const cssArray& cp, const char* nm);
  cssArray(const cssArrayType& cp, const char* nm);
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

  void operator=(Real)	 	{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void*)	 	{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

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

  virtual void  Fill(cssEl*) {};             // nop
  virtual int   Alloc(int no) { size = no; return true; }   // just set size
  virtual int   AllocAll(int na, cssEl* arg[]); // reallocate this and sub-arrays based on args

  cssArrayType();
  cssArrayType(int no, cssEl* it);
  cssArrayType(int no, cssEl* it, const char* nm);
  cssArrayType(const cssArrayType& cp);
  cssArrayType(const cssArrayType& cp, const char* nm);
  ~cssArrayType();

  cssCloneOnly(cssArrayType);
  cssEl*        MakeToken_stub(int na, cssEl* arg[]);

  // converters
  void operator=(Real)          { CvtErr("(Real)"); }
  void operator=(Int)           { CvtErr("(Int)"); }
  void operator=(const String&) { CvtErr("(String)"); }
  void operator=(void*)         { CvtErr("(void*)"); }
  void operator=(void**)        { CvtErr("(void**)"); }

  // operators
  cssEl* operator+(cssEl&)      { NopErr("+"); return &cssMisc::Void; }
  cssEl* operator-()            { NopErr("-"); return &cssMisc::Void; }
  cssEl* operator-(cssEl&)      { NopErr("-"); return &cssMisc::Void; }
  cssEl* operator*(cssEl&)      { NopErr("*"); return &cssMisc::Void; }
  cssEl* operator*()            { NopErr("*"); return &cssMisc::Void; }
  cssEl* operator[](int) const
  { NopErr("[]"); return &cssMisc::Void; }
  void operator+=(cssEl&)       { NopErr("+="); }
  void operator-=(cssEl&)       { NopErr("-="); }
  bool operator< (cssEl&)       { NopErr("<"); return false; }
  bool operator> (cssEl&)       { NopErr(">"); return false; }
  bool operator! ()             { NopErr("!"); return false; }
  bool operator<=(cssEl&)       { NopErr("<="); return false; }
  bool operator>=(cssEl&)       { NopErr(">="); return false; }
  bool operator==(cssEl&)       { NopErr("=="); return false; }
  bool operator!=(cssEl&)       { NopErr("!="); return false; }
  bool operator&&(cssEl&)       { NopErr("&&"); return false; }
  bool operator||(cssEl&)       { NopErr("||"); return false; }

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
  // todo: is this what we want? probably ok..
  cssTypes	GetPtrType() 		{ return ptr.El()->GetPtrType(); }
  const char*	GetTypeName() const	{ return ptr.El()->GetTypeName(); }
  cssEl*	GetTypeObject() const	{ return ptr.El()->GetTypeObject(); }
  int		IsRef()	const		{ return true; }
  bool		IsStringType() const   	{ return ptr.El()->IsStringType(); }
  bool		IsNumericTypeStrict() const { return ptr.El()->IsNumericTypeStrict(); }
  cssElPtr	GetAddr() const;

  cssEl*	GetActualObj() 		{ return ptr.El()->GetActualObj(); }

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
  void		Constr()			{ Register(); }
  void		Copy(const cssRef& cp) 		{ cssEl::Copy(cp); ptr = cp.ptr; }
  cssRef()					{ Constr(); }
  cssRef(const cssElPtr& it)			{ Constr(); ptr = it; }
  cssRef(cssEl* it)				{ Constr(); ptr.SetDirect(it); }
  cssRef(cssEl* it, const char* nm)		{ Constr(); name = nm; ptr.SetDirect(it); }
  cssRef(const cssElPtr& it, const char* nm)	{ Constr(); name = nm; ptr = it; }
  cssRef(const cssRef& cp)			{ Constr(); Copy(cp); }
  cssRef(const cssRef& cp, const char* nm)	{ Constr(); Copy(cp); name = nm; }

  cssCloneOnly(cssRef);
  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssRef(ptr, (const char*)*(arg[1])); }

  // converters
  String GetStr() const 	{ return ptr.El()->GetStr(); }
  Variant GetVar() const 	{ return ptr.El()->GetVar(); }
  operator Real() const	 	{ return (Real)*(ptr.El()); }
  operator Int() const	 	{ return (Int)*(ptr.El()); }
  operator void*() const	{ return (void*)*(ptr.El()); }
  operator void**() const	{ return (void**)*(ptr.El()); }

  void* GetVoidPtrOfType(TypeDef* td) const
  { return ptr.El()->GetVoidPtrOfType(td); }
  void* GetVoidPtrOfType(const char* td) const
  { return ptr.El()->GetVoidPtrOfType(td); }

  operator int*() const	 	{ return (int*)*(ptr.El()); }
  operator short*() const	{ return (short*)*(ptr.El()); }
  operator long*() const	{ return (long*)*(ptr.El()); }
  operator double*() const	{ return (double*)*(ptr.El()); }
  operator float*() const	{ return (float*)*(ptr.El()); }
  operator String*() const	{ return (String*)*(ptr.El()); }
#ifndef NO_BUILTIN_BOOL
  operator bool*() const	{ return (bool*)*(ptr.El()); }
#endif

  operator int**() const	{ return (int**)*(ptr.El()); }
  operator short**() const	{ return (short**)*(ptr.El()); }
  operator long**() const	{ return (long**)*(ptr.El()); }
  operator double**() const	{ return (double**)*(ptr.El()); }
  operator float**() const	{ return (float**)*(ptr.El()); }
  operator String**() const	{ return (String**)*(ptr.El()); }
#ifndef NO_BUILTIN_BOOL
  operator bool**() const	{ return (bool**)*(ptr.El()); }
#endif

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
#ifdef CSS_SUPPORT_TYPEA
  operator TAPtr() const	{ return (TAPtr)*(ptr.El()); }
  operator TAPtr*() const	{ return (TAPtr*)*(ptr.El()); }
  operator TypeDef*() const	{ return (TypeDef*)*(ptr.El()); }
  operator MemberDef*() const	{ return (MemberDef*)*(ptr.El()); }
  operator MethodDef*() const	{ return (MethodDef*)*(ptr.El()); }
#endif

  // assign from types
  void operator=(Real cp) 		{ ptr.El()->operator=(cp); }
  void operator=(Int cp)		{ ptr.El()->operator=(cp); }
  void operator=(const String& cp)	{ ptr.El()->operator=(cp); }

  void operator=(void* cp) 	{ ptr.El()->operator=(cp); }
  void operator=(void** cp)	{ ptr.El()->operator=(cp); }

  void AssignFromType(TypeDef* td, void* bs)  	{ ptr.El()->AssignFromType(td, bs); }
  void AssignFromType(const char* td, void* bs) { ptr.El()->AssignFromType(td, bs); }

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
  cssEl* operator*()	   	{ return ptr.El()->operator*(); }
  cssEl* operator[](int i) const		{ return ptr.El()->operator[](i); }
  int    GetMemberNo(const char* s) const;
  cssEl* GetMember(const char* s) const  	{ return ptr.El()->GetMember(s); }
  cssEl* GetMember(int s) const  		{ return ptr.El()->GetMember(s); }
  int	 GetMemberFunNo(const char* s) const;
  cssEl* GetMemberFun(int s) const		{ return ptr.El()->GetMemberFun(s); }
  cssEl* GetScoped(const char* s) const  	{ return ptr.El()->GetScoped(s); }
  cssEl* NewOpr()   				{ return ptr.El()->NewOpr(); }
  void	 DelOpr() 				{ ptr.El()->DelOpr(); }

  bool operator< (cssEl& s) 	{ return ptr.El()->operator<(s); }
  bool operator> (cssEl& s) 	{ return ptr.El()->operator>(s); }
  bool operator! () 	  	{ return ptr.El()->operator!(); }
  bool operator<=(cssEl& s) 	{ return ptr.El()->operator<=(s); }
  bool operator>=(cssEl& s) 	{ return ptr.El()->operator>=(s); }
  bool operator==(cssEl& s) 	{ return ptr.El()->operator==(s); }
  bool operator!=(cssEl& s) 	{ return ptr.El()->operator!=(s); }
  bool operator&&(cssEl& s) 	{ return ptr.El()->operator&&(s); }
  bool operator||(cssEl& s) 	{ return ptr.El()->operator||(s); }

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

  virtual void	SetTypeName(const char* nm);

  void		TypeInfo(ostream& fh = cout) const;

  // constructors
  void 		Constr();
  void		Copy(const cssEnumType& cp);
  cssEnumType()				{ Constr(); }
  cssEnumType(const char* nm)		{ Constr(); SetTypeName(nm); }
  cssEnumType(const cssEnumType& cp);
  cssEnumType(const cssEnumType& cp, const char* nm);
  ~cssEnumType();

  cssCloneOnly(cssEnumType);
  cssEl* 	MakeToken_stub(int, cssEl* arg[]); // make an instance instead

  cssEl* 	GetScoped(const char* memb) const;
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
  cssEnum()                      : cssInt()		{ Constr(); }
  cssEnum(Int vl)                : cssInt(vl)		{ Constr(); }
  cssEnum(Int vl, const char* nm ) : cssInt(vl, nm)	{ Constr(); }
  cssEnum(cssEnumType* cp, Int vl, const char* nm);
  // this is how it should be created..
  cssEnum(const cssEnum& cp);
  cssEnum(const cssEnum& cp, const char* nm);
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

  // operators
  void operator=(const cssEl& s);

  bool operator==(cssEl& s);
  bool operator!=(cssEl& s);
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
  cssClassMember()  : cssEl()		{ Constr(); }
  cssClassMember(cssEl* mbtp);
  cssClassMember(cssEl* mbtp, const char* mbnm);
  cssClassMember(const cssClassMember& cp);
  cssClassMember(const cssClassMember& cp, const char* nm);
  ~cssClassMember();
  cssCloneFuns(cssClassMember, *this);

  int    GetMemberNo(const char* s) const	{ return mbr_type->GetMemberNo(s); }
  cssEl* GetMember(const char* s) const  	{ return mbr_type->GetMember(s); }
  cssEl* GetMember(int s) const  		{ return mbr_type->GetMember(s); }
  int	 GetMemberFunNo(const char* s) const	{ return mbr_type->GetMemberFunNo(s); }
  cssEl* GetMemberFun(const char* s) const	{ return mbr_type->GetMemberFun(s); }
  cssEl* GetMemberFun(int s) const		{ return mbr_type->GetMemberFun(s); }
  cssEl* GetScoped(const char* s) const  	{ return mbr_type->GetScoped(s); }
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

  void		SetTypeName(const char* nm);
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
  cssClassType()			{ Constr(); AddBuiltins(); }
  cssClassType(const char* nm)		{ Constr(); SetTypeName(nm); AddBuiltins(); }
  cssClassType(const cssClassType& cp);
  cssClassType(const cssClassType& cp, const char* nm);
  ~cssClassType();
  cssCloneOnly(cssClassType);

  cssEl* 	MakeToken_stub(int, cssEl* arg[]); // make an instance instead

  void	CallVoidMethod(cssClassInst* tok, const char* meth_nm);	// call a method taking no args & returning nothing..

  void	ConstructToken(cssClassInst* tok);	// call all constructors on this class
  void	DestructToken(cssClassInst* tok);	// call all destructors on this class

  void	ConstructToken_impl(cssClassInst* tok); // call constructor on this class
  void	DestructToken_impl(cssClassInst* tok); // call destructor on this class

  void	UpdateAfterEdit_impl(cssClassInst* tok);
  // call updateafteredit function on token if it is defined

  void		SetDesc_impl(String& dsc, String& opt, const char* des);
  // set given description and opts strings based on des input string
  String	OptionAfter_impl(const String& optns, const char* opt) const;

  void		SetDesc(const char* des)	   { SetDesc_impl(desc, opts, des); }
  String	OptionAfter(const char* opt) const { return OptionAfter_impl(opts, opt); }
  bool		HasOption(const char* opt) const   { return opts.contains(opt); }

  void		MbrSetDesc(int mbr, const char* des);
  String	MbrOptionAfter(int mbr, const char* opt) const
  { return OptionAfter_impl(member_opts.SafeEl(mbr), opt); }
  bool		MbrHasOption(int mbr, const char* opt) const
  { return member_opts.SafeEl(mbr).contains(opt); }

  void          GetComments(cssEl* obj, cssElPtr cmt);
  // extract info for options and desc for class object from comment

  // converters
  String	GetStr() const;
  Variant	GetVar() const 	{ CvtErr("(Variant)"); return _nilVariant; }
  operator 	Real() const		{ CvtErr("(Real)"); return 0; }
  operator 	Int() const		{ CvtErr("(Int)"); return 0; }

  int 	 GetMemberNo(const char* memb) const;
  cssEl* GetMember(const char* memb) const;
  cssEl* GetMember(int memb) const;
  int 	 GetMemberFunNo(const char* memb) const;
  cssEl* GetMemberFun(const char* memb) const;
  cssEl* GetMemberFun(int memb) const;
  cssEl* GetScoped(const char* memb) const;
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
  cssClassInst()			{ Constr(); }
  cssClassInst(const char* nm)		{ Constr(); name = nm; }
  cssClassInst(cssClassType* cp, const char* nm);
  // use this to create from type
  cssClassInst(const cssClassInst& cp);
  cssClassInst(const cssClassInst& cp, const char* nm);
  ~cssClassInst();

  cssCloneFuns(cssClassInst, *this);

  void	 ConstructToken();	// call constructor on this class
  void	 DestructToken();	// call destructor on this class

  // converters
  String	GetStr() const;
  Variant	GetVar() const 	{ CvtErr("(Variant)"); return _nilVariant; }
  operator 	Real() const		{ CvtErr("(Real)"); return 0; }
  operator 	Int() const		{ CvtErr("(Int)"); return 0; }

  void operator=(Real) 	 	{ CvtErr("(Real)"); }
  void operator=(Int) 		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void*)		{ CvtErr("(void*)"); }
  void operator=(void**)	{ CvtErr("(void**)"); }

  // operators
  void operator=(const cssEl& s);

  void	UpdateAfterEdit();

  int 	 GetMemberNo(const char* memb) const;
  cssEl* GetMember(const char* memb) const;
  cssEl* GetMember(int memb) const;
  int 	 GetMemberFunNo(const char* memb) const;
  cssEl* GetMemberFun(const char* memb) const;
  cssEl* GetMemberFun(int memb) const;
  cssEl* GetScoped(const char* memb) const;
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

  cssSubShell();
  cssSubShell(const char* nm);
  cssSubShell(const cssSubShell& cp);
  cssSubShell(const cssSubShell& cp, const char* nm);

  cssEl*	MakeToken_stub(int, cssEl* arg[])
  { return new cssSubShell((const char*)*(arg[1])); }

  cssCloneOnly(cssSubShell);
};

#endif // CSS_BASIC_TYPES_H
