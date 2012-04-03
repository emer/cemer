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


#ifndef css_ta_h
#define css_ta_h 1

// for special eveptr class

#include "css_machine.h"
#include "css_basic_types.h"
#include "ta_type.h"

///////////////////////////////////////////////////////////////////////
//		Assignment semantics
//
// ArgCopy (InitAssign): initialize pointer if ptr_cnt > 0, else copy contents
// for most taBase, we assume more "structure" / reference semantics and that
// assignment in context of css is typically for pointer assignment, so 
// operator= : pointer assign except for ptr_cnt == 0
//
// however, for taMatrix, which have more "value" semantics like an int or float
// we override to use contents copy except for ptr_cnt == 2 (ptr ptr)
//

class CSS_API cssTA : public cssCPtr {
  // a pointer that has a TA TypeDef associated with it: uses type info to perform ops 
  // NOTE: specialized versions exist for specific types: those must be used (e.g., taBase, etc)
public:
  TypeDef*	type_def;	// TypeDef Info

  uint		GetSize() const		{ return sizeof(*this); }
  const char*	GetTypeName() const	{ if(type_def) return type_def->name; return "ta"; }
  cssEl*	GetTypeObject() const;
  cssTypes	GetType() const		{ return T_TA; }
  cssTypes	GetPtrType() const	{ return T_TA; }

  virtual TypeDef* GetNonRefTypeDef() const	{ return type_def; }
  // any kind of reference-semantics object can override to de-ref'd type
  virtual int 	GetNonRefPtrCnt() const		{ return ptr_cnt; }
  // any kind of reference-semantics object can override to de-ref'd ptr_cnt
  virtual void* GetNonRefPtr() const		{ return ptr; }
  // any kind of reference-semantics object can override to de-ref'd ptr

  String	PrintStr() const;
  String	PrintFStr() const;

#ifdef TA_GUI
  int		Edit(bool wait = false);
#endif

  String&	PrintType(String& fh) const;
  String&	PrintInherit(String& fh) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  String&	PrintTokens(String& fh) const;
  cssEl*	GetToken(int idx) const;

  // constructors
  void		Constr();
  void		Copy(const cssTA& cp) { cssCPtr::Copy(cp); type_def = cp.type_def; }
  void		CopyType(const cssTA& cp) { cssCPtr::CopyType(cp); type_def = cp.type_def; }

  cssTA();
  cssTA(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL, bool ro = false);
  cssTA(const cssTA& cp);
  cssTA(const cssTA& cp, const String& nm);

  cssCloneOnly(cssTA);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  // converters
  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const String& td) const;
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

  String GetStr() const;
  Variant GetVar() const;
  operator void*() const;

  operator TypeDef*() const;

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String& s);
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }
  USING(cssCPtr::operator=);

  void 	ArgCopy(const cssEl& s);

  // copying: uses typedef auto copy function for ptr_cnt = 0
  void operator=(const cssEl& s);
  void PtrAssignPtr(const cssEl& s);
  // for ArgCopy and ptr_cnt >= 2 -- assign our pointer
  override bool AssignCheckSource(const cssEl& s);
  // make sure source is typedef compatible
  virtual bool AssignObjCheck(const cssEl& s);
  // do basic checks on us and source for Copy value assignment

  int	 GetMemberNo(const String& memb) const;
  cssEl* GetMemberFmName(const String& memb) const;
  cssEl* GetMemberFmNo(int memb) const;
  int	 GetMethodNo(const String& memb) const;
  cssEl* GetMethodFmName(const String& memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetScoped(const String&) const;
};

#define cssTA_inst(l,n,c,t)		l .Push(new cssTA(n, c, t, #n))
#define cssTA_inst_nm(l,n,c,t,s)	l .Push(new cssTA(n, c, t, s))
#define cssTA_inst_ptr(l,n,c,t,x)	l .Push(x = new cssTA(n, c, t, #x))
#define cssTA_inst_ptr_nm(l,n,c,t,x,s)	l .Push(x = new cssTA(n, c, t, s))

class CSS_API cssTA_Base : public cssTA {
  // specifically for taBase types -- calls the overloaded versions of TypeDef functions
INHERITED(cssTA)
public:
  String	PrintStr() const;

  String&	PrintType(String& fh) const;
  String&	PrintInherit(String& fh) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  // constructors
  void 		Constr();
  void		Copy(const cssTA_Base& cp);
  void		CopyType(const cssTA_Base& cp);

  cssTA_Base() : cssTA()	    { Constr();}
  cssTA_Base(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	     bool ro=false) : cssTA(it,pc,td,nm,cls_par,ro)	{ Constr(); }
  cssTA_Base(const cssTA_Base& cp);
  cssTA_Base(const cssTA_Base& cp, const String& nm);
  ~cssTA_Base();

  cssCloneOnly(cssTA_Base);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA_Base((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  // converters
  taBase*  GetTAPtr() const 	{ return (taBase*)GetVoidPtr(); }

  operator taBase*() const	{ return GetTAPtr(); }
  operator taBase**() const	{ return (taBase**)GetVoidPtr(2); }
  String GetStr() const;
  Variant GetVar() const 	{ return Variant(GetTAPtr());}

  // operators
  void operator=(const String& s);
  void operator=(const cssEl& s); // use obj->UnSafeCopy for ptr_cnt == 0
  void operator=(taBase*);
  void operator=(taBase**);
  void operator=(void* cp);	// these are very bad because of the ref counting but we just have to assume the pointer is a taBase*!
  void operator=(void** cp);
  USING(cssTA::operator=);

  void 	ArgCopy(const cssEl& s); // init pointers for ptr_cnt > 0
  void 	InitAssign(const cssEl& s); // reset our object type to match source!
  
  void PtrAssignPtr(const cssEl& s);
  override bool PtrAssignPtrPtr(void* new_ptr_val);
  // use SetPointer..
  override void PtrAssignNull();

  void UpdateAfterEdit();

  cssEl* operator[](const Variant& idx) const;
  cssEl* GetMemberFmName(const String& memb) const; // use recursive path!
  cssEl* NewOpr();		// only ta_base get a new operator..
  void 	 DelOpr();		// and a del operator

  virtual void InstallThis(cssProgSpace* sp); // install this into a progspace
};


#define cssTA_Base_inst(l,n,c,t)	l .Push(new cssTA_Base(n, c, t, #n))
#define cssTA_Base_inst_nm(l,n,c,t,s)	l .Push(new cssTA_Base(n, c, t, s))
#define cssTA_Base_inst_ptr(l,n,c,t,x)	l .Push(x = new cssTA_Base(n, c, t, #x))
#define cssTA_Base_inst_ptr_nm(l,n,c,t,x,s) l .Push(x = new cssTA_Base(n, c, t, s))

class CSS_API cssSmartRef : public cssTA {
  // a taSmartRef object (ptr_cnt = 0)
public:
  cssTA_Base*	cssref;		// css item representing the object being pointed to

  inline taSmartRef* GetSmartRef() const { if(ptr) return (taSmartRef*)ptr; return NULL; }
  inline taBase* GetSmartRefPtr() const  { taSmartRef* rf = GetSmartRef(); if(rf) return rf->ptr(); return NULL; }

  String	PrintStr() const;
  String	PrintFStr() const;

  bool		IsTaMatrix() const	{ return cssref->IsTaMatrix(); }
  cssTypes 	GetType() const		{ return cssref->GetType(); }
  cssTypes	GetPtrType() const	{ return cssref->GetPtrType(); }
  const char*	GetTypeName() const	{ return cssref->GetTypeName(); }
  cssEl*	GetTypeObject() const	{ return cssref->GetTypeObject(); }
  TypeDef* 	GetNonRefTypeDef() const { return cssref->GetNonRefTypeDef(); }
  int 		GetNonRefPtrCnt() const  { return cssref->GetNonRefPtrCnt(); }
  void* 	GetNonRefPtr() const     { return cssref->GetNonRefPtr(); }

  cssEl*	GetActualObj() const	{ return cssref->GetActualObj(); }
  cssEl*	GetNonRefObj() const	{ return cssref; }

  String&	PrintType(String& fh) const;
  String&	PrintInherit(String& fh) const;
  String&	PrintTokens(String& fh) const { return cssref->PrintTokens(fh); }
  cssEl*	GetToken(int idx) const	      { return cssref->GetToken(idx); }

  void		UpdateCssRef();	// update the cssref object based on current ptr()

  // constructors
  cssSmartRef() : cssTA() { cssref = NULL; UpdateCssRef(); }
  cssSmartRef(void* it, int pc, TypeDef* td, const String& nm = _nilString,
	      cssEl* cls_par=NULL,  bool ro = false)
    : cssTA(it, pc, td, nm, cls_par, ro) { cssref = NULL; UpdateCssRef(); }
  cssSmartRef(const cssSmartRef& cp) : cssTA(cp)
  { cssref = NULL; UpdateCssRef(); }
  cssSmartRef(const cssSmartRef& cp, const String& nm) : cssTA(cp, nm)
    { cssref = NULL; UpdateCssRef(); }
  cssCloneOnly(cssSmartRef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssSmartRef((void*)NULL, ptr_cnt, type_def, (const String&)*(arg[1])); }
  ~cssSmartRef();

  // converters
  String GetStr() const 	{ return cssref->GetStr(); }
  Variant GetVar() const 	{ return cssref->GetVar(); }
  operator Real() const	 	{ return (Real)*(cssref); }
  operator Int() const	 	{ return (Int)*(cssref); }
  operator void*() const	{ return (void*)*(cssref); }
  operator void**() const	{ return (void**)*(cssref); }

  operator bool() const		{ return (bool)*(cssref); }
  operator taBase*() const	{ return (taBase*)*(cssref); }

  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const String& td) const;

  // operators
  void operator=(const String& s)	{ cssref->operator=(s); }
  void operator=(const cssEl& s);
  void operator=(taBase*);
  void operator=(taBase**);
  USING(cssTA::operator=)

  void CastFm(const cssEl& cp)	{ ArgCopy(cp); }
  void ArgCopy(const cssEl& cp);

  void PtrAssignPtr(const cssEl& s);

  void UpdateAfterEdit()	{ cssref->UpdateAfterEdit(); }

  bool	 MembersDynamic()	{ return true; }
  int	 GetMemberNo(const String& memb) const { return -1; } // never static lookup
  cssEl* GetMemberFmNo(int s) const  		{ return cssref->GetMemberFmNo(s); }
  cssEl* GetMemberFmName(const String& s) const { return cssref->GetMemberFmName(s); }
  int	 GetMethodNo(const String& memb) const { return -1; }
  cssEl* GetMethodFmNo(int s) const		{ return cssref->GetMethodFmNo(s); }
  cssEl* GetMethodFmName(const String& s) const	{ return cssref->GetMethodFmName(s); }
  cssEl* GetScoped(const String& s) const  	{ return cssref->GetScoped(s); }
  cssEl* NewOpr()   				{ return cssref->NewOpr(); }
  void	 DelOpr() 				{ cssref->DelOpr(); }

  cssEl* operator+(cssEl& s) 	{ return cssref->operator+(s); }
  cssEl* operator-(cssEl& s) 	{ return cssref->operator-(s); }
  cssEl* operator*(cssEl& s) 	{ return cssref->operator*(s); }
  cssEl* operator/(cssEl& s) 	{ return cssref->operator/(s); }
  cssEl* operator%(cssEl& s) 	{ return cssref->operator%(s); }
  cssEl* operator<<(cssEl& s)	{ return cssref->operator<<(s); }
  cssEl* operator>>(cssEl& s)	{ return cssref->operator>>(s); }
  cssEl* operator&(cssEl& s)	{ return cssref->operator&(s); }
  cssEl* operator^(cssEl& s)	{ return cssref->operator^(s); }
  cssEl* operator|(cssEl& s)	{ return cssref->operator|(s); }
  cssEl* operator-()       	{ return cssref->operator-(); }
  cssEl* operator~()       	{ return cssref->operator~(); }
  cssEl* operator*()	   	{ return cssref->operator*(); }
  cssEl* operator[](const Variant& idx) const	{ return cssref->operator[](idx); }

  cssEl* operator! ()		{ return cssref->operator!(); }
  cssEl* operator&&(cssEl& s)	{ return cssref->operator&&(s); }
  cssEl* operator||(cssEl& s)	{ return cssref->operator||(s); }

  cssEl* operator< (cssEl& s) 	{ return cssref->operator<(s); }
  cssEl* operator> (cssEl& s) 	{ return cssref->operator>(s); }
  cssEl* operator<=(cssEl& s) 	{ return cssref->operator<=(s); }
  cssEl* operator>=(cssEl& s) 	{ return cssref->operator>=(s); }
  cssEl* operator==(cssEl& s) 	{ return cssref->operator==(s); }
  cssEl* operator!=(cssEl& s) 	{ return cssref->operator!=(s); }

  void operator+=(cssEl& s) 	{ cssref->operator+=(s); }
  void operator-=(cssEl& s) 	{ cssref->operator-=(s); }
  void operator*=(cssEl& s) 	{ cssref->operator*=(s); }
  void operator/=(cssEl& s) 	{ cssref->operator/=(s); }
  void operator%=(cssEl& s) 	{ cssref->operator%=(s); }
  void operator<<=(cssEl& s) 	{ cssref->operator<<=(s); }
  void operator>>=(cssEl& s) 	{ cssref->operator>>=(s); }
  void operator&=(cssEl& s) 	{ cssref->operator&=(s); }
  void operator^=(cssEl& s) 	{ cssref->operator^=(s); }
  void operator|=(cssEl& s) 	{ cssref->operator|=(s); }

};

class CSS_API cssIOS : public cssTA {
  // a pointer to an iostream object of any sort: supports various streaming ops
public:
  String	PrintFStr() const;
  String	GetStr() const;

  // constructors
  cssIOS() : cssTA() { };
  cssIOS(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	 bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssIOS(const cssIOS& cp) : cssTA(cp) { };
  cssIOS(const cssIOS& cp, const String& nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssIOS);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssIOS((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  USING(cssTA::operator=)

  void PtrAssignPtr(const cssEl& s); // use type casts to make it work right for diff offsets

  operator Real() const;
  operator Int() const;

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

  cssEl* operator<<(cssEl& s);	// for iostreams..
  cssEl* operator>>(cssEl& s);
};

class CSS_API cssFStream : public cssIOS {
  // owns its own fstream with ptr_cnt = 0: manages the construction and destruction of obj
public:
  static TypeDef*	TA_TypeDef(); // returns TA_fstream
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr()	{ ptr = new fstream; }
  cssFStream() 				: cssIOS(NULL, 1, TA_TypeDef())	   { Constr(); }
  cssFStream(const String& nm)			: cssIOS(NULL, 1, TA_TypeDef(), nm)  { Constr(); }
  cssFStream(const cssFStream& cp)		: cssIOS(cp) 	{ Constr(); }
  cssFStream(const cssFStream& cp, const String&)	: cssIOS(cp)	{ Constr(); }
  ~cssFStream()			{ fstream* str = (fstream*)ptr; delete str; }

  cssCloneOnly(cssFStream);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssFStream(arg[1]->GetStr()); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }
  USING(cssIOS::operator=)

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }
};

class CSS_API cssSStream : public cssIOS {
  // owns its own sstream with ptr_cnt = 0: manages the construction and destruction of obj
public:
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr();
  cssSStream();
  cssSStream(const String& nm);
  cssSStream(const cssSStream& cp);
  cssSStream(const cssSStream& cp, const String&);
  ~cssSStream();

  cssCloneOnly(cssSStream);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssSStream((const String&)*(arg[1])); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }
  USING(cssIOS::operator=)

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }
};

class CSS_API cssLeafItr : public cssTA {
  // owns its own leafitr with ptr_cnt = 0: manages the construction and destruction of obj
public:
  static TypeDef*	TA_TypeDef(); // returns TA_taLeafItr
  uint		GetSize() const	{ return sizeof(*this); }

  // constructors
  void		Constr();
  cssLeafItr() 			: cssTA(NULL, 1, TA_TypeDef())   { Constr(); }
  cssLeafItr(const String& nm)		: cssTA(NULL, 1, TA_TypeDef(), nm)  { Constr(); }
  cssLeafItr(const cssLeafItr& cp)	: cssTA(cp) 	{ Constr(); }
  cssLeafItr(const cssLeafItr& cp, const String&)	: cssTA(cp)	{ Constr(); }
  ~cssLeafItr();

  cssCloneOnly(cssLeafItr);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssLeafItr(arg[1]->GetStr()); }

  void operator=(Real) 		{ CvtErr("(Real)"); }
  void operator=(Int)		{ CvtErr("(Int)"); }
  void operator=(const String&)	{ CvtErr("(String)"); }
  void operator=(void* cp)	{ ptr = cp; ptr_cnt = 1; }
  void operator=(void** cp)	{ ptr = (void*)cp; ptr_cnt = 2; }
  USING(cssTA::operator=)

  // operators
  void operator=(const cssEl&)		{ NopErr("="); }
};

class CSS_API cssTypeDef : public cssTA {
  // a pointer to a TypeDef (any number of ptr_cnt)
public:
  String	PrintStr() const;
  String	PrintFStr() const;
  String&	PrintType(String& fh) const;
  String&	PrintInherit(String& fh) const;

  // constructors
  cssTypeDef() : cssTA() { };
  cssTypeDef(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	     bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssTypeDef(const cssTypeDef& cp) : cssTA(cp) { };
  cssTypeDef(const cssTypeDef& cp, const String& nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssTypeDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTypeDef((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  operator TypeDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
  USING(cssTA::operator=)
};

class CSS_API cssMemberDef : public cssTA {
  // a pointer to a MemberDef (any number of ptr_cnt)
public:
  // constructors
  cssMemberDef() : cssTA() { };
  cssMemberDef(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	       bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssMemberDef(const cssMemberDef& cp) : cssTA(cp) { };
  cssMemberDef(const cssMemberDef& cp, const String& nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssMemberDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssMemberDef((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  operator MemberDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
  USING(cssTA::operator=)
};

class CSS_API cssMethodDef : public cssTA {
  // a pointer to a MethodDef (any number of ptr_cnt)
public:
  // constructors
  cssMethodDef() : cssTA() { };
  cssMethodDef(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	       bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssMethodDef(const cssMethodDef& cp) : cssTA(cp) { };
  cssMethodDef(const cssMethodDef& cp, const String& nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssMethodDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssMethodDef((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  operator MethodDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
  USING(cssTA::operator=)
};


class CSS_API cssTA_Matrix : public cssTA_Base {
  // a matrix ta base object -- handles all the matrix math magically..
INHERITED(cssTA_Base)
public:
  static bool IsMatrix(const cssEl& s);
  // check to see if the given item is also a cssTA_Matrix object -- must have valid ta base object pointer too
  static taMatrix* MatrixPtr(const cssEl& s);
  // return the matrix object from a given element known to be a matrix
  taMatrix* 	GetMatrixPtr() 	const { return (taMatrix*)GetTAPtr(); }
  // return matrix pointer for this object

  override bool	IsTaMatrix() const { return true; }

  cssTA_Matrix() : cssTA_Base()	    { }
  cssTA_Matrix(void* it, int pc, TypeDef* td, const String& nm = _nilString, cssEl* cls_par=NULL,
	       bool ro=false) : cssTA_Base(it,pc,td,nm,cls_par,ro) { };
  cssTA_Matrix(taMatrix* mtx);
  // this treats mtx arg as an OWN_OBJ and ref's it
  cssTA_Matrix(const cssTA_Matrix& cp) : cssTA_Base(cp) { };
  cssTA_Matrix(const cssTA_Base& cp, const String& nm) : cssTA_Base(cp, nm) { };
  ~cssTA_Matrix();

  cssCloneOnly(cssTA_Matrix);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA_Matrix((void*)NULL, ptr_cnt, type_def, arg[1]->GetStr()); }

  Variant GetVar() const 	{ return Variant(GetMatrixPtr()); }
  String GetStr() const;

  // void UpdateAfterEdit();

  cssEl* operator[](const Variant& idx) const;

  override bool AssignCheckSource(const cssEl& s);

  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const String& td) const;
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

#ifndef NO_TA_BASE
  operator int_Matrix*() const;
  operator byte_Matrix*() const;
  operator float_Matrix*() const;
  operator double_Matrix*() const;
  operator String_Matrix*() const;
  operator Variant_Matrix*() const;
#endif

  operator Real() const;
  operator Int() const;
  operator bool() const;

  void operator=(const cssEl& s);
  USING(cssTA_Base::operator=)

  cssEl* operator+(cssEl& t);
  cssEl* operator-(cssEl& t);
  cssEl* operator*(cssEl& t);
  cssEl* operator/(cssEl& t);
  cssEl* operator%(cssEl& t);
  cssEl* operator^(cssEl& s);

  cssEl* operator-();
  cssEl* operator*()		{ return cssTA_Base::operator*(); }

  void operator+=(cssEl& t);
  void operator-=(cssEl& t);
  void operator*=(cssEl& t);
  void operator/=(cssEl& t);
  void operator%=(cssEl& t);

  cssEl* operator! ();
  cssEl* operator&&(cssEl& s);
  cssEl* operator||(cssEl& s);

  cssEl* operator< (cssEl& s);
  cssEl* operator> (cssEl& s);
  cssEl* operator<=(cssEl& s);
  cssEl* operator>=(cssEl& s);
  cssEl* operator==(cssEl& s);
  cssEl* operator!=(cssEl& s);
};


#endif // css_ta.h
