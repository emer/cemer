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


#ifndef css_ta_h
#define css_ta_h 1

// for special eveptr class

#include "css_machine.h"
#include "css_basic_types.h"
#include "ta_type.h"

class CSS_API cssTA : public cssCPtr {
  // a pointer that has a TA TypeDef associated with it: uses type info to perform ops 
  // NOTE: specialized versions exist for specific types: those must be used (e.g., taBase, etc)
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
  cssTA(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL, bool ro = false);
  cssTA(const cssTA& cp);
  cssTA(const cssTA& cp, const char* nm);

  cssCloneOnly(cssTA);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  // converters
  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const char* td) const;
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

  // copying: uses typedef auto copy function for ptr_cnt = 0
  void operator=(const cssEl& s);
  void PtrAssignPtr(const cssEl& s);
  override bool AssignCheckSource(const cssEl& s);
  virtual bool AssignObjCheck(const cssEl& s);
  // do basic checks on us and source for object assign (ptr_cnt = 0)

  virtual cssEl*	GetElement_impl(taBase* ths, int i) const;

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

class CSS_API cssTA_Base : public cssTA {
  // specifically for taBase types -- calls the overloaded versions of TypeDef functions
public:
  void 		Print(ostream& fh = cout) const;
  void 		PrintR(ostream& fh = cout) const;		// recursive

  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  void		Save(ostream& fh = cout);
  void		Load(istream& fh = cin);

  // constructors
  void 		Constr();
  cssTA_Base() 						: cssTA()	    { Constr();}
  cssTA_Base(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	     bool ro=false) : cssTA(it,pc,td,nm,cls_par,ro)	{ Constr(); }
  cssTA_Base(const cssTA_Base& cp)			: cssTA(cp) 	    { Constr();}
  cssTA_Base(const cssTA_Base& cp, const char* nm)	: cssTA(cp,nm) 	    { Constr();}
  ~cssTA_Base();

  cssCloneOnly(cssTA_Base);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTA_Base((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  // converters
  taBase*  GetTAPtr() const 	{ return (taBase*)GetVoidPtr(); }

  operator taBase*() const	{ return GetTAPtr(); }
  operator taBase**() const	{ return (taBase**)GetVoidPtr(2); }
  Variant GetVar() const 	{ return Variant(GetTAPtr());}

  // operators
  void operator=(const String& s);
  void operator=(const cssEl& s); // use obj->UnSafeCopy for ptr_cnt == 0

  override bool PtrAssignPtrPtr(void* new_ptr_val);
  // use SetPointer..

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

class CSS_API cssSmartRef : public cssTA {
  // a taSmartRef object (ptr_cnt = 0)
public:
  void 		Print(ostream& fh = cout) const;
  void 		PrintR(ostream& fh = cout) const;	// recursive

  const char*	GetTypeName() const;
  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;
  void		TokenInfo(ostream& fh = cout) const;
  cssEl*	GetToken(int idx) const;

  // constructors
  cssSmartRef() : cssTA() { };
  cssSmartRef(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	      bool ro = false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssSmartRef(const cssSmartRef& cp) : cssTA(cp) { };
  cssSmartRef(const cssSmartRef& cp, const char* nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssSmartRef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssSmartRef((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  // converters
  void* 	GetVoidPtrOfType(TypeDef* td) const;
  void* 	GetVoidPtrOfType(const char* td) const;
  // these are type-safe ways to convert a cssEl into a ptr to object of given type

  String GetStr() const;
  operator void*() const;

  void operator=(const String& s);
  void operator=(const cssEl& s);

  // operators
  void PtrAssignPtr(const cssEl& s);
  void UpdateAfterEdit();

  cssEl* operator[](int) const;
  bool	MembersDynamic()	{ return true; }
  int	 GetMemberNo(const char* memb) const { return -1; } // never static lookup
  cssEl* GetMemberFmName(const char* memb) const;
  cssEl* GetMemberFmNo(int memb) const;
  int	 GetMethodNo(const char* memb) const { return -1; }
  cssEl* GetMethodFmName(const char* memb) const;
  cssEl* GetMethodFmNo(int memb) const;
  cssEl* GetScoped(const char*) const;
};

class CSS_API cssIOS : public cssTA {
  // a pointer to an iostream object of any sort: supports various streaming ops
public:
  String	PrintFStr() const;
  String	GetStr() const;

  // constructors
  cssIOS() : cssTA() { };
  cssIOS(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	 bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssIOS(const cssIOS& cp) : cssTA(cp) { };
  cssIOS(const cssIOS& cp, const char* nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssIOS);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssIOS((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

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
  cssFStream(const char* nm)			: cssIOS(NULL, 1, TA_TypeDef(), nm)  { Constr(); }
  cssFStream(const cssFStream& cp)		: cssIOS(cp) 	{ Constr(); }
  cssFStream(const cssFStream& cp, const char*)	: cssIOS(cp)	{ Constr(); }
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
};

class CSS_API cssSStream : public cssIOS {
  // owns its own sstream with ptr_cnt = 0: manages the construction and destruction of obj
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
};

class CSS_API cssLeafItr : public cssTA {
  // owns its own leafitr with ptr_cnt = 0: manages the construction and destruction of obj
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
};

class CSS_API cssTypeDef : public cssTA {
  // a pointer to a TypeDef (any number of ptr_cnt)
public:
  void 		Print(ostream& fh = cout) const;
  void 		PrintR(ostream& fh = cout) const;	// recursive

  const char*	GetTypeName() const;
  void 		TypeInfo(ostream& fh = cout) const;
  void		InheritInfo(ostream& fh = cout) const;

  // constructors
  cssTypeDef() : cssTA() { };
  cssTypeDef(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	     bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssTypeDef(const cssTypeDef& cp) : cssTA(cp) { };
  cssTypeDef(const cssTypeDef& cp, const char* nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssTypeDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssTypeDef((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  operator TypeDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
};

class CSS_API cssMemberDef : public cssTA {
  // a pointer to a MemberDef (any number of ptr_cnt)
public:
  // constructors
  cssMemberDef() : cssTA() { };
  cssMemberDef(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	       bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssMemberDef(const cssMemberDef& cp) : cssTA(cp) { };
  cssMemberDef(const cssMemberDef& cp, const char* nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssMemberDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssMemberDef((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  operator MemberDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
};

class CSS_API cssMethodDef : public cssTA {
  // a pointer to a MethodDef (any number of ptr_cnt)
public:
  // constructors
  cssMethodDef() : cssTA() { };
  cssMethodDef(void* it, int pc, TypeDef* td, const char* nm=NULL, cssEl* cls_par=NULL,
	       bool ro=false) : cssTA(it, pc, td, nm, cls_par, ro) { };
  cssMethodDef(const cssMethodDef& cp) : cssTA(cp) { };
  cssMethodDef(const cssMethodDef& cp, const char* nm) : cssTA(cp, nm) { };
  cssCloneOnly(cssMethodDef);
  cssEl*	MakeToken_stub(int, cssEl *arg[])
  { return new cssMethodDef((void*)NULL, ptr_cnt, type_def, (const char*)*(arg[1])); }

  operator MethodDef*() const;
  String GetStr() const;
  void operator=(const String& s);
  void operator=(const cssEl& s);
};


#endif // css_ta.h
