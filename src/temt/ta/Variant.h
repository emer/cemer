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

#ifndef Variant_h
#define Variant_h 1

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class ostream; // 
class istream; // 
class taBase; // 
class taMatrix; // 
class TypeItem; // 
class EnumDef; // 
class TypeDef; // 
class MemberDefBase; // 
class MemberDef; // 
class PropertyDef; // 
class MethodDef; // 


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
    T_Matrix = 12, 	// #LABEL_taMatrix taMatrix ref counted
    
    T_TypeItem = 13	// #LABEL_TypeItem
#ifndef __MAKETA__
    ,T_Atomic_Min = T_Bool,
    T_Atomic_Max = T_String
#endif
  };

  static const String	formatNumber(const Variant& val,
     short prec, bool hex = false) // format a number (hex for integral types only)
     {return val.formatNumber_impl(prec, hex);} 

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
  bool			isPtrType() const {return ((m_type >= T_Ptr) && (m_type <= T_TypeItem));} 
    // 'true' if the value is a void*, taBase*, or taMatrix*
  bool			isBaseType() const {return ((m_type == T_Base) || (m_type == T_Matrix));} 
    // 'true' if the value is a taBase* or taMatrix*
  bool			isMatrixType() const {return (m_type == T_Matrix);} 
    // 'true' if the value is a taMatrix*, BUT could be NULL
  bool			isTypeItem() const {return (m_type == T_TypeItem);} 
    // 'true' if the value is a TypeItem*, BUT could be NULL
  bool			isEnumDef() const {return (toEnumDef() != NULL);}
    // 'true' if the value is a NON-NULL EnumDef*, else false
  bool			isTypeDef() const {return (toTypeDef() != NULL);} 
    // 'true' if the value is a NON-NULL TypeDef*, else false
  bool			isMemberDefBase() const {return (toMemberDefBase() != NULL);} 
    // 'true' if the value is a NON-NULL MemberDefBase*, else false
  bool			isMemberDef() const {return (toMemberDef() != NULL);} 
    // 'true' if the value is a NON-NULL MemberDef*, else false
  bool			isPropertyDef() const {return (toPropertyDef() != NULL);} 
    // 'true' if the value is a NON-NULL PropertyDef*, else false
  bool			isMethodDef() const {return (toMethodDef() != NULL);} 
    // 'true' if the value is a NON-NULL MethodDef*, else false
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
  void 			setInt64(ta_int64_t val, bool null = false);
  void 			setUInt64(ta_uint64_t val, bool null = false);
  void 			setIntPtr(intptr_t val, bool null = false)
    {if (sizeof(intptr_t) == sizeof(int)) setInt((int)val, null); else setInt64(val, null);}
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
  void			setTypeItem(TypeItem* cp); // handles setting of a TypeItem

  void			updateFromString(const String& val); // set value from string, but keep current type
  
  // the "<type> toXxx()" return a result of requested type, leaving current value as is
  bool 			toBool() const;
  byte 			toByte() const {return (byte)toUInt();}
  int 			toInt() const;
  uint 			toUInt() const;
  ta_int64_t 		toInt64() const;
  ta_uint64_t 		toUInt64() const; //
  float 		toFloat() const {return (float)toDouble();}
  double 		toDouble() const;
  char 			toChar() const;
  void* 		toPtr() const; // must be a void*, Base, or Matrix, otherwise returns NULL
  String 		toString() const;
#ifndef NO_TA_BASE
  taBase* 		toBase() const; // must be a Base or Matrix, otherwise returns NULL
  taMatrix* 		toMatrix() const; // must be a Matrix, otherwise returns NULL
#endif  
  TypeItem* 		toTypeItem() const; // must be a TypeItem, otherwise returns NULL
  EnumDef* 		toEnumDef() const; // must be a EnumDef, otherwise returns NULL
  TypeDef* 		toTypeDef() const; // must be a TypeDef, otherwise returns NULL
  MemberDefBase* 	toMemberDefBase() const; // must be a MemberDefBase, otherwise returns NULL
  MemberDef* 		toMemberDef() const; // must be a MemberDef, otherwise returns NULL
  PropertyDef* 		toPropertyDef() const; // must be a PropertyDef, otherwise returns NULL
  MethodDef* 		toMethodDef() const; // must be a MethodDef, otherwise returns NULL
  const String		toCssLiteral() const; // to a form suitable for initializing a Css variable, ex. quoted strings, U suffix for unsigned, path for taBase variable, etc.
  // following are the operators for C++ casting -- note they are all explicit
  // because you can't mix having cast operators with having various math operators
/*can't have converters and ops
  operator bool() const {return toBool();}
  operator byte() const {return toByte();}
  operator int() const {return toInt();}
  operator uint() const {return toUInt();}
  operator ta_int64_t() const {return toInt64();}
  operator ta_uint64_t() const {return toUInt64();} //
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
  bool 			eqInt64(ta_int64_t val) const;
  bool 			eqUInt64(ta_uint64_t val) const;
  bool 			eqIntPtr(intptr_t val) const
    {if (sizeof(intptr_t) == sizeof(int)) return eqInt((int)val); else return eqInt64(val);}
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
  bool			eqTypeItem(const TypeItem* val) const {return eqPtr(val);} 

  // comparison operations, < -ve, == 0, > +ve
  int			cmpVariant(const Variant& val) const; // value compare, using fairly relaxed type rules; Invalid never == anything
  int 			cmpBool(bool val) const;
  int 			cmpByte(byte val) const {return cmpUInt(val);}
  int 			cmpInt(int val) const;
  int 			cmpUInt(uint val) const;
  int 			cmpInt64(ta_int64_t val) const;
  int 			cmpUInt64(ta_uint64_t val) const;
  int 			cmpIntPtr(intptr_t val) const
    {if (sizeof(intptr_t) == sizeof(int)) return cmpInt((int)val); else return cmpInt64(val);}
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
  int			cmpTypeItem(const TypeItem* val) const {return cmpPtr(val);} 

//TODO  bool			canCast(VarType new_type);
    // returns 'true' if current type can be successfully cast to requested type
  // assignment operators
  Variant& 	operator=(const Variant& val) {setVariant(val); return *this;}
  Variant& 	operator=(bool val) {setBool(val); return *this;}
  Variant& 	operator=(byte val) {setUInt(val); return *this;}
  Variant& 	operator=(int val) {setInt(val); return *this;}
  Variant& 	operator=(uint val) {setUInt(val); return *this;}
  Variant& 	operator=(ta_int64_t val) {setInt64(val); return *this;}
  Variant& 	operator=(ta_uint64_t val) {setUInt64(val); return *this;}
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
  Variant& 	operator=(TypeItem* val) {setTypeItem(val); return *this;} //

  Variant& 	operator+=(const String& rhs); 
    // concatenation, result is String if input is String or atomic

  //note: x= operators can cause type to change following C++ type promotion semantics:
  // if either is "double" result is "double"
  // else if equal-sized or longer op is "unsigned" other op is forced to "unsigned" 
  //   and size is forced to largest of two ops 
  Variant& 	operator+=(char rhs); // note: also does concat if we are a String
  Variant& 	operator+=(int rhs);
  Variant& 	operator+=(uint rhs);
  Variant& 	operator+=(ta_int64_t rhs);
  Variant& 	operator+=(ta_uint64_t rhs);
  Variant& 	operator+=(double rhs);
  Variant& 	operator+=(const Variant& rhs);
  
  Variant& 	operator-=(char rhs);
  Variant& 	operator-=(int rhs);
  Variant& 	operator-=(uint rhs);
  Variant& 	operator-=(ta_int64_t rhs);
  Variant& 	operator-=(ta_uint64_t rhs);
  Variant& 	operator-=(double rhs);
  Variant& 	operator-=(const Variant& rhs);
  
  Variant& 	operator*=(char rhs);
  Variant& 	operator*=(int rhs);
  Variant& 	operator*=(uint rhs);
  Variant& 	operator*=(ta_int64_t rhs);
  Variant& 	operator*=(ta_uint64_t rhs);
  Variant& 	operator*=(double rhs);
  Variant& 	operator*=(const Variant& rhs);
  
  Variant& 	operator/=(char rhs);
  Variant& 	operator/=(int rhs);
  Variant& 	operator/=(uint rhs);
  Variant& 	operator/=(ta_int64_t rhs);
  Variant& 	operator/=(ta_uint64_t rhs);
  Variant& 	operator/=(double rhs);
  Variant& 	operator/=(const Variant& rhs); //
  
  //note: %, <<, >>, and bitwise operator only valid for int types, not reals
  Variant& 	operator%=(char rhs);
  Variant& 	operator%=(int rhs);
  Variant& 	operator%=(uint rhs);
  Variant& 	operator%=(ta_int64_t rhs);
  Variant& 	operator%=(ta_uint64_t rhs);
  Variant& 	operator%=(const Variant& rhs); //

  Variant& 	operator<<=(char rhs);
  Variant& 	operator<<=(int rhs);
  Variant& 	operator<<=(uint rhs);
  Variant& 	operator<<=(ta_int64_t rhs);
  Variant& 	operator<<=(ta_uint64_t rhs);
  Variant& 	operator<<=(const Variant& rhs); //

  Variant& 	operator>>=(char rhs);
  Variant& 	operator>>=(int rhs);
  Variant& 	operator>>=(uint rhs);
  Variant& 	operator>>=(ta_int64_t rhs);
  Variant& 	operator>>=(ta_uint64_t rhs);
  Variant& 	operator>>=(const Variant& rhs); //

  Variant& 	operator&=(char rhs);
  Variant& 	operator&=(int rhs);
  Variant& 	operator&=(uint rhs);
  Variant& 	operator&=(ta_int64_t rhs);
  Variant& 	operator&=(ta_uint64_t rhs);
  Variant& 	operator&=(const Variant& rhs); //

  Variant& 	operator|=(char rhs);
  Variant& 	operator|=(int rhs);
  Variant& 	operator|=(uint rhs);
  Variant& 	operator|=(ta_int64_t rhs);
  Variant& 	operator|=(ta_uint64_t rhs);
  Variant& 	operator|=(const Variant& rhs); //

  Variant& 	operator^=(char rhs); // xor
  Variant& 	operator^=(int rhs);
  Variant& 	operator^=(uint rhs);
  Variant& 	operator^=(ta_int64_t rhs);
  Variant& 	operator^=(ta_uint64_t rhs);
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
  
#if defined(TA_USE_QT) && !defined(__MAKETA__)
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
  Variant(ta_int64_t val);
  Variant(ta_uint64_t val);
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
  Variant(TypeItem* val);
  ~Variant(); //

public: // following primarily for TypeDef usage, streaming, etc.
  void			GetRepInfo(TypeDef*& typ, void*& data) const; // current typedef, and pointer to the data
  void			UpdateAfterLoad(); // called after internal modifications, to reassert correctness of null etc.
  void			ForceType(VarType vt, bool null);
    // called by streaming system to force the type to be indicated kind
  void			Dump_Save_Type(ostream& strm) const; // dumps type and null 
  bool			Dump_Load_Type(istream& strm, int& last_char); 
    // loads type and null, using taMisc:: strm routines; calls ForceType; returns 'true' if type loaded 
  void			warn(const char* msg) const; // emit warning message
  void			error(const char* msg) const; // emit error message
protected:
#ifdef __MAKETA__
  unsigned char d[12];
#else
  union Data // sizes are given for 32/64 sys
  {
      bool b; // 8
      int i; // 32
      uint u; // 32 -- also for byte
      ta_int64_t i64; // 64
      ta_uint64_t u64; // 64
      double d; // 64
      char c;
      intptr_t str; // 32/64 note: this is an in-place taString, NOT a pointer
      void* ptr; // 32/64
#ifndef NO_TA_BASE
      taBase* tab; // 32/64 note: properly ref counted; also used for matrix
#endif
      TypeItem* ti; // 32/64
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
  const String	formatNumber_impl(const Variant& val,
     short prec, bool hex = false) const; // format a number 
  void			releaseType(); // #IGNORE handles undoing of specials
  //note: following ops don't affect m_is_null -- context must determine that
  
  //note: following gets ONLY valid when m_type is known to be of correct type
  inline const String& 	getString() const { return *((String*)(&d.str));} // #IGNORE
  inline String& 	getString() { return *((String*)(&d.str));} // #IGNORE
#ifndef NO_TA_BASE
  taMatrix*		getMatrix() { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
  taMatrix*		getMatrix() const { return (taMatrix*)(d.tab);} // #IGNORE only if m_type=T_Matrix
#endif
};

#endif // Variant_h
