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


// tai_type.h: easily extendable interface support for all types

#ifndef TA_QTTYPE_H
#define TA_QTTYPE_H

#include "ta_qtdata.h"
#include "ta_filer.h"

#include <stdlib.h>

#include "ta_qttype_def.h"

// pre-declare classes:
class taiMember;
class taiMethod;
class taiArgType;
class taiData;
class TypeDef;
class TypeDefault;
class taiEditDataHost;

class taiType_List : public taPtrList<taiType> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (taiType*)it; }

public:
  ~taiType_List()              { Reset(); }
};


//////////////////////////////
//       taiTypes         //
//////////////////////////////

class taiIntType : public taiType {
public:
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);
  bool		handlesReadOnly() { return true; } // uses a RO iSpinBox

  TAQT_TYPE_INSTANCE(taiIntType, taiType);
};



class taiEnumType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO iLineEdit w/ enum name
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiEnumType, taiType);
protected:
  bool		isBit; // true if a BIT type enum
  void		Initialize();
};

class taiBoolType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO iCheckBox
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 	GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiBoolType, taiType);
};

class taiVariantType : public taiType { // TEMP: just use string values
public:
  int		BidForType(TypeDef* td);

  TAQT_TYPE_INSTANCE(taiVariantType, taiType);
};

class taiClassType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO PolyData or RO EditButton
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);
  override bool		CanBrowse();

  TAQT_TYPE_INSTANCE(taiClassType, taiType);
};

class taiStringType : public taiClassType { //note: inherits handlesReadOnly==true
public:
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);
  TAQT_TYPE_INSTANCE(taiStringType, taiClassType);
};

class taiMatrixGeomType : public taiClassType { // special editor for matrix geoms
public:
  int 		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiMatrixGeomType, taiClassType);
};

class gpiListType : public taiClassType {
public:
  int 		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(gpiListType, taiClassType);
};

class gpiGroupType : public gpiListType {
public:
  int 		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(gpiGroupType, gpiListType);
};

class gpiArray_Type : public taiClassType {
public:
  int 		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(gpiArray_Type, taiClassType);
};


// TODO: does anything actually use this???
class taiTokenPtrType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO EditButton
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiTokenPtrType, taiType);
};

class taiTypePtr : public taiType {
  // typedef pointer
public:
  TypeDef*	orig_typ;

   int		BidForType(TypeDef* td);
   taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
   void		GetImage_impl(taiData* dat, void* base);
   void		GetValue_impl(taiData* dat, void* base);

  void		Initialize();
  TAQT_TYPE_INSTANCE(taiTypePtr, taiType);
};

class taiFilePtrType : public taiType {// ptr to tai_getFiles
public:
   int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
   void		GetImage_impl(taiData* dat, void* base);
   void  	GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiFilePtrType, taiType);
};


//////////////////////////////
//       taiEdits         //
//////////////////////////////

class taiDefaultEdit : public taiEdit {
public:
  int	BidForEdit(TypeDef*) { return 0; }
  TAQT_EDIT_INSTANCE(taiDefaultEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


//////////////////////////
// 	taiMember	//
//////////////////////////

class taiMember : public taiType {
public:
  MemberDef*	mbr;

  taiMember* 		sub_types() {return (taiMember*)m_sub_types;}
  taiMember** 		addr_sub_types() {return (taiMember**)&m_sub_types;}

   int		BidForType(TypeDef*) 			{return 0; }
	// none of the member specific ones should apply types
   virtual int	BidForMember(MemberDef*, TypeDef*)      {return 1;}
	// bid for (appropriateness) for given type of member (and members owner type)
//
//   // default member action is to pass thru to the type
//
  override taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent,
  	taiType* parent_type_ = NULL);
  // get taiData rep of type -- delegates to mbr's it
  override void		GetImage(taiData* dat, void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  override void		GetValue(taiData* dat, void* base);
  // get the value from the representation -- same rules as GetDataRep

  virtual void	GetMbrValue(taiData* dat, void* base, bool& first_diff);
        // this is the one to call to get a member value
//
  // script-generation code
  virtual void	GetOrigVal(taiData* dat, void* base);
        // gets original value and adds it to the orig_vals list
  virtual void	StartScript(void* base);
  static  void	EndScript(void* base);
        // calling function has to use this function to end script if neccesary
  virtual void	CmpOrigVal(taiData* dat, void* base, bool& first_diff);
        // compares to original value and generates script code to change it

  taiMember(MemberDef* mb, TypeDef* td) : taiType(td)	{ mbr = mb; }
  taiMember()							{ mbr = NULL; }
  ~taiMember()						{ };

  virtual void 		AddMember(MemberDef* md);	// add an iv to a member
  virtual taiMember* 	MembInst(MemberDef* md, TypeDef* td)
  	{ return new taiMember(md, td);}
  TypeDef*	GetTypeDef() const {return &TA_taiMember;}
protected:
  override taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetImage_impl(taiData* dat, void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  override void		GetValue_impl(taiData* dat, void* base)
   	{ mbr->type->it->GetValue(dat, mbr->GetOff(base)); }
  override bool		isReadOnly(taiData* dat, taiDataHost* host_ = NULL); // used dlg, par, and member directives to determine if RO
};

#define TAQT_MEMBER_INSTANCE(x,y) x(MemberDef* md, TypeDef* td) 	\
: y(md,td) 	{ Initialize(); }					\
x()             { Initialize(); }        				\
~x()            { Destroy(); }						\
taiMember* 	MembInst(MemberDef* md, TypeDef* td)			\
{ return (taiMember*) new x(md, td); }					\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}


//////////////////////////////
//       taiMembers       //
//////////////////////////////

// these have BidforMember() functions which may depend on the opts
// of a member
/*
class taiROMember : public taiMember {
  // read-only member -- delegates to types that handle readOnly, otherwise defaults to readonly string
public:
  override bool	handlesReadOnly() {return true;}
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiROMember, taiMember);
};

*/
class taiTokenPtrMember : public taiMember {
  // for taBase pointer members (allows scoping by owner obj)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiTokenPtrMember, taiMember);
};

// Special edit menu for the TDefault's token member
class taiDefaultToken : public taiTokenPtrMember {
public:
  TypeDefault*	tpdflt;

  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);
  void		Initialize()	{ tpdflt = NULL; }

  TAQT_MEMBER_INSTANCE(taiDefaultToken, taiTokenPtrMember);
};

class taiSubTokenPtrMember : public taiMember {
  // a token ptr that points to sub-objects of current object
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiSubTokenPtrMember, taiMember);
};

class taiTypePtrMember : public taiMember {
  // typedef ptrs that have member-comment directives
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
   taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
   void		GetImage_impl(taiData* dat, void* base);
   void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiTypePtrMember, taiMember);
};

class taiMemberDefPtrMember : public taiMember {
  // pointer to a member-def
public:
  TypeDef*	tmbr; // type for the member
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiMemberDefPtrMember, taiMember);
};

class taiFunPtrMember : public taiMember {
  // pointer to a function
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiFunPtrMember, taiMember);
};

class taiCondEditMember : public taiMember {
  // conditional editing member
public:
  taiMember*	ro_im;		// member for read-only (no editing)
  bool		use_ro; // true if we are actually using the RO member

  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  taiCondEditMember(MemberDef* md, TypeDef* td);
  taiCondEditMember()             { Initialize(); }
  ~taiCondEditMember();
  taiMember* 	MembInst(MemberDef* md, TypeDef* td)
    { return (taiMember*) new taiCondEditMember(md, td); }
  TypeDef*	GetTypeDef() const {return &TA_taiCondEditMember;}
  void		Initialize();
protected:
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
};

class TypeDefault;

class taiTDefaultMember : public taiMember {
// special for the TypeDefault member (add the "active" box)
public:
  TypeDefault*	tpdflt;

  virtual int	BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);
  void		Initialize()	{ tpdflt = NULL; }
  void		Destroy()	{ m_sub_types = NULL; } // prevent from being destroyed

  TAQT_MEMBER_INSTANCE(taiTDefaultMember, taiMember);
};



//////////////////////////
// 	taiMethod	//
//////////////////////////

// the default method bids 0, so it doesn't get created, and is more of a virtual
// type

class taiMethod : public taiType {
public:
  MethodDef*	meth;

  taiMethod* 		sub_types() {return (taiMethod*)m_sub_types;}
  taiMethod** 		addr_sub_types() {return (taiMethod**)&m_sub_types;}

  int		BidForType(TypeDef*) 			{ return 0; }
  // none of the method specific ones should apply to types
  virtual int	BidForMethod(MethodDef*, TypeDef*) 	{ return 0; }
  // bid for (appropriateness) for given type of method (default is not at all approp.)

  taiMethodData*	GetMethodRep(void* base, taiDataHost* host_, taiData* par, QWidget* gui_parent_);
  override void		GetImage(taiData*, void*)	{ }
  override void		GetValue(taiData*, void*)	{ }

  taiMethod(MethodDef* mb, TypeDef* td) : taiType(td)	{ meth = mb; }
  taiMethod()						{ meth = NULL; }
  ~taiMethod()						{ };

  virtual void 		AddMethod(MethodDef* md);	// add an iv to a member

  virtual taiMethod* 	MethInst(MethodDef* md, TypeDef* td)
  	{ return new taiMethod(md,td);}
  TypeDef*	GetTypeDef() const {return &TA_taiMethod;}
protected:
  virtual taiMethodData* GetMethodRep_impl(void* base, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_) {return NULL;}
};

#define TAQT_METHOD_INSTANCE(x,y) x(MethodDef* md,TypeDef*td)		\
: y(md, td) 	{ Initialize(); }					\
x()             { Initialize(); } 					\
~x()            { Destroy(); }						\
taiMethod* 	MethInst(MethodDef* md,TypeDef*td) 			\
{ return (taiMethod*) new x(md,td); }					\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}



//////////////////////////////
//       taiMethods        //
//////////////////////////////

class taiButtonMethod : public taiMethod {
public:
  int			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiButtonMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};

class taiMenuMethod : public taiMethod {
  // covers MENU and MENU_BUTTON types
public:
  int 			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiMenuMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};

/*class taiMenuButtonMethod : public taiMethod {
public:
  int			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiMenuButtonMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
}; */


//////////////////////////
// 	taiArgType	//
//////////////////////////

class taiArgType : public taiType {
  // unlike taiTypes, these are created and destroyed each time
  // thus, they cache their values
public:
  MethodDef*	meth;		// method that has the args
  TypeDef*	arg_typ;	// which arg typedef this one is
  bool		err_flag;	// true if one of the args was improperly set

  taiType*	use_it;		// alternate iv type to use
  void*		arg_base;	// base value is computed for typedef
  cssEl*	arg_val;	// argument value (as a css element)
  taBase*	obj_inst;	// instance of taBase object for ptr=0 args

  int		BidForType(TypeDef*) 			{ return 0; }
  // none of the argtype specific ones should apply to types
  virtual int	BidForArgType(int, TypeDef*, MethodDef*, TypeDef*) 	{ return 1; }
  // bid for (appropriateness) for given type of method and argument type

  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);

  virtual cssEl* GetElFromArg(const char* arg_nm, void* base);
  // this is responsible for setting arg_base and arg_val (base is parent base)

  // base passed here is of the parent object(!)
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  taiArgType(TypeDef* argt, MethodDef* mb, TypeDef* td);
  taiArgType();
  ~taiArgType();

  void 	Initialize()	{};
  void 	Destroy()	{};
  virtual taiArgType*  ArgTypeInst(TypeDef* argt, MethodDef* md, TypeDef* td)
  { return new taiArgType(argt,md,td);}
  TypeDef*	GetTypeDef() const {return &TA_taiArgType;}
};

#define TAQT_ARGTYPE_INSTANCE(x,y) x(TypeDef* argt, MethodDef* md, TypeDef*td) 	\
: y(argt,md,td) { Initialize(); } 						\
x()            	{ Initialize(); } 						\
~x()           	{ Destroy(); }							\
taiArgType* 	ArgTypeInst(TypeDef* argt, MethodDef* md, TypeDef*td) 		\
{ return (taiArgType*) new x(argt,md,td); }					\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}



//////////////////////////////
//       taiArgTypes	    //
//////////////////////////////

class taiStreamArgType : public taiArgType {
  // for ios derived args (uses a file-requestor)
public:
  taFiler*	gf;
  String	filter; 	// our filter if any
  String	old_filter; 	// previous filter
  bool		old_compress;	// previous value of compress flag
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);
  virtual void	GetValueFromGF(); // actually get the value from the getfile

  void Initialize();
  void Destroy();
  TAQT_ARGTYPE_INSTANCE(taiStreamArgType, taiArgType);
};

class taiBoolArgType : public taiArgType {
  // for bool int types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);

  TAQT_ARGTYPE_INSTANCE(taiBoolArgType, taiArgType);
};

class taiTokenPtrArgType : public taiArgType {
  // for pointers to tokens (uses appropriate scoping)
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  void 	Initialize()	{};
  void 	Destroy()	{};
  TAQT_ARGTYPE_INSTANCE(taiTokenPtrArgType, taiArgType);
};

class taiTypePtrArgType : public taiArgType {
  // for typedef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  void 	Initialize()	{};
  void 	Destroy()	{};
  TAQT_ARGTYPE_INSTANCE(taiTypePtrArgType, taiArgType);
};

class taiMemberPtrArgType : public taiArgType {
  // for memberdef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  void 	Initialize()	{};
  void 	Destroy()	{};
  TAQT_ARGTYPE_INSTANCE(taiMemberPtrArgType, taiArgType);
};

class taiMethodPtrArgType : public taiArgType {
  // for methoddef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  void 	Initialize()	{};
  void 	Destroy()	{};
  TAQT_ARGTYPE_INSTANCE(taiMethodPtrArgType, taiArgType);
};


//////////////////////////////////////////////////////////
// 		taiMembers: 	 Groups			//
//////////////////////////////////////////////////////////
/* All these are subsumed under EditButton's ability to handle read_only

class taiROListMember: public taiROMember {
// allows one to view a listing of the members of a ReadOnly object that is a list or array
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiROListMember, taiROMember);
};


class taiROGroupMember: public taiROMember {
// allows one to view a listing of the members of a ReadOnly object that is a Group or array
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiROGroupMember, taiROMember);
};


class taiROArrayMember: public taiROMember {
// allows one to view a listing of the members of a ReadOnly object that is a array or array
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiROArrayMember, taiROMember);
};
*/

class gpiDefaultEl : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiDefaultEl, taiMember);
};

class gpiLinkGP : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiLinkGP, taiMember);
};

class gpiLinkList : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiLinkList, taiMember);
};

class gpiFromGpTokenPtrMember : public taiTokenPtrMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  virtual MemberDef*	GetFromMd();
  virtual TABLPtr	GetList(MemberDef* from_md, void* base);

  TAQT_MEMBER_INSTANCE(gpiFromGpTokenPtrMember, taiTokenPtrMember);
};



//////////////////////////////////////////////////
// 	 taiArgTypes:   Lists and Groups	//
//////////////////////////////////////////////////

class gpiTAPtrArgType : public taiTokenPtrArgType {
  // for taBase pointers in groups, sets the typedef to be the right one..
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  TAQT_ARGTYPE_INSTANCE(gpiTAPtrArgType, taiTokenPtrArgType);
};

class gpiInObjArgType : public gpiTAPtrArgType {
  // for taBase pointers in groups with ARG_IN_OBJ
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(gpiInObjArgType, gpiTAPtrArgType);
};

class gpiFromGpArgType : public taiTokenPtrArgType {
  // for taBase pointers with FROM_GROUP_xxx
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, void* base);
  void		GetValue_impl(taiData* dat, void* base);

  virtual MemberDef*	GetFromMd();
  virtual TABLPtr	GetList(MemberDef* from_md, void* base);

  TAQT_ARGTYPE_INSTANCE(gpiFromGpArgType, taiTokenPtrArgType);
};

//////////////////////////////////////////////////////////
// 		taiType: 	Array Edits		//
//////////////////////////////////////////////////////////

class gpiListEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiListEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class gpiGroupEdit : public gpiListEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiGroupEdit, gpiListEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class gpiArrayEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiArrayEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class SArgEdit : public gpiArrayEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(SArgEdit, gpiArrayEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class gpiSelectEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiSelectEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};



//////////////////////////
//   Viewer Types 	//
//////////////////////////

class tabViewType: public taiViewType { // for taBase and descendants
#ifndef __MAKETA__
typedef taiViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  override const iColor* GetEditColorInherit(taiDataLink* dl) const;// #IGNORE background color for edit
  override taiDataLink*	GetDataLink(void* data_);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabViewType, taiViewType)
protected:
  virtual taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class tabOViewType: public tabViewType { // for taOBase and descendants
#ifndef __MAKETA__
typedef tabViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  override taiDataLink*	GetDataLink(void* data_); // optimized version of tabViewType
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabOViewType, tabViewType)
protected:
  override taiDataLink*	CreateDataLink_impl(taBase* data_);
};

class tabListViewType: public tabOViewType { // for taList
#ifndef __MAKETA__
typedef tabOViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabListViewType, tabOViewType)
protected:
  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class tabGroupViewType: public tabListViewType { // for taGroup
#ifndef __MAKETA__
typedef tabListViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabGroupViewType, tabListViewType)
protected:
  override taiDataLink*	CreateDataLink_impl(taBase* data_);
};



#endif // tai_type_h

