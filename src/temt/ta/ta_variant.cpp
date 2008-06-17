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

// ta_variant.cpp -- variant datatype

#include "ta_variant.h"

#include "ta_type.h"
#ifndef NO_TA_BASE
#include "ta_matrix.h"
#endif

#ifdef TA_USE_QT
#  include <QVariant>
#else
inline double qAbs(const double& t) { return t >= 0 ? t : -t; }
static bool qFuzzyCompare(double p1, double p2)
{
    return qAbs(p1 - p2) < 0.00000000001;
}
#endif

using namespace std;

/* prototype case
  switch (m_type) {
  case T_Invalid: 
  case T_Bool:
  case T_Int:
  case T_UInt:
  case T_Int64:
  case T_UInt64:
  case T_Double:
  case T_Char:
  case T_String: 
  case T_Ptr: 
  case T_Base: 
  case T_Matrix:
  default: return ;
  }
*/

ostream& operator<<(ostream& s, const Variant& x) {
  x.save(s);
  return s;
}

istream& operator>>(istream& s, Variant& x) {
  x.load(s);
  return s;
} 

const String Variant::formatNumber_impl(const Variant& val,
     short prec, bool hex) const 
{
  switch (m_type) {
  case T_Invalid: return _nilString;
//case T_Bool:
  case T_Int: return String(d.i, (hex) ? "%X" : "%d");
  case T_UInt: return String(d.u, (hex) ? "%X" : "%u");
  case T_Int64: return String(d.i64); //TODO: formats
  case T_UInt64: return String(d.u64); // TODO: formats
  case T_Double: {
    if ((prec < 1) || ( prec > 16)) prec = 5; // prob an error
    String p = String("%.").cat(String(prec)).cat("G");
    return String(d.d, p); // hex ignored
  }
//case T_Char:
//case T_String: 
//case T_Ptr: 
//case T_Base: 
//case T_Matrix:
  default: return toString();
  }
}


Variant::Variant(VarType type) {
  memset(&d, 0, sizeof(d)); // pretty much valid for most types
  m_is_null = false; // except ptr types
  switch (type) {
  case T_String: new(&d.str)String(); break;
  case T_Ptr: 
  case T_Base: 
  case T_Matrix:
    m_is_null = true; 
    break;
  default: break;
  }
  m_type = type;
}

Variant::Variant(const Variant &cp) {
  init(cp.m_type, cp.m_is_numeric, cp.m_is_numeric_valid, cp.m_is_null);
  switch (cp.m_type) {
  case T_String: new(&d.str)String(cp.getString()); break;
#ifndef NO_TA_BASE
  case T_Base:
  case T_Matrix: d.tab = NULL; taBase::SetPointer(&d.tab, cp.d.tab); break;
#endif
  default: d = cp.d; // just copy bits, valid for all other types
  }
}

#ifndef NO_TA_BASE
Variant::Variant(taBase* val) 
{
  init(T_Base);
  if (val == NULL) {
    m_is_null = true;
    d.tab = NULL;
  } else {
    m_is_null = false;
    taBase::Ref(val);
    d.tab = val;
  }
}

Variant::Variant(taMatrix* val) 
{
  init(T_Matrix);
  if (val == NULL) {
    m_is_null = true;
    d.tab = NULL;
  } else {
    m_is_null = false;
    taBase::Ref(val);
    d.tab = val;
  }
}
#endif

Variant::~Variant() { 
  releaseType();
  m_type = T_Invalid; m_is_null = true; // helps avoid hard-to-find zombie problems
}

#define cmp(a,b) (((a) < (b)) ? -1 : ((a) == (b)) ?  0 : 1)

int Variant::cmpVariant(const Variant& b) const {
  // invalid never cmpuates
  if (isInvalid() || b.isInvalid()) return -2;
  // for pointer types, proceed directly
  if (b.isPtrType())
    return cmpPtr(b.toPtr());
  // otherwise, null never cmpuates
  else if (b.isNull()) return false;
  else switch (b.type()) {
  case T_Bool: return cmpBool(b.toBool());
  case T_Int: return cmpInt(b.toInt());
  case T_UInt: return cmpUInt(b.toUInt());
  case T_Int64: return cmpInt64(b.toInt64());
  case T_UInt64: return cmpUInt64(b.toUInt64());
  case T_Double: return cmpDouble(b.toDouble());
  case T_Char: return cmpChar(b.toChar());
  case T_String:  return cmpString(b.toString());
  default: return -2; //compiler food, never happens
  }
}

int Variant::cmpBool(bool val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return cmp(d.b, val);
  case T_Int: 
  case T_UInt: 
  case T_Int64: 
  case T_UInt64:
  case T_Char: 
     return cmp(toBool(), val);
  default: return -2;
  }
}

int  Variant::cmpInt(int val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((int)d.b, val);
  case T_Int: return cmp(d.i, val);
  case T_UInt: return cmp(d.u, (uint)val);
  case T_Int64:  return cmp(d.i64, val);
  case T_UInt64: return cmp(d.u64, (uint)val);
  case T_Double: return cmp((int)d.d, val);
  case T_Char: return cmp(d.c, val);
  default: return -2;
  }
}


int  Variant::cmpUInt(uint val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((uint)d.b, val);
  case T_Int: return cmp((uint)d.i, val);
  case T_UInt: return cmp(d.u, val);
  case T_Int64:  return cmp((ta_uint64_t)d.i64, val);
  case T_UInt64: return cmp(d.u64, val);
  case T_Double: return cmp(d.d, val);
  case T_Char: return cmp((uint)d.c, val);
  default: return -2;
  }
}

int  Variant::cmpInt64(ta_int64_t val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((int)d.b, val);
  case T_Int: return cmp(d.i, val);
  case T_UInt: return cmp(d.u, (ta_uint64_t)val);
  case T_Int64:  return cmp(d.i64, val);
  case T_UInt64: return cmp(d.u64, (ta_uint64_t)val);
  case T_Double: return cmp(d.d, val);
  case T_Char: return cmp(d.c, val);
  default: return -2;
  }
}

int  Variant::cmpUInt64(ta_uint64_t val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((uint)d.b, val);
  case T_Int: return cmp((uint)d.i, val);
  case T_UInt: return cmp(d.u, val);
  case T_Int64:  return cmp((ta_uint64_t)d.i64, val);
  case T_UInt64: return cmp(d.u64, val);
  case T_Double: return cmp(d.d, val);
  case T_Char: return cmp((uint)d.c, val);
  default: return -2;
  }
}

int  Variant::cmpDouble(double val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((double)d.b, val);
  case T_Int: return cmp((double)d.i, val);
  case T_UInt: return cmp(d.u, val);
  case T_Int64:  return cmp(d.i64, val);
  case T_UInt64: return cmp(d.u64, val);
  case T_Double: return cmp(d.d, val);
  case T_Char: return cmp(d.c, val);
  default: return -2;
  }
}

int  Variant::cmpChar(char val) const {
  if (isNull()) return -2;
  switch (m_type) {
  case T_Bool: return cmp((char)d.b, val);
  case T_Int: return cmp(d.i, val);
  case T_UInt: return cmp(d.u, (uint)val);
  case T_Int64:  return cmp(d.i64, val);
  case T_UInt64: return cmp(d.u64, (uint)val);
  case T_Double: return cmp(d.d, val);
  case T_Char: return cmp(d.c, val);
  case T_String: {
    const String& str = getString();
    return cmp(str, val);
  }
  default: return -2;
  }
}

int Variant::cmpString(const String& val) const {
  if (isNull()) return -2;
  //  if (!isStringType()) return -2;
  // otherwise, compare our string rep
  const String str(toString());
  return cmp(str, val);
}

int Variant::cmpPtr(const void* val) const { // note: works for taBase/taMatrix as well
  if (!isPtrType()) return -2;
  return cmp(d.ptr, val);
}

bool Variant::Dump_Load_Type(istream& strm, int& c) {
  c = taMisc::read_word(strm, true); //note: use peek mode, so we don't read the terminator
  // if we read a proper type code, it should be an int, otherwise we encountered
  // a special streaming char like = { ; in which case it won't convert -- ditto for null
  if (!taMisc::LexBuf.isInt()) return false;
  Variant::VarType vt = (Variant::VarType)taMisc::LexBuf.toInt();
  c = taMisc::read_word(strm, true); // s/b '1' or '0'
  if (!taMisc::LexBuf.isInt()) return false;
  bool null = taMisc::LexBuf.toBool();
  ForceType(vt, null);
  return true;
}

void Variant::Dump_Save_Type(ostream& strm) const {
  strm << " " << (int)type() << " " << ((isNull()) ? '1' : '0');
} 

bool Variant::eqVariant(const Variant& b) const {
  // invalid never equates
  if (isInvalid() || b.isInvalid()) return false;
  // for pointer types, proceed directly -- we skip null test
  if (b.isPtrType())
    return eqPtr(b.toPtr());
  // otherwise, null never equates
  else if (b.isNull()) return false;
  else switch (b.type()) {
  case T_Bool: return eqBool(b.toBool());
  case T_Int: return eqInt(b.toInt());
  case T_UInt: return eqUInt(b.toUInt());
  case T_Int64: return eqInt64(b.toInt64());
  case T_UInt64: return eqUInt64(b.toUInt64());
  case T_Double: return eqDouble(b.toDouble());
  case T_Char: return eqChar(b.toChar());
  case T_String:  return eqString(b.toString());
  default: return false; //compiler food, never happens
  }
}

bool Variant::eqBool(bool val) const {
  if (isInvalid() || isNull()) return false;
  return (toBool() == val);
}

bool Variant::eqInt(int val) const {
  // handle the Ptr == or != NULL case -- only matches on NULL
  if (isPtrType())
    return ((!d.ptr) && (val == 0));
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return ((int)d.b == val);
  case T_Int: return (d.i == val);
  case T_UInt: 
    return ((val > 0) && (d.u <= (unsigned)INT_MAX) && (d.u == (unsigned)val));
  case T_Int64:  return (d.i64 == val);
  case T_UInt64: 
    return ((val > 0) && (d.u64 <= (unsigned)INT_MAX) && (d.u64 == (unsigned)val));
  case T_Double:
    return qFuzzyCompare(d.d, (double)val);
  case T_Char: return (d.c == val);
  case T_String: return (getString() == String(val));
  default: return false;
  }
}

bool  Variant::eqUInt(uint val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return ((uint)d.b == val);
  case T_Int: return ((d.i > 0) && (d.i == (int)val));
  case T_UInt:  return (d.u == val);
  case T_Int64:  return ((d.i64 > 0) && (d.i64 == (ta_int64_t)val));
  case T_UInt64: return (d.u64 == val);
  case T_Double:
    return qFuzzyCompare(d.d, (double)val);
  // sleaze a bit, but usually what we want:
  case T_Char: return ((unsigned char)d.c == val);
  case T_String: return (getString() == String(val));
  default: return false;
  }
}

bool  Variant::eqInt64(ta_int64_t val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return ((ta_int64_t)d.b == val);
  case T_Int: return (d.i == val);
  case T_UInt: 
    return ((val > 0) && (d.u == val));
  case T_Int64:  return (d.i64 == val);
  case T_UInt64: 
    return ((val > 0) && (d.u64 < (unsigned)LLONG_MAX) && (d.u64 == (unsigned)val));
  case T_Double: // note: the conversion could overflow...
    return qFuzzyCompare(d.d, (double)val);
  case T_Char: return (d.c == val);
  case T_String: return (getString() == String(val));
  default: return false;
  }
}

bool  Variant::eqUInt64(ta_uint64_t val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return ((ta_uint64_t)d.b == val);
  case T_Int: return ((d.i > 0) && (d.i == (int)val));
  case T_UInt:  return (d.u == val);
  case T_Int64:  return ((d.i64 > 0) && (d.i64 == (int)val));
  case T_UInt64: return (d.u64 == val);
  case T_Double: // the conversion could overflow
    return qFuzzyCompare(d.d, (double)val);
  // sleaze a bit, but usually what we want:
  case T_Char: return ((unsigned char)d.c == val);
  case T_String: return (getString() == String(val));
  default: return false;
  }
}

bool  Variant::eqDouble(double val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return false; // note: never really makes sense
  case T_Int: 
  case T_UInt:  
  case T_Int64:  
  case T_UInt64: 
    return (toDouble() == val);
  case T_Double:
    return qFuzzyCompare(d.d, val);
  case T_Char: return (d.c == val);
  //note: this is mostly for cases where the val will already be ex an int
  case T_String: return (getString() == String(val));
  default: return false;
  }
}

bool  Variant::eqChar(char val) const {
  // note: unsigned conversions below a bit sleazy, but usually what we want
  if (isNull()) return false;
  switch (m_type)  {
  case T_Bool: return (d.b == (bool)(val));
  case T_Int: return (d.i == val);
  case T_UInt:  return (d.u == (unsigned char)val);
  case T_Int64:  return (d.i64 == val);
  case T_UInt64: return (d.u64 == (unsigned char)val);
  case T_Double:
    return qFuzzyCompare(d.d, (double)val);
  case T_Char: return (d.c == val); 
  case T_String: {
    const String& str = getString();
    return ((str.length() == 1) && (str[0] == val));
  }
  default: return false;
  }
}

bool Variant::eqString(const String& val) const {
  if (isNull()) return false;
  // otherwise, compare our string rep
  return (toString() == val);
}

bool  Variant::eqPtr(const void* val) const { // note: works for taBase/taMatrix as well
  if (!isPtrType()) return false;
  return (d.ptr == val);
}


void Variant::ForceType(VarType vt, bool null) {
  if ((int)vt != m_type) {
    if (vt == T_String)
      setString(_nilString);
    else {
     releaseType();
     d.i64 = 0; // fine for all others
     m_type = vt;
    }
    // we need to ignore null for the ptr types, and force it true
    if ((vt >= T_Ptr) && (vt <= T_Matrix))
      m_is_null = true;
    else m_is_null = null;
  }
}

void Variant::GetRepInfo(TypeDef*& typ, void*& data) {
  data = &d; 
  switch (m_type) {
  case T_Invalid: typ = &TA_void; break;
  case T_Bool: typ = &TA_bool; break;
  case T_Int: typ = &TA_int; break;
  case T_UInt: typ = &TA_unsigned_int; break;
  case T_Int64: typ = &TA_int64_t; break;
  case T_UInt64: typ = &TA_uint64_t; break;
  case T_Double: typ = &TA_double; break;
  case T_Char: typ = &TA_char; break;
  case T_String:  typ = &TA_taString; break;
  //note: in pdp, a member variable of type "void*" return md->type = "void_ptr"
  case T_Ptr: typ = &TA_void_ptr; break; 
#ifndef NO_TA_BASE
  case T_Base:  
  case T_Matrix: {
    // if null, get the base type, else the actual type
    TypeDef* temp_typ;
    if (d.tab == NULL) {
      if (m_type == T_Base) temp_typ = &TA_taBase;
      else temp_typ = &TA_taMatrix;
    } else
      temp_typ = d.tab->GetTypeDef();
    // now, get a ptr to that type
    typ = temp_typ->GetPtrType();
  } break;
#endif
  }
}

bool Variant::isDefault() const {
  switch (m_type) {
  case T_Invalid: return true; 
  case T_Bool: return (!d.b);
  case T_Int: return (d.i == 0);
  case T_UInt: return (d.u == 0U);
  case T_Int64: return (d.i64 == 0LL);
  case T_UInt64: return (d.u64 == 0ULL);
  case T_Double: return (d.d == 0.0);
  case T_Char: return (d.c == '\0');
  case T_String: return (getString().empty()); 
  case T_Ptr: 
  case T_Base:  
  case T_Matrix: 
     return (d.ptr == 0) ;
//  default: return ;
  }
  return false;
}

bool Variant::isNull() const {
  //note: we try to keep m_is_null valid, but way safer to 
  // base this on the actual value, particularly to avoid
  // obscure issues when streaming in values, in case FixNull not called
  switch (m_type) {
  case T_Ptr: return (d.ptr == NULL);
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix: return (d.tab == NULL);
#endif
  default: return m_is_null;
  }
}

bool Variant::isNumeric() const {
  if ((m_type >= T_Int) && (m_type <= T_Double))
    return true;
  else if (m_type == T_String) {
    if (!m_is_numeric_valid) {
#ifdef TA_USE_QT
      // we check if a valid number by using QString's converter
      QString tmp(getString().chars());
      bool ok;
      tmp.toDouble(&ok);
      m_is_numeric = ok;
      m_is_numeric_valid = true;
#else
      // note: should reallysupport, but not needed for maketa
      warn("isNumeric() routine without Qt");
#endif
    }
    return m_is_numeric;
  } else return false;
}

bool Variant::isNumericStrict() const {
  return (((m_type >= T_Int) && (m_type <= T_Double)) 
    || (m_type == T_Char));
}

void Variant::load(istream& s) {
  setType(T_String);
  s >> getString();
}

Variant& Variant::operator+=(const String& rhs) {
  if (isAtomic())
    setString(toString() + rhs);
  return *this;
}

Variant& Variant::operator+=(char rhs) {
  switch (m_type) {
  case T_Int: d.i += rhs; break;
  case T_UInt: d.u += rhs; break;
  case T_Int64: d.i64 += rhs; break;
  case T_UInt64: d.u64 += rhs; break;
  case T_Double: d.d += rhs; break;
  case T_Char: d.c += rhs; break;
  case T_String: setString(getString() + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(int rhs) {
  switch (m_type) {
  case T_Int: d.i += rhs; break;
  case T_UInt: d.u += rhs; break;
  case T_Int64: d.i64 += rhs; break;
  case T_UInt64: d.u64 += rhs; break;
  case T_Double: d.d += rhs; break;
  case T_Char: setInt(d.c + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i + rhs); break;
  case T_UInt: d.u += rhs; break;
  case T_Int64: d.i64 += rhs; break;
  case T_UInt64: d.u64 += rhs; break;
  case T_Double: d.d += rhs; break;
  case T_Char: setUInt(d.c + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i + rhs); break;
  case T_UInt: setInt64(d.u + rhs); break;
  case T_Int64: d.i64 += rhs; break;
  case T_UInt64: d.u64 += rhs; break;
  case T_Double: d.d += rhs; break;
  case T_Char: setInt64(d.c + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i + rhs); break;
  case T_UInt: setUInt64(d.u + rhs); break;
  case T_Int64: setUInt64(d.i64 + rhs); break;
  case T_UInt64: d.u64 += rhs; break;
  case T_Double: d.d += rhs; break;
  case T_Char: setUInt64(d.c + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(double rhs) {
  switch (m_type) {
  case T_Int: setDouble(d.i + rhs); break;
  case T_UInt: setDouble(d.u + rhs); break;
  case T_Int64: setDouble(d.i64 + rhs); break;
  case T_UInt64: setDouble(d.u64 + rhs); break;
  case T_Double: setDouble(d.d + rhs); break;
  case T_Char: setDouble(d.c + rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator+=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator+=(rhs.d.i);
  case T_UInt: return operator+=(rhs.d.u);
  case T_Int64: return operator+=(rhs.d.i64);
  case T_UInt64: return operator+=(rhs.d.u64);
  case T_Double: return operator+=(rhs.d.d);
  case T_Char: return operator+=(rhs.d.c);
  case T_String: return operator+=(rhs.getString());
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(char rhs) {
  switch (m_type) {
  case T_Int: d.i -= rhs; break;
  case T_UInt: d.u -= rhs; break;
  case T_Int64: d.i64 -= rhs; break;
  case T_UInt64: d.u64 -= rhs; break;
  case T_Double: d.d -= rhs; break;
  case T_Char: d.c -= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(int rhs) {
  switch (m_type) {
  case T_Int: d.i -= rhs; break;
  case T_UInt: d.u -= rhs; break;
  case T_Int64: d.i64 -= rhs; break;
  case T_UInt64: d.u64 -= rhs; break;
  case T_Double: d.d -= rhs; break;
  case T_Char: setInt(d.c - rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i - rhs); break;
  case T_UInt: d.u -= rhs; break;
  case T_Int64: d.i64 -= rhs; break;
  case T_UInt64: d.u64 -= rhs; break;
  case T_Double: d.d -= rhs; break;
  case T_Char: setUInt(d.c - rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i - rhs); break;
  case T_UInt: setInt64(d.u - rhs); break;
  case T_Int64: d.i64 -= rhs; break;
  case T_UInt64: d.u64 -= rhs; break;
  case T_Double: d.d -= rhs; break;
  case T_Char: setInt64(d.c - rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i - rhs); break;
  case T_UInt: setUInt64(d.u - rhs); break;
  case T_Int64: setUInt64(d.i64 - rhs); break;
  case T_UInt64: d.u64 -= rhs; break;
  case T_Double: d.d -= rhs; break;
  case T_Char: setUInt64(d.c - rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(double rhs) {
  switch (m_type) {
  case T_Int: setDouble(d.i - rhs); break;
  case T_UInt: setDouble(d.u - rhs); break;
  case T_Int64: setDouble(d.i64 - rhs); break;
  case T_UInt64: setDouble(d.u64 - rhs); break;
  case T_Double: setDouble(d.d - rhs); break;
  case T_Char: setDouble(d.c - rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator-=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator-=(rhs.d.i);
  case T_UInt: return operator-=(rhs.d.u);
  case T_Int64: return operator-=(rhs.d.i64);
  case T_UInt64: return operator-=(rhs.d.u64);
  case T_Double: return operator-=(rhs.d.d);
  case T_Char: return operator-=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(char rhs) {
  switch (m_type) {
  case T_Int: d.i *= rhs; break;
  case T_UInt: d.u *= rhs; break;
  case T_Int64: d.i64 *= rhs; break;
  case T_UInt64: d.u64 *= rhs; break;
  case T_Double: d.d *= rhs; break;
  case T_Char: d.c *= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(int rhs) {
  switch (m_type) {
  case T_Int: d.i *= rhs; break;
  case T_UInt: d.u *= rhs; break;
  case T_Int64: d.i64 *= rhs; break;
  case T_UInt64: d.u64 *= rhs; break;
  case T_Double: d.d *= rhs; break;
  case T_Char: setInt(d.c * rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i * rhs); break;
  case T_UInt: d.u *= rhs; break;
  case T_Int64: d.i64 *= rhs; break;
  case T_UInt64: d.u64 *= rhs; break;
  case T_Double: d.d *= rhs; break;
  case T_Char: setUInt(d.c * rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i * rhs); break;
  case T_UInt: setInt64(d.u * rhs); break;
  case T_Int64: d.i64 *= rhs; break;
  case T_UInt64: d.u64 *= rhs; break;
  case T_Double: d.d *= rhs; break;
  case T_Char: setInt64(d.c * rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i * rhs); break;
  case T_UInt: setUInt64(d.u * rhs); break;
  case T_Int64: setUInt64(d.i64 * rhs); break;
  case T_UInt64: d.u64 *= rhs; break;
  case T_Double: d.d *= rhs; break;
  case T_Char: setUInt64(d.c * rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(double rhs) {
  switch (m_type) {
  case T_Int: setDouble(d.i * rhs); break;
  case T_UInt: setDouble(d.u * rhs); break;
  case T_Int64: setDouble(d.i64 * rhs); break;
  case T_UInt64: setDouble(d.u64 * rhs); break;
  case T_Double: setDouble(d.d * rhs); break;
  case T_Char: setDouble(d.c * rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator*=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator*=(rhs.d.i);
  case T_UInt: return operator*=(rhs.d.u);
  case T_Int64: return operator*=(rhs.d.i64);
  case T_UInt64: return operator*=(rhs.d.u64);
  case T_Double: return operator*=(rhs.d.d);
  case T_Char: return operator*=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(char rhs) {
  switch (m_type) {
  case T_Int: d.i /= rhs; break;
  case T_UInt: d.u /= rhs; break;
  case T_Int64: d.i64 /= rhs; break;
  case T_UInt64: d.u64 /= rhs; break;
  case T_Double: d.d /= rhs; break;
  case T_Char: d.c /= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(int rhs) {
  switch (m_type) {
  case T_Int: d.i /= rhs; break;
  case T_UInt: d.u /= rhs; break;
  case T_Int64: d.i64 /= rhs; break;
  case T_UInt64: d.u64 /= rhs; break;
  case T_Double: d.d /= rhs; break;
  case T_Char: setInt(d.c / rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i / rhs); break;
  case T_UInt: d.u /= rhs; break;
  case T_Int64: d.i64 /= rhs; break;
  case T_UInt64: d.u64 /= rhs; break;
  case T_Double: d.d /= rhs; break;
  case T_Char: setUInt(d.c / rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i / rhs); break;
  case T_UInt: setInt64(d.u / rhs); break;
  case T_Int64: d.i64 /= rhs; break;
  case T_UInt64: d.u64 /= rhs; break;
  case T_Double: d.d /= rhs; break;
  case T_Char: setInt64(d.c / rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i / rhs); break;
  case T_UInt: setUInt64(d.u / rhs); break;
  case T_Int64: setUInt64(d.i64 / rhs); break;
  case T_UInt64: d.u64 /= rhs; break;
  case T_Double: d.d /= rhs; break;
  case T_Char: setUInt64(d.c / rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(double rhs) {
  switch (m_type) {
  case T_Int: setDouble(d.i / rhs); break;
  case T_UInt: setDouble(d.u / rhs); break;
  case T_Int64: setDouble(d.i64 / rhs); break;
  case T_UInt64: setDouble(d.u64 / rhs); break;
  case T_Double: setDouble(d.d / rhs); break;
  case T_Char: setDouble(d.c / rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator/=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator/=(rhs.d.i);
  case T_UInt: return operator/=(rhs.d.u);
  case T_Int64: return operator/=(rhs.d.i64);
  case T_UInt64: return operator/=(rhs.d.u64);
  case T_Double: return operator/=(rhs.d.d);
  case T_Char: return operator/=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(char rhs) {
  switch (m_type) {
  case T_Int: d.i %= rhs; break;
  case T_UInt: d.u %= rhs; break;
  case T_Int64: d.i64 %= rhs; break;
  case T_UInt64: d.u64 %= rhs; break;
  case T_Char: d.c %= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(int rhs) {
  switch (m_type) {
  case T_Int: d.i %= rhs; break;
  case T_UInt: d.u %= rhs; break;
  case T_Int64: d.i64 %= rhs; break;
  case T_UInt64: d.u64 %= rhs; break;
  case T_Char: setInt(d.c % rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i % rhs); break;
  case T_UInt: d.u %= rhs; break;
  case T_Int64: d.i64 %= rhs; break;
  case T_UInt64: d.u64 %= rhs; break;
  case T_Char: setUInt(d.c % rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i % rhs); break;
  case T_UInt: setInt64(d.u % rhs); break;
  case T_Int64: d.i64 %= rhs; break;
  case T_UInt64: d.u64 %= rhs; break;
  case T_Char: setInt64(d.c % rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i % rhs); break;
  case T_UInt: setUInt64(d.u % rhs); break;
  case T_Int64: setUInt64(d.i64 % rhs); break;
  case T_UInt64: d.u64 %= rhs; break;
  case T_Char: setUInt64(d.c % rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator%=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator%=(rhs.d.i);
  case T_UInt: return operator%=(rhs.d.u);
  case T_Int64: return operator%=(rhs.d.i64);
  case T_UInt64: return operator%=(rhs.d.u64);
  case T_Char: return operator%=(rhs.d.c);
  default: break;
  }
  return *this;
}
Variant& Variant::operator<<=(char rhs) {
  switch (m_type) {
  case T_Int: d.i <<= rhs; break;
  case T_UInt: d.u <<= rhs; break;
  case T_Int64: d.i64 <<= rhs; break;
  case T_UInt64: d.u64 <<= rhs; break;
  case T_Char: d.c <<= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator<<=(int rhs) {
  switch (m_type) {
  case T_Int: d.i <<= rhs; break;
  case T_UInt: d.u <<= rhs; break;
  case T_Int64: d.i64 <<= rhs; break;
  case T_UInt64: d.u64 <<= rhs; break;
  case T_Char: setInt(d.c << rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator<<=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i << rhs); break;
  case T_UInt: d.u <<= rhs; break;
  case T_Int64: d.i64 <<= rhs; break;
  case T_UInt64: d.u64 <<= rhs; break;
  case T_Char: setUInt(d.c << rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator<<=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i << rhs); break;
  case T_UInt: setInt64(d.u << rhs); break;
  case T_Int64: d.i64 <<= rhs; break;
  case T_UInt64: d.u64 <<= rhs; break;
  case T_Char: setInt64(d.c << rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator<<=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i << rhs); break;
  case T_UInt: setUInt64(d.u << rhs); break;
  case T_Int64: setUInt64(d.i64 << rhs); break;
  case T_UInt64: d.u64 <<= rhs; break;
  case T_Char: setUInt64(d.c << rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator<<=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator<<=(rhs.d.i);
  case T_UInt: return operator<<=(rhs.d.u);
  case T_Int64: return operator<<=(rhs.d.i64);
  case T_UInt64: return operator<<=(rhs.d.u64);
  case T_Char: return operator<<=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(char rhs) {
  switch (m_type) {
  case T_Int: d.i >>= rhs; break;
  case T_UInt: d.u >>= rhs; break;
  case T_Int64: d.i64 >>= rhs; break;
  case T_UInt64: d.u64 >>= rhs; break;
  case T_Char: d.c >>= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(int rhs) {
  switch (m_type) {
  case T_Int: d.i >>= rhs; break;
  case T_UInt: d.u >>= rhs; break;
  case T_Int64: d.i64 >>= rhs; break;
  case T_UInt64: d.u64 >>= rhs; break;
  case T_Char: setInt(d.c >> rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i >> rhs); break;
  case T_UInt: d.u >>= rhs; break;
  case T_Int64: d.i64 >>= rhs; break;
  case T_UInt64: d.u64 >>= rhs; break;
  case T_Char: setUInt(d.c >> rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i >> rhs); break;
  case T_UInt: setInt64(d.u >> rhs); break;
  case T_Int64: d.i64 >>= rhs; break;
  case T_UInt64: d.u64 >>= rhs; break;
  case T_Char: setInt64(d.c >> rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i >> rhs); break;
  case T_UInt: setUInt64(d.u >> rhs); break;
  case T_Int64: setUInt64(d.i64 >> rhs); break;
  case T_UInt64: d.u64 >>= rhs; break;
  case T_Char: setUInt64(d.c >> rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator>>=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator>>=(rhs.d.i);
  case T_UInt: return operator>>=(rhs.d.u);
  case T_Int64: return operator>>=(rhs.d.i64);
  case T_UInt64: return operator>>=(rhs.d.u64);
  case T_Char: return operator>>=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(char rhs) {
  switch (m_type) {
  case T_Int: d.i &= rhs; break;
  case T_UInt: d.u &= rhs; break;
  case T_Int64: d.i64 &= rhs; break;
  case T_UInt64: d.u64 &= rhs; break;
  case T_Char: d.c &= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(int rhs) {
  switch (m_type) {
  case T_Int: d.i &= rhs; break;
  case T_UInt: d.u &= rhs; break;
  case T_Int64: d.i64 &= rhs; break;
  case T_UInt64: d.u64 &= rhs; break;
  case T_Char: setInt(d.c & rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i & rhs); break;
  case T_UInt: d.u &= rhs; break;
  case T_Int64: d.i64 &= rhs; break;
  case T_UInt64: d.u64 &= rhs; break;
  case T_Char: setUInt(d.c & rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i & rhs); break;
  case T_UInt: setInt64(d.u & rhs); break;
  case T_Int64: d.i64 &= rhs; break;
  case T_UInt64: d.u64 &= rhs; break;
  case T_Char: setInt64(d.c & rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i & rhs); break;
  case T_UInt: setUInt64(d.u & rhs); break;
  case T_Int64: setUInt64(d.i64 & rhs); break;
  case T_UInt64: d.u64 &= rhs; break;
  case T_Char: setUInt64(d.c & rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator&=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator&=(rhs.d.i);
  case T_UInt: return operator&=(rhs.d.u);
  case T_Int64: return operator&=(rhs.d.i64);
  case T_UInt64: return operator&=(rhs.d.u64);
  case T_Char: return operator&=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(char rhs) {
  switch (m_type) {
  case T_Int: d.i |= rhs; break;
  case T_UInt: d.u |= rhs; break;
  case T_Int64: d.i64 |= rhs; break;
  case T_UInt64: d.u64 |= rhs; break;
  case T_Char: d.c |= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(int rhs) {
  switch (m_type) {
  case T_Int: d.i |= rhs; break;
  case T_UInt: d.u |= rhs; break;
  case T_Int64: d.i64 |= rhs; break;
  case T_UInt64: d.u64 |= rhs; break;
  case T_Char: setInt(d.c | rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i | rhs); break;
  case T_UInt: d.u |= rhs; break;
  case T_Int64: d.i64 |= rhs; break;
  case T_UInt64: d.u64 |= rhs; break;
  case T_Char: setUInt(d.c | rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i | rhs); break;
  case T_UInt: setInt64(d.u | rhs); break;
  case T_Int64: d.i64 |= rhs; break;
  case T_UInt64: d.u64 |= rhs; break;
  case T_Char: setInt64(d.c | rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i | rhs); break;
  case T_UInt: setUInt64(d.u | rhs); break;
  case T_Int64: setUInt64(d.i64 | rhs); break;
  case T_UInt64: d.u64 |= rhs; break;
  case T_Char: setUInt64(d.c | rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator|=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator|=(rhs.d.i);
  case T_UInt: return operator|=(rhs.d.u);
  case T_Int64: return operator|=(rhs.d.i64);
  case T_UInt64: return operator|=(rhs.d.u64);
  case T_Char: return operator|=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(char rhs) {
  switch (m_type) {
  case T_Int: d.i ^= rhs; break;
  case T_UInt: d.u ^= rhs; break;
  case T_Int64: d.i64 ^= rhs; break;
  case T_UInt64: d.u64 ^= rhs; break;
  case T_Char: d.c ^= rhs; break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(int rhs) {
  switch (m_type) {
  case T_Int: d.i ^= rhs; break;
  case T_UInt: d.u ^= rhs; break;
  case T_Int64: d.i64 ^= rhs; break;
  case T_UInt64: d.u64 ^= rhs; break;
  case T_Char: setInt(d.c ^ rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(uint rhs) {
  switch (m_type) {
  case T_Int: setUInt(d.i ^ rhs); break;
  case T_UInt: d.u ^= rhs; break;
  case T_Int64: d.i64 ^= rhs; break;
  case T_UInt64: d.u64 ^= rhs; break;
  case T_Char: setUInt(d.c ^ rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(ta_int64_t rhs) {
  switch (m_type) {
  case T_Int: setInt64(d.i ^ rhs); break;
  case T_UInt: setInt64(d.u ^ rhs); break;
  case T_Int64: d.i64 ^= rhs; break;
  case T_UInt64: d.u64 ^= rhs; break;
  case T_Char: setInt64(d.c ^ rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(ta_uint64_t rhs) {
  switch (m_type) {
  case T_Int: setUInt64(d.i ^ rhs); break;
  case T_UInt: setUInt64(d.u ^ rhs); break;
  case T_Int64: setUInt64(d.i64 ^ rhs); break;
  case T_UInt64: d.u64 ^= rhs; break;
  case T_Char: setUInt64(d.c ^ rhs); break;
  default: break;
  }
  return *this;
}

Variant& Variant::operator^=(const Variant& rhs) {
  switch (rhs.type()) {
  case T_Int: return operator^=(rhs.d.i);
  case T_UInt: return operator^=(rhs.d.u);
  case T_Int64: return operator^=(rhs.d.i64);
  case T_UInt64: return operator^=(rhs.d.u64);
  case T_Char: return operator^=(rhs.d.c);
  default: break;
  }
  return *this;
}

Variant& Variant::operator++() {
  switch (m_type) {
  case T_Int: ++(d.i); break;
  case T_UInt: ++(d.u); break;
  case T_Int64: ++(d.i64); break;
  case T_UInt64: ++(d.u64); break;
  case T_Double: d.d += 1.0; break;
  case T_Char: ++(d.c); break;
  default: break;
  }
  return *this;
}

Variant Variant::operator++(int) {
  Variant rval(*this);
  operator++();
  return rval;
}

Variant& Variant::operator--() {
  switch (m_type) {
  case T_Int: --(d.i); break;
  case T_UInt: --(d.u); break;
  case T_Int64: --(d.i64); break;
  case T_UInt64: --(d.u64); break;
  case T_Double: d.d -= 1.0; break;
  case T_Char: --(d.c); break;
  default: warn("-- operator"); break;
  }
  return *this;
}

Variant Variant::operator--(int) {
  Variant rval(*this);
  operator--();
  return rval;
}

Variant& Variant::operator-() {
  switch (m_type) {
  case T_Invalid: break;
  case T_Bool: d.b = -(int)d.b; break; // makes no sense, but compiler allows
  case T_Int: d.i = -d.i; break;
  case T_UInt: d.u = (uint)-((int)d.u); break; // forces unsigned
  case T_Int64: d.i64 = -d.i64; break;
  case T_UInt64: d.u64 = (ta_uint64_t)-((ta_int64_t)d.u64); break; // forces unsigned
  case T_Double: d.d = -d.d; break;
  case T_Char: d.c = -d.c; break;
  default: warn("unary - operator"); break ;
  }
  return *this;
}

Variant& Variant::operator~() {
  switch (m_type) {
  case T_Invalid: break;
  case T_Bool: d.b = ~(int)d.b; break; // makes no sense, but compiler allows
  case T_Int: d.i = ~d.i; break;
  case T_UInt: d.u = ~d.u; break; 
  case T_Int64: d.i64 = ~d.i64; break;
  case T_UInt64: d.u64 = ~d.u64; break;
  case T_Char: d.c = -d.c; break;
  default: warn("~ operator"); break ;
  }
  return *this;
}

void Variant::releaseType() {
  // undo specials
  switch (m_type) {
  case T_String: getString().~String(); break;
#ifndef NO_TA_BASE
  case T_Base:
  case T_Matrix: taBase::DelPointer(&d.tab); break;
#endif
	default: break; // compiler food
  }
}


void Variant::save(ostream& s) const {
  s << (int)type();
  
  switch (m_type) {
  case T_Invalid:
    break; 
  case T_Bool:
    s << d.b;
    break;
  case T_Int:
    s << d.i;
    break;
  case T_UInt:
    s << d.u;
    break;
  case T_Int64:
    s << d.i64;
    break;
  case T_UInt64:
    s << d.u64;
    break;
  case T_Double:
    s << d.d;
    break;
  case T_Char:
    s << d.c;
    break;
  case T_String:
    s << getString(); 
    break;
  case T_Ptr: 
    s << toString(); //NOIE: cannot be streamed back in!!!
    break;
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix:
    if (d.tab == NULL) {
     s << "TA_void"; // indicates NULL
    } else {
      s << d.tab->GetTypeDef()->name;
      d.tab->Save_strm(s);
    }
    break;
#endif
  default: break ;
  }
}

void Variant::setInvalid() {
  releaseType();
  d.i64 = 0LL; // valid for all other types
  m_type = T_Invalid;
  m_is_null = true;
}

void Variant::setVariant(const Variant &cp) {
  switch (cp.m_type) {
  case T_String: setString(cp.getString()); break;
#ifndef NO_TA_BASE
  case T_Base: setBase(cp.d.tab); break;
  case T_Matrix: setMatrix(cp.getMatrix()); break;
#endif
	default: 
    releaseType();
    d = cp.d; // just copy bits, valid for all other types
    m_type = cp.m_type;
    break;
  }
  m_is_null = cp.m_is_null;
}


void Variant::setVariantData(const Variant& cp) {
  switch (m_type) {
  case T_Invalid:
    break; 
  case T_Bool:
    setBool(cp.toBool());
    break;
  case T_Int:
    setInt(cp.toInt());
    break;
  case T_UInt:
    setUInt(cp.toUInt());
    break;
  case T_Int64:
    setInt64(cp.toInt64());
    break;
  case T_UInt64:
    setUInt64(cp.toUInt64());
    break;
  case T_Double:
    setDouble(cp.toDouble());
    break;
  case T_Char:
    setChar(cp.toChar());
    break;
  case T_String:
    setString(cp.toString());
    break;
  case T_Ptr: 
    setPtr(cp.toPtr());
    break;
#ifndef NO_TA_BASE
  case T_Base: 
    setBase(cp.toBase());
    break;
  case T_Matrix:
    setMatrix(cp.toMatrix());
    break;
#endif
  default: break ;
  }
}

void Variant::setBool(bool val, bool null) {
  releaseType();
  d.b = val;
  m_type = T_Bool;
  m_is_null = null;
}

void Variant::setInt(int val, bool null) {
  releaseType();
  d.i = val;
  m_type = T_Int;
  m_is_null = null;
}

void Variant::setUInt(uint val, bool null) {
  releaseType();
  d.u = val;
  m_type = T_UInt;
  m_is_null = null;
}

void Variant::setInt64(ta_int64_t val, bool null) {
  releaseType();
  d.i64 = val;
  m_type = T_Int64;
  m_is_null = null;
}

void Variant::setUInt64(ta_uint64_t val, bool null) {
  releaseType();
  d.u64 = val;
  m_type = T_UInt64;
  m_is_null = null;
}

void Variant::setFloat(float val, bool null) {
  releaseType();
  d.d = val;
  m_type = T_Double;
  m_is_null = null;
}

void Variant::setDouble(double val, bool null) {
  releaseType();
  d.d = val;
  m_type = T_Double;
  m_is_null = null;
}

void Variant::setChar(char val, bool null) {
  releaseType();
  d.c = val;
  m_type = T_Char;
  m_is_null = null;
}

void Variant::setPtr(void* val) {
  releaseType();
  d.ptr = val;
  m_type = T_Ptr;
  m_is_null = (val == NULL);
}

#ifndef NO_TA_BASE
void Variant::setBase(taBase* cp) {
  if (m_type == T_Base)
    taBase::SetPointer(&d.tab, cp);
  else {
    // we should ref cp first, to avoid obscure self-assign deletions (ex matrix to base, but the same)
    if (cp != NULL) taBase::Ref(cp);
    releaseType();
    d.tab = cp;
    m_type = T_Base;
  }
  m_is_null = (cp == NULL);
}

void Variant::setMatrix(taMatrix* cp) {
  if (m_type == T_Matrix)
    taBase::SetPointer(&d.tab, cp);
  else {
    // we should ref cp first, to avoid obscure self-assign deletions (ex matrix to base, but the same)
    if (cp != NULL) taBase::Ref(cp);
    releaseType();
    d.tab = cp;
    m_type = T_Matrix;
  }
  m_is_null = (cp == NULL);
}
#endif

void Variant::setString(const String& val, bool null) {
  if (m_type == T_String)
    getString() = val;
  else {
    releaseType();
    new(&d.str)String(val);
    m_type = T_String;
  }
  m_is_null = null;
  m_is_numeric_valid = false;
}

void Variant::setType(VarType value) {
  if (m_type == value) return;
  releaseType();
  switch (value) {
  case T_Invalid: setInvalid(); break;
  case T_Bool: setBool(false); break;
  case T_Int: setInt(0); break;
  case T_UInt:  setUInt(0U); break;
  case T_Int64:  setInt64(0LL); break;
  case T_UInt64: setUInt64(0ULL); break;
  case T_Double: setDouble(0.0); break;
  case T_Char: setChar('\0'); break;
  case T_String:  setString(""); break;
  case T_Ptr:  setPtr(NULL); break;
#ifndef NO_TA_BASE
  case T_Base:  
  case T_Matrix: setBase(NULL); break;
#endif
  default: return ;
  }
}

static const char* var_types_as_str[] = {
  "T_Invalid", "T_Bool", "T_Int", "T_UInt", "T_Int64", "T_UInt64", "T_Double",
  "T_Char", "T_na", "T_String", "T_Ptr", "T_Base", "T_Matrix"};

String Variant::getTypeAsString() const {
  return var_types_as_str[m_type];
}

bool Variant::toBool() const {
  switch (m_type) {
  case T_Invalid: 
    return false;
  case T_Bool:
    return d.b;
  case T_Int:
    return (d.i != 0);
  case T_UInt:
    return (d.u != 0);
  case T_Int64:
    return (d.i64 != 0);
  case T_UInt64:
    return (d.u64 != 0);
  case T_Double:
    return (d.d != 0.0);
  case T_Char:
    return (d.c != 0);
  case T_String: {
    const String& str = getString();
    if (str.empty()) return false;
    else {
      char c = str[0];
      return ((c == 't') || (c == 'T') || (c == '1'));
    }
    } break;
  case T_Ptr: 
    return (d.ptr != NULL);
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix:
    return (d.tab != NULL);
#endif
	default: break;
  }
  return false;
}

const String Variant::toCssLiteral() const {
  String rval;
  switch (type()) {
  case T_Invalid: rval = "_nilVariant"; break;
  case T_Bool: rval = toString(); break;
  case T_Int: rval = toString(); break;
  case T_UInt:
    rval = toString(); 
    rval += "U";
    break;
  case T_Int64:
    rval = toString(); 
    rval += "LL";
    break;
  case T_UInt64:
    rval = toString(); 
    rval += "ULL";
    break;
  case T_Double: rval = toString(); break;
  case T_Char:
    rval = String::CharToCppLiteral(d.c);
    break;
  case T_String: 
    rval = String::StringToCppLiteral(getString());
    break;
  case T_Ptr: 
    if (isNull()) {
      rval += "NULL";
    } else {
      warn("toCssLiteral() on a non-null raw pointer");
      // todo, maybe should emit code breaking literal
    }  break; 
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix:
    if (isNull()) {
      rval += "NULL";
    } else {
      rval += d.tab->GetPath();
    }
#endif
  default: break ;
  }
  return rval;
}

int Variant::toInt() const {
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return (d.b) ? 1 : 0;
  case T_Int:
    return d.i;
  case T_UInt:
    return (int)d.u;
  case T_Int64:
    return (int)d.i64;
  case T_UInt64:
    return (int)d.u64;
  case T_Double:
    return (int)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toInt();
  case T_Ptr: // note: not a word-size-safe conversion
    break;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0;
}

uint Variant::toUInt() const {
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return (d.b) ? 1 : 0;
  case T_Int:
    return (uint)d.i;
  case T_UInt:
    return d.u;
  case T_Int64:
    return (uint)d.i64;
  case T_UInt64:
    return (uint)d.u64;
  case T_Double:
    return (uint)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toUInt();
  case T_Ptr: // note: not a word-size-safe conversion
    break;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0;
}

ta_int64_t Variant::toInt64() const {
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return (d.b) ? 1 : 0;
  case T_Int:
    return d.i;
  case T_UInt:
    return d.u;
  case T_Int64:
    return d.i64;
  case T_UInt64:
    return (ta_int64_t)d.u64;
  case T_Double:
    return (ta_int64_t)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toInt64();
  case T_Ptr: 
    return (ta_int64_t)d.ptr;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0;
}

ta_uint64_t Variant::toUInt64() const {
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return (d.b) ? 1 : 0;
  case T_Int:
    return d.i;
  case T_UInt:
    return d.u;
  case T_Int64:
    return d.i64;
  case T_UInt64:
    return d.u64;
  case T_Double:
    return (ta_uint64_t)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toUInt64();
  case T_Ptr: 
    return (ta_uint64_t)d.ptr;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0;
} 

double Variant::toDouble() const {
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return (d.b) ? 1.0 : 0.0;
  case T_Int:
    return (double)d.i;
  case T_UInt:
    return (double)d.u;
  case T_Int64:
    return (double)d.i64;
  case T_UInt64:
    return (double)d.u64;
  case T_Double:
    return d.d;
  case T_Char:
    return (double)d.c;
  case T_String: //note: may fail, if so, 0.0
    return getString().toDouble();
  case T_Ptr: 
    return (double)(intptr_t)d.ptr;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0.0;
}

char Variant::toChar() const {
  //note: we sort of follow Qt here
  switch (m_type) {
  case T_Invalid: 
    break;
  case T_Bool:
    if (d.b) return '1'; else return '0';
  case T_Int:
    return (char)d.i;
  case T_UInt:
    return (char)d.u;
  case T_Int64:
    break ;
  case T_UInt64:
    break ;
  case T_Double:
    break ;
  case T_Char:
    return d.c;
  case T_String: {
    const String& str = getString();
    if (str.length() >= 1) 
      return str.elem(0);
    } break;
  case T_Ptr: 
    break ;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return '\0';
}

void* Variant::toPtr() const {
  switch (m_type) {
  case T_Invalid: 
    return NULL;
  case T_Bool:
    return NULL;
  case T_Int:
    return NULL;
  case T_UInt:
    return NULL;
  case T_Int64:
    return NULL;
  case T_UInt64:
    return NULL;
  case T_Double:
    return NULL;
  case T_Char:
    return NULL;
  case T_String: 
    return NULL;
  case T_Ptr: 
    return d.ptr;
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix:
    return d.tab;
#endif
	default: break ;
  }
  return NULL;
}
 
String Variant::toString() const {
  switch (m_type) {
  case T_Invalid: 
    return _nilString;
  case T_Bool:
    if (d.b) return String("true"); 
    else     return String("false");
  case T_Int:
    return String(d.i);
  case T_UInt:
    return String(d.u);
  case T_Int64:
    return String(d.i64);
  case T_UInt64:
    return String(d.u64);
  case T_Double:
    return String(d.d);
  case T_Char:
    return String(d.c);
  case T_String: 
    return getString();
  case T_Ptr: 
    return String(d.ptr); // renders as hex
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix:
    return taBase::GetStringRep(d.tab);
#endif
  default: break ;
  }
  return _nilString;
}

void Variant::UpdateAfterLoad() {
  switch (m_type) {
  case T_Ptr: m_is_null = (d.ptr == NULL); break;
#ifndef NO_TA_BASE
  case T_Base: 
  case T_Matrix: m_is_null = (d.tab == NULL); break;
#endif
  default: break ;
  }
}

void Variant::updateFromString(const String& val) {
  switch (m_type) {
  case T_Invalid: break; // ignored 
  case T_Bool: d.b = val.toBool(); break;
  case T_Int: d.i = val.toInt(); break;
  case T_UInt: d.u = val.toUInt(); break;
  case T_Int64: d.i64 = val.toInt64(); break;
  case T_UInt64: d.u64 = val.toUInt64(); break;
  case T_Double: d.d = val.toDouble(); break;
  case T_Char: d.c = val.toChar(); break;
  case T_String: getString() = val; break;
  case T_Ptr:
    if ((val == "NULL") || (val == "(NULL)")) d.ptr = NULL;
    else warn("updateFromString() setting ptr to other than NULL");
    break; 
#ifndef NO_TA_BASE
 // TODO: should look up from path
  case T_Base: 
  case T_Matrix: {
    warn("updateFromString() setting T_Base");
  }
#endif
  default: break ;
  }
}

void Variant::warn(const char* msg) const {
  cerr << "** Warning: operation not supported on Variant type " << type() << ": " << msg << "\n";
}

#ifndef NO_TA_BASE
taBase* Variant::toBase() const {
  switch (m_type) {
  case T_Invalid: 
    return NULL;
  case T_Bool:
    return NULL;
  case T_Int:
    return NULL;
  case T_UInt:
    return NULL;
  case T_Int64:
    return NULL;
  case T_UInt64:
    return NULL;
  case T_Double:
    return NULL;
  case T_Char:
    return NULL;
  case T_String: 
    return NULL;
  case T_Ptr: 
    return NULL;
#ifndef NO_TA_BASE
  case T_Base:
  case T_Matrix:
    return d.tab;
#endif
	default: break ;
  }
  return NULL;
} 

taMatrix* Variant::toMatrix() const {
  switch (m_type) {
  case T_Invalid: 
    return NULL;
  case T_Bool:
    return NULL;
  case T_Int:
    return NULL;
  case T_UInt:
    return NULL;
  case T_Int64:
    return NULL;
  case T_UInt64:
    return NULL;
  case T_Double:
    return NULL;
  case T_Char:
    return NULL;
  case T_String: 
    return NULL;
  case T_Ptr: 
    return NULL;
  case T_Base:
    if ((d.tab != NULL) && (d.tab->GetTypeDef()->InheritsFrom(TA_taMatrix)))
      return getMatrix();
    else return NULL;
  case T_Matrix:
    return getMatrix();
  default: break ;
  }
  return NULL;
} 
#endif 

#ifdef TA_USE_QT
Variant::Variant(const QVariant &val) {
  init();
  setQVariant(val);
}

Variant::Variant(const QString& val) {
  init();
  setCString(val.toLatin1());
}


void Variant::setQVariant(const QVariant& cp) {
  switch (cp.userType()) {
  case QVariant::Invalid: 
    setInvalid(); break;
  case QVariant::Bool:
    setBool(cp.toBool(), cp.isNull()); break; 
  case QVariant::Int:
    setInt(cp.toInt(), cp.isNull()); break;
  case QVariant::UInt:
    setUInt(cp.toUInt(), cp.isNull()); break;
  case QVariant::LongLong:
    setInt64(cp.toLongLong(), cp.isNull()); break;
  case QVariant::ULongLong:
    setUInt64(cp.toULongLong(), cp.isNull()); break;
  case QVariant::Double:
    setDouble(cp.toDouble(), cp.isNull()); break;
  case QVariant::Char:
    setChar(cp.toChar().toLatin1(), cp.isNull()); break;
  case QVariant::String: 
    setString(cp.toString(), cp.isNull()); break;
  default:
#ifdef DEBUG
  taMisc::Warning("Attempt to set Variant from QVariant failed, can't handle QVariant::Type: ",
    String(cp.userType()) );
#endif
    setInvalid(); break;
  }

}

QVariant Variant::toQVariant() const {
  switch (m_type) {
  case T_Invalid: 
    return QVariant();
  case T_Bool:
    return QVariant(d.b); 
  case T_Int:
    return QVariant(d.i);
  case T_UInt:
    return QVariant(d.u);
  case T_Int64:
    return QVariant(d.i64);
  case T_UInt64:
    return QVariant(d.u64);
  case T_Double:
    return QVariant(d.d);
  case T_Char:
    return QVariant(d.c);
  case T_String: 
    return QVariant(getString().chars());
/* others are invalid
  case T_Ptr: 
    return QVariant(d.ptr); // renders as hex
#ifndef NO_TA_BASE
//TODO:  maybe stream the data, or copy the path 
  case T_Base: 
  case T_Matrix:
    return taBase::GetStringRep(d.tab);
    
#endif
*/
  default: 
#ifdef DEBUG
  taMisc::Warning("Attempt to set QVariant from Variant failed, can't handle Variant::Type: ",
    String(m_type) );
#endif
    break ;
  }
  return QVariant();
}
#endif 

//////////////////////////////////
//  NameVar			//
//////////////////////////////////

bool NameVar::Parse(const String& raw, String& name, String& val) {
  String namet = trim(raw.before('='));
  if (namet.length() == 0) return false;
  name = namet;
  val = trim(raw.after('='));
  return true;
}

void NameVar::SetFmStr(const String& val) {
  String valt;
  if (Parse(val, name, valt)) {
    if(value.type() == Variant::T_Invalid) {
      value = valt;		// sets as string
    }
    else {
      value.updateFromString(valt);
    }
  }
  // else error!  todo.
}

