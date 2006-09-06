/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef ta_base_h
#define ta_base_h 1

#include "ta_type.h"
#include "icolor.h"
//#include "ta_qtbrowse_def.h"
#include "ta_TA_type.h"

#ifndef __MAKETA__
#ifdef TA_GUI
# include <qobject.h>
#endif
#endif


// externals
class SelectEdit;
class taFiler;

#ifdef TA_GUI
class taiMimeSource; //
#endif

// forwards
class taBase;
class taSmartRef;
class taOBase;
class taDataView;
class taNBase;
class taRootBase;
class taBase_List;
class taBase_PtrList;
class String_Array;

class TA_API tabMisc {
  // #NO_TOKENS #INSTANCE miscellaneous useful stuff for taBase
public:
  static taRootBase*	root;
  // root of the structural object hierarchy
  static taBase_PtrList	delayed_remove;
  // list of objs to be removed in the wait process (e.g. when objs delete themselves)
  static taBase_PtrList	delayed_updateafteredit;
  // list of objs to be update-after-edit'd in the wait process

  static void		Close_Obj(TAPtr obj);
  // call this to implement closing object function
  static void		WaitProc();
  // wait process function: remove objects from groups, update others
//obs  static bool	NotifyEdits(TAPtr obj);
  // notify any edit dialogs of a taptr object that object has changed
  static void		DelayedUpdateAfterEdit(TAPtr obj);
  // call update-after-edit on object in wait process (in case this does other kinds of damage..)
};

#define TA_BASEFUNS(y) 	y () { Register(); Initialize(); SetDefaultName(); } \
			y (const y& cp) { Register(); Initialize(); Copy(cp); } \
			~y () { unRegister(); Destroy(); } \
			TAPtr Clone() { return new y(*this); }  \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y& rf = *((y*)cp); rf.Copy(*this); } \
			TAPtr MakeToken(){ return (TAPtr)(new y); } \
			TAPtr MakeTokenAry(int no){ return (TAPtr)(new y[no]); } \
			void operator=(const y& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }

#define TA_BASEFUNS_LITE(y) y () {Initialize();} \
			y (const y& cp) {Initialize(); Copy(cp); } \
			~y () {Destroy();} \
			TAPtr Clone() { return new y(*this); }  \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y& rf = *((y*)cp); rf.Copy(*this); } \
			TAPtr MakeToken(){ return (TAPtr)(new y); } \
			TAPtr MakeTokenAry(int no){ return (TAPtr)(new y[no]); } \
			void operator=(const y& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }

// macro for abstract base classes
#define TA_ABSTRACT_BASEFUNS(y) y () { Register(); Initialize(); } \
			y (const y& cp) { Register(); Initialize(); Copy(cp); } \
			~y () {unRegister(); Destroy(); } \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y& rf = *((y*)cp); rf.Copy(*this); } \
			void operator=(const y& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }

// use this when you have consts in your class and can't use the generic constrs
#define TA_CONST_BASEFUNS(y) ~y () { unRegister(); Destroy(); } \
			TAPtr Clone() { return new y(*this); }  \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y& rf = *((y*)cp); rf.Copy(*this); } \
			TAPtr MakeToken(){ return (TAPtr)(new y); } \
			TAPtr MakeTokenAry(int no){ return (TAPtr)(new y[no]); } \
			void operator=(const y& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }

// for use with templates
#define TA_TMPLT_BASEFUNS(y,T) y () { Register(); Initialize(); SetDefaultName(); } \
			y (const y<T>& cp) { Register(); Initialize(); Copy(cp); } \
			~y () { unRegister(); Destroy(); } \
			TAPtr Clone() { return new y<T>(*this); } \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y<T>*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y<T>& rf = *((y<T>*)cp); rf.Copy(*this); } \
			TAPtr MakeToken(){ return (TAPtr)(new y<T>); }  \
			TAPtr MakeTokenAry(int no){ return (TAPtr)(new y<T>[no]); }  \
			void operator=(const y<T>& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }

//for abstract templates
#define TA_ABSTRACT_TMPLT_BASEFUNS(y,T) y () { Initialize(); } \
			y (const y<T>& cp) { Initialize(); Copy(cp); } \
			~y () {Destroy(); } \
			void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(&TA_##y)) Copy(*((y<T>*)cp)); \
						     else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
			void  CastCopyTo(TAPtr cp) { y<T>& rf = *((y<T>*)cp); rf.Copy(*this); } \
			void operator=(const y& cp) { Copy(cp); } \
			TypeDef* GetTypeDef() const { return &TA_##y; } \
			static TypeDef* StatTypeDef(int) { return &TA_##y; }


//WARNING: _INLINE is not very stable, since it assumes registration before any access to StatTypeDef...

#define TA_BASEFUNS_INLINE(y) 	\
        y () { Register(); Initialize(); SetDefaultName(); } \
        y (const y& cp) { Register(); Initialize(); Copy(cp); } \
        ~y () { unRegister(); Destroy(); } \
        TAPtr Clone() { return new y(*this); }  \
        void  UnSafeCopy(TAPtr cp) { if(cp->InheritsFrom(y::StatTypeDef(0))) Copy(*((y*)cp)); \
                                      else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
        void  CastCopyTo(TAPtr cp) { y& rf = *((y*)cp); rf.Copy(*this); } \
        TAPtr MakeToken(){ return (TAPtr)(new y); } \
        TAPtr MakeTokenAry(int no){ return (TAPtr)(new y[no]); } \
        void operator=(const y& cp) { Copy(cp); } \
        TypeDef* GetTypeDef() const { return StatTypeDef(0); } \
        static TypeDef* StatTypeDef(int) { static TypeDef* type_def = NULL; \
            if (type_def == NULL) type_def = GetTypeDefByName(#y); \
            return type_def; }


// for use with templates
#define TA_TMPLT_TYPEFUNS(y,T) \
        TypeDef* GetTypeDef() const { return &TA_##y; } \
        static TypeDef* StatTypeDef(int) {  return &TA_##y; }

// NOTE: the generic copy assumes that a Copy is a Structural updating operation, and
// so brackets the actual copy with the beg/end struct update
#define COPY_FUNS(T, P)   void Copy(const T& cp) \
   {StructUpdate(true); P::Copy(cp); Copy_(cp); StructUpdate(false);}

#define SIMPLE_COPY(T)	  void Copy_(const T& cp)	{T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); }

#define	SIMPLE_INITLINKS() void InitLinks() { inherited::InitLinks(); InitLinks_taAuto(); }
#define	SIMPLE_CUTLINKS()  void CutLinks() { CutLinks_taAuto(); inherited::CutLinks(); }

#define TA_SIMPLE_BASEFUNS(T)  SIMPLE_COPY(T); SIMPLE_INITLINKS(); SIMPLE_CUTLINKS(); TA_BASEFUNS(T)

// simplified Get owner functions B = ta_base object, T = class name
#define GET_MY_OWNER(T) (T *) GetOwner(&TA_##T)
#define GET_OWNER(B,T)  (T *) B ->GetOwner(T::StatTypeDef(0))

#define SET_POINTER(var,obj) (taBase::SetPointer((TAPtr*)&(var), (TAPtr)(obj)))
#define DEL_POINTER(var) (taBase::DelPointer((TAPtr*)&(var)))

/* Clipboard (Edit) operation summary

   Clipboard operations are of two basic types:
     Src -- source operations: Cut, Copy, Delete
     Dst -- destination ops: Paste, Link, etc.

   Clipboard API calls are of two types, and several subtypes:
     Query -- determines allowed operations (ex., to control UI enabling of Cut, Copy, etc.)
     Action -- perform the indicated action

   Src ops support both single and multi selected items.
   Dst ops support a single selected item, and single or multi items on the clipboard.

   There must always be at least one item selected in the UI to allow calling of clipboard functions.

   Data that is already on the clipboard is passed using a taiMimeSource iterator object
   defined in ta_qtclipdata.h -- this object supports both single and multi-item data.

   For all ops below, ms=NULL indicates a Src-op (CUT, COPY, etc.).

*/

class TA_API taBase {
  // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER Base type for all type-aware classes
  // has auto instances for all taBases unless NO_INSTANCE
friend class taSmartRef;
friend class taDataView;
friend class taBase_PtrList;
friend class taList_impl;
public:
  enum Orientation { // must be same values as Qt::Orientation
    Horizontal = 0x1,
    Vertical = 0x2
  };

  enum ValType { // the basic data types widely supported by data-handling api's, esp. matrices
    VT_STRING,		// an ANSI string of any length
    VT_FLOAT,		// a 4-byte floating point value (aprox 7 sig decimal digits)
    VT_INT,		// a 32-bit signed integer
    VT_BYTE,		// an unsigned 8-bit integer; used mostly for image components (rgb)
    VT_VARIANT		// a Variant, which can hold scalars, matrices, and objects
  };
  
  static const String 	ValTypeToStr(ValType vt);
  static ValType	ValTypeForType(TypeDef* td); // return the appropriate ValType
  static  TypeDef*	StatTypeDef(int);	// #IGNORE
  static TAPtr 		MakeToken(TypeDef* td);
  // #IGNORE make a token of the given type
  static TAPtr 		MakeTokenAry(TypeDef* td, int no);
  // #IGNORE make an array of tokens of the given type

  static String		GetStringRep(const taBase& it) {return it.GetStringRep_impl();}
   // #IGNORE string representation
  static String		GetStringRep(TAPtr it); 
  // string representation; ok if null, calls ->GetStringRep_impl
  
  // Reference counting mechanisms, all static just for consistency..
  static int		GetRefn(TAPtr it)	{ return it->refn; } // #IGNORE
  static void  		Ref(taBase& it)	{ it.refn++; }	     // #IGNORE
  static void  		Ref(TAPtr it) 	{ it->refn++; }	     // #IGNORE
#ifdef DEBUG
  static void		UnRef(TAPtr it);// #IGNORE
#else
  static void		UnRef(TAPtr it) { if (--(it->refn) == 0) delete it;}// #IGNORE
#endif
  static void		Own(taBase& it, TAPtr onr);	// #IGNORE note: also does a RefStatic() on first ownership
  static void		Own(TAPtr it, TAPtr onr);	// #IGNORE note: also does a Ref() on new ownership
  static void		Own(taSmartRef& it, TAPtr onr);	// #IGNORE for semantic compat with other Owns
  static taFiler*	StatGetFiler(TypeItem* td);
    // #IGNORE gets file dialog for the TypeItem -- clients must ref/unrefdone

protected: // legacy ref counting routines, for compatability -- do not use for new code
  static void   	unRef(TAPtr it) { it->refn--; }	     // #IGNORE
#ifdef DEBUG
  static void   	Done(TAPtr it); // #IGNORE
#else
  static void   	Done(TAPtr it) 	{ if (it->refn == 0) delete it;} // #IGNORE
#endif
  static void		unRefDone(TAPtr it) 	{unRef(it); Done(it);}	 // #IGNORE

public:
  // Pointer management routines (all pointers should be ref'd!!)
  static void 		InitPointer(TAPtr* ptr) { *ptr = NULL; } // #IGNORE
  static void 		SetPointer(TAPtr* ptr, TAPtr new_val);	 // #IGNORE
  static void 		OwnPointer(TAPtr* ptr, TAPtr new_val, TAPtr onr); // #IGNORE
  static void 		DelPointer(TAPtr* ptr);				  // #IGNORE

  static int		NTokensInScope(TypeDef* type, TAPtr ref_obj, TypeDef* scp_tp=NULL);
  // #IGNORE number of tokens of taBase objects of given type in same scope as ref_obj

  // utility functions for doing path stuff
  static int		GetNextPathDelimPos(const String& path, int start);
  // #IGNORE get the next delimiter ('.' or '[') position in the path
  static int		GetLastPathDelimPos(const String& path);
  // #IGNORE get the last delimiter ('.' or '[') position in the path

public: // standard keys for standard gui descriptions, mostly for list columns
  static const KeyString key_name; // #IGNORE "name" -- Name, note: can easily be empty
  static const KeyString key_type; // #IGNORE "type" -- def to typename, but some like progvar append their own subtype
  static const KeyString key_type_desc; // #IGNORE "type_desc" -- static type description
  static const KeyString key_desc; // #IGNORE "desc" -- per-instance desc if available (def to type)
  static const KeyString key_disp_name; // #IGNORE "disp_name" -- DisplayName, never empty
  
  virtual String	GetColText(const KeyString& key, int itm_idx = -1) const;
  // #IGNORE default keys are: name, type, desc, disp_name
#ifdef TA_GUI
public:
//friend class tabDataLink;
  virtual void		ChildQueryEditActions(const MemberDef* md, const taBase* child, taiMimeSource* ms,
    int& allowed, int& forbidden);
    // gives ops allowed on child, with ms being clipboard or drop contents, md valid if we are a member, o/w NULL
  virtual int		ChildEditAction(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea);
  // note: multi source ops will have child=NULL
  virtual void		QueryEditActions(const taiMimeSource* ms, int& allowed, int& forbidden)
    {QueryEditActions_impl(ms, allowed, forbidden);} // ms is null for Src-op query
  virtual int		EditAction(taiMimeSource* ms, int ea)
    {return EditAction_impl(ms, ea);}
protected:
    // gives ops allowed on child, with ms being clipboard or drop contents, md valid if we are a member, o/w NULL
  virtual int		ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea);
  // note: multi source ops will have child=NULL
  virtual int		ChildEditActionS_impl(const MemberDef* md, taBase* child, int ea) {return 0;}
    // src op implementation (non-list/grp)
  virtual int		ChildEditActionD_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea) {return 0;}
    // dst op implementation (non-list/grp)
  virtual void		ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden);
  virtual int		EditAction_impl(taiMimeSource* ms, int ea);
  virtual void		QueryEditActions_impl(const taiMimeSource* ms, int& allowed, int& forbidden);
#endif
protected:
  virtual String 	ChildGetColText_impl(taBase* child, const KeyString& key, 
    int itm_idx = -1) const {return _nilKeyString;}
  
public:
  virtual taDataLink* 	data_link() {return NULL;} // #IGNORE link for viewer system created when needed, deleted when 0 clients -- all delegated functions must be of form: if(data_link()) data_link->SomeFunc(); NOT autocreated by call to this func -- call GetDataLink() to force creation
  
  virtual taDataLink* 	GetDataLink(); // forces creation; can still be NULL if the type doesn't support datalinks
  void			AddDataClient(IDataLinkClient* dlc); //note: only applicable for classes that implement datalinks
  bool			RemoveDataClient(IDataLinkClient* dlc); // WARNING: link is undefined after this 
  
//temp  int			Ref() {return ++refn;}
//temp  virtual void		UnRef() {--refn;} //note: overridden by ref-semantic classes to delete
//temp  void			UnRefNoDelete() {--refn;}

  virtual void		InitLinks()		{ };
  // #IGNORE initialize links to other objs, called after construction & SetOwner, call parent
  virtual void		CutLinks();
  // #IGNORE cut any links to other objs, called upon removal from a group or owner
  void			Copy(const taBase&)	{ };
  // #IGNORE the copy (=) operator, call parent

  virtual void		InitLinks_taAuto();
  // #IGNORE automatic TA-based initlinks: calls inherited and goes through only my members & owns them
  virtual void		CutLinks_taAuto();
  // #IGNORE automatic TA-based cutlinks: goes through only my members & calls cutlinks and calls inherited

  void 			Register()			// #IGNORE non-virtual, called in constructors
  { if(!taMisc::not_constr) GetTypeDef()->Register((void*)this); }
  void 			unRegister()			// #IGNORE non-virtual, called in destructors
  { if(!taMisc::not_constr) GetTypeDef()->unRegister((void*)this); }

  // these are all "free" with the TA_BASEFUNS
  void			Initialize()		{ refn = 0; }
  // #IGNORE initializer; same considerations as Destroy
  void			Destroy(); // #IGNORE destroy for classes that don't need their own -- MIGHT BE CALLED MULTIPLE TIMES
  taBase()					{ Register(); Initialize(); }
  taBase(taBase& cp)				{ Register(); Initialize(); Copy(cp); }
  taBase(int)					{ Initialize(); } // don't register...
  virtual ~taBase() 				{ Destroy(); }

  virtual TAPtr		Clone()			{ return new taBase(*this); } // #IGNORE
  virtual void		UnSafeCopy(TAPtr)	{ };
  virtual void		CastCopyTo(TAPtr)	{ };
  virtual TAPtr 	MakeToken()		{ return new taBase; }	// #IGNORE
  virtual TAPtr 	MakeTokenAry(int no)	{ return new taBase[no]; } // #IGNORE
  void 			operator=(const taBase& cp)	{ Copy(cp); }
  virtual TypeDef*	GetTypeDef() const;	// #IGNORE

  virtual bool		SetName(const String& nm) {return false;}
  virtual String	GetName() const 	{ return _nilString; } // #IGNORE
  virtual String	GetDisplayName() const; // #IGNORE can be overridden to provide synthetic name, or to strip out chars from mangled names (ex. DataTable column names) -- will/must never be empty
  virtual String	GetDesc() const {return _nilString;} // a type-specific "description" of the instance
  virtual bool		SetFileName(const String& val)  {return false;} // #IGNORE
  virtual String	GetFileName() const 	{ return _nilString; } // #IGNORE
  taFiler*		GetFiler(TypeItem* td = NULL);
    // #IGNORE gets filer for the TypeItem if specified, or this object-- clients must ref/unrefdone
  virtual void 		SetDefaultName();			    // #IGNORE
  virtual void		SetTypeDefaults();			    // #IGNORE
  virtual void		SetTypeDefaults_impl(TypeDef* ttd, TAPtr scope); // #IGNORE
  virtual void		SetTypeDefaults_parents(TypeDef* ttd, TAPtr scope); // #IGNORE

  virtual void* 	GetTA_Element(int, TypeDef*& eltd) 
    { eltd = NULL; return NULL; } // #IGNORE a bracket opr
  virtual TAPtr 	SetOwner(TAPtr)		{ return(NULL); } // #IGNORE
  virtual TAPtr 	GetOwner() const	{ return(NULL); }
  virtual TAPtr		GetOwner(TypeDef* td) const;
  virtual String 	GetPath_Long(TAPtr ta=NULL, TAPtr par_stop=NULL) const;
  // #IGNORE get path from root (default), but stop at par_stop if non-null
  virtual String	GetPath(TAPtr ta=NULL, TAPtr par_stop=NULL) const;
  // get path without name informtation, stop at par_stop if non-null
  virtual TAPtr		FindFromPath(const String& path, MemberDef*& ret_md=no_mdef, int start=0) const;
  // find object from path (starting from this, and position start of the path)

  // functions for managing structural scoping
  virtual TypeDef*	GetScopeType();
  // #IGNORE gets my scope type (if NULL, it means no scoping, or root)
  virtual TAPtr		GetScopeObj(TypeDef* scp_tp=NULL);
  // #IGNORE gets the object that is at the scope type above me (uses GetScopeType() or scp_tp)
  virtual bool		SameScope(TAPtr ref_obj, TypeDef* scp_tp=NULL);
  // #IGNORE determine if this is in the same scope as given ref_obj (uses my scope type)
  virtual TAPtr		New(int n_objs=0, TypeDef* type=NULL);
  // Create n_objs objects of given type (type is optional)
  virtual uint		GetSize() const		{ return GetTypeDef()->size; }  // #IGNORE

  virtual void		UpdateAfterEdit();
  // called after editing, or any user change to members (eg. in the interface, script)
  virtual void		ChildUpdateAfterEdit(TAPtr child, bool& handled);
   // #IGNORE called by a child in its UAE routine; provides child notifications\nNOTE: only member objects are detected; subclasses that want to notify on owned TAPtr members must override and check for those instances manually
  virtual void		UpdateAllViews();
  // called after data changes, to update views
  virtual void 		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
    // sends the indicated notification to all datalink clients, if any;\nvirtual so we can override to trap/monitor
  void			StructUpdate(bool begin) {BatchUpdate(begin, true);}
  // bracket structural changes with (nestable) true/false calls;
  void			DataUpdate(bool begin) {BatchUpdate(begin, false);}
  // bracket data value changes with (nestable) true/false calls;

  // viewing-related functions -- usually not overridden base
  virtual void		AddDataView(taDataView* dv); // add a dataview (not usually overridden)
  virtual bool		RemoveDataView(taDataView* dv); // removes a dataview (not usually overridden)
protected:
  virtual void		DataViewAdding(taDataView* dv); // adds a dataview that has been validated
  virtual void		DataViewRemoving(taDataView* dv) {} // signals removal of dataview
  virtual void		BatchUpdate(bool begin, bool struc);
  // bracket changes with (nestable) true/false calls; data clients can use it to supress change updates
  virtual void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {}
    // the obj (to which we have a ref) is about to destroy (the ref will null its ptr on return)
  virtual void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_) {}
    // the obj (to which we have a ref) has signalled the indicated data change

public:
  // the following are selected functions from TypeDef
  bool		HasOption(const char* op) const		// #IGNORE
  { return GetTypeDef()->HasOption(op); }
  bool		CheckList(const String_PArray& lst) const // #IGNORE
  { return GetTypeDef()->CheckList(lst); }

  bool 		InheritsFrom(TypeDef* it) const		{ return GetTypeDef()->InheritsFrom(it); }
  bool		InheritsFrom(const TypeDef& it) const	{ return GetTypeDef()->InheritsFrom(it); }
  bool 		InheritsFrom(const char* nm) const	// does this inherit from given type?
  { return GetTypeDef()->InheritsFrom(nm); }

  bool		InheritsFormal(TypeDef* it) const	// #IGNORE
  { return GetTypeDef()->InheritsFormal(it); }
  bool		InheritsFormal(const TypeDef& it) const	// #IGNORE
  { return GetTypeDef()->InheritsFormal(it); }

  virtual MemberDef*	FindMember(const String& nm, int& idx=no_idx) const // #IGNORE
  { return GetTypeDef()->members.FindName(nm, idx); }
  virtual MemberDef*	FindMember(TypeDef* it, int& idx=no_idx) const 	// #IGNORE
  { return GetTypeDef()->members.FindType(it, idx); }
  virtual MemberDef* 	FindMember(void* mbr, int& idx=no_idx) const 	// #IGNORE
  { return GetTypeDef()->members.FindAddr((void*)this, mbr, idx); }
  virtual MemberDef* 	FindMemberPtr(void* mbr, int& idx=no_idx) const	// #IGNORE
  { return GetTypeDef()->members.FindAddrPtr((void*)this, mbr, idx); }	// #IGNORE

  // these get overwritten by groups, lists, etc.
  virtual MemberDef* 	FindMembeR(const String& nm, void*& ptr) const 	// #IGNORE
    { return GetTypeDef()->members.FindNameAddrR(nm, (void*)this, ptr); }
  virtual MemberDef* 	FindMembeR(TypeDef* it, void*& ptr) const	// #IGNORE
    { return GetTypeDef()->members.FindTypeAddrR(it, (void*)this, ptr); }

  virtual bool		FindCheck(const String& nm) const // #IGNORE check this for the name
    { return ((GetName() == nm) || InheritsFrom(nm)); }

  virtual String	GetEnumString(const String& enum_tp_nm, int enum_val) const
    { return GetTypeDef()->GetEnumString(enum_tp_nm, enum_val); }
  // get the name corresponding to given enum value in enum type enum_tp_nm
  virtual int		GetEnumVal(const String& enum_nm, String& enum_tp_nm = no_name) const
    { return GetTypeDef()->GetEnumVal(enum_nm, enum_tp_nm); }
  // get the enum value corresponding to the given enum name (-1 if not found), and sets enum_tp_nm to name of type this enum belongs in (empty if not found)

  virtual void		CopyFromSameType(void* src_base) 	// #IGNORE
    { GetTypeDef()->CopyFromSameType((void*)this, src_base); }
  virtual void		CopyOnlySameType(void* src_base)
    { GetTypeDef()->CopyOnlySameType((void*)this, src_base); }
  // #IGNORE copy only those members from same type (no inherited)
  virtual void		MemberCopyFrom(int memb_no, void* src_base) // #IGNORE
    { GetTypeDef()->MemberCopyFrom(memb_no, (void*)this, src_base); }

  virtual ostream&  	OutputType(ostream& strm) const		// #IGNORE
    { return GetTypeDef()->OutputType(strm); }
  virtual ostream&  	OutputInherit(ostream& strm) const 	// #IGNORE
    { return GetTypeDef()->OutputInherit(strm); }
  virtual ostream&  	OutputTokens(ostream& strm) const	//#IGNORE
    { GetTypeDef()->tokens.List(strm); return strm; }

  virtual ostream& 	Output(ostream& strm, int indent = 0) const // #IGNORE
    { return GetTypeDef()->Output(strm, (void*)this, indent); }
  virtual ostream& 	OutputR(ostream& strm, int indent = 0) const // #IGNORE
    { return GetTypeDef()->OutputR(strm, (void*)this, indent); }

  virtual int	 	Load(istream& strm, TAPtr par=NULL, void** el = NULL)
  // #MENU #MENU_ON_Object #ARGC_1 #UPDATE_MENUS #MENU_CONTEXT Load object data from a file
    { return GetTypeDef()->Dump_Load(strm, (void*)this, par, el); }
  virtual int	 	Dump_Load_impl(istream& strm, TAPtr par=NULL) // #IGNORE
    { return GetTypeDef()->Dump_Load_impl(strm, (void*)this, par); }
  virtual int	 	Dump_Load_Value(istream& strm, TAPtr par=NULL) // #IGNORE
    { return GetTypeDef()->Dump_Load_Value(strm, (void*)this, par); }

  virtual int 		Save(ostream& strm, TAPtr par=NULL, int indent=0)
  // Save object data to a file
    { return GetTypeDef()->Dump_Save(strm, (void*)this, par, indent); }
  virtual int 		SaveAs(ostream& strm, TAPtr par=NULL, int indent=0)
  // #ARGC_1 Save object data to a new file
    { return Save(strm,par,indent); }
  virtual int		Save_File(); 
    // #MENU #MENU_ON_Object #LABEL_Save saves the object to a file
  virtual int		SaveAs_File(const String& fname = ""); 
    // #MENU #ARGC_0 #LABEL_Save_As Saves object data to a new file
  virtual int	 	Load_File(TypeDef* td = NULL, void** el = NULL);
  // load object data from a file -- gets the filename
  virtual int	 	LoadAs_File(const String& fname, TypeDef* td = NULL
    , void** el = NULL);
  //  Load object data from a file

  
  virtual int 		Dump_Save_impl(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_impl(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_inline(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_inline(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_Path(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_Path(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_Value(strm, (void*)this, par, indent); } // #IGNORE

  virtual int		Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_SaveR(strm, (void*)this, par, indent); } 	// #IGNORE
  virtual int 		Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_PathR(strm, (void*)this, par, indent); } // #IGNORE
  virtual bool		Dump_QuerySaveMember(MemberDef* md); 
     // default checks NO_SAVE directive; override to make save decision at runtime
  virtual bool		Dump_QuerySaveChildren() 
    {return true;} // override to make save decision at runtime

  virtual int		Edit();
  // #MENU #ARGC_0 #MENU_ON_Object #MENU_CONTEXT #NO_SCRIPT Edit this object using the gui
//obs  virtual bool		CloseEdit();	// close any open edit dialogs for this object
  virtual bool		ReShowEdit(bool force = false);	// reshows any open edit dialogs for this object
  virtual const iColor* GetEditColor()  { return NULL; } // #IGNORE background color for edit dialog
  virtual const iColor* GetEditColorInherit(); // #IGNORE background color for edit dialog, include inherited colors from parents
#ifdef TA_GUI
  virtual const QPixmap* GetDataNodeBitmap(int, int& flags_supported) const
    {return NULL; } // #IGNORE gets the NodeBitmapFlags for the tree or list node -- see ta_qtbrowse_def.h
#endif
  virtual void		CloseLater();
  // #MENU #CONFIRM #NO_REVERT_AFTER #LABEL_Close_(Destroy) #NO_MENU_CONTEXT PERMANENTLY Destroy this object!  This is not Iconify.
  virtual void		Close(); // an immediate version of Close for use in code (no waitproc delay)
  virtual bool		Close_Child(TAPtr obj);
  // #IGNORE actually closes a child object (should be immediate child)
  virtual bool		CopyFrom(TAPtr cpy_from);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #TYPE_ON_this #NO_SCOPE #UPDATE_MENUS Copy from given object into this object
  // this is a safe interface to UnSafeCopy
  virtual bool		CopyTo(TAPtr cpy_to);
  // #MENU #TYPE_ON_this #NO_SCOPE #UPDATE_MENUS Copy to given object from this object
  // need both directions to more easily handle scoping of types on menus
  virtual bool		DuplicateMe();
  // #MENU #CONFIRM #UPDATE_MENUS Make another copy of myself (done through owner)
  virtual bool		ChangeMyType(TypeDef* new_type);
  // #MENU #TYPE_this #UPDATE_MENUS Change me into a different type of object, copying current info (done through owner)
  virtual bool		SelectForEdit(MemberDef* member, SelectEdit* editor, const String& extra_label);
  // select a given member for editing --<br>if already on dialog, removes it & returns false (else true)
  virtual bool		SelectForEditNm(const String& memb_nm, SelectEdit* editor, const String& extra_label);
  // #IGNORE hard code interface for updating editors
  virtual bool		SelectFunForEdit(MethodDef* function, SelectEdit* editor, const String& extra_label);
  // #MENU select a given function (method) for calling in a select edit dialog --\nif already on dialog, removes it & returns false (else true)
  virtual bool		SelectFunForEditNm(const String& function_nm, SelectEdit* editor, const String& extra_label);
  // #IGNORE hard code interface for updating editors
  virtual void		Help();
  // #MENU get help on using this object

  virtual void		ReplacePointersHook(TAPtr old_ptr);
  // #IGNORE hook to replace all pointers that might have pointed to old version of this object to this: default is to call taMisc::ReplaceAllPtrs
  virtual int		ReplaceAllPtrsThis(TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  // #IGNORE replace all pointers to old_ptr with new_ptr (calls TypeDef::ReplaceAllPtrsThis by default)

  virtual void		CallFun(const String& fun_name);
  // call function of given name on this object, prompting for args using gui interface

#ifdef DMEM_COMPILE
  virtual bool DMem_IsLocal()      // #IGNORE check if local given stored "this process" number
  { return true; }
  virtual bool DMem_IsLocalProc(int proc)  // #IGNORE check if local given external process number
  { return true; }
  virtual int  DMem_GetLocalProc()  // #IGNORE get local processor number for this object
  { return 0; }
  virtual void DMem_SetLocalProc(int lproc) // #IGNORE set the local process number for this object
  { };
  virtual void DMem_SetThisProc(int proc) // #IGNORE set the local processor number RELATIVE to relevant communicator
  { }
#endif
protected:
  virtual String	GetStringRep_impl() const; // string representation, ex. for variants; default is typename:fullpath

  int			refn;		// number of references to this object; note: MAXINT is the max allowed value
  static String		no_name; 	// return this for no names
  static int		no_idx;		// return this for no index
  static MemberDef*	no_mdef;	// return this for no memberdef ptr
};

inline istream& operator>>(istream &strm, taBase &obj)
{ obj.Load(strm); return strm; }
inline ostream& operator<<(ostream &strm, taBase &obj)
{ obj.Save(strm); return strm; }

class TA_API taSmartPtr { // ##NO_INSTANCE ##NO_TOKENS "safe" ptr for taBase objects -- automatically does ref counts; designed to be binary compatible with taBase*
public:
  inline taBase*	ptr() const {return m_ptr;}
  inline void		set(taBase* src) {taBase::SetPointer(&m_ptr, src);}
  
  inline		operator bool() const {return (m_ptr);}
    // needed to avoid ambiguities when we have derived T* operators
  inline 		operator taBase*() const {return m_ptr;}
  inline taBase* 	operator->() const {return m_ptr;}
  inline TAPtr* 	operator&() const {return &m_ptr;}
    //note: operator& is *usually* verbotten but we are binary compat -- it simplifies low-level compat
  inline taBase* 	operator=(const taSmartPtr& src) 
    {set(src.m_ptr); return m_ptr;} 
  inline taBase* 	operator=(taBase* src) {set(src); return m_ptr;} 
  
  taSmartPtr(taBase* val) {m_ptr = NULL; set(val);}
  taSmartPtr() {m_ptr = NULL;}
  ~taSmartPtr() {set(NULL);} //note: DO NOT change to be virtual!
protected:
  mutable taBase*	m_ptr;
private:
  taSmartPtr(const taSmartPtr& src); // not defined 
};


template<class T>
class taSmartPtrT: public taSmartPtr { 
public:
  inline T*	ptr() const {return (T*)m_ptr;} // typed alias for the base version

  inline 	operator T*() const {return (T*)m_ptr;}
  inline T* 	operator->() const {return (T*)m_ptr;}
  inline T** 	operator&() const {return (T**)(&m_ptr);}
    //note: operator& is *usually* verbotten but we are binary compat -- it simplifies low-level compat
  T* 		operator=(const taSmartPtrT<T>& src) 
    {set((T*)src.m_ptr); return (T*)m_ptr;} 
    //NOTE: copy only implies ptr, NOT the owner!
  T* 		operator=(T* src) {set(src); return (T*)m_ptr;}
   
  friend bool	operator==(const taSmartPtrT<T>& a, const taSmartPtrT<T>& b)
    {return (a.m_ptr == b.m_ptr);} 
  
  taSmartPtrT(T* val): taSmartPtr(val) {}
  taSmartPtrT() {} 
  
private:
  taSmartPtrT(const taSmartPtrT<T>& src); // not defined 
};

// macro for creating smart ptrs of taBase classes

#define SmartPtr_Of(T)  typedef taSmartPtrT<T> T ## Ptr;

// compatibility macro 
#define taPtr_Of(T)  SmartPtr_Of(T)


class TA_API taSmartRef: protected IDataLinkClient { // ##NO_INSTANCE ##NO_TOKENS "safe" reference for taBase objects -- does not ref count, but is a dlc so it tracks changes etc.
friend class taBase;
friend class TypeDef; // for various
friend class MemberDef; // for streaming
public:
  inline taBase*	ptr() const {return m_ptr;}
  void			set(taBase* src) {if (src == m_ptr) return;
    if (m_ptr) {m_ptr->RemoveDataClient(this);}
    if (src) {src->AddDataClient(this);}  m_ptr = src;}
  
  inline		operator bool() const {return (m_ptr);}
    // needed to avoid ambiguities when we have derived T* operators
  inline 		operator taBase*() const {return m_ptr;}
  inline taBase* 	operator->() const {return m_ptr;}
  taBase* 		operator=(const taSmartRef& src) 
    {set(src.m_ptr); return m_ptr;} 
    //NOTE: copy only implies ptr, NOT the owner!
  taBase* 		operator=(taBase* src) {set(src); return m_ptr;} 
  
  inline void 		Init(taBase* own_) {m_own = own_;} // call in owner's Initialize or InitLinks
  inline void		CutLinks() {set(NULL); m_own = NULL;}
  taSmartRef() {m_own = NULL; m_ptr = NULL;}
  ~taSmartRef() {CutLinks();}
  
protected:
  taBase*		m_own; 
  mutable taBase*	m_ptr;
  
private:
  taSmartRef(const taSmartRef& src); // not defined 
  
public: // ITypedObject interface
  override void*	This() {return (void*)this;} //
  override TypeDef*	GetTypeDef() const {return &TA_taSmartRef;} //note: only one typedef for all

public: // IDataLinkClient interface
  override TypeDef*	GetDataTypeDef() const 
    {return (m_ptr) ? m_ptr->GetTypeDef() : &TA_taBase;} // TypeDef of the data
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2);
  override void		DataLinkDestroying(taDataLink* dl);
};


template<class T>
class taSmartRefT: public taSmartRef { 
public:
  inline T*	ptr() const {return (T*)m_ptr;} // typed alias for the base version

  inline 	operator T*() const {return (T*)m_ptr;} //
  inline T* 	operator->() const {return (T*)m_ptr;} //
  
protected: // must use macro below to make instance classes
  taSmartRefT() {} 
  
private:
  taSmartRefT(const taSmartRefT<T>& src); // not defined 
};

// macro for creating smart refs of taBase classes

#define SmartRef_Of(T)  class T ## Ref: public taSmartRefT<T> { \
public: \
  T* operator=(const T ## Ref& src) {set((T*)src.m_ptr); return (T*)m_ptr;} \
  T* operator=(T* src) {set(src); return (T*)m_ptr;} \
  TypeDef* GetDataTypeDef() const {return (m_ptr) ? m_ptr->GetTypeDef() : &TA_ ## T;} \
  friend bool	operator==(const T ## Ref& a, const T ## Ref& b) \
    {return (a.m_ptr == b.m_ptr);}  \
  T ## Ref() {} \
private:\
  T ## Ref(const T ## Ref& src); \
};

#ifdef TA_USE_QT

/*
 * taBaseAdapter enables a taOBase object to handle Qt events, via a
 * proxy(taBaseAdapter)/stub(taBase) approach. Note that dual-parenting a taBase object
 * with QObject is not workable, because QObject must come first, and then all the (TAPtr)(void*)
 * casts in the system break...

 * To use, subclass taBaseAdapter when events need to be handled. Create the instance in
 * the Initialize() call and set with SetAdapter. The adapter object does not participate
 * in copying/cloning/etc. (it has no state information).

 * Since classes can have subclasses, there may be successive calls to Initialize with subclasses
 * of an adapter. The adapters will be chained, with the new adapter becoming the QObject parent of
 * the previous adapter. Therefore, when the current adapter is destroyed, child adapters will also
 * get destroyed. It will not matter whether a base class hooks to its own adapter, or to a subclass
 * or to a combination.
*/

class TA_API taBaseAdapter: public QObject {
  // ##IGNORE QObject for attaching events/signals for its taBase owner
friend class taOBase;
public:
  taBaseAdapter(taOBase* owner_): QObject(NULL) {owner = owner_;}
  ~taBaseAdapter();
protected:
  taOBase* owner; // #IGNORE
};
#endif // TA_USE_QT


class TA_API taOBase : public taBase {
  // #NO_TOKENS #NO_UPDATE_AFTER owned base class of taBase
INHERITED(taBase)
friend class taBaseAdapter;
public:
  TAPtr		owner;		// #READ_ONLY #NO_SAVE pointer to owner
#ifdef TA_USE_QT
  taBaseAdapter* 	adapter; // #IGNORE
  void			SetAdapter(taBaseAdapter* adapter_);
#endif

  taDataLink**		addr_data_link() {return &m_data_link;}
  override taDataLink*	data_link() {return m_data_link;}
//  override void		set_data_link(taDataLink* dl) {m_data_link = dl;}

  TAPtr 	GetOwner() const	{ return owner; }
  TAPtr		GetOwner(TypeDef* tp) const { return taBase::GetOwner(tp); }
  TAPtr 	SetOwner(TAPtr ta)	{ owner = ta; return ta; }

#ifdef TA_USE_QT
  void 	Initialize()			{ owner = NULL; adapter = NULL; m_data_link = NULL;}
#else
  void 	Initialize()			{ owner = NULL; m_data_link = NULL;}
#endif
  void	Destroy();
  void	CutLinks();
  TA_BASEFUNS(taOBase); //
protected:
  taDataLink*		m_data_link; //
};


/* taDataView -- exemplar base class of a view of an object, of class taOBase or later

   A view is a high-level depiction of an object, ex. "Network", "Graph", etc.

  The IDataViews list object does not own the view object -- some outer controller is
  responsible for lifetime management of dataview objects.

  However, if a dataobject is destroying, it will destroy all its views

*/
class TA_API taDataView: public taOBase, public virtual IDataLinkClient {
  // #NO_TOKENS  base class for views of an object
#ifndef __MAKETA__
typedef taOBase inherited_taBase;
typedef IDataLinkClient inherited_IDataLinkClient;
#endif
friend class taBase;
public:
  taBase*		m_data;		// #READ_ONLY data -- referent of the item -- the data
  TypeDef*		data_base;	// #READ_ONLY #NO_SAVE Minimum type for data object

  taBase*		data() {return m_data;} // subclasses usually redefine a strongly typed version
  void 			SetData(taBase* ta); // set the data to which this points -- must be subclass of data_base
  int			dbu_cnt() {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  virtual MemberDef*	md() const {return NULL;} // ISelectable property member stub
  virtual int		par_dbu_cnt(); // dbu of parent(s); note: only sign is accurate, not necessarily value (optimized)
  taDataView*		parent() const;

  virtual MemberDef*	GetDataMemberDef() {return NULL;} // returns md if known and/or knowable (ex. NULL for list members)
  virtual String	GetLabel() const; // returns a label suitable for tabview tabs, etc.
  virtual void		DataDestroying() {}
  virtual void		ChildClearing(taDataView* child) {} // override to implement par's portion of clear
  virtual void		ChildRendered(taDataView* child) {} // override to implement par's portion of render
  virtual void		Clear(taDataView* par = NULL) {Clear_impl(par);} // clears the view (but doesn't delete any components) (usually override _impl)
  virtual void		CloseChild(taDataView* child) {}
  virtual void		Render() {Clear_impl(); Render_pre(); Render_impl(); Render_post();} // renders the visible contents (usually override the _impl)
  virtual void		Reset() {Clear_impl(); Reset_impl();} // clears, and deletes any components (note: usually override _impl)

  virtual void		ItemRemoving(taDataView* item) {} // items call this on the root item -- usually used by a viewer to insure item removed from things like sel lists
  
  void	UpdateAfterEdit();
  void			InitLinks();
  void			CutLinks();
  TA_BASEFUNS(taDataView)

public: // ITypedObject interface
  override void*	This() {return (void*)this;} //
//already in taBase: override TypeDef*	GetTypeDef() const;

public: // IDataLinkClient interface
  override TypeDef*	GetDataTypeDef() const {return m_data->GetTypeDef();} // TypeDef of the data
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2);
   // called when the data item has changed, esp. ex lists and groups; dispatches to the DataXxx_impl's
  override void		DataLinkDestroying(taDataLink* dl);
   // called by DataLink when it is destroying
  override bool		IsDataView() {return true;}

protected:
  int			m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
  // NOTE: all Dataxxx_impl are supressed if dbu_cnt or par_dbu_cnt <> 0 -- see ta_type.h for detailed rules
  mutable taDataView* 	m_parent; // cached/autoset
  virtual void		DataDataChanged_impl(int dcr, void* op1, void* op2) {}
   // called when the data item has changed, esp. ex lists and groups, *except* UAE
  virtual void		DataUpdateAfterEdit_impl() {} // called by data for an UAE, i.e., after editing etc.
  virtual void		DataUpdateView_impl() {Render_impl();} // called for Update All Views, and at end of a DataUpdate batch
  virtual void		DataStructUpdateEnd_impl() {Reset(); Render();} // called ONLY at end of a struct update
  virtual void 		SetData_impl(taBase* ta); // called to actually set or clear m_data -- can be trapped
  virtual void		Clear_impl(taDataView* par = NULL) {} // override  to implement clear; NULL means not known yet
  virtual void		Render_pre(taDataView* par = NULL) {} // #IGNORE replace with pre-rendering code, if needed
  virtual void		Render_impl() {} // #IGNORE replace with code that renders the window contents
  virtual void		Render_post() {} // #IGNORE replace with post-rendering code, if needed
  virtual void		Reset_impl() {} // override  to implement reset
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};

// for explicit lifetime management
#define TA_DATAVIEWFUNS(x) \
  TA_BASEFUNS(x); \
  void* This() {return (void*)this;}

// for ref-counting lifetime management
#define TA_REF_DATAVIEWFUNS(x) \
  TA_REF_BASEFUNS(x); \
  void* This() {return (void*)this;}

class TA_API taNBase : public taOBase { // #NO_TOKENS Named, owned base class of taBase
#ifndef __MAKETA__
typedef taOBase inherited;
#endif
public:
  String	name;		// name of the object

  bool 		SetName(const String& nm)    	{ name = nm; return true; }
  String	GetName() const		{ return name; }

  void 	Initialize()			{ }
  void	Destroy()			{ CutLinks(); }
  void	CutLinks();
  void	Copy_(const taNBase& cp);
  COPY_FUNS(taNBase, taOBase);
  TA_BASEFUNS(taNBase);
};

typedef taNBase* TANPtr; // this comment needed for maketa parser


class TA_API taFBase: public taNBase {
  // #NO_TOKENS #NO_UPDATE_AFTER named/owned base class of taBase, with filename
#ifndef __MAKETA__
typedef taNBase inherited;
#endif
public:
  String		file_name; // #READ_ONLY #NO_SAVE

  override bool		SetFileName(const String& val)  {file_name = val; return true;} // #IGNORE
  override String	GetFileName() const { return file_name; } // #IGNORE
  void 			Initialize() {}
  void			Destroy() {}
  void			Copy_(const taFBase& cp) {file_name = cp.file_name;}
  COPY_FUNS(taFBase, taNBase);
  TA_BASEFUNS(taFBase)
};



class TA_API taBase_PtrList: public taPtrList<taBase> { // a primitive taBase list type, used for global lists that manage taBase objects

protected:
  String	El_GetName_(void* it) const { return ((TAPtr)it)->GetName(); }

  void*		El_Ref_(void* it)	{ taBase::Ref((TAPtr)it); return it; }
  void*		El_unRef_(void* it)	{ taBase::unRef((TAPtr)it); return it; }
  void		El_Done_(void* it)	{ taBase::Done((TAPtr)it); }

/*nuke  void*		El_MakeToken_(void* it) { return (void*)((TAPtr)it)->MakeToken(); }
  void*		El_Copy_(void* trg, void* src)
  { ((TAPtr)trg)->UnSafeCopy((TAPtr)src); return trg; } */
};

typedef taPtrList_base<taBase>  taPtrList_ta_base; // this comment needed for maketa parser

class taList_impl;
typedef taList_impl* TABLPtr; // this comment needed for maketa parser

class TA_API taList_impl : public taOBase, public taPtrList_ta_base {
  // #INSTANCE #NO_TOKENS #NO_UPDATE_AFTER ##MEMB_HIDDEN_EDIT ##HIDDEN_INLINE implementation for a taBase list class
#ifndef __MAKETA__
typedef taBase inherited_taBase;
typedef taPtrList_ta_base inherited_taPtrList;
#endif
public:
  static MemberDef* find_md;	// #HIDDEN #NO_SAVE return value for findmember of data

  String	name;		// name of the object
  TypeDef*	el_base;	// #HIDDEN #NO_SAVE Base type for objects in group
  TypeDef* 	el_typ;		// #TYPE_ON_el_base Default type for objects in group
  int		el_def;		// Index of default element in group

  // stuff for the taBase
  bool 		SetName(const String& nm)    	{name = nm; return true;}
  String	GetName() const		{ return name; }

  override TypeDef* 	GetElType() const {return el_typ;}		// #IGNORE Default type for objects in group
  override void* 	GetTA_Element(int i, TypeDef*& eltd)
    {return taPtrList_ta_base::GetTA_Element_(i, eltd); } // #IGNORE a bracket opr

  TAPtr		New(int n_objs=0, TypeDef* typ=NULL);
  // #MENU #MENU_ON_Object #ARGC_0 #UPDATE_MENUS #NO_SCRIPT #MENU_CONTEXT create n_objs new objects of given type

  String 	GetPath_Long(TAPtr ta=NULL, TAPtr par_stop = NULL) const;
  String 	GetPath(TAPtr ta=NULL, TAPtr par_stop = NULL) const;

  bool 		FindCheck(const String& nm) const // also check for el_typ
  { return ((name == nm) || InheritsFrom(nm) || el_typ->InheritsFrom(nm)); }

  MemberDef* 	FindMembeR(const String& nm, void*& ptr) const;    // extended to search in the list
  MemberDef* 	FindMembeR(TypeDef* it, void*& ptr) const; // extended to search in the list

  override void	Close();
  override bool	Close_Child(TAPtr obj);
  override void	ChildUpdateAfterEdit(TAPtr child, bool& handled); 
  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); 
    // called when list has changed 

  // IO
  ostream& 	OutputR(ostream& strm, int indent = 0) const;

  int		Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0);
  int		Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0);
  virtual int 	Dump_Save_PathR_impl(ostream& strm, TAPtr par, int indent); 
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);

  TAPtr		DefaultEl_() const	{ return (TAPtr)SafeEl_(el_def); } // #IGNORE

  virtual int	SetDefaultEl(TypeDef* it);
  virtual int	SetDefaultEl(const String& nm);
  virtual int	SetDefaultEl(TAPtr it);
  // set the default element to be given item
  virtual int	SetDefaultElName(const String& nm)	{ return SetDefaultEl(nm); }
  // set the default element to be item with given name
  virtual int	SetDefaultElType(TypeDef* it)	{ return SetDefaultEl(it); }
  // set the default element to be item with given type

  virtual int	Find(const taBase* item) const		{ return taPtrList_ta_base::Find(item); }
//  virtual int	Find(const TAPtr item) const		{ return taPtrList_ta_base::Find(item); }
  virtual int	Find(TypeDef* item) const;
  // find element of given type
  virtual int	Find(const String& item_nm) const	{ return taPtrList_ta_base::Find(item_nm); }

  virtual bool	Remove(const String& item_nm)	{ return taPtrList_ta_base::Remove(item_nm); }
  virtual bool	Remove(TAPtr item)	{ return taPtrList_ta_base::Remove(item); }
  virtual bool	Remove(int idx);
  // Remove object at given index on list
  virtual bool	RemoveEl(TAPtr item)	{ return Remove(item); }
  // Remove given item from the list

  virtual void	EnforceSize(int sz);
  // add or remove elements to force list to be of given size
  virtual void	EnforceType();
  // enforce current type (all elements have to be of this type)
  void	EnforceSameStru(const taList_impl& cp);
  // make the two lists identical in terms of size and types of objects

  virtual bool	ChangeType(int idx, TypeDef* new_type);
  // change type of item at index
  virtual bool	ChangeType(TAPtr itm, TypeDef* new_type);
  // #MENU #MENU_ON_Object #UPDATE_MENUS #ARG_ON_OBJ #TYPE_ON_el_base change type of item to new type, copying current info
  virtual int	ReplaceType(TypeDef* old_type, TypeDef* new_type);
  // #MENU #MENU_ON_Object #USE_RVAL #UPDATE_MENUS #TYPE_ON_el_base replace all items of old type with new type (returns number changed)

  int	ReplaceAllPtrsThis(TypeDef* obj_typ, void* old_ptr, void* new_ptr);
  virtual TAPtr	FindType_(TypeDef* item_tp, int& idx) const; 	// #IGNORE

  void	SetBaseType(TypeDef* it); // set base (and default) type to given td

  MemberDef*	ReturnFindMd() const;	// return the find_md variable, initialized if necessary
#ifdef TA_GUI
  override const QPixmap* GetDataNodeBitmap(int bmf, int& flags_supported) const;
#endif
  override int		NumListCols() const {return 3;} // number of columns in a default list view for this list type
  override const KeyString GetListColKey(int col) const; // #IGNORE get the key for the default list view
  override String	GetColHeading(const KeyString& key) const; // #IGNORE header text for the indicated column
  override String	ChildGetColText(void* child, TypeDef* typ, const KeyString& key, 
    int itm_idx = -1) const;	// #IGNORE

  void 	Initialize();
  void	Destroy();
  void 	CutLinks();
  void 	Copy(const taList_impl& cp);
  void	UpdateAfterEdit(); // we skip the taOBase version, and inherit only taBase
  TA_BASEFUNS(taList_impl);

protected:
  String	GetListName_() const	{ return name; }
  String	El_GetName_(void* it) const { return ((TAPtr)it)->GetName(); }
  TALPtr	El_GetOwner_(void* it) const { return (TABLPtr)((TAPtr)it)->GetOwner(); }
  void*		El_SetOwner_(void* it)	{ ((TAPtr)it)->SetOwner(this); return it; }
  bool		El_FindCheck_(void* it, const String& nm) const
  { return (((TAPtr)it)->FindCheck(nm) &&
	    ((El_GetOwner_(it) != NULL) || (El_GetOwner_(it) == (TALPtr) this))); }

  void*		El_Ref_(void* it)	{ taBase::Ref((TAPtr)it); return it; }
  void*		El_unRef_(void* it)	{ taBase::unRef((TAPtr)it); return it; }
  void		El_Done_(void* it)	{ taBase::Done((TAPtr)it); }
  void*		El_Own_(void* it)	{ taBase::Own((TAPtr)it,this); return it; }
  void		El_disOwn_(void* it)
  { if(El_GetOwner_(it) == this) ((TAPtr)it)->CutLinks(); El_Done_(El_unRef_(it)); }
  // cut links to other objects when removed from owner group

  void*		El_MakeToken_(void* it) { return (void*)((TAPtr)it)->MakeToken(); }
  void*		El_Copy_(void* trg, void* src)
  { ((TAPtr)trg)->UnSafeCopy((TAPtr)src); return trg; }
#ifdef TA_GUI
protected:
  override void	ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden);
     // gives the src ops allowed on child (ex CUT)
  virtual void	ChildQueryEditActionsL_impl(const MemberDef* md, const taBase* lst_itm, const taiMimeSource* ms,
    int& allowed, int& forbidden);
    // returns the operations allowed for list items (ex Paste)
  override int	ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea);
     // called by a child -- follows same delegation logic as the child's EditActions call */
  virtual int	ChildEditActionLS_impl(const MemberDef* md, taBase* lst_itm, int ea);
  virtual int	ChildEditActionLD_impl_inproc(const MemberDef* md, int item_idx, taBase* lst_itm, taiMimeSource* ms, int ea);
  virtual int	ChildEditActionLD_impl_ext(const MemberDef* md, int item_idx, taBase* lst_itm, taiMimeSource* ms, int ea);
#endif
protected:
  override String ChildGetColText_impl(taBase* child, const KeyString& key, int itm_idx = -1) const;
};

template<class T> 
class taList: public taList_impl { // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER
public:
  T*		SafeEl(int idx) const		{ return (T*)SafeEl_(idx); }
  // get element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // fast element (no range checking)
  T* 		operator[](int i) const	{ return (T*)el[i]; }

  T*		DefaultEl() const		{ return (T*)DefaultEl_(); }
  // returns the element specified as the default for this list

  T*		Edit_El(T* item) const		{ return SafeEl(Find((TAPtr)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ Edit given list item

  virtual T*	FindName(const String& item_nm, int& idx=Idx) const { return (T*)FindName_(item_nm, idx); }
  // #MENU #USE_RVAL #ARGC_1 #LABEL_Find Find element with given name (item_nm)
  virtual T* 	FindType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindType_(item_tp, idx); }
  // find given type element (NULL = not here), sets idx

  T*		First() const			{ return (T*)First_(); }
  // look at the first element; NULL if none
  virtual T*	Pop()				{ return (T*)Pop_(); }
  // pop the last element off the stack
  virtual T*	Peek() const			{ return (T*)Peek_(); }
  // peek at the last element on the stack, if any

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)	{ return (T*)LinkUniqNameOld_((void*)item); }
  // link so that name is unique, old used if dupl, returns one used

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // move item so that it appears just after the target item trg in the list

// when a taList is declared in advance of defining the type within it, this breaks
//  void 	Initialize() 			{ SetBaseType(T::StatTypeDef(1)); }
  void	Initialize()			{ SetBaseType(taBase::StatTypeDef(0)); }
  void	Destroy()			{ };

  taList() 				{ Register(); Initialize(); }
  taList(const taList<T>& cp):taList_impl() { Register(); Initialize(); Copy(cp); }
  ~taList() 				{ unRegister(); Destroy(); }
  TAPtr Clone() 			{ return new taList<T>(*this); }
  void  UnSafeCopy(TAPtr cp) 		{ if(cp->InheritsFrom(taList::StatTypeDef(0))) Copy(*((taList<T>*)cp));
                                          else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); }
  void  CastCopyTo(TAPtr cp)            { taList<T>& rf = *((taList<T>*)cp); rf.Copy(*this); }
  TAPtr MakeToken()			{ return (TAPtr)(new taList<T>); }
  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new taList<T>[no]); }
  void operator=(const taList<T>& cp)	{ Copy(cp); }
  TA_TMPLT_TYPEFUNS(taList,T);
};

// do not use this macro, since you typically will want ##NO_TOKENS, #NO_UPDATE_AFTER
// which cannot be put inside the macro!
//
// #define taList_of(T)
// class T ## _List : public taList<T> {
// public:
//   void Initialize()	{ };
//   void Destroy()	{ };
//   TA_BASEFUNS(T ## _List);
// }

// use the following as a template instead..

// define default base list to not keep tokens
class TA_API taBase_List : public taList<taBase> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER list of objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(taBase_List);
};


class TA_API taArray_base : public taOBase, public taArray_impl {
  // #VIRT_BASE #NO_TOKENS #NO_UPDATE_AFTER base for arrays (from taBase)
public:
  ostream& 	Output(ostream& strm, int indent = 0) const;
  ostream& 	OutputR(ostream& strm, int indent = 0) const
  { return Output(strm, indent); }

  int		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);

  void 	Initialize()	{ }
  void 	Destroy()	{ CutLinks(); }
  void	CutLinks();
  virtual void	Copy(const taArray_base& cp) {taOBase::Copy(cp); taArray_impl::Copy_Duplicate(cp);}
    //WARNING: Copy_Duplicate is not a true copy, but we retain the behavior for compatibility, except Matrix_Array
  TA_ABSTRACT_BASEFUNS(taArray_base);
};


#define TA_ARRAY_FUNS(y,T) \
public: \
  explicit y(int init_size) {Initialize(); EnforceSize(init_size); } \
  T&		operator[](int i) { return el[i]; } \
  const T&	operator[](int i) const	{ return el[i]; } \
protected: \
  override const void*	El_GetBlank_() const	{ return (const void*)&blank; }

#define TA_ARRAY_OPS(y) \
  inline bool operator ==(const y& a, const y& b) {return a.Equal_(b);} \
  inline bool operator !=(const y& a, const y& b) {return !(a.Equal_(b));}

template<class T>
class taArray : public taArray_base {
  // #VIRT_BASE #NO_TOKENS #NO_INSTANCE #NO_UPDATE_AFTER
public:
  T*		el;		// #HIDDEN #NO_SAVE Pointer to actual array memory
  T		err;		// #HIDDEN what is returned when out of range; MUST INIT IN CONSTRUCTOR
  
  ////////////////////////////////////////////////
  // 	functions that return the type		//
  ////////////////////////////////////////////////

  const T&	SafeEl(int i) const {return *(T*)SafeEl_(i);}
  // #MENU #MENU_ON_Edit #USE_RVAL the element at the given index
  T&		FastEl(int i)  {return el[i]; }
  // fast element (no range checking)
  const T&	FastEl(int i) const	{return el[i]; }
  // fast element (no range checking)
  const T&	RevEl(int idx) const	{ return SafeEl(size - idx - 1); }
  // reverse (index) element (ie. get from the back of the list first)

  const T	Pop() 
    {if (size == 0) return *(static_cast<const T*>(El_GetErr_()));
     else return el[--size]; }
  // pop the last item in the array off
  const T& Peek() const {return SafeEl(size - 1);}
  // peek at the last item on the array

  ////////////////////////////////////////////////
  // 	functions that are passed el of type	//
  ////////////////////////////////////////////////

  void	Set(int i, const T& item) 	
    { if (InRange(i)) el[i] = item; }
  // use this for safely assigning values to items in the array (Set should update if needed)
  void	Add(const T& item)		{ Add_((void*)&item); }
  // #MENU add the item to the array
  bool	AddUnique(const T& item)	{ return AddUnique_((void*)&item); }
  // add the item to the array if it isn't already on it, returns true if unique
  void	Push(const T& item)		{ Add(item); }
  // push the item on the end of the array (same as add)
  void	Insert(const T& item, int indx, int n_els=1)	{ Insert_((void*)&item, indx, n_els); }
  // #MENU Insert (n_els) item(s) at indx (-1 for end) in the array
  int	Find(const T& item, int indx=0) const { return Find_((void*)&item, indx); }
  // #MENU #USE_RVAL Find item starting from indx in the array (-1 if not there)
//  virtual bool	Remove(const T& item)		{ return Remove_((void*)&item); }
//  virtual bool	Remove(uint indx, int n_els=1)	{ return taArray_impl::Remove(indx,n_els); }
  // Remove (n_els) item(s) at indx, returns success
  virtual bool	RemoveEl(const T& item)		{ return Remove_((void*)&item); }
  // remove given item, returns success
  virtual void	InitVals(const T& item, int start=0, int end=-1) { InitVals_((void*)&item, start, end); }
  // set array elements to specified value starting at start through end (-1 = size)

  taArray()				{ el = NULL; } // no_tokens is assumed
  ~taArray()				{ SetArray_(NULL); }
  
  void  UnSafeCopy(TAPtr cp) { 
    if(cp->InheritsFrom(taArray::StatTypeDef(0))) Copy(*((taArray<T>*)cp));
    else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); }
  void  CastCopyTo(TAPtr cp)            { taArray<T>& rf = *((taArray<T>*)cp); rf.Copy(*this); } //
//  TAPtr MakeToken()			{ return (TAPtr)(new taArray<T>); }
//  TAPtr MakeTokenAry(int no)		{ return (TAPtr)(new taArray<T>[no]); }
  TA_TMPLT_TYPEFUNS(taArray,T); //
public:
  void*		FastEl_(int i)			{ return &(el[i]); }// #IGNORE
protected:
  mutable T		tmp; // #IGNORE temporary item
  
  override void*	MakeArray_(int n) const	{ return new T[n]; }
  override void		SetArray_(void* nw) {if (el) delete [] el; el = (T*)nw;}
  void		El_Copy_(void* to, const void* fm) { *((T*)to) = *((T*)fm); }
  uint		El_SizeOf_() const		{ return sizeof(T); }
  const void*	El_GetErr_() const		{ return (void*)&err; }
  void*		El_GetTmp_() const		{ return (void*)&tmp; } //

};


class TA_API int_Array : public taArray<int> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST int blank; // #HIDDEN #READ_ONLY 
  virtual void	FillSeq(int start=0, int inc=1);
  // fill array with sequential values starting at start, incrementing by inc
  
  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_int; return FastEl_(i); }
  void Initialize()	{err = 0; };
  void Destroy()	{ }; //
  //note: Register() is not necessary for arrays, so we omit in these convenience constructors
  int_Array(int num, int i0) {Initialize(); EnforceSize(1); el[0] = i0;}
  int_Array(int num, int i0, int i1) {Initialize(); EnforceSize(2); el[0] = i0; el[1] = i1;}
  int_Array(int num, int i0, int i1, int i2) 
    {Initialize(); EnforceSize(3); el[0] = i0; el[1] = i1; el[2] = i2;}
  int_Array(int num, int i0, int i1, int i2, int i3) 
    {Initialize(); EnforceSize(4); el[0] = i0; el[1] = i1; el[2] = i2; el[3] = i3;}
  TA_BASEFUNS(int_Array);
  TA_ARRAY_FUNS(int_Array, int)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((int*)a) > *((int*)b)) rval=1; else if(*((int*)a) == *((int*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((int*)a) == *((int*)b)); }
  String	El_GetStr_(const void* it) const { return (*((int*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { int tmp = (int)val; *((int*)it) = tmp; }
};
TA_ARRAY_OPS(int_Array)


class TA_API float_Array : public taArray<float> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST float blank; // #HIDDEN #READ_ONLY 
  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_float; return FastEl_(i); }
  void Initialize()	{err = 0.0f; };
  void Destroy()	{ };
  TA_BASEFUNS(float_Array);
  TA_ARRAY_FUNS(float_Array, float)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((float*)a) > *((float*)b)) rval=1; else if(*((float*)a) == *((float*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((float*)a) == *((float*)b)); }
  String	El_GetStr_(const void* it) const { return (*((float*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { float tmp = (float)val; *((float*)it) = tmp; }
};
TA_ARRAY_OPS(float_Array)


class TA_API double_Array : public taArray<double> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST double blank; // #HIDDEN #READ_ONLY 
  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_double; return FastEl_(i); }
  void Initialize()	{err = 0.0;};
  void Destroy()	{ };
  TA_BASEFUNS(double_Array);
  TA_ARRAY_FUNS(double_Array, double)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((double*)a) > *((double*)b)) rval=1; else if(*((double*)a) == *((double*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((double*)a) == *((double*)b)); }
  String	El_GetStr_(const void* it) const { return (*((double*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  { double tmp = (double)val; *((double*)it) = tmp; }
};
TA_ARRAY_OPS(double_Array)


class TA_API String_Array : public taArray<String> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST String blank; // #HIDDEN #READ_ONLY 
  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_taString; return FastEl_(i); }
  void Initialize()	{ };
  void Destroy()	{ };
  TA_BASEFUNS(String_Array);
  TA_ARRAY_FUNS(String_Array, String)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((String*)a) > *((String*)b)) rval=1; else if(*((String*)a) == *((String*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((String*)a) == *((String*)b)); }
  String	El_GetStr_(const void* it) const { return (*((String*)it)); }
  void		El_SetFmStr_(void* it, const String& val)
  {*((String*)it) = val; }
};
TA_ARRAY_OPS(String_Array)


class TA_API SArg_Array : public String_Array {
  // string argument array: has labels for each argument to make it easier in the interface
INHERITED(String_Array)
public:
  String_Array	labels;		// #HIDDEN labels for each argument

  int		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent = 0);
  int		Dump_Load_Value(istream& strm, TAPtr par=NULL);

  void	UpdateAfterEdit();
  void	Initialize();
  void	Destroy()	{ };
  void	InitLinks();
  void	Copy_(const SArg_Array& cp);
  COPY_FUNS(SArg_Array, String_Array);
  TA_BASEFUNS(SArg_Array);
};


typedef void* voidptr; // for maketa, which chokes on void* in a template
class TA_API voidptr_Array : public taArray<voidptr> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST voidptr blank; // #HIDDEN #READ_ONLY 

  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_voidptr; return FastEl_(i); }
  void Initialize()	{err = 0; };
  void Destroy()	{ };
  TA_BASEFUNS(voidptr_Array);
  TA_ARRAY_FUNS(voidptr_Array, voidptr)
protected:
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((voidptr*)a) == *((voidptr*)b)); }
};

// define selectedit if no gui
#ifdef TA_NO_GUI
class TA_API SelectEdit {
  bool	do_nothing;
};
#endif // def TA_NO_GUI

class TA_API taRootBase: public taNBase {
  // #VIRT_BASE ##NO_INSTANCE ##NO_TOKENS basic methods that the root/app object must support
INHERITED(taNBase)
public:
  virtual void  Settings() = 0;		// #MENU #MENU_ON_Object edit global settings/parameters (taMisc)
  virtual void	SaveConfig() = 0;		// #MENU #CONFIRM save current configuration to file ~/.pdpconfig that is automatically loaded at startup: IMPORTANT: DO NOT HAVE A PROJECT LOADED!
  virtual void	LoadConfig() = 0;		// #MENU #CONFIRM load current configuration from file ~/.pdpconfig that is automatically loaded at startup
  virtual void	Info() = 0;			// #MENU get information/copyright notice
  // #MENU #ARGC_0 #USE_RVAL #NO_REVERT_AFTER use object browser to find an object, starting with initial path if given
  virtual void	Quit() = 0;
  // #MENU #CONFIRM #MENU_SEP_BEFORE #NO_REVERT_AFTER quit from software..
  virtual void	SaveAll() = 0; // saves all the contents of the app object
  
  TA_ABSTRACT_BASEFUNS(taRootBase)
private:
  void	Initialize() {}
  void	Destroy() {if (tabMisc::root == this) tabMisc::root = NULL;}
};


#endif // ta_base_h
