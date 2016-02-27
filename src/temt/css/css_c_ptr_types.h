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


// elaboration of pointers to internal C variables

#ifndef css_ptr_i_h
#define css_ptr_i_h 1

#include "css_machine.h"
#include "css_basic_types.h"

#include <DynEnum>

class CSS_API cssCPtr_int : public cssCPtr {
  // Points to a C integer
public:
  static int	null_int;	// safe rval

  int&		GetIntRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Int; }
  uint		GetSize() const override 	{ return sizeof(int); } // use for ptrs
  const char*	GetTypeName() const override 	{ return "(c_int)"; }

  // constructors
  cssCPtr_int() 				: cssCPtr(){};
  cssCPtr_int(void* it, int pc, const String& nm = _nilString, cssEl* cls_par = NULL, bool ro = false)
  : cssCPtr(it,pc,nm,cls_par,ro) {};
  cssCPtr_int(const cssCPtr_int& cp) 		: cssCPtr(cp){};
  cssCPtr_int(const cssCPtr_int& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_int, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetIntRef("(String)")); }
  Variant GetVar() const override	{ return Variant(GetIntRef("(Variant)")); }
  operator Int() const override		{ return GetIntRef("(Int)"); }
  operator Real() const override		{ return (Real)(GetIntRef("(Real)")); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetIntRef("(bool)"); return (bool)ptr; }
  operator int*() const override		{ return (int*)GetNonNullVoidPtr("(int)"); }
  operator int**() const override	{ return (int**)GetNonNullVoidPtr("(int**)", 2); }

  void operator=(Real cp) override 	{ GetIntRef("=(Real)") = (int)cp; }
  void operator=(Int cp) override	{ GetIntRef("=(Int)") = cp; }
  void operator=(const String& cp) override { GetIntRef("=(String)") = (int)cp; }
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef("%")); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef("<<")); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef(">>")); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef("&")); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef("^")); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetIntRef("|")); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetIntRef("-")); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetIntRef("~")); r->val = ~r->val; return r; }

  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetIntRef("<") < (Int)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetIntRef(">") > (Int)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetIntRef("<=") <= (Int)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetIntRef(">=") >= (Int)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetIntRef("==") == (Int)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetIntRef("!=") != (Int)s); return cssCPtr::operator!=(s); }
};

#define cssCPtr_int_inst(l,n)		l .Push(new cssCPtr_int(&n,1,#n))
#define cssCPtr_int_inst_nm(l,n,s)	l .Push(new cssCPtr_int(n,1,s))
#define cssCPtr_int_inst_ptr(l,n,x)	l .Push(x = new cssCPtr_int(&n,1,#x))
#define cssCPtr_int_inst_ptr_nm(l,n,x,s) l .Push(x = new cssCPtr_int(n,1,s))

class CSS_API cssCPtr_bool : public cssCPtr {
  // points to a C bool
public:
  static bool null_bool;

  bool&		GetBoolRef(const char* opr) const;

  cssTypes	GetPtrType() const override	{ return T_Bool; }
  uint		GetSize() const override 	{ return sizeof(bool); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_bool)"; }

  // constructors
  cssCPtr_bool() 				: cssCPtr(){};
  cssCPtr_bool(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_bool(const cssCPtr_bool& cp) 		: cssCPtr(cp){};
  cssCPtr_bool(const cssCPtr_bool& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_bool, (void*)NULL);

  // converters
  String GetStr() const override;
  Variant GetVar() const override	{ return Variant(GetBoolRef("(Variant)")); }
  operator Int() const override		{ return GetBoolRef("(Int)"); }
  operator Real() const override		{ return (Real)(GetBoolRef("(Real)")); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetBoolRef("(bool)"); return (bool)ptr; }
  operator bool*() const override	{ return (bool*)GetNonNullVoidPtr("(bool*)"); }
  operator bool**() const override	{ return (bool**)GetNonNullVoidPtr("(bool**)", 2); }

  void operator=(Real cp) override 	{ GetBoolRef("=") = (bool)cp; }
  void operator=(Int cp) override	{ GetBoolRef("=") = (bool)cp; }
  void operator=(const String& cp) override;
  using cssCPtr::operator=;

  cssEl* operator&(cssEl &t) override
  { cssBool *r = new cssBool(GetBoolRef("&")); r->val &= (bool)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssBool *r = new cssBool(GetBoolRef("^")); r->val ^= (bool)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssBool *r = new cssBool(GetBoolRef("|")); r->val |= (bool)t; return r; }

  // operators
  void operator=(const cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetBoolRef("==") == (bool)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetBoolRef("!=") != (bool)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_short : public cssCPtr {
  // points to a C short
public:
  static short null_short;

  short&	GetShortRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Short; }
  uint		GetSize() const override 	{ return sizeof(short); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_short)"; }

  // constructors
  cssCPtr_short() 				: cssCPtr(){};
  cssCPtr_short(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_short(const cssCPtr_short& cp) 		: cssCPtr(cp){};
  cssCPtr_short(const cssCPtr_short& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_short, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetShortRef()); }
  Variant GetVar() const override	{ return Variant(GetShortRef()); }
  operator Int() const override		{ return GetShortRef("(Int)"); }
  operator Real() const override		{ return (Real)(GetShortRef("(Real)")); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetShortRef("(bool)"); return (bool)ptr; }
  operator short() const override	{ return GetShortRef("(short)"); }
  operator short*() const override	{ return (short*)GetNonNullVoidPtr("(short*)"); }
  operator short**() const override	{ return (short**)GetNonNullVoidPtr("(short**)", 2); }

  void operator=(Real cp) override 	{ GetShortRef("=") = (short)cp; }
  void operator=(Int cp) override	{ GetShortRef("=") = (short)cp; }
  void operator=(const String& cp) override { GetShortRef("=") = (short)(int)cp; }
  using cssCPtr::operator=;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef("%")); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef("<<")); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef(">>")); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef("&")); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef("^")); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetShortRef("|")); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetShortRef("-")); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetShortRef("-")); r->val = ~r->val; return r; }

  // operators
  void operator=(const cssEl& t) override;
  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetShortRef("<") < (short)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetShortRef(">") > (short)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetShortRef("<=") <= (short)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetShortRef(">=") >= (short)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetShortRef("==") == (short)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetShortRef("!=") != (short)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_long : public cssCPtr {
  // points to a C long
public:
  static long null_long;

  long&		GetLongRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Long; }
  uint		GetSize() const override 	{ return sizeof(long); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_long)"; }

  // constructors
  cssCPtr_long() 				: cssCPtr(){};
  cssCPtr_long(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_long(const cssCPtr_long& cp) 		: cssCPtr(cp){};
  cssCPtr_long(const cssCPtr_long& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_long, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetLongRef()); }
  Variant GetVar() const override	{ return Variant(GetLongRef()); }
  operator Int() const override		{ return GetLongRef("(Int)"); }
  operator Real() const override		{ return (Real)(GetLongRef("(Real)")); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetLongRef("(bool)"); return (bool)ptr; }
  operator long() const override		{ return GetLongRef("(long)"); }
  operator long*() const override	{ return (long*)GetNonNullVoidPtr("(long*)"); }
  operator long**() const override	{ return (long**)GetNonNullVoidPtr("(long**)", 2); }

  void operator=(Real cp) override 	{ GetLongRef("=") = (long)cp; }
  void operator=(Int cp) override	{ GetLongRef("=") = (long)cp; }
  void operator=(const String& cp) override { GetLongRef("=") = (long)(int)cp; }
  using cssCPtr::operator=;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef("%")); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef("<<")); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef(">>")); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef("&")); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef("^")); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetLongRef("|")); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetLongRef("-")); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetLongRef("~")); r->val = ~r->val; return r; }

  // operators
  void operator=(const cssEl& t) override;
  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetLongRef("<") < (long)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetLongRef(">") > (long)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetLongRef("<=") <= (long)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetLongRef(">=") >= (long)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetLongRef("==") == (long)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetLongRef("!=") != (long)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_long_long : public cssCPtr {
public:
  static int64_t null_long_long;

  int64_t&	GetLongLongRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_LongLong; }
  uint		GetSize() const override 	{ return sizeof(long long); }
  const char*	GetTypeName() const override  	{ return "(c_long_long)"; }

  // constructors
  cssCPtr_long_long() 				: cssCPtr(){};
  cssCPtr_long_long(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_long_long(const cssCPtr_long_long& cp) 		: cssCPtr(cp){};
  cssCPtr_long_long(const cssCPtr_long_long& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_long_long, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetLongLongRef()); }
  Variant GetVar() const override	{ return Variant(GetLongLongRef()); }
  operator Int() const override		{ return GetLongLongRef("(Int)"); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetLongLongRef("(bool)"); return (bool)ptr; }
  operator long() const override		{ return GetLongLongRef("(long)"); }

  void operator=(Real cp) override 	{ GetLongLongRef("=") = (int64_t)cp; }
  void operator=(Int cp) override	{ GetLongLongRef("=") = (int64_t)cp; }
  void operator=(const String& cp) override { GetLongLongRef("=") = (int64_t)cp; }
  using cssCPtr::operator=;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef("%")); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef("<<")); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef(">>")); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef("&")); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef("^")); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetLongLongRef("|")); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetLongLongRef("-")); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetLongLongRef("~")); r->val = ~r->val; return r; }

  // operators
  void operator=(const cssEl& t) override;
  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetLongLongRef("<") < (int64_t)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetLongLongRef(">") > (int64_t)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetLongLongRef("<=") <= (int64_t)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetLongLongRef(">=") >= (int64_t)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetLongLongRef("==") == (int64_t)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetLongLongRef("!=") != (int64_t)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_char : public cssCPtr {
  // points to a C char
public:
  static char null_char;

  char&		GetCharRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Char; }
  uint		GetSize() const override 	{ return sizeof(char); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_char)"; }

  // constructors
  cssCPtr_char() 				: cssCPtr(){};
  cssCPtr_char(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_char(const cssCPtr_char& cp) 		: cssCPtr(cp){};
  cssCPtr_char(const cssCPtr_char& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_char, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetCharRef()); }
  Variant GetVar() const override	{ return Variant(GetCharRef()); }
  operator Int() const override		{ return GetCharRef("(Int)"); }
  operator Real() const override		{ return (Real)(GetCharRef("(Real)")); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetCharRef("(bool)"); return (bool)ptr; }
  operator char() const override		{ return GetCharRef("(char)"); }
  operator char*() const override	{ return (char*)GetNonNullVoidPtr("(char*)"); }
  operator char**() const 	{ return (char**)GetNonNullVoidPtr("(char**)", 2); }

  void operator=(Real cp) override 	{ GetCharRef("=") = (char)cp; }
  void operator=(Int cp) override	{ GetCharRef("=") = (char)cp; }
  void operator=(const String& cp) override { GetCharRef("=") = (char)cp[0]; }
  using cssCPtr::operator=;

  cssEl* operator+(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("+")); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("-")); r->val -= (Int)t; return r; }
  cssEl* operator*() override		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("*")); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("/")); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("%")); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("<<")); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef(">>")); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("&")); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("^")); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetCharRef("|")); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetCharRef("-")); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetCharRef("~")); r->val = ~r->val; return r; }

  // operators
  void operator=(const cssEl& t) override;
  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetCharRef("<") < (char)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetCharRef(">") > (char)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetCharRef("<=") <= (char)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetCharRef(">=") >= (char)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetCharRef("==") == (char)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetCharRef("!=") != (char)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_enum : public cssCPtr_int {
public:
  TypeDef*	enum_type;	// typedef of the enum if available

  cssTypes	GetPtrType() const override	{ return T_Enum; }
  uint		GetSize() const override 	{ return sizeof(int); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_enum)"; }

  TypeDef*	GetEnumType() const;
  // if enum_type not present, attempts to get member def info from class_parent (use md to get type def)

  void		Copy(const cssCPtr_enum& cp)
  { cssCPtr_int::Copy(cp); enum_type = cp.enum_type; }
  void		CopyType(const cssCPtr_enum& cp)
  { cssCPtr_int::CopyType(cp); enum_type = cp.enum_type; }

  // constructors
  cssCPtr_enum() 				: cssCPtr_int() { enum_type = NULL; }
  cssCPtr_enum(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
    : cssCPtr_int(it,pc,nm,cp,ro) { enum_type = NULL; }
  cssCPtr_enum(void* it, int pc, const String& nm, TypeDef* et)
    : cssCPtr_int(it,pc,nm) { enum_type = et; }
  cssCPtr_enum(const cssCPtr_enum& cp) 		: cssCPtr_int(cp) { enum_type = cp.enum_type; }
  cssCPtr_enum(const cssCPtr_enum& cp, const String& nm)
    : cssCPtr_int(cp,nm) { enum_type = cp.enum_type; }

  cssCPtr_CloneFuns(cssCPtr_enum, (void*)NULL);

  // converters
  String 	GetStr() const override;

  void operator=(Real cp) override 	{ GetIntRef("=") = (int)cp; }
  void operator=(Int cp) override	{ GetIntRef("=") = cp; }
  void operator=(const String& cp) override;
  using cssCPtr_int::operator=;

  // operators
  void operator=(const cssEl& t) override;
  cssEl* operator==(cssEl& s) override;
  cssEl* operator!=(cssEl& s) override;
};

class CSS_API cssCPtr_double : public cssCPtr {
public:
  static double null_double;

  double&	GetDoubleRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Real; }
  uint		GetSize() const override 	{ return sizeof(double); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_double)"; }

  // constructors
  cssCPtr_double() 				: cssCPtr(){};
  cssCPtr_double(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_double(const cssCPtr_double& cp) 		: cssCPtr(cp){};
  cssCPtr_double(const cssCPtr_double& cp, const String& nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_double, (void*)NULL);

  // converters
  String GetStr() const override	  	{ return String(GetDoubleRef()); }
  Variant GetVar() const override	{ return Variant(GetDoubleRef()); }
  operator Real() const override		{ return GetDoubleRef("(Real)"); }
  operator Int() const override		{ return (Int)GetDoubleRef("(Int)"); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetDoubleRef("(bool)"); return (bool)ptr; }
  operator double*() const override	{ return (double*)GetNonNullVoidPtr("(double*)"); }
  operator double**() const override	{ return (double**)GetNonNullVoidPtr("(double**)", 2); }

  void operator=(Real cp) override 	{ GetDoubleRef("=") = (double)cp; }
  void operator=(Int cp) override	{ GetDoubleRef("=") = (double)cp; }
  void operator=(const String& cp) override { GetDoubleRef("=") = atof((const char*)cp); }
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl &t) override
  { cssReal *r = new cssReal(); r->val = pow(GetDoubleRef("^"), (Real)t); return r; }

  cssEl* operator-() override
  { cssReal *r = new cssReal(GetDoubleRef("-"),""); r->val = -r->val; return r; }

  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetDoubleRef("<") < (Real)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetDoubleRef(">") > (Real)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetDoubleRef("<=") <= (Real)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetDoubleRef(">=") >= (Real)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetDoubleRef("==") == (Real)s); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetDoubleRef("!=") != (Real)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_float : public cssCPtr {
public:
  static float null_float;

  float&	GetFloatRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_Real; }
  uint		GetSize() const override 	{ return sizeof(float); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_float)"; }

  // constructors
  cssCPtr_float() 				: cssCPtr(){};
  cssCPtr_float(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_float(const cssCPtr_float& cp) 		: cssCPtr(cp){};
  cssCPtr_float(const cssCPtr_float& cp, const String& nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_float, (void*)NULL);

  // converters
  String GetStr() const override	  	{ return String(GetFloatRef()); }
  Variant GetVar() const override	{ return Variant(GetFloatRef()); }
  operator Real() const override		{ return GetFloatRef("(Real)"); }
  operator Int() const override		{ return (Int)GetFloatRef("(Int)"); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetFloatRef("(bool)"); return (bool)ptr; }
  operator float*() const override	{ return (float*)GetNonNullVoidPtr("(float*)"); }
  operator float**() const override	{ return (float**)GetNonNullVoidPtr("(float**)", 2); }

  void operator=(Real cp) override 	{ GetFloatRef("=") = (float)cp; }
  void operator=(Int cp) override	{ GetFloatRef("=") = (float)cp; }
  void operator=(const String& cp) override { GetFloatRef("=") = atof((const char*)cp); }
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl& t) override;
  cssEl* operator-(cssEl& t) override;
  cssEl* operator*(cssEl& t) override;
  cssEl* operator/(cssEl& t) override;
  cssEl* operator*() override		{ return cssCPtr::operator*(); }

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl &t) override
  { cssReal *r = new cssReal(); r->val = pow((Real)GetFloatRef("^"), (Real)t); return r; }

  cssEl* operator-() override
  { cssReal *r = new cssReal(GetFloatRef("-"),""); r->val = -r->val; return r; }

  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetFloatRef("<") < (Real)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetFloatRef(">") > (Real)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetFloatRef("<=") <= (Real)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetFloatRef(">=") >= (Real)s); }
  cssEl* operator==(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetFloatRef("==") == (Real)s);  return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override 	{ if(ptr_cnt == 0) return new cssBool(GetFloatRef("!=") != (Real)s); return cssCPtr::operator!=(s); }
};

class CSS_API cssCPtr_String : public cssCPtr {
public:
  static String	null_string;	// for null pointers

  String&	GetStringRef(const char* opr="") const;

  cssTypes	GetPtrType() const override	{ return T_String; }
  uint		GetSize() const override 	{ return sizeof(String); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_String)"; }

  // constructors
  cssCPtr_String() 				: cssCPtr() {  }
  cssCPtr_String(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
    : cssCPtr(it,pc,nm,cp,ro) {  }
  cssCPtr_String(const cssCPtr_String& cp) 		: cssCPtr(cp) {  }
  cssCPtr_String(const cssCPtr_String& cp, const String& nm) : cssCPtr(cp,nm) {  }
  ~cssCPtr_String();

  cssCPtr_CloneFuns(cssCPtr_String, (void*)NULL);

  // converters
  String GetStr() const override		{ return GetStringRef(); }
  Variant GetVar() const override	{ return Variant(GetStr()); }
  operator Real() const override 	{ return atof((const char*)GetStringRef("(Real)")); }
  operator Int() const override		{ return (int)GetStringRef("(Int)"); }
  operator bool() const override;
  operator String*() const override	{ return (String*)GetNonNullVoidPtr("(String*)"); }
  operator String**() const override	{ return (String**)GetNonNullVoidPtr("(String**)", 2); }

  operator taBase*() const override;	// lookup as a path..
  operator TypeDef*() const override;	// lookup as name
  operator MemberDef*() const override;
  operator MethodDef*() const override;

  void operator=(Real cp) override 	{ GetStringRef("=") = String(cp); }
  void operator=(Int cp) override	{ GetStringRef("=") = String(cp); }
  void operator=(const String& cp) override { GetStringRef("=") = cp; }
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl &t) override
  { cssString *r = new cssString(GetStringRef("+"),""); r->val += t.GetStr(); return r; }
  cssEl* operator-(cssEl&) override		{ NopErr("-"); return &cssMisc::Void; }
  cssEl* operator*() override			{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl&) override		{ NopErr("*"); return &cssMisc::Void; }
  cssEl* operator/(cssEl&) override		{ NopErr("/"); return &cssMisc::Void; }
  cssEl* operator%(cssEl&) override		{ NopErr("%"); return &cssMisc::Void; }
  cssEl* operator-() override    		{ NopErr("-"); return &cssMisc::Void; }

  void operator+=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override { return new cssBool(GetStringRef("<") < s.GetStr()); }
  cssEl* operator> (cssEl& s) override { return new cssBool(GetStringRef(">") > s.GetStr()); }
  cssEl* operator<=(cssEl& s) override { return new cssBool(GetStringRef("<=") <= s.GetStr()); }
  cssEl* operator>=(cssEl& s) override { return new cssBool(GetStringRef(">=") >= s.GetStr()); }
  cssEl* operator==(cssEl& s) override { if(ptr_cnt == 0) return new cssBool(GetStringRef("==") == s.GetStr()); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override { if(ptr_cnt == 0) return new cssBool(GetStringRef("!=") != s.GetStr()); return cssCPtr::operator!=(s); }

  // these use the TA info to perform actions
  cssEl* operator[](const Variant& idx) const override;
  int	 GetMethodNo(const String&) const override;
  cssEl* GetMethodFmName(const String& memb) const override;
  cssEl* GetMethodFmNo(int memb) const override;
  cssEl* GetScoped(const String& nm) const override;
};


class CSS_API cssCPtr_Variant : public cssCPtr {
public:
  static Variant null_var;

  Variant&	GetVarRef(const char* opr="") const;

  cssTypes	GetPtrType() const override;
  uint		GetSize() const override 	{ return sizeof(Variant); } // use for ptrs
  const char*	GetTypeName() const override  	{ return "(c_Variant)"; }
  String	PrintStr() const override;

  String&	PrintType(String& fh) const override;
  String&	PrintInherit(String& fh) const override;

  // constructors
  cssCPtr_Variant() 				: cssCPtr(){};
  cssCPtr_Variant(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_Variant(const cssCPtr_Variant& cp) 		: cssCPtr(cp){};
  cssCPtr_Variant(const cssCPtr_Variant& cp, const String& nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_Variant, (void*)NULL);

  // converters
  String GetStr() const override		{ return (GetVarRef("(String)")).toString(); }
  Variant GetVar() const override	{ return GetVarRef("(Variant)"); }
  operator Real() const override 	{ return GetVarRef("(Real)").toDouble(); }
  operator Int() const override		{ return GetVarRef("(Int)").toInt(); }
  operator bool() const override		{ if(ptr_cnt == 0) return GetVarRef("(bool)").toBool(); return (bool)ptr; }

  void operator=(Real cp) override 	{ GetVarRef("=") = cp; }
  void operator=(Int cp) override	{ GetVarRef("=") = cp; }
  void operator=(const String& cp) override { GetVarRef("=") = cp; }
  void operator=(const Variant& val) override; 
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl &t) override
    { cssVariant *r = new cssVariant(GetVarRef("+"),""); r->val += t.GetVar(); return r; }
  cssEl* operator-(cssEl& t) override
    { cssVariant *r = new cssVariant(GetVarRef("-"),""); r->val -= t.GetVar(); return r; }
  cssEl* operator*() override			{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl& t) override
  { cssVariant *r = new cssVariant(GetVarRef("*"),""); r->val *= t.GetVar(); return r; }
  cssEl* operator/(cssEl& t) override
  { cssVariant *r = new cssVariant(GetVarRef("/"),""); r->val /= t.GetVar(); return r; }
  cssEl* operator%(cssEl& t) override
  { cssVariant *r = new cssVariant(GetVarRef("%"),""); r->val %= t.GetVar(); return r; }
  cssEl* operator-() override
  { cssVariant *r = new cssVariant(-GetVarRef("-"),""); return r; }
  cssEl* operator~() override
  { cssVariant *r = new cssVariant(~GetVarRef("~"),""); return r; }

  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;

  cssEl* operator< (cssEl& s) override { return new cssBool(GetVarRef("<") < s.GetVar()); }
  cssEl* operator> (cssEl& s) override { return new cssBool(GetVarRef(">") > s.GetVar()); }
  cssEl* operator<=(cssEl& s) override { return new cssBool(GetVarRef("<=") <= s.GetVar()); }
  cssEl* operator>=(cssEl& s) override { return new cssBool(GetVarRef(">=") >= s.GetVar()); }
  cssEl* operator==(cssEl& s) override { if(ptr_cnt == 0) return new cssBool(GetVarRef("==") == s.GetVar()); return cssCPtr::operator==(s); }
  cssEl* operator!=(cssEl& s) override { if(ptr_cnt == 0) return new cssBool(GetVarRef("!=") != s.GetVar()); return cssCPtr::operator!=(s); }

  // these use the TA info to perform actions
  cssEl* operator[](const Variant& idx) const override;
  bool	 MembersDynamic() override	{ return true; }
  int	 GetMemberNo(const String& memb) const override { return -1; } // never do static lookup!
  cssEl* GetMemberFmNo(int memb) const override;
  cssEl* GetMemberFmName(const String& memb) const override;
  int	 GetMethodNo(const String&) const override { return -1; }
  cssEl* GetMethodFmNo(int memb) const override;
  cssEl* GetMethodFmName(const String& memb) const override;
  cssEl* GetScoped(const String& nm) const override;
};

class CSS_API cssCPtr_DynEnum : public cssCPtr {
  // Points to a C++ dynamic enum type
public:
  static DynEnum null_enum;	// safe rval

  DynEnum&	GetEnumRef(const char* opr="") const;

  String&	PrintType(String& fh) const override;
  cssTypes	GetPtrType() const override	{ return T_DynEnum; }
  uint		GetSize() const override 	{ return sizeof(DynEnum); } // use for ptrs
  const char*	GetTypeName() const override 	{ return "(c_DynEnum)"; }

  // constructors
  cssCPtr_DynEnum() 				: cssCPtr(){};
  cssCPtr_DynEnum(void* it, int pc, const String& nm=_nilString, cssEl* cp=NULL, bool ro=false)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_DynEnum(const cssCPtr_DynEnum& cp) 		: cssCPtr(cp){};
  cssCPtr_DynEnum(const cssCPtr_DynEnum& cp, const String& nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_DynEnum, (void*)NULL);

  // converters
  String GetStr() const override		{ return String(GetEnumRef("(String)").NameVal()); }
  Variant GetVar() const override	{ return Variant(GetEnumRef("(Variant)").NumVal()); }
  operator Int() const override		{ return GetEnumRef("(Int)").NumVal(); }
  operator Real() const override		{ return (Real)(GetEnumRef("(Real)").NumVal()); }

  void operator=(Real cp) override 	{ GetEnumRef("=").SetNumVal((int)cp); }
  void operator=(Int cp) override	{ GetEnumRef("=").SetNumVal(cp); }
  void operator=(const String& cp) override;
  using cssCPtr::operator=;

  // operators
  void operator=(const cssEl& t) override;

  cssEl* operator+(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("+").NumVal()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("-").NumVal()); r->val -= (Int)t; return r; }
  cssEl* operator*() override		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("*").NumVal()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("/").NumVal()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("%").NumVal()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("<<").NumVal()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef(">>").NumVal()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("&").NumVal()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("^").NumVal()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t) override
  { cssInt *r = new cssInt(GetEnumRef("|").NumVal()); r->val |= (Int)t; return r; }

  cssEl* operator-() override
  { cssInt *r = new cssInt(GetEnumRef("-").NumVal()); r->val = -r->val; return r; }
  cssEl* operator~() override
  { cssInt *r = new cssInt(GetEnumRef("~").NumVal()); r->val = ~r->val; return r; }

  cssEl* operator< (cssEl& s) override 	{ return new cssBool(GetEnumRef("<").NumVal() < (Int)s); }
  cssEl* operator> (cssEl& s) override 	{ return new cssBool(GetEnumRef(">").NumVal() > (Int)s); }
  cssEl* operator<=(cssEl& s) override 	{ return new cssBool(GetEnumRef("<=").NumVal() <= (Int)s); }
  cssEl* operator>=(cssEl& s) override 	{ return new cssBool(GetEnumRef(">=").NumVal() >= (Int)s); }
  cssEl* operator==(cssEl& s) override;
  cssEl* operator!=(cssEl& s) override;

  void operator+=(cssEl& t) override;
  void operator-=(cssEl& t) override;
  void operator*=(cssEl& t) override;
  void operator/=(cssEl& t) override;
  void operator%=(cssEl& t) override;
  void operator<<=(cssEl& t) override;
  void operator>>=(cssEl& t) override;
  void operator&=(cssEl& t) override;
  void operator^=(cssEl& t) override;
  void operator|=(cssEl& t) override;

  int	 GetMethodNo(const String&) const override;
  cssEl* GetMethodFmName(const String& memb) const override;
  cssEl* GetMethodFmNo(int memb) const override;
  cssEl* GetScoped(const String& nm) const override;
};

#endif // css_ptr_i_h
