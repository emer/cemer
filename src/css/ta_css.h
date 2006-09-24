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


#ifndef ta_css_h
#define ta_css_h 1

// for special eveptr class

#include "css_machine.h"
#include "css_basic_types.h"
#include "ta_type.h"

class CSS_API cssTA : public cssCPtr {
public:
  TypeDef*	type_def;	// TypeDef Info

  uint		GetSize() const		{ return sizeof(*this); }
  const char*	GetTypeName() const	{ return type_def->name; }
  cssEl*	GetTypeObject() const;
  cssTypes	GetType() const		{ return T_TA; }
  cssTypes	GetPtrType() const	{ return T_TA; }

  String	PrintStr() const;
  String	PrintFStr() const;
  void 		PrintF(ostream& fh = cout) const 	{ Print(fh); }

  void 		Print(ostream& fh = cout) const;
  void 		PrintR(ostream& fh = cout) const;	// recursive
#ifdef TA_GUI
  int		Edit(bool wait = false);
#endif

  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  void		TokenInfo(ostream& fh = cout) const;
  cssEl*	GetToken(int idx) const;

  // constructors
  void		Constr();
  cssTA();
  cssTA(void* it, int pc, TypeDef* td);
  cssTA(void* it, int pc, TypeDef* td, const char* nm);
  cssTA(void* it, int pc, TypeDef* td, const char* nm, cssEl* cp, bool ro);
  cssTA(const cssTA& cp);
  cssTA(const cssTA& cp, const char* nm);

  cssCloneOnly(cssTA);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  // converters
  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const char* td) const;
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

  String GetStr() const; 		// check for istream, get result if so
  Variant GetVar() const {return Variant(GetStr());} // can't see much use for anything else...
  operator Real() const;
  operator Int() const;
  operator TAPtr() const;
  operator TAPtr*() const;
  operator void*() const;

#ifdef CSS_SUPPORT_TYPEA
  operator TypeDef*() const;
  operator MemberDef*() const;
  operator MethodDef*() const;
#endif

  operator iostream*() const;
  operator istream*() const;
  operator ostream*() const;
  operator fstream*() const;
  operator stringstream*() const;

  operator iostream**() const;
  operator istream**() const;
  operator ostream**() const;
  operator fstream**() const;
  operator stringstream**() const;

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String& s);
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl& s); // assign and cast don't set the type
  void PtrAssignPtr(cssCPtr *s); // call SetPointer when setting TA pointers..
  void InitAssign(const cssEl& s);	// init assign sets the type
  void CastFm(const cssEl& s);	// cast does not set type, but allows any cast..

  void UpdateAfterEdit();

  cssEl* operator<<(cssEl& s);	// for iostreams..
  cssEl* operator>>(cssEl& s);

  virtual cssEl*	GetElement_impl(TAPtr ths, int i) const;

  cssEl* operator[](int) const;
  int	 GetMemberNo(const char* memb) const;
  cssEl* GetMemberFmName(const char* memb) const;
  cssEl* GetMemberFmNo(int memb) const;
  int	 GetMethodNo(const char* memb) const;
  cssEl* GetMethodFmName(const char* memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetScoped(const char*) const;
};

#define cssTA_inst(l,n,c,t)		l .Push(new cssTA(n, c, t, #n))
#define cssTA_inst_nm(l,n,c,t,s)	l .Push(new cssTA(n, c, t, s))
#define cssTA_inst_ptr(l,n,c,t,x)	l .Push(x = new cssTA(n, c, t, #x))
#define cssTA_inst_ptr_nm(l,n,c,t,x,s)	l .Push(x = new cssTA(n, c, t, s))


// this one is specially for taBase types
// calls the overloaded (taBase) versions of TypeDef functions

class CSS_API cssTA_Base : public cssTA {
public:
  String	PrintStr() const;
  String	PrintFStr() const;

  void 		Print(ostream& fh = cout) const;
  void 		PrintR(ostream& fh = cout) const;		// recursive

  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  // constructors
  void 		Constr();
  cssTA_Base() 						: cssTA()	    { Constr();}
  cssTA_Base(void* it, int pc, TypeDef* td) 		: cssTA(it,pc,td)   { Constr();}
  cssTA_Base(void* it, int pc, TypeDef* td, const char* nm)  	: cssTA(it,pc,td,nm){ Constr();}
  cssTA_Base(void* it, int pc, TypeDef* td, const char* nm, cssEl* cp, bool ro)
  	: cssTA(it,pc,td,nm,cp,ro)	{ Constr(); }
  cssTA_Base(const cssTA_Base& cp)			: cssTA(cp) 	    { Constr();}
  cssTA_Base(const cssTA_Base& cp, const char* nm)	: cssTA(cp,nm) 	    { Constr();}

  cssCloneOnly(cssTA_Base);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA_Base((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  // converters
  TAPtr   	GetTAPtr() const { return (TAPtr)GetVoidPtr(); }

  operator TAPtr() const	{ return (TAPtr)GetVoidPtr(); }
  operator TAPtr*() const	{ return (TAPtr*)GetVoidPtr(2); }

  Variant GetVar() const { return Variant(GetTAPtr());}
  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String& cp)	{ cssTA::operator=(cp); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void Assign_impl(const cssEl& s);	// does taBase special things
  void operator=(const cssEl& s);
  void InitAssign(const cssEl& s);	// init assign sets the type

  void UpdateAfterEdit();

  cssEl* operator[](int) const;
  cssEl* GetMemberFmName(const char* memb) const; // use recursive path!
  cssEl* NewOpr();		// only ta_base get a new operator..
  void 	 DelOpr();		// and a del operator

  virtual void InstallThis(cssProgSpace* sp); // install this into a progspace
};


#define cssTA_Base_inst(l,n,c,t)	l .Push(new cssTA_Base(n, c, t, #n))
#define cssTA_Base_inst_nm(l,n,c,t,s)	l .Push(new cssTA_Base(n, c, t, s))
#define cssTA_Base_inst_ptr(l,n,c,t,x)	l .Push(x = new cssTA_Base(n, c, t, #x))
#define cssTA_Base_inst_ptr_nm(l,n,c,t,x,s) l .Push(x = new cssTA_Base(n, c, t, s))


class CSS_API cssFStream : public cssTA {
public:
  static TypeDef*	TA_TypeDef(); // returns TA_fstream
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr()	{ ptr = new fstream; }
  cssFStream() 				: cssTA(NULL, 1, TA_TypeDef())	   { Constr(); }
  cssFStream(const char* nm)			: cssTA(NULL, 1, TA_TypeDef(), nm)  { Constr(); }
  cssFStream(const cssFStream& cp)		: cssTA(cp) 	{ Constr(); }
  cssFStream(const cssFStream& cp, const char*)	: cssTA(cp)	{ Constr(); }
  ~cssFStream()			{ fstream* str = (fstream*)ptr; delete str; }

  cssCloneOnly(cssFStream);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssFStream((const char*)*(arg[1])); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }

  cssEl* operator-(cssEl&)		{ NopErr("-"); return this; }
  cssEl* operator*()			{ return cssTA::operator*(); }
  cssEl* operator*(cssEl&)		{ NopErr("*"); return this; }
  cssEl* operator/(cssEl&)		{ NopErr("/"); return this; }
  cssEl* operator%(cssEl&)		{ NopErr("%"); return this; }
  cssEl* operator-()    		{ NopErr("-"); return this; }

  void operator+=(cssEl&) 	{ NopErr("+="); }
  void operator-=(cssEl&) 	{ NopErr("-="); }
  void operator*=(cssEl&) 	{ NopErr("*="); }
  void operator/=(cssEl&) 	{ NopErr("/="); }
};

class CSS_API cssSStream : public cssTA {
public:
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr();
  cssSStream();
  cssSStream(const char* nm);
  cssSStream(const cssSStream& cp);
  cssSStream(const cssSStream& cp, const char*);
  ~cssSStream();

  cssCloneOnly(cssSStream);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssSStream((const char*)*(arg[1])); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }

  cssEl* operator-(cssEl&)		{ NopErr("-"); return this; }
  cssEl* operator*()			{ return cssTA::operator*(); }
  cssEl* operator*(cssEl&)		{ NopErr("*"); return this; }
  cssEl* operator/(cssEl&)		{ NopErr("/"); return this; }
  cssEl* operator%(cssEl&)		{ NopErr("%"); return this; }
  cssEl* operator-()    		{ NopErr("-"); return this; }

  void operator+=(cssEl&) 	{ NopErr("+="); }
  void operator-=(cssEl&) 	{ NopErr("-="); }
  void operator*=(cssEl&) 	{ NopErr("*="); }
  void operator/=(cssEl&) 	{ NopErr("/="); }
};

class CSS_API cssLeafItr : public cssTA {
public:
  static TypeDef*	TA_TypeDef(); // returns TA_taLeafItr
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr();
  cssLeafItr() 			: cssTA(NULL, 1, TA_TypeDef())   { Constr(); }
  cssLeafItr(const char* nm)		: cssTA(NULL, 1, TA_TypeDef(), nm)  { Constr(); }
  cssLeafItr(const cssLeafItr& cp)	: cssTA(cp) 	{ Constr(); }
  cssLeafItr(const cssLeafItr& cp, const char*)	: cssTA(cp)	{ Constr(); }
  ~cssLeafItr();

  cssCloneOnly(cssLeafItr);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssLeafItr((const char*)*(arg[1])); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }

  cssEl* operator-(cssEl&)		{ NopErr("-"); return this; }
  cssEl* operator*()			{ return cssTA::operator*(); }
  cssEl* operator*(cssEl&)		{ NopErr("*"); return this; }
  cssEl* operator/(cssEl&)		{ NopErr("/"); return this; }
  cssEl* operator%(cssEl&)		{ NopErr("%"); return this; }
  cssEl* operator-()    		{ NopErr("-"); return this; }

  void operator+=(cssEl&) 	{ NopErr("+="); }
  void operator-=(cssEl&) 	{ NopErr("-="); }
  void operator*=(cssEl&) 	{ NopErr("*="); }
  void operator/=(cssEl&) 	{ NopErr("/="); }
};

class CSS_API cssTAEnum : public cssTA {
public:
  Int 		val;
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr()  	{ ptr = (void*)&val; }
  cssTAEnum(TypeDef* td) 			: cssTA(NULL, 1, td)   { Constr(); }
  cssTAEnum(TypeDef* td, const char* nm)	: cssTA(NULL, 1, td, nm)  { Constr(); }
  cssTAEnum(TypeDef* td, int vl)		: cssTA(NULL, 1, td)  { Constr(); val = vl; }
  cssTAEnum(const cssTAEnum& cp): cssTA(cp) 	{ Constr(); val = cp.val;}
  cssTAEnum(const cssTAEnum& cp, const char*)	: cssTA(cp)	{ Constr(); val = cp.val; }
  ~cssTAEnum()  		{ };

  cssCloneOnly(cssTAEnum);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTAEnum(type_def, (const char*)*(arg[1])); }

  // converters
  Variant GetVar() const { return Variant(val);}
  operator Real() const	 	{ return (Real)val; }
  operator Int() const	 	{ return val; }

  void operator=(Real cp) 		{ val = (int)cp; }
  void operator=(Int cp)		{ val = cp; }
  void operator=(const String& cp);
  void operator=(void* cp)	{ cssTA::operator=(cp); }
  void operator=(void** cp)	{ cssTA::operator=(cp); }

  // operators
  void operator=(const cssEl& s);

  cssEl* operator+(cssEl& t)
  { cssInt* r = new cssInt(val); r->val += (Int)t; return r; }
  cssEl* operator-(cssEl& t)
  { cssInt* r = new cssInt(val); r->val -= (Int)t; return r; }
  cssEl* operator*()		{ return cssEl::operator*(); }
  cssEl* operator*(cssEl& t)
  { cssInt* r = new cssInt(val); r->val *= (Int)t; return r; }
  cssEl* operator/(cssEl& t)
  { cssInt* r = new cssInt(val); r->val /= (Int)t; return r; }
  cssEl* operator%(cssEl& t)
  { cssInt* r = new cssInt(val); r->val %= (Int)t; return r; }
  cssEl* operator<<(cssEl& t)
  { cssInt* r = new cssInt(val); r->val <<= (Int)t; return r; }
  cssEl* operator>>(cssEl& t)
  { cssInt* r = new cssInt(val); r->val >>= (Int)t; return r; }
  cssEl* operator&(cssEl& t)
  { cssInt* r = new cssInt(val); r->val &= (Int)t; return r; }
  cssEl* operator^(cssEl& t)
  { cssInt* r = new cssInt(val); r->val ^= (Int)t; return r; }
  cssEl* operator|(cssEl& t)
  { cssInt* r = new cssInt(val); r->val |= (Int)t; return r; }

  cssEl* operator-()
  { cssInt* r = new cssInt(val); r->val = -val; return r; }

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
  bool operator==(cssEl& s);
  bool operator!=(cssEl& s);
  bool operator&&(cssEl& s) 	{ return (val && (Int)s); }
  bool operator||(cssEl& s) 	{ return (val || (Int)s); }
};

#endif // ta_css.h
