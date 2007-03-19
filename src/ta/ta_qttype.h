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

class TA_API taiType_List : public taPtrList<taiType> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (taiType*)it; }

public:
  ~taiType_List()              { Reset(); }
};


//////////////////////////////
//       taiTypes         //
//////////////////////////////

class TA_API taiIntType : public taiType { // handles numeric int types, up to 32-bits
public:
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
  bool		handlesReadOnly() { return true; } // uses a RO iSpinBox

  TAQT_TYPE_INSTANCE(taiIntType, taiType);
};

class TA_API taiInt64Type : public taiType { // handles 64-bit numeric int types
public:
  int		BidForType(TypeDef* td);
//  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
//  void 		GetImage_impl(taiData* dat, const void* base);
//  void		GetValue_impl(taiData* dat, void* base);
//  bool		handlesReadOnly() { return true; } // uses a RO iSpinBox

  TAQT_TYPE_INSTANCE(taiInt64Type, taiType);
};




class TA_API taiEnumType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO iLineEdit w/ enum name
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiEnumType, taiType);
protected:
  bool		isBit; // true if a BIT type enum
  void		Initialize();
};

class TA_API taiBoolType : public taiType {
public:
  bool		handlesReadOnly() { return true; } // uses a RO iCheckBox
  int		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(taiBoolType, taiType);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
};

class TA_API taiStringType : public taiType { 
public:
  bool		handlesReadOnly() { return true; } 
  int		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(taiStringType, taiType);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
};


class TA_API taiVariantType : public taiType { 
public:
  bool		handlesReadOnly() { return true; } 
  int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiVariantType, taiType);
};


class TA_API taiClassType : public taiType {
INHERITED(taiType)
public:
  bool		allowsInline() const {return true;}
  bool		handlesReadOnly() { return true; } // uses a RO PolyData or RO EditButton
  int		BidForType(TypeDef* td);
  override taiData*	GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
  	taiType* parent_type_ = NULL, int flags = 0); // add in req for inline
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
  override bool		CanBrowse();

  TAQT_TYPE_INSTANCE(taiClassType, taiType);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  taiData*	GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class TA_API taiMatrixGeomType : public taiClassType { // special editor for matrix geoms
public:
  bool		requiresInline() const {return true;}
  int 		BidForType(TypeDef* td);

  TAQT_TYPE_INSTANCE(taiMatrixGeomType, taiClassType);
protected:
  taiData*	GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class TA_API gpiListType : public taiClassType {
public:
//  bool		allowsInline() const {return false;}
  int 		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(gpiListType, taiClassType);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class TA_API gpiGroupType : public gpiListType {
public:
  int 		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(gpiGroupType, gpiListType);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class TA_API gpiArray_Type : public taiClassType {
public:
  bool		allowsInline() const {return false;}
  int 		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(gpiArray_Type, taiClassType);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class TA_API taiTokenPtrType : public taiType {
INHERITED(taiType)
public:
  enum Mode {
    MD_BASE,		// taBase pointer
    MD_SMART_PTR,	// taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF	// taSmartRef
  };
  bool		handlesReadOnly() { return true; } // uses a RO tokenptr button
  
  taBase*	GetTokenPtr(const void* base) const; // depends on mode
  TypeDef*	GetMinType(const void* base);
  

  int		BidForType(TypeDef* td);
  TAQT_TYPE_INSTANCE(taiTokenPtrType, taiType);
protected:
  Mode 		mode; // set during first GetDataRep (is garbage until then) 
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void 		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
};

class TA_API taiTypePtr : public taiType {
  // typedef pointer
public:
  TypeDef*	orig_typ;

   int		BidForType(TypeDef* td);
   taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
   void		GetImage_impl(taiData* dat, const void* base);
   void		GetValue_impl(taiData* dat, void* base);

  void		Initialize();
  TAQT_TYPE_INSTANCE(taiTypePtr, taiType);
};

class TA_API taiFilePtrType : public taiType {// ptr to tai_getFiles
public:
   int		BidForType(TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
   void		GetImage_impl(taiData* dat, const void* base);
   void  	GetValue_impl(taiData* dat, void* base);

  TAQT_TYPE_INSTANCE(taiFilePtrType, taiType);
};


//////////////////////////////
//       taiEdits         //
//////////////////////////////

class TA_API taiDefaultEdit : public taiEdit {
public:
  int	BidForEdit(TypeDef*) { return 0; }
  TAQT_EDIT_INSTANCE(taiDefaultEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


//////////////////////////
// 	taiMember	//
//////////////////////////

class TA_API taiMember : public taiType {
public:
  static  void	EndScript(const void* base);
        // calling function has to use this function to end script if neccesary
  
  MemberDef*	mbr;

  taiMember* 		sub_types() {return (taiMember*)m_sub_types;}
  taiMember** 		addr_sub_types() {return (taiMember**)&m_sub_types;}
  bool			isCondShow() const;

   int		BidForType(TypeDef*) 			{return 0; }
	// none of the member specific ones should apply types
   virtual int	BidForMember(MemberDef*, TypeDef*)      {return 1;}
	// bid for (appropriateness) for given type of member (and members owner type)
//
//   // default member action is to pass thru to the type
//
  override taiData*	GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent,
  	taiType* parent_type_ = NULL, int flags = 0);
  // get taiData rep of type -- delegates to mbr's it
  override void		GetImage(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void		GetMbrValue(taiData* dat, void* base, bool& first_diff);
        // this is the one to call to get a member value (GetValue is not used)
  
  virtual TypeDef*	GetTargetType(const void* base); // for XxxDef* and token ptrs, returns the target type, which can be specified in several ways, or this can be overridden

//
  // script-generation code
  virtual void	GetOrigVal(taiData* dat, const void* base);
        // gets original value and adds it to the orig_vals list
  virtual void	StartScript(const void* base);
  virtual void	CmpOrigVal(taiData* dat, const void* base, bool& first_diff);
        // compares to original value and generates script code to change it

  taiMember(MemberDef* mb, TypeDef* td) : taiType(td)	{ mbr = mb; }
  taiMember()							{ mbr = NULL; }
  ~taiMember()						{ };

  virtual void 		AddMember(MemberDef* md);	// add an iv to a member
  virtual taiMember* 	MembInst(MemberDef* md, TypeDef* td)
  	{ return new taiMember(md, td);}
  TypeDef*	GetTypeDef() const {return &TA_taiMember;}
protected:
  override taiData*	GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_);
  virtual void		GetImage_CondShow(taiData* dat, const void* base);
  override void		GetImage_impl(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void		GetMbrValue_impl(taiData* dat, void* base);
  override bool		isReadOnly(taiData* dat, IDataHost* host_ = NULL); // used dlg, par, and member directives to determine if RO
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

class TA_API taiTokenPtrMember : public taiMember {
  // for taBase pointer members (allows scoping by owner obj)
INHERITED(taiMember)
public:
  enum Mode {
    MD_BASE,		// taBase pointer
    MD_SMART_PTR,	// taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF	// taSmartRef
  };
  
  bool		handlesReadOnly() { return true; } 
  TypeDef*	GetMinType(const void* base);
  
  taBase*	GetTokenPtr(const void* base) const; // depends on mode
  
  int		BidForMember(MemberDef* md, TypeDef* td);

  TAQT_MEMBER_INSTANCE(taiTokenPtrMember, taiMember);
protected:
  Mode 		mode; // set during first GetDataRep (is garbage until then) 
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiDefaultToken : public taiTokenPtrMember {
// Special edit menu for the TDefault's token member
INHERITED(taiTokenPtrMember)
public:
  TypeDefault*	tpdflt;

  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);

  TAQT_MEMBER_INSTANCE(taiDefaultToken, taiTokenPtrMember);
protected:
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize()	{ tpdflt = NULL; }
  void		Destroy() {}
};

class TA_API taiSubTokenPtrMember : public taiMember {
  // a token ptr that points to sub-objects of current object
INHERITED(taiMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);

  TAQT_MEMBER_INSTANCE(taiSubTokenPtrMember, taiMember);
protected:
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiTypePtrMember : public taiMember {
  // typedef ptrs that have member-comment directives
INHERITED(taiMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);

  TAQT_MEMBER_INSTANCE(taiTypePtrMember, taiMember);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiEnumTypePtrMember : public taiTypePtrMember {
  // typedef ptrs that have member-comment directives, Enums
INHERITED(taiTypePtrMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);

  TAQT_MEMBER_INSTANCE(taiEnumTypePtrMember, taiTypePtrMember);
protected:
   taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiMemberDefPtrMember : public taiMember {
  // pointer to a member-def
INHERITED(taiMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);

  TAQT_MEMBER_INSTANCE(taiMemberDefPtrMember, taiMember);
protected:
   taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiMethodDefPtrMember : public taiMember {
  // pointer to a MethodDef
INHERITED(taiMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);

  TAQT_MEMBER_INSTANCE(taiMethodDefPtrMember, taiMember);
protected:
   taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiFunPtrMember : public taiMember {
  // pointer to a function
INHERITED(taiMember)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);

  TAQT_MEMBER_INSTANCE(taiFunPtrMember, taiMember);
protected:
  override void	GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiCondEditMember : public taiMember {
  // conditional editing member
public:
  taiMember*	ro_im;		// member for read-only (no editing)
  bool		use_ro; // true if we are actually using the RO member

  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiCondEditMember, taiMember);
protected:
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void	GetImage_impl(taiData* dat, const void* base);
private:
  void		Initialize();
  void		Destroy() {}
};

class TypeDefault;

class TA_API taiTDefaultMember : public taiMember {
// special for the TypeDefault member (add the "active" box)
INHERITED(taiMember)
public:
  TypeDefault*	tpdflt;

  virtual int	BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(taiTDefaultMember, taiMember);
protected:
  override void	GetImage_impl(taiData* dat, const void* base);
private:
  void		Initialize()	{ tpdflt = NULL; }
  void		Destroy()	{ m_sub_types = NULL; } // prevent from being destroyed
};



//////////////////////////
// 	taiMethod	//
//////////////////////////

// the default method bids 0, so it doesn't get created, and is more of a virtual
// type

class TA_API taiMethod : public taiType {
public:
  MethodDef*	meth;

  taiMethod* 		sub_types() {return (taiMethod*)m_sub_types;}
  taiMethod** 		addr_sub_types() {return (taiMethod**)&m_sub_types;}

  int		BidForType(TypeDef*) 			{ return 0; }
  // none of the method specific ones should apply to types
  virtual int	BidForMethod(MethodDef*, TypeDef*) 	{ return 0; }
  // bid for (appropriateness) for given type of method (default is not at all approp.)

  taiMethodData*	GetMethodRep(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_);
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
  virtual taiMethodData* GetMethodRep_impl(void* base, IDataHost* host_, taiData* par,
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

class TA_API taiButtonMethod : public taiMethod {
public:
  int			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiButtonMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};

class TA_API taiMenuMethod : public taiMethod {
  // covers MENU and MENU_BUTTON types
public:
  int 			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiMenuMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};

/*class TA_API taiMenuButtonMethod : public taiMethod {
public:
  int			BidForMethod(MethodDef* md, TypeDef* td);

  TAQT_METHOD_INSTANCE(taiMenuButtonMethod, taiMethod);
protected:
  taiMethodData*	GetMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
}; */


//////////////////////////
// 	taiArgType	//
//////////////////////////

class TA_API taiArgType : public taiType {
  // unlike taiTypes, these are created and destroyed each time
  // thus, they cache their values
public:
  MethodDef*	meth;		// method that has the args
  int		arg_idx;	// index of argument in list of args
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

  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);

  virtual cssEl* GetElFromArg(const char* arg_nm, void* base);
  // this is responsible for setting arg_base and arg_val (base is parent base)

  // base passed here is of the parent object(!)
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  virtual bool	GetHasOption(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // check the meth->HasOption, also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx
  virtual String GetOptionAfter(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // get the meth->OptionAfter(opt), also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx

  taiArgType(int aidx, TypeDef* argt, MethodDef* mb, TypeDef* td);
  taiArgType();
  ~taiArgType();

  virtual taiArgType*  ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td)
  { return new taiArgType(aidx, argt,md,td);}
  TypeDef*	GetTypeDef() const {return &TA_taiArgType;}
private:
  void		Initialize() {}
  void		Destroy() {}
};

#define TAQT_ARGTYPE_INSTANCE(x,y) x(int aidx, TypeDef* argt, MethodDef* md, TypeDef*td)	\
  : y(aidx, argt,md,td) { Initialize(); }				\
x()            	{ Initialize(); } 						\
~x()           	{ Destroy(); }							\
  taiArgType* 	ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef*td) \
  { return (taiArgType*) new x(aidx, argt,md,td); }			\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}



class TA_API taiStreamArgType : public taiArgType {
  // for ios derived args (uses a file-requestor)
public:
  taFiler*	gf;
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);
  virtual void	GetValueFromGF(); // actually get the value from the getfile

  TAQT_ARGTYPE_INSTANCE(taiStreamArgType, taiArgType);
private:
  void Initialize();
  void Destroy();
};

class TA_API taiTokenPtrArgType : public taiArgType {
  // for pointers to tokens (uses appropriate scoping)
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(taiTokenPtrArgType, taiArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiTypePtrArgType : public taiArgType {
  // for typedef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(taiTypePtrArgType, taiArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiMemberPtrArgType : public taiArgType {
  // for memberdef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(taiMemberPtrArgType, taiArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API taiMethodPtrArgType : public taiArgType {
  // for methoddef ptr types
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(taiMethodPtrArgType, taiArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};



class TA_API gpiDefaultEl : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiDefaultEl, taiMember);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API gpiLinkGP : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiLinkGP, taiMember);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API gpiLinkList : public taiMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  TAQT_MEMBER_INSTANCE(gpiLinkList, taiMember);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API gpiFromGpTokenPtrMember : public taiTokenPtrMember {
public:
  int 		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  virtual MemberDef*	GetFromMd();
  virtual TABLPtr	GetList(MemberDef* from_md, const void* base);

  TAQT_MEMBER_INSTANCE(gpiFromGpTokenPtrMember, taiTokenPtrMember);
private:
  void		Initialize() {}
  void		Destroy() {}
};



//////////////////////////////////////////////////
// 	 taiArgTypes:   Lists and Groups	//
//////////////////////////////////////////////////

class TA_API gpiTAPtrArgType : public taiTokenPtrArgType {
  // for taBase pointers in groups, sets the typedef to be the right one..
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  TAQT_ARGTYPE_INSTANCE(gpiTAPtrArgType, taiTokenPtrArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API gpiInObjArgType : public gpiTAPtrArgType {
  // for taBase pointers in groups with ARG_IN_OBJ
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(gpiInObjArgType, gpiTAPtrArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

class TA_API gpiFromGpArgType : public taiTokenPtrArgType {
  // for taBase pointers with FROM_GROUP_xxx
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*	GetElFromArg(const char* arg_nm, void* base);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  virtual MemberDef*	GetFromMd();
  virtual TABLPtr	GetList(MemberDef* from_md, const void* base);

  TAQT_ARGTYPE_INSTANCE(gpiFromGpArgType, taiTokenPtrArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

//////////////////////////////////////////////////////////
// 		taiType: 	Array Edits		//
//////////////////////////////////////////////////////////

class TA_API gpiListEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiListEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class TA_API gpiArrayEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiArrayEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class TA_API SArgEdit : public gpiArrayEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(SArgEdit, gpiArrayEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};

class TA_API gpiSelectEdit : public taiEdit {
public:
  int		BidForEdit(TypeDef* td);
  TAQT_EDIT_INSTANCE(gpiSelectEdit, taiEdit);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};



//////////////////////////
//   Viewer Types 	//
//////////////////////////

class TA_API tabViewType: public taiViewType { // for taBase and descendants
INHERITED(taiViewType)
public:
  override int		BidForView(TypeDef*);
  override const iColor* GetEditColorInherit(taiDataLink* dl) const;// #IGNORE background color for edit
  override iDataPanel*	CreateDataPanel(taiDataLink* dl_); 
  override taiDataLink*	GetDataLink(void* data_, TypeDef* el_typ);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabViewType, taiViewType)
protected:
  virtual taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
private:
  EditDataPanel*	edit_panel; // cached during construction
};

class TA_API tabOViewType: public tabViewType { // for taOBase and descendants
#ifndef __MAKETA__
typedef tabViewType inherited;
#endif
public:
  override int		BidForView(TypeDef*);
  override taiDataLink*	GetDataLink(void* data_, TypeDef* el_typ); // optimized version of tabViewType
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabOViewType, tabViewType)
protected:
  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API tabListViewType: public tabOViewType { // for taList
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabListViewType, tabOViewType)
protected:
  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API tabGroupViewType: public tabListViewType { // for taGroup
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

