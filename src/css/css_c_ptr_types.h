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


// elaboration of pointers to internal C variables

#ifndef css_ptr_i_h
#define css_ptr_i_h 1

#include "css_machine.h"
#include "css_basic_types.h"
#include "dynenum.h"

class CSS_API cssCPtr_int : public cssCPtr {
  // Points to a C integer
public:
  static int	null_int;	// safe rval

  int&		GetIntRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_int; return *(int*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Int; }
  uint		GetSize() const 	{ return sizeof(int); } // use for ptrs
  const char*	GetTypeName() const 	{ return "(c_int)"; }

  // constructors
  cssCPtr_int() 				: cssCPtr(){};
  cssCPtr_int(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_int(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_int(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_int(const cssCPtr_int& cp) 		: cssCPtr(cp){};
  cssCPtr_int(const cssCPtr_int& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_int, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetIntRef()); }
  Variant GetVar() const	{ return Variant(GetIntRef()); }
  operator Int() const		{ return GetIntRef(); }
  operator Real() const		{ return (Real)(GetIntRef()); }
  operator int*() const		{ return (int*)GetNonNullVoidPtr(); }
  operator int**() const	{ return (int**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetIntRef() = (int)cp; }
  void operator=(Int cp)	{ GetIntRef() = cp; }
  void operator=(const String& cp) { GetIntRef() = (int)cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetIntRef()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetIntRef()); r->val = -r->val; return r; }

  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);
  void operator<<=(cssEl& t);
  void operator>>=(cssEl& t);
  void operator&=(cssEl& t);
  void operator^=(cssEl& t);
  void operator|=(cssEl& t);

  bool operator< (cssEl& s) 	{ return (GetIntRef() < (Int)s); }
  bool operator> (cssEl& s) 	{ return (GetIntRef() > (Int)s); }
  bool operator! () 	    	{ return ( ! GetIntRef()); }
  bool operator<=(cssEl& s) 	{ return (GetIntRef() <= (Int)s); }
  bool operator>=(cssEl& s) 	{ return (GetIntRef() >= (Int)s); }
  bool operator==(cssEl& s) 	{ return (GetIntRef() == (Int)s); }
  bool operator!=(cssEl& s) 	{ return (GetIntRef() != (Int)s); }
  bool operator&&(cssEl& s) 	{ return (GetIntRef() && (Int)s); }
  bool operator||(cssEl& s) 	{ return (GetIntRef() || (Int)s); }
};

#define cssCPtr_int_inst(l,n)		l .Push(new cssCPtr_int(&n,1,#n))
#define cssCPtr_int_inst_nm(l,n,s)	l .Push(new cssCPtr_int(n,1,s))
#define cssCPtr_int_inst_ptr(l,n,x)	l .Push(x = new cssCPtr_int(&n,1,#x))
#define cssCPtr_int_inst_ptr_nm(l,n,x,s) l .Push(x = new cssCPtr_int(n,1,s))

class CSS_API cssCPtr_bool : public cssCPtr_int {
public:
  bool&		GetBoolRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return (bool&)null_int; return *(bool*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Bool; }
  uint		GetSize() const 	{ return sizeof(bool); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_bool)"; }

  // constructors
  cssCPtr_bool() 				: cssCPtr_int(){};
  cssCPtr_bool(void* it, int pc) 		: cssCPtr_int(it,pc){};
  cssCPtr_bool(void* it, int pc, const char* nm)	: cssCPtr_int(it,pc,nm){};
  cssCPtr_bool(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr_int(it,pc,nm,cp,ro){};
  cssCPtr_bool(const cssCPtr_bool& cp) 		: cssCPtr_int(cp){};
  cssCPtr_bool(const cssCPtr_bool& cp, const char* nm) 	: cssCPtr_int(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_bool, (void*)NULL);

  // converters
  String GetStr() const;
  Variant GetVar() const 	{ return Variant(GetBoolRef()); } // make a bool Variant
  operator Int() const		{ return GetBoolRef(); }
#ifndef NO_BUILTIN_BOOL
  operator bool*() const	{ return (bool*)GetNonNullVoidPtr(); }
  operator bool**() const	{ return (bool**)GetNonNullVoidPtr(2); }
  operator int*() const		{ CvtErr("(int*)"); return NULL; }
  operator int**() const	{ CvtErr("(int**)"); return NULL; }
#endif

  void operator=(Real cp) 	{ GetBoolRef() = (bool)cp; }
  void operator=(Int cp)	{ GetBoolRef() = (bool)cp; }
  void operator=(const String& cp);
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);
};

class CSS_API cssCPtr_short : public cssCPtr {
  // points to a C short
public:
  static short null_short;

  short&		GetShortRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return (short&)null_short; return *(short*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Short; }
  uint		GetSize() const 	{ return sizeof(short); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_short)"; }

  // constructors
  cssCPtr_short() 				: cssCPtr(){};
  cssCPtr_short(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_short(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_short(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_short(const cssCPtr_short& cp) 		: cssCPtr(cp){};
  cssCPtr_short(const cssCPtr_short& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_short, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetShortRef()); }
  Variant GetVar() const	{ return Variant(GetShortRef()); }
  operator Int() const		{ return GetShortRef(); }
  operator Real() const		{ return (Real)(GetShortRef()); }
  operator short() const		{ return GetShortRef(); }
  operator short*() const	{ return (short*)GetNonNullVoidPtr(); }
  operator short**() const	{ return (short**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetShortRef() = (short)cp; }
  void operator=(Int cp)	{ GetShortRef() = (short)cp; }
  void operator=(const String& cp) { GetShortRef() = (short)(int)cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetShortRef()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetShortRef()); r->val = -r->val; return r; }

  // operators
  void operator=(const cssEl& t);
  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);
  void operator<<=(cssEl& t);
  void operator>>=(cssEl& t);
  void operator&=(cssEl& t);
  void operator^=(cssEl& t);
  void operator|=(cssEl& t);
};

class CSS_API cssCPtr_long : public cssCPtr {
  // points to a C long
public:
  static long null_long;

  long&		GetLongRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return (long&)null_long; return *(long*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Long; }
  uint		GetSize() const 	{ return sizeof(long); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_long)"; }

  // constructors
  cssCPtr_long() 				: cssCPtr(){};
  cssCPtr_long(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_long(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_long(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_long(const cssCPtr_long& cp) 		: cssCPtr(cp){};
  cssCPtr_long(const cssCPtr_long& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_long, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetLongRef()); }
  Variant GetVar() const	{ return Variant(GetLongRef()); }
  operator Int() const		{ return GetLongRef(); }
  operator Real() const		{ return (Real)(GetLongRef()); }
  operator long() const		{ return GetLongRef(); }
  operator long*() const	{ return (long*)GetNonNullVoidPtr(); }
  operator long**() const	{ return (long**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetLongRef() = (long)cp; }
  void operator=(Int cp)	{ GetLongRef() = (long)cp; }
  void operator=(const String& cp) { GetLongRef() = (long)(int)cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetLongRef()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetLongRef()); r->val = -r->val; return r; }

  // operators
  void operator=(const cssEl& t);
  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);
  void operator<<=(cssEl& t);
  void operator>>=(cssEl& t);
  void operator&=(cssEl& t);
  void operator^=(cssEl& t);
  void operator|=(cssEl& t);
};

class CSS_API cssCPtr_long_long : public cssCPtr {
public:
  static int64_t null_long_long;

  int64_t&		GetLongLongRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return (int64_t&)null_long_long; return *(int64_t*)nnp; }

  cssTypes	GetPtrType() const	{ return T_LongLong; }
  uint		GetSize() const 	{ return sizeof(long long); }
  const char*	GetTypeName() const  	{ return "(c_long_long)"; }

  // constructors
  cssCPtr_long_long() 				: cssCPtr(){};
  cssCPtr_long_long(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_long_long(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_long_long(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_long_long(const cssCPtr_long_long& cp) 		: cssCPtr(cp){};
  cssCPtr_long_long(const cssCPtr_long_long& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_long_long, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetLongLongRef()); }
  Variant GetVar() const	{ return Variant(GetLongLongRef()); }
  operator Int() const		{ return GetLongLongRef(); }
  operator long() const		{ return GetLongLongRef(); }

  void operator=(Real cp) 	{ GetLongLongRef() = (int64_t)cp; }
  void operator=(Int cp)	{ GetLongLongRef() = (int64_t)cp; }
  void operator=(const String& cp) { GetLongLongRef() = (int64_t)cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetLongLongRef()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetLongLongRef()); r->val = -r->val; return r; }

  // operators
  void operator=(const cssEl& t);
  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);
  void operator<<=(cssEl& t);
  void operator>>=(cssEl& t);
  void operator&=(cssEl& t);
  void operator^=(cssEl& t);
  void operator|=(cssEl& t);
};

class CSS_API cssCPtr_char : public cssCPtr {
  // points to a C char
public:
  static char null_char;

  char&		GetCharRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return (char&)null_char; return *(char*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Char; }
  uint		GetSize() const 	{ return sizeof(char); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_char)"; }

  // constructors
  cssCPtr_char() 				: cssCPtr(){};
  cssCPtr_char(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_char(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_char(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_char(const cssCPtr_char& cp) 		: cssCPtr(cp){};
  cssCPtr_char(const cssCPtr_char& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_char, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetCharRef()); }
  Variant GetVar() const	{ return Variant(GetCharRef()); }
  operator Int() const		{ return GetCharRef(); }
  operator Real() const		{ return (Real)(GetCharRef()); }
  operator char() const		{ return GetCharRef(); }
  operator char*() const	{ return (char*)GetNonNullVoidPtr(); }
  operator char**() const	{ return (char**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetCharRef() = (char)cp; }
  void operator=(Int cp)	{ GetCharRef() = (char)cp; }
  void operator=(const String& cp) { GetCharRef() = (char)(int)cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetCharRef()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetCharRef()); r->val = -r->val; return r; }

  // operators
  void operator=(const cssEl& t);
  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);
  void operator<<=(cssEl& t);
  void operator>>=(cssEl& t);
  void operator&=(cssEl& t);
  void operator^=(cssEl& t);
  void operator|=(cssEl& t);
};

class CSS_API cssCPtr_enum : public cssCPtr_int {
public:
  TypeDef*	enum_type;	// typedef of the enum if available

  cssTypes	GetPtrType() const	{ return T_Enum; }
  uint		GetSize() const 	{ return sizeof(int); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_enum)"; }

  TypeDef*	GetEnumType() const;
  // if enum_type not present, attempts to get member def info from class_parent (use md to get type def)

  // constructors
  cssCPtr_enum() 				: cssCPtr_int() { enum_type = NULL; }
  cssCPtr_enum(void* it, int pc) 		: cssCPtr_int(it,pc) { enum_type = NULL; }
  cssCPtr_enum(void* it, int pc, const char* nm): cssCPtr_int(it,pc,nm) { enum_type = NULL; }
  cssCPtr_enum(void* it, int pc, const char* nm, TypeDef* et)
    : cssCPtr_int(it,pc,nm) { enum_type = et; }
  cssCPtr_enum(void* it, int pc, const char* nm, cssEl* cp, bool ro)
    : cssCPtr_int(it,pc,nm,cp,ro) { enum_type = NULL; }
  cssCPtr_enum(const cssCPtr_enum& cp) 		: cssCPtr_int(cp) { enum_type = cp.enum_type; }
  cssCPtr_enum(const cssCPtr_enum& cp, const char* nm)
    : cssCPtr_int(cp,nm) { enum_type = cp.enum_type; }

  cssCPtr_CloneFuns(cssCPtr_enum, (void*)NULL);

  // converters
  String 	GetStr() const;

  void operator=(Real cp) 	{ GetIntRef() = (int)cp; }
  void operator=(Int cp)	{ GetIntRef() = cp; }
  void operator=(const String& cp);
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);
  bool operator==(cssEl& s);
  bool operator!=(cssEl& s);
};

class CSS_API cssCPtr_double : public cssCPtr {
public:
  static double null_double;

  double&	GetDoubleRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_double; return *(double*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Real; }
  uint		GetSize() const 	{ return sizeof(double); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_double)"; }

  // constructors
  cssCPtr_double() 				: cssCPtr(){};
  cssCPtr_double(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_double(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_double(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_double(const cssCPtr_double& cp) 		: cssCPtr(cp){};
  cssCPtr_double(const cssCPtr_double& cp, const char* nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_double, (void*)NULL);

  // converters
  String GetStr() const	  	{ return String(GetDoubleRef()); }
  Variant GetVar() const	{ return Variant(GetDoubleRef()); }
  operator Real() const		{ return GetDoubleRef(); }
  operator Int() const		{ return (Int)GetDoubleRef(); }
  operator double*() const	{ return (double*)GetNonNullVoidPtr(); }
  operator double**() const	{ return (double**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetDoubleRef() = (double)cp; }
  void operator=(Int cp)	{ GetDoubleRef() = (double)cp; }
  void operator=(const String& cp) { GetDoubleRef() = atof((const char*)cp); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);

  cssEl* operator+(cssEl &t)
  { cssReal *r = new cssReal(GetDoubleRef(),""); r->val += (Real)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssReal *r = new cssReal(GetDoubleRef(),""); r->val -= (Real)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssReal *r = new cssReal(GetDoubleRef(),""); r->val *= (Real)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssReal *r = new cssReal(GetDoubleRef(),""); r->val /= (Real)t; return r; }

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl &t)
  { cssReal *r = new cssReal(); r->val = pow(GetDoubleRef(), (Real)t); return r; }

  cssEl* operator-()
  { cssReal *r = new cssReal(GetDoubleRef(),""); r->val = -r->val; return r; }

  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);

  bool operator< (cssEl& s) 	{ return (GetDoubleRef() < (Real)s); }
  bool operator> (cssEl& s) 	{ return (GetDoubleRef() > (Real)s); }
  bool operator! () 	    	{ return ( ! GetDoubleRef()); }
  bool operator<=(cssEl& s) 	{ return (GetDoubleRef() <= (Real)s); }
  bool operator>=(cssEl& s) 	{ return (GetDoubleRef() >= (Real)s); }
  bool operator==(cssEl& s) 	{ return (GetDoubleRef() == (Real)s); }
  bool operator!=(cssEl& s) 	{ return (GetDoubleRef() != (Real)s); }
  bool operator&&(cssEl& s) 	{ return (GetDoubleRef() && (Real)s); }
  bool operator||(cssEl& s) 	{ return (GetDoubleRef() || (Real)s); }
};

class CSS_API cssCPtr_float : public cssCPtr {
public:
  static float null_float;

  float&	GetFloatRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_float; return *(float*)nnp; }

  cssTypes	GetPtrType() const	{ return T_Real; }
  uint		GetSize() const 	{ return sizeof(float); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_float)"; }

  // constructors
  cssCPtr_float() 				: cssCPtr(){};
  cssCPtr_float(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_float(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_float(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_float(const cssCPtr_float& cp) 		: cssCPtr(cp){};
  cssCPtr_float(const cssCPtr_float& cp, const char* nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_float, (void*)NULL);

  // converters
  String GetStr() const	  	{ return String(GetFloatRef()); }
  Variant GetVar() const	{ return Variant(GetFloatRef()); }
  operator Real() const		{ return GetFloatRef(); }
  operator Int() const		{ return (Int)GetFloatRef(); }
  operator float*() const	{ return (float*)GetNonNullVoidPtr(); }
  operator float**() const	{ return (float**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetFloatRef() = (float)cp; }
  void operator=(Int cp)	{ GetFloatRef() = (float)cp; }
  void operator=(const String& cp) { GetFloatRef() = atof((const char*)cp); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);

  cssEl* operator+(cssEl &t)
  { cssReal *r = new cssReal(GetFloatRef(),""); r->val += (Real)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssReal *r = new cssReal(GetFloatRef(),""); r->val -= (Real)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssReal *r = new cssReal(GetFloatRef(),""); r->val *= (Real)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssReal *r = new cssReal(GetFloatRef(),""); r->val /= (Real)t; return r; }

  // implement the to-the-power of operator as ^
  cssEl* operator^(cssEl &t)
  { cssReal *r = new cssReal(); r->val = pow(GetFloatRef(), (Real)t); return r; }

  cssEl* operator-()
  { cssReal *r = new cssReal(GetFloatRef(),""); r->val = -r->val; return r; }

  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);

  bool operator< (cssEl& s) 	{ return (GetFloatRef() < (Real)s); }
  bool operator> (cssEl& s) 	{ return (GetFloatRef() > (Real)s); }
  bool operator! () 	    	{ return ( ! GetFloatRef()); }
  bool operator<=(cssEl& s) 	{ return (GetFloatRef() <= (Real)s); }
  bool operator>=(cssEl& s) 	{ return (GetFloatRef() >= (Real)s); }
  bool operator==(cssEl& s) 	{ return (GetFloatRef() == (Real)s); }
  bool operator!=(cssEl& s) 	{ return (GetFloatRef() != (Real)s); }
  bool operator&&(cssEl& s) 	{ return (GetFloatRef() && (Real)s); }
  bool operator||(cssEl& s) 	{ return (GetFloatRef() || (Real)s); }
};

class CSS_API cssCPtr_String : public cssCPtr {
public:
  static String	null_string;	// for null pointers

  String&	GetStringRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_string; return *(String*)nnp; }

  cssTypes	GetPtrType() const	{ return T_String; }
  uint		GetSize() const 	{ return sizeof(String); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_String)"; }

  // constructors
  cssCPtr_String() 				: cssCPtr(){};
  cssCPtr_String(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_String(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_String(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_String(const cssCPtr_String& cp) 		: cssCPtr(cp){};
  cssCPtr_String(const cssCPtr_String& cp, const char* nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_String, (void*)NULL);

  // converters
  String GetStr() const		{ return GetStringRef(); }
  Variant GetVar() const	{ return Variant(GetStr()); }
  operator Real() const 	{ return atof((const char*)GetStringRef()); }
  operator Int() const		{ return (int)GetStringRef(); }
  operator bool() const;
  operator String*() const	{ return (String*)GetNonNullVoidPtr(); }
  operator String**() const	{ return (String**)GetNonNullVoidPtr(2); }

  void operator=(Real cp) 	{ GetStringRef() = String(cp); }
  void operator=(Int cp)	{ GetStringRef() = String(cp); }
  void operator=(const String& cp) { GetStringRef() = cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);

  cssEl* operator+(cssEl &t)
  { cssString *r = new cssString(GetStr(),""); r->val += t.GetStr(); return r; }
  cssEl* operator-(cssEl&)		{ NopErr("-"); return &cssMisc::Void; }
  cssEl* operator*()			{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl&)		{ NopErr("*"); return &cssMisc::Void; }
  cssEl* operator/(cssEl&)		{ NopErr("/"); return &cssMisc::Void; }
  cssEl* operator%(cssEl&)		{ NopErr("%"); return &cssMisc::Void; }
  cssEl* operator-()    		{ NopErr("-"); return &cssMisc::Void; }

  void operator+=(cssEl& t);

  bool operator< (cssEl& s) { return (GetStringRef() < s.GetStr()); }
  bool operator> (cssEl& s) { return (GetStringRef() > s.GetStr()); }
  bool operator! () 	    { return ((String*)GetNonNullVoidPtr())->length(); }
  bool operator<=(cssEl& s) { return (GetStringRef() <= s.GetStr()); }
  bool operator>=(cssEl& s) { return (GetStringRef() >= s.GetStr()); }
  bool operator==(cssEl& s) { return (GetStringRef() == s.GetStr()); }
  bool operator!=(cssEl& s) { return (GetStringRef() != s.GetStr()); }
  bool operator&&(cssEl& s) { return (GetStringRef().length() && (Int)s); }
  bool operator||(cssEl& s) { return (GetStringRef().length() || (Int)s); }

  // these use the TA info to perform actions
  cssEl* operator[](int idx) const;
  int	 GetMethodNo(const char*) const;
  cssEl* GetMethodFmName(const char* memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetScoped(const char* nm) const;
};


class CSS_API cssCPtr_Variant : public cssCPtr {
public:
  static Variant null_var;

  Variant&	GetVarRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_var; return *(Variant*)nnp; }

  cssTypes	GetPtrType() const;
  uint		GetSize() const 	{ return sizeof(Variant); } // use for ptrs
  const char*	GetTypeName() const  	{ return "(c_Variant)"; }
  String	PrintStr() const;

  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  // constructors
  cssCPtr_Variant() 				: cssCPtr(){};
  cssCPtr_Variant(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_Variant(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_Variant(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_Variant(const cssCPtr_Variant& cp) 		: cssCPtr(cp){};
  cssCPtr_Variant(const cssCPtr_Variant& cp, const char* nm) : cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_Variant, (void*)NULL);

  // converters
  String GetStr() const	{ return (GetVarRef()).toString(); }
  Variant GetVar() const	{ return GetVarRef(); }
  operator Real() const 	{ return GetVarRef().toDouble(); }
  operator Int() const		{ return GetVarRef().toInt(); }

  void operator=(Real cp) 	{ GetVarRef() = cp; }
  void operator=(Int cp)	{ GetVarRef() = cp; }
  void operator=(const String& cp) { GetVarRef() = cp; }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }
  void operator=(const Variant& val); 

  // operators
  void operator=(const cssEl& t);

  // todo: look at more of these..
  cssEl* operator+(cssEl &t)
    { cssVariant *r = new cssVariant(GetVarRef(),""); r->val += t.GetVar(); return r; }
  cssEl* operator-(cssEl& t)
    { cssVariant *r = new cssVariant(GetVarRef(),""); r->val -= t.GetVar(); return r; }
  cssEl* operator*()			{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssVariant *r = new cssVariant(GetVarRef(),""); r->val *= t.GetVar(); return r; }
  cssEl* operator/(cssEl& t)
  { cssVariant *r = new cssVariant(GetVarRef(),""); r->val /= t.GetVar(); return r; }
  cssEl* operator%(cssEl& t)
  { cssVariant *r = new cssVariant(GetVarRef(),""); r->val %= t.GetVar(); return r; }
  cssEl* operator-()
  { cssVariant *r = new cssVariant(-GetVarRef(),""); return r; }

  void operator+=(cssEl& t);

  bool operator< (cssEl& s) { return (GetVarRef() < s.GetVar()); }
  bool operator> (cssEl& s) { return (GetVarRef() > s.GetVar()); }
  bool operator! () 	    { return !((Variant*)GetNonNullVoidPtr())->toBool(); }
  bool operator<=(cssEl& s) { return (GetVarRef() <= s.GetVar()); }
  bool operator>=(cssEl& s) { return (GetVarRef() >= s.GetVar()); }
  bool operator==(cssEl& s) { return (GetVarRef() == s.GetVar()); }
  bool operator!=(cssEl& s) { return (GetVarRef() != s.GetVar()); }
  bool operator&&(cssEl& s) { return (GetVarRef().toBool() && (Int)s); }
  bool operator||(cssEl& s) { return (GetVarRef().toBool() || (Int)s); }

  // these use the TA info to perform actions
  cssEl* operator[](int idx) const;
  bool	 MembersDynamic()	{ return true; }
  int	 GetMemberNo(const char* memb) const { return -1; } // never do static lookup!
  cssEl* GetMemberFmNo(int memb) const;
  cssEl* GetMemberFmName(const char* memb) const;
  int	 GetMethodNo(const char*) const { return -1; }
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetMethodFmName(const char* memb) const;
  cssEl* GetScoped(const char* nm) const;
};

class CSS_API cssCPtr_DynEnum : public cssCPtr {
  // Points to a C++ dynamic enum type
public:
  static DynEnum null_enum;	// safe rval

  DynEnum&	GetEnumRef() const
  { void* nnp = GetNonNullVoidPtr(); if(nnp == NULL) return null_enum; return *(DynEnum*)nnp; }
  void 		TypeInfo(ostream& fh = cout) const;
  cssTypes	GetPtrType() const	{ return T_DynEnum; }
  uint		GetSize() const 	{ return sizeof(DynEnum); } // use for ptrs
  const char*	GetTypeName() const 	{ return "(c_DynEnum)"; }

  // constructors
  cssCPtr_DynEnum() 				: cssCPtr(){};
  cssCPtr_DynEnum(void* it, int pc) 		: cssCPtr(it,pc){};
  cssCPtr_DynEnum(void* it, int pc, const char* nm)	: cssCPtr(it,pc,nm){};
  cssCPtr_DynEnum(void* it, int pc, const char* nm, cssEl* cp, bool ro)
  : cssCPtr(it,pc,nm,cp,ro){};
  cssCPtr_DynEnum(const cssCPtr_DynEnum& cp) 		: cssCPtr(cp){};
  cssCPtr_DynEnum(const cssCPtr_DynEnum& cp, const char* nm) 	: cssCPtr(cp,nm){};

  cssCPtr_CloneFuns(cssCPtr_DynEnum, (void*)NULL);

  // converters
  String GetStr() const		{ return String(GetEnumRef().NameVal()); }
  Variant GetVar() const	{ return Variant(GetEnumRef().NumVal()); }
  operator Int() const		{ return GetEnumRef().NumVal(); }
  operator Real() const		{ return (Real)(GetEnumRef().NumVal()); }

  void operator=(Real cp) 	{ GetEnumRef().SetNumVal((int)cp); }
  void operator=(Int cp)	{ GetEnumRef().SetNumVal(cp); }
  void operator=(const String& cp);
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& t);

  cssEl* operator+(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssCPtr::operator*(); }
  cssEl* operator*(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl &t)
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt *r = new cssInt(GetEnumRef().NumVal()); r->val = -r->val; return r; }

  bool operator< (cssEl& s) 	{ return (GetEnumRef().NumVal() < (Int)s); }
  bool operator> (cssEl& s) 	{ return (GetEnumRef().NumVal() > (Int)s); }
  bool operator! () 	    	{ return ( ! GetEnumRef().NumVal()); }
  bool operator<=(cssEl& s) 	{ return (GetEnumRef().NumVal() <= (Int)s); }
  bool operator>=(cssEl& s) 	{ return (GetEnumRef().NumVal() >= (Int)s); }
  bool operator==(cssEl& s);
  bool operator!=(cssEl& s);
  bool operator&&(cssEl& s) 	{ return (GetEnumRef().NumVal() && (Int)s); }
  bool operator||(cssEl& s) 	{ return (GetEnumRef().NumVal() || (Int)s); }
};

#endif // css_ptr_i_h
