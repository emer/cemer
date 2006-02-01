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

// ta_variant.cpp -- variant datatype

#include "ta_variant.h"

#include "ta_matrix.h"

using namespace std;

ostream& operator<<(ostream& s, const Variant& x) {
  x.save(s);
  return s;
}

istream& operator>>(istream& s, Variant& x) {
  x.load(s);
  return s;
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

Variant::Variant(const Variant &cp)
  :m_type(cp.m_type), m_is_null(cp.m_is_null)
{
  switch (cp.m_type) {
  case T_String: new(&d.str)String(cp.getString()); break;
  case T_Base:
  case T_Matrix: d.tab = NULL; taBase::SetPointer(&d.tab, cp.d.tab); break;
  default: d = cp.d; // just copy bits, valid for all other types
  }
}

Variant::Variant(taBase* val) 
:m_type(T_Base)
{
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
:m_type(T_Matrix)
{
  if (val == NULL) {
    m_is_null = true;
    d.tab = NULL;
  } else {
    m_is_null = false;
    taBase::Ref(val);
    d.tab = val;
  }
}

Variant::~Variant() { 
  releaseType();
  m_type = T_Invalid; m_is_null = true; // helps avoid hard-to-find zombie problems
}
/*
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

void Variant::Dump_Save_Type(ostream& strm) {
  strm << " " << (int)type() << " " << (isNull()) ? '1' : '0';
} 

bool Variant::eqVariant(const Variant& b) const {
  // invalid never equates
  if (isInvalid() || b.isInvalid()) return false;
  // for pointer types, proceed directly
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

bool  Variant::eqBool(bool val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return (d.b == val);
  case T_Int: 
  case T_UInt: 
  case T_Int64: 
  case T_UInt64: 
    return (toBool() == val);
  default: return false;
  }
}

bool  Variant::eqInt(int val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return (d.b == val);
  case T_Int: return (d.i == val);
  case T_UInt: 
    return ((val > 0) && (d.u <= INT_MAX) && (d.u == val));
  case T_Int64:  return (d.i64 == val);
  case T_UInt64: 
    return ((val > 0) && (d.u64 <= INT_MAX) && (d.u64 == val));
  default: return false;
  }
}

bool  Variant::eqUInt(uint val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return (d.b == val);
  case T_Int: return ((d.i > 0) && (d.i == val));
  case T_UInt:  return (d.u == val);
  case T_Int64:  return ((d.i64 > 0) && (d.i64 == val));
  case T_UInt64: return (d.u64 == val);
  default: return false;
  }
}

bool  Variant::eqInt64(int64_t val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return (d.b == val);
  case T_Int: return (d.i == val);
  case T_UInt: 
    return ((val > 0) && (d.u == val));
  case T_Int64:  return (d.i64 == val);
  case T_UInt64: 
    return ((val > 0) && (d.u64 < LLONG_MAX) && (d.u64 == val));
  default: return false;
  }
}

bool  Variant::eqUInt64(uint64_t val) const {
  if (isNull()) return false;
  switch (m_type) {
  case T_Bool: return (d.b == val);
  case T_Int: return ((d.i > 0) && (d.i == val));
  case T_UInt:  return (d.u == val);
  case T_Int64:  return ((d.i64 > 0) && (d.i64 == val));
  case T_UInt64: return (d.u64 == val);
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
    return d.d == val;
  default: return false;
  }
}

bool  Variant::eqChar(char val) const {
  if (isNull()) return false;
  switch (m_type)  {
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
  if (!isPtrType()) return false;
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
  }
}

void Variant::UpdateAfterLoad() {
  switch (m_type) {
  case T_Ptr: m_is_null = (d.ptr == NULL); break;
  case T_Base: 
  case T_Matrix: m_is_null = (d.tab == NULL); break;
  default: break ;
  }
}

bool Variant::isNull() const {
  //note: we try to keep m_is_null valid, but way safer to 
  // base this on the actual value, particularly to avoid
  // obscure issues when streaming in values, in case FixNull not called
  switch (m_type) {
  case T_Ptr: return (d.ptr == NULL);
  case T_Base: 
  case T_Matrix: return (d.tab == NULL);
  default: return m_is_null;
  }
}

void Variant::load(istream& s) {
  uint t;
  s >> t;
  if (t > T_MaxType) {
    taMisc::Error("unrecognized Variant type code in istream: ", String(t));
    return;
  }
  
  // complex types are special, otherwise simple types are handled similarly
  if (t == T_String) {
      String str;
      s >> str; 
      setString(str);
  } else if ((t == T_Base) || (t == T_Matrix)) { // handled almost the same
    taString typ_name;
    s >> typ_name; // 
    if (typ_name == "TA_void") {
      if (t == T_Base) setBase(NULL);
      else setMatrix(NULL);
    } else {
      // get the type name
      TypeDef* typ = taMisc::types.FindName(typ_name);
      if (typ == NULL) {
        taMisc::Error("While loading Variant, TypeDef not found:", typ_name);
        return;
      }
      // make a token of that type, and load it in
      taBase* ta = taBase::MakeToken(typ);
      if (ta == NULL) {
        taMisc::Error("While loading Variant could not make token of type:", typ_name);
        return;
      }
      ta->Load(s);
      //note: we set value according to actual type, just to be absolutely safe
      if (ta->InheritsFrom(&TA_taMatrix))
        setMatrix((taMatrix*)ta);
      else setBase(ta);
    }
  } else {
    releaseType();
    m_type = t;
    m_is_null = false; // except pointer
    switch (t) {
    case T_Invalid:
      break; 
    case T_Bool:
      s >> d.b;
      break;
    case T_Int:
      s >> d.i;
      break;
    case T_UInt:
      s >> d.u;
      break;
    case T_Int64:
      s >> d.i64;
      break;
    case T_UInt64:
      s >> d.u64;
      break;
    case T_Double:
      s >> d.d;
      break;
    case T_Char:
      s >> d.c;
      break;
    case T_Ptr: {
      String str;
      s >> str; // we ignore this!
      d.ptr = NULL;
      m_is_null = true;
      } break;
    default: break ;
    }
  }
}




void Variant::releaseType() {
  // undo specials
  switch (m_type) {
  case T_String: getString().~String(); break;
  case T_Base:
  case T_Matrix: taBase::DelPointer(&d.tab); break;
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
  case T_Base: 
  case T_Matrix:
    if (d.tab == NULL) {
     s << "TA_void"; // indicates NULL
    } else {
      s << d.tab->GetTypeDef()->name;
      d.tab->Save(s);
    }
    break;
  default: break ;
  }
}

void Variant::setVariant(const Variant &cp) {
  switch (cp.m_type) {
  case T_String: setString(cp.getString()); break;
  case T_Base: setBase(cp.d.tab); break;
  case T_Matrix: setMatrix(cp.getMatrix()); break;
  default: 
    releaseType();
    d = cp.d; // just copy bits, valid for all other types
    m_type = cp.m_type;
    break;
  }
  m_is_null = cp.m_is_null;
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

void Variant::setInt64(int64_t val, bool null) {
  releaseType();
  d.i64 = val;
  m_type = T_Int64;
  m_is_null = null;
}

void Variant::setUInt64(uint64_t val, bool null) {
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

void Variant::setString(const String& val, bool null) {
  if (m_type == T_String)
    getString() = val;
  else {
    releaseType();
    new(&d.str)String(val);
    m_type = T_String;
  }
  m_is_null = null;
}
/*
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
  
  switch (m_type) {
  case T_Invalid: 
    break ;
  case T_Bool:
    return ;
  case T_Int:
    return ;
  case T_UInt:
    return ;
  case T_Int64:
    return ;
  case T_UInt64:
    return ;
  case T_Double:
    return ;
  case T_Char:
    return ;
  case T_String: 
    return ;
  case T_Ptr: 
    return ;
  case T_Base: 
  case T_Matrix:
    return ;
  default: break ;
  }
  return ;
  
*/
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
  case T_Base: 
  case T_Matrix:
    return (d.tab != NULL);
  default: break;
  }
  return false;
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

int64_t Variant::toInt64() const {
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
    return (int64_t)d.u64;
  case T_Double:
    return (int64_t)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toInt64();
  case T_Ptr: 
    return (int64_t)d.ptr;
  case T_Base: 
  case T_Matrix:
    break ;
  default: break ;
  }
  return 0;
}

uint64_t Variant::toUInt64() const {
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
    return (uint64_t)d.d;
  case T_Char:
    return d.c;
  case T_String: 
    return getString().toUInt64();
  case T_Ptr: 
    return (uint64_t)d.ptr;
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
    if ((str.length() == 1))
      return str[0];
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
  case T_Base: 
  case T_Matrix:
    return d.tab;
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
  case T_Base: 
  case T_Matrix:
    return taBase::GetStringRep(d.tab);
  default: break ;
  }
  return _nilString;
}

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
  case T_Base:
    return d.tab; 
  case T_Matrix:
    return d.tab;
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
