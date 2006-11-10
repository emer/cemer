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
class taRootBase;

#ifdef TA_GUI
class taiMimeSource; //
#endif

// forwards
class taBase;
class taSmartRef;
class taSmartPtr;
class taOBase;
class taOABase;
class taDataView;
class taNBase;
class taBase_List;
class taBase_PtrList;
class String_Array;
class UserDataItem;
class UserDataItem_List;

class TA_API tabMisc {
  // #NO_TOKENS #INSTANCE miscellaneous useful stuff for taBase
public:
  static taRootBase*	root;
  // root of the structural object hierarchy
  static taBase_PtrList	delayed_remove;
  // list of objs to be removed in the wait process (e.g. when objs delete themselves)
  static taBase_PtrList	delayed_updateafteredit;
  // list of objs to be update-after-edit'd in the wait process
  static taBase_PtrList	post_load_opr;  // #HIDDEN objects that need to have operations performed on them after loading

  static void		Close_Obj(TAPtr obj);
  // call this to implement closing object function
  static void		DeleteRoot(); // get rid of root, if not nuked already
  static void		WaitProc();
  // wait process function: remove objects from groups, update others
//obs  static bool	NotifyEdits(TAPtr obj);
  // notify any edit dialogs of a taptr object that object has changed
  static void		DelayedUpdateAfterEdit(TAPtr obj);
  // call update-after-edit on object in wait process (in case this does other kinds of damage..)
};

#define TA_BASEFUNS(y) 	y () { Register(); Initialize(); SetDefaultName(); } \
			y (const y& cp) { Register(); Initialize(); Copy(cp); } \
			~y () { unRegister(); Destroying(); Destroy(); } \
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
			~y () {CheckDestroyed(); Destroying(); Destroy();} \
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
#define TA_CONST_BASEFUNS(y) ~y () { unRegister(); Destroying(); Destroy(); } \
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
			~y () {CheckDestroyed(); Destroy(); } \
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
        ~y () { unRegister(); Destroying(); Destroy(); } \
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

#define	SIMPLE_INITLINKS(T) void InitLinks() { inherited::InitLinks(); InitLinks_taAuto(&TA_##T); }
#define	SIMPLE_CUTLINKS(T)  void CutLinks() { CutLinks_taAuto(&TA_##T); inherited::CutLinks(); }
#define SIMPLE_LINKS(T)	  SIMPLE_INITLINKS(T); SIMPLE_CUTLINKS(T)

#define TA_SIMPLE_BASEFUNS(T)  SIMPLE_COPY(T); COPY_FUNS(T,inherited); SIMPLE_LINKS(T); TA_BASEFUNS(T)

// simplified Get owner functions B = ta_base object, T = class name
#define GET_MY_OWNER(T) (T *) GetOwner(&TA_##T)
#define GET_OWNER(B,T)  (T *) B ->GetOwner(T::StatTypeDef(0))

#define SET_POINTER(var,obj) (taBase::SetPointer((TAPtr*)&(var), (TAPtr)(obj)))
#define DEL_POINTER(var) (taBase::DelPointer((TAPtr*)&(var)))

#define TA_USERDATAFUNS(T) \
mutable UserDataItem_ListPtr user_data_; /* #OWN_POINTER #SHOW_TREE */ \
UserDataItem_List* GetUserDataList(bool fc = false) const { \
if (!user_data_ && fc) {user_data_ = new UserDataItem_List; \
 taBase::Own(user_data_.ptr(), const_cast<T*>(this));} return user_data_;}

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
    VT_DOUBLE,		// a 8-byte floating point value (aprox 15 sig decimal digits)
    VT_FLOAT,		// a 4-byte floating point value (aprox 7 sig decimal digits)
    VT_INT,		// a 32-bit signed integer
    VT_BYTE,		// an unsigned 8-bit integer; used mostly for image components (rgb)
    VT_VARIANT		// a Variant, which can hold scalars, matrices, and objects
  };
  
  enum Flags { // #BITS control flags 
    THIS_INVALID	= 0x01, // CheckThisConfig_impl has detected a problem
    CHILD_INVALID	= 0x02, // CheckChildConfig_impl returns issue with a child
    DESTROYING		= 0X40, // Set in Destroying at the very beginning of destroy
    DESTROYED		= 0x80  // set in base destroy (DEBUG only); lets us detect multi destroys
#ifndef __MAKETA__
    ,INVALID_MASK	= THIS_INVALID | CHILD_INVALID
#endif
  };
  
  static const String 	ValTypeToStr(ValType vt);    // #IGNORE
  static ValType	ValTypeForType(TypeDef* td); // #IGNORE return the appropriate ValType
  static  TypeDef*	StatTypeDef(int);	// #IGNORE
  static TAPtr 		MakeToken(TypeDef* td);
  // #IGNORE make a token of the given type
  static TAPtr 		MakeTokenAry(TypeDef* td, int no);
  // #IGNORE make an array of tokens of the given type

  static String		GetStringRep(const taBase& it) {return it.GetStringRep_impl();}
   // #IGNORE string representation
  static String		GetStringRep(TAPtr it); 
  // #IGNORE string representation; ok if null, calls ->GetStringRep_impl
  
  // Reference counting mechanisms, all static just for consistency..
  static int		GetRefn(TAPtr it)	{ return it->refn; } // #IGNORE
  static void  		Ref(taBase& it)		{ it.refn++; }	     // #IGNORE
  static void  		Ref(taBase* it) 	{ it->refn++; }	     // #IGNORE
#ifdef DEBUG
  static void		UnRef(taBase* it);// #IGNORE
#else
  static void		UnRef(taBase* it) { if (--(it->refn) == 0) delete it;}// #IGNORE
#endif
  static void		Own(taBase& it, TAPtr onr);	// #IGNORE note: also does a RefStatic() on first ownership
  static void		Own(taBase* it, TAPtr onr);	// #IGNORE note: also does a Ref() on new ownership
  static void		Own(taSmartRef& it, TAPtr onr);	// #IGNORE for semantic compat with other Owns
  static taFiler*	StatGetFiler(TypeItem* td);
    // #IGNORE gets file dialog for the TypeItem -- clients must ref/unrefdone

protected: // legacy ref counting routines, for compatability -- do not use for new code
  static void   	unRef(taBase* it) { it->refn--; }	     // #IGNORE
#ifdef DEBUG
  static void   	Done(taBase* it); // #IGNORE
#else
  static void   	Done(taBase* it) 	{ if (it->refn == 0) delete it;} // #IGNORE
#endif
  static void		unRefDone(taBase* it) 	{unRef(it); Done(it);}	 // #IGNORE

public:
  // Pointer management routines (all pointers should be ref'd!!)
  static void 		InitPointer(taBase** ptr) { *ptr = NULL; } // #IGNORE
  static void 		SetPointer(taBase** ptr, taBase* new_val);	 // #IGNORE
  static void 		OwnPointer(taBase** ptr, taBase* new_val, taBase* onr); // #IGNORE
  static void 		DelPointer(taBase** ptr);				  // #IGNORE

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
  
  virtual UserDataItem_List* GetUserDataList(bool force_create = false) const {return NULL;}
  // #IGNORE gets the userdatalist for this class
  virtual bool		HasUserData(const String& name) const;
  // #IGNORE returns true if UserData exists for this name (case sens)
  virtual const Variant	GetUserData(const String& name) const;
  // #IGNORE get specified user data; nilVariant if not present or class doesn't support user data
  inline bool		GetUserDataAsBool(const String& name) const
    {return GetUserData(name).toBool();} // #IGNORE strongly-typed convenience accessor
  inline int		GetUserDataAsInt(const String& name) const
    {return GetUserData(name).toInt();} // #IGNORE strongly-typed convenience accessor
  inline float		GetUserDataAsFloat(const String& name) const
    {return GetUserData(name).toFloat();} // #IGNORE strongly-typed convenience accessor
  inline double		GetUserDataAsDouble(const String& name) const
    {return GetUserData(name).toDouble();} // #IGNORE strongly-typed convenience accessor
  inline const String	GetUserDataAsString(const String& name) const
    {return GetUserData(name).toString();} // #IGNORE strongly-typed convenience accessor
  virtual void		SetUserData(const String& name, const Variant& value);
  // #IGNORE set user data; NOTE: class must support user data
  virtual String	GetColText(const KeyString& key, int itm_idx = -1) const;
  // #IGNORE default keys are: name, type, desc, disp_name
#ifdef TA_GUI
public:
  virtual void		BrowseMe();
  // #MENU #MENU_ON_Object #MENU_SEP_AFTER #MENU_CONTEXT #CAT_Display show this object in its own browser 
  virtual void		ChildQueryEditActions(const MemberDef* md, const taBase* child, taiMimeSource* ms,
    int& allowed, int& forbidden);
    // #IGNORE gives ops allowed on child, with ms being clipboard or drop contents, md valid if we are a member, o/w NULL
  virtual int		ChildEditAction(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea);
  // #IGNORE note: multi source ops will have child=NULL
  virtual void		QueryEditActions(const taiMimeSource* ms, int& allowed, int& forbidden)
    {QueryEditActions_impl(ms, allowed, forbidden);} // #IGNORE ms is null for Src-op query
  virtual int		EditAction(taiMimeSource* ms, int ea)
    {return EditAction_impl(ms, ea);} // #IGNORE 
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
  int			baseFlags() const {return m_flags;} // #IGNORE flag values; see also HasFlag
  virtual taDataLink* 	data_link() {return NULL;} // #IGNORE link for viewer system created when needed, deleted when 0 clients -- all delegated functions must be of form: if(data_link()) data_link->SomeFunc(); NOT autocreated by call to this func -- call GetDataLink() to force creation
  bool			isDestroying() const {return HasFlag(DESTROYING);}
    // #CAT_ObjectMgmt true if the object is already destroying
  virtual bool		isDirty() const {return false;} // #IGNORE implemented by very few, esp. Project
  virtual void 		setDirty(bool value); // #CAT_ObjectMgmt 'true' gets forwarded up; 'false' does nothing
  
  
  virtual taDataLink* 	GetDataLink(); // #IGNORE forces creation; can still be NULL if the type doesn't support datalinks
  bool			AddDataClient(IDataLinkClient* dlc); // #IGNORE note: only applicable for classes that implement datalinks
  bool			RemoveDataClient(IDataLinkClient* dlc); // #IGNORE WARNING: link is undefined after this 
  
  virtual void		InitLinks()		{ };
  // #IGNORE initialize links to other objs, called after construction & SetOwner, call parent
  virtual void		CutLinks();
  // #IGNORE cut any links to other objs, called upon removal from a group or owner
  void			Copy(const taBase&)	{ };
  // #IGNORE the copy (=) operator, call parent

  virtual void		InitLinks_taAuto(TypeDef* td);
  // #IGNORE automatic TA-based initlinks: calls inherited and goes through only my members & owns them
  virtual void		CutLinks_taAuto(TypeDef* td);
  // #IGNORE automatic TA-based cutlinks: goes through only my members & calls cutlinks and calls inherited

  void 			Register()			// #IGNORE non-virtual, called in constructors
  { if(!taMisc::not_constr) GetTypeDef()->Register((void*)this); }
  void 			unRegister()			// #IGNORE non-virtual, called in destructors
  { CheckDestroyed(); if(!taMisc::not_constr) GetTypeDef()->unRegister((void*)this); }

  // these are all "free" with the TA_BASEFUNS
  void			Initialize()		{ refn_flags = 0; }
  // #IGNORE initializer; same considerations as Destroy
  void			Destroy(); // #IGNORE destroy for classes that don't need their own -- MIGHT BE CALLED MULTIPLE TIMES
  taBase()					{ Register(); Initialize(); }
  taBase(taBase& cp)				{ Register(); Initialize(); Copy(cp); }
  taBase(int)					{ Initialize(); } // don't register...
  virtual ~taBase() 				{ Destroy(); } //

  bool			HasFlag(int flag) const;
  // #CAT_ObjectMgmt true if flag set, or if multiple, any set
  void			SetFlag(int flag);
  // #CAT_ObjectMgmt sets the flag(s)
  void			ClearFlag(int flag);
  // #CAT_ObjectMgmt clears the flag(s)
  
  virtual TAPtr		Clone()			{ return new taBase(*this); } // #IGNORE
  virtual void		UnSafeCopy(TAPtr)	{ }; // #IGNORE assumes source is same type
  virtual void		CastCopyTo(TAPtr)	{ }; // #IGNORE ??
  virtual TAPtr 	MakeToken()		{ return new taBase; }	// #IGNORE
  virtual TAPtr 	MakeTokenAry(int no)	{ return new taBase[no]; } // #IGNORE
  void 			operator=(const taBase& cp)	{ Copy(cp); }
  virtual TypeDef*	GetTypeDef() const;	// #IGNORE

  virtual int		GetIndex() const {return -1;} // #CAT_ObjectMgmt typically its index in an own List
  virtual void		SetIndex(int value) {} // #IGNORE note: typically don't do a notify, because list itself will take care of notifying gui clients
  
  virtual bool		SetName(const String& nm) {return false;} // #IGNORE 
  virtual String	GetName() const 	{ return _nilString; } // #IGNORE
  virtual String	GetDisplayName() const; // #IGNORE can be overridden to provide synthetic name, or to strip out chars from mangled names (ex. DataTable column names) -- will/must never be empty
  virtual String	GetDesc() const {return _nilString;} // #IGNORE a type-specific "description" of the instance
  virtual bool		SetFileName(const String& val)  {return false;} // #CAT_File set file name for object
  virtual String	GetFileName() const 	{ return _nilString; } // #CAT_File get file name object was last saved with
  taFiler*		GetFiler(TypeItem* td = NULL);
  // #IGNORE gets filer for the TypeItem if specified, or this object-- clients must ref/unrefdone
  virtual void 		SetDefaultName();			    // #IGNORE
  virtual void		SetTypeDefaults();			    // #IGNORE
  virtual void		SetTypeDefaults_impl(TypeDef* ttd, TAPtr scope); // #IGNORE
  virtual void		SetTypeDefaults_parents(TypeDef* ttd, TAPtr scope); // #IGNORE

  virtual void* 	GetTA_Element(int, TypeDef*& eltd) 
    { eltd = NULL; return NULL; } // #IGNORE a bracket opr
  virtual TAPtr 	SetOwner(TAPtr)		{ return(NULL); } // #IGNORE
  virtual TAPtr 	GetOwner() const	{ return(NULL); } // #CAT_ObjectMgmt 
  virtual TAPtr		GetOwner(TypeDef* td) const; // #CAT_ObjectMgmt 
  virtual String 	GetPath_Long(TAPtr ta=NULL, TAPtr par_stop=NULL) const;
  // #IGNORE get path from root (default), but stop at par_stop if non-null
  virtual String	GetPath(TAPtr ta=NULL, TAPtr par_stop=NULL) const;
  // #CAT_ObjectMgmt get path without name informtation, stop at par_stop if non-null
  virtual TAPtr		FindFromPath(const String& path, MemberDef*& ret_md=no_mdef, int start=0) const;
  // #CAT_ObjectMgmt find object from path (starting from this, and position start of the path)

  // functions for managing structural scoping
  virtual TypeDef*	GetScopeType();
  // #IGNORE gets my scope type (if NULL, it means no scoping, or root)
  virtual TAPtr		GetScopeObj(TypeDef* scp_tp=NULL);
  // #IGNORE gets the object that is at the scope type above me (uses GetScopeType() or scp_tp)
  virtual bool		SameScope(TAPtr ref_obj, TypeDef* scp_tp=NULL);
  // #IGNORE determine if this is in the same scope as given ref_obj (uses my scope type)
  virtual TAPtr		New(int n_objs=0, TypeDef* type=NULL);
  // #CAT_ObjectMgmt Create n_objs objects of given type (type is optional)
  virtual uint		GetSize() const		{ return GetTypeDef()->size; }  // #IGNORE

  virtual void		UpdateAfterEdit();
  // #CAT_ObjectMgmt called after editing, or any user change to members (eg. in the interface, script)
  virtual void		ChildUpdateAfterEdit(TAPtr child, bool& handled);
   // #IGNORE called by a child in its UAE routine; provides child notifications\nNOTE: only member objects are detected; subclasses that want to notify on owned TAPtr members must override and check for those instances manually
  virtual void		UpdateAllViews();
  // #CAT_Display called after data changes, to update views
  virtual void 		DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
  // #IGNORE sends the indicated notification to all datalink clients, if any;\nvirtual so we can override to trap/monitor
  void			StructUpdate(bool begin) {BatchUpdate(begin, true);}
  // #CAT_ObjectMgmt bracket structural changes with (nestable) true/false calls;
  void			DataUpdate(bool begin) {BatchUpdate(begin, false);}
  // #CAT_ObjectMgmt bracket data value changes with (nestable) true/false calls;

#ifndef __MAKETA__
  void			CheckConfig(bool quiet, bool& rval)
    {if (!CheckConfig_impl(quiet)) rval = false;}
    // this one is typically used in CheckXxx_impl routines; we don't do gui wrap stuff
#endif
  bool			CheckConfig(bool quiet = false)
  { return CheckConfig_Gui(false, quiet);}
  // #CAT_ObjectMgmt check the configuration of this object and all its children (defaults to no confirm of success)
  bool			CheckConfig_Gui(bool confirm_success = true,
    bool quiet = false);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt #ARGC_0 #LABEL_CheckConfig check the configuration of this object and all its children -- failed items highlighted in red, items with failed children in yellow
  void			ClearCheckConfig(); // #IGNORE this can be called when a CheckConfig_impl routine blindly assert ok, ex. for an object that has an "off" or "disable" state; this routine updates the gui if the state has now changed

protected:
  virtual void		BatchUpdate(bool begin, bool struc);
  // #IGNORE bracket changes with (nestable) true/false calls; data clients can use it to supress change updates
  virtual void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {}
  // #IGNORE the obj (to which we have a ref) is about to destroy (the ref will null its ptr on return)
  virtual void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_) {}
  // #IGNORE the obj (to which we have a ref) has signalled the indicated data change

public:
  // the following are selected functions from TypeDef
  bool		HasOption(const char* op) const		// #IGNORE
  { return GetTypeDef()->HasOption(op); }
  bool		CheckList(const String_PArray& lst) const // #IGNORE
  { return GetTypeDef()->CheckList(lst); }

  bool 		InheritsFrom(TypeDef* it) const		{ return GetTypeDef()->InheritsFrom(it); }
  bool		InheritsFrom(const TypeDef& it) const	{ return GetTypeDef()->InheritsFrom(it); }
  bool 		InheritsFrom(const char* nm) const	// #CAT_ObjectMgmt does this inherit from given type?
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
  // #CAT_ObjectMgmt get the name corresponding to given enum value in enum type enum_tp_nm
  virtual int		GetEnumVal(const String& enum_nm, String& enum_tp_nm = no_name) const
    { return GetTypeDef()->GetEnumVal(enum_nm, enum_tp_nm); }
  // #CAT_ObjectMgmt get the enum value corresponding to the given enum name (-1 if not found), and sets enum_tp_nm to name of type this enum belongs in (empty if not found)

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
  virtual ostream&  	OutputTokens(ostream& strm) const	// #IGNORE
    { GetTypeDef()->tokens.List(strm); return strm; }

  virtual ostream& 	Output(ostream& strm, int indent = 0) const // #IGNORE
    { return GetTypeDef()->Output(strm, (void*)this, indent); }
  virtual ostream& 	OutputR(ostream& strm, int indent = 0) const // #IGNORE
    { return GetTypeDef()->OutputR(strm, (void*)this, indent); }

  virtual int	 	Dump_Load_impl(istream& strm, TAPtr par=NULL) // #IGNORE
    { return GetTypeDef()->Dump_Load_impl(strm, (void*)this, par); }
  virtual int	 	Dump_Load_Value(istream& strm, TAPtr par=NULL) // #IGNORE
    { return GetTypeDef()->Dump_Load_Value(strm, (void*)this, par); }

  virtual int	 	Load(istream& strm, TAPtr par=NULL, void** el = NULL);
  // #CAT_File Load object data from a file -- el is what!?
  virtual int	 	Load_File(TypeDef* td = NULL, void** el = NULL);
  // #IGNORE load object data from a file -- gets the filename from user
  virtual int	 	LoadAs_File(const String& fname="", TypeDef* td = NULL, void** el = NULL);
  // #MENU #MENU_ON_Object #ARGC_0 #LABEL_Load #CAT_File Load object data from given file name (if empty, prompt user for a name) -- use this in programs

  virtual int 		Save(ostream& strm, TAPtr par=NULL, int indent=0);
  // #CAT_File Save object data to a file
  virtual int 		SaveAs(ostream& strm, TAPtr par=NULL, int indent=0)
  // #ARGC_1 #CAT_File Save object data to a new file
    { return Save(strm,par,indent); }
  virtual int		Save_File(); 
  // #MENU #MENU_ON_Object #LABEL_Save #CAT_File saves the object to a file using current file name -- use this in programs
  virtual int		SaveAs_File(const String& fname = ""); 
  // #MENU #ARGC_0 #LABEL_Save_As #CAT_File Saves object data to a new file -- if fname is empty, it prompts the user -- use this in programs

  virtual String	GetFileNameFmProject(const String& ext, const String& tag = "", bool dmem_proc_no = false);
  // #CAT_File get file name from project file name -- useful for saving files associated with the project; ext = extension; tag = additional tag; fname = proj->base_name + tag + ext; if dmem_proc_no, add dmem proc no to file name.  empty if project not found
  
  virtual int 		Dump_Save_impl(ostream& strm, TAPtr par=NULL, int indent=0)
    { Dump_Save_pre(); 
      return GetTypeDef()->Dump_Save_impl(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_inline(ostream& strm, TAPtr par=NULL, int indent=0)
    { Dump_Save_pre(); 
      return GetTypeDef()->Dump_Save_inline(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_Path(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_Path(strm, (void*)this, par, indent); } // #IGNORE
  virtual int 		Dump_Save_Value(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_Value(strm, (void*)this, par, indent); } // #IGNORE

  virtual int		Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_SaveR(strm, (void*)this, par, indent); } 	// #IGNORE
  virtual int 		Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0)
    { return GetTypeDef()->Dump_Save_PathR(strm, (void*)this, par, indent); } // #IGNORE
  virtual bool		Dump_QuerySaveMember(MemberDef* md); 
     // #IGNORE default checks NO_SAVE directive; override to make save decision at runtime
  virtual bool		Dump_QuerySaveChildren() 
    {return true;} // #IGNORE override to make save decision at runtime

  virtual int		Edit();
  // #MENU #ARGC_0 #MENU_ON_Object #MENU_CONTEXT #NO_SCRIPT #CAT_Display Edit this object using the gui
//obs  virtual bool		CloseEdit();	// close any open edit dialogs for this object
  virtual bool		ReShowEdit(bool force = false);	// #CAT_Display reshows any open edit dialogs for this object
  virtual const iColor* GetEditColor(); // #IGNORE background color for edit dialog
  virtual const iColor* GetEditColorInherit(); // #IGNORE background color for edit dialog, include inherited colors from parents
#ifdef TA_GUI
  virtual const QPixmap* GetDataNodeBitmap(int, int& flags_supported) const
    {return NULL; } // #IGNORE gets the NodeBitmapFlags for the tree or list node -- see ta_qtbrowse_def.h
#endif
  virtual void		CloseLater();
  // #MENU #CONFIRM #NO_REVERT_AFTER #LABEL_Close_(Destroy) #NO_MENU_CONTEXT #CAT_ObjectMgmt PERMANENTLY Destroy this object!  This is not Iconify.
  virtual void		Close(); // #IGNORE an immediate version of Close for use in code (no waitproc delay)
  virtual bool		Close_Child(TAPtr obj);
  // #IGNORE actually closes a child object (should be immediate child)
  virtual bool		CopyFrom(TAPtr cpy_from);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #TYPE_ON_this #NO_SCOPE #CAT_ObjectMgmt Copy from given object into this object
  // this is a safe interface to UnSafeCopy
  virtual bool		CopyTo(TAPtr cpy_to);
  // #MENU #TYPE_ON_this #NO_SCOPE #CAT_ObjectMgmt Copy to given object from this object
  // need both directions to more easily handle scoping of types on menus
  virtual bool		DuplicateMe();
  // #MENU #CONFIRM #CAT_ObjectMgmt Make another copy of myself (done through owner)
  virtual bool		ChangeMyType(TypeDef* new_type);
  // #MENU #TYPE_this #CAT_ObjectMgmt Change me into a different type of object, copying current info (done through owner)
  virtual bool		SelectForEdit(MemberDef* member, SelectEdit* editor, const String& extra_label);
  // #MENU #CAT_Display select a given member for editing --\n if already on dialog, removes it & returns false (else true)
  virtual bool		SelectForEditNm(const String& memb_nm, SelectEdit* editor, const String& extra_label);
  // #IGNORE hard code interface for updating editors
  virtual bool		SelectFunForEdit(MethodDef* function, SelectEdit* editor, const String& extra_label);
  // #MENU #CAT_Display select a given function (method) for calling in a select edit dialog --\nif already on dialog, removes it & returns false (else true)
  virtual bool		SelectFunForEditNm(const String& function_nm, SelectEdit* editor, const String& extra_label);
  // #IGNORE hard code interface for updating editors
  virtual void		Help();
  // #MENU #CAT_Display get help on using this object

  
  static bool	UpdatePointers_NewPar_Ptr(taBase** ptr, taBase* old_par, taBase* new_par,
					  bool null_not_found = true);
  // #IGNORE update pointer if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated
  static bool	UpdatePointers_NewPar_SmPtr(taSmartPtr& ptr, taBase* old_par, taBase* new_par,
					    bool null_not_found = true);
  // #IGNORE update pointer if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated
  static bool	UpdatePointers_NewPar_Ref(taSmartRef& ref, taBase* old_par,
					  taBase* new_par, bool null_not_found = true);
  // #IGNORE update reference if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated

  virtual int	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  // #IGNORE update pointers for a new parent (e.g., after a copy operation): and anything that lives under old_par and points to something else that lives under old_par is updated to point to new_par: this default impl uses TA info to walk the members and find the guys to change; returns number changed

  static bool	UpdatePointers_NewParType_Ptr(taBase** ptr, TypeDef* par_typ, taBase* new_par,
					  bool null_not_found = false);
  // #IGNORE update pointer if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.
  static bool	UpdatePointers_NewParType_SmPtr(taSmartPtr& ptr, TypeDef* par_typ,
						taBase* new_par, bool null_not_found = false);
  // #IGNORE update pointer if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.
  static bool	UpdatePointers_NewParType_Ref(taSmartRef& ref, TypeDef* par_typ,
					  taBase* new_par, bool null_not_found = false);
  // #IGNORE update reference if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.

  virtual int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  // #IGNORE update pointers for a new parent of given type -- any pointer under this object that points to an object that lives under an object of par_typ (type of new_par): and anything that lives under old_par and points to something else that lives under old_par is updated to point to new_par: this default impl uses TA info to walk the members and find the guys to change; returns number changed

  static bool	UpdatePointers_NewObj_Ptr(taBase** ptr, taBase* ptr_owner, 
					  taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner
  static bool	UpdatePointers_NewObj_SmPtr(taSmartPtr& ptr, taBase* ptr_owner, 
					    taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner
  static bool	UpdatePointers_NewObj_Ref(taSmartRef& ref, taBase* ptr_owner, 
					  taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner

  virtual int	UpdatePointersToMe(taBase* new_ptr);
  // #IGNORE replace all pointers that might have pointed to old version of this object (and all of its kids!) to new_ptr -- called during ChangeType: default is to call UpdatePointers_NewObj on taMisc::default_scope; returns number changed
  virtual int	UpdatePointersToMe_impl(taBase* scope_obj, taBase* new_ptr);
  // #IGNORE actually does the work: above just passes in the scoper
  virtual int	UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr);
  // #IGNORE override this for list/group guys that contain sub-objs -- update all the sub objs that we own!

  virtual int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  // #IGNORE replace all pointers to old_ptr with new_ptr: walks the entire structure (members, lists, etc) and iteratively calls; returns number changed

  virtual void	CallFun(const String& fun_name);
  // #CAT_ObjectMgmt call function of given name on this object, prompting for args using gui interface

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
  static String		no_name; 	// return this for no names
  static int		no_idx;		// return this for no index
  static MemberDef*	no_mdef;	// return this for no memberdef ptr
  
#ifndef __MAKETA__
  union {
  int			refn_flags; // for efficient initialization
  struct {
  int			refn: 24;	// number of references to this object
  mutable int		m_flags: 8;
  };
  };
#endif
  
#ifdef DEBUG
  void			CheckDestroyed();// issues assertion if destroyed
#else
  inline void		CheckDestroyed() {} // should get optimized out
#endif
  void			Destroying(); // non-virtual called at beginning of destroy
  virtual bool		CheckConfig_impl(bool quiet);
  // #IGNORE usually not overridden, see Check[This/Child]_impl
  virtual void		CheckThisConfig_impl(bool quiet, bool& ok) {}
    // impl for us; can include embedded objects (but don't incl them in Child check); only clear ok (if invalid), don't set
  virtual void		CheckChildConfig_impl(bool quiet, bool& ok) {}
   // impl for checking children; only clear ok (if invalid), don't set
  virtual void 		Dump_Save_pre() {} // called before _impl, enables jit updating before save
  virtual String	GetStringRep_impl() const; // string representation, ex. for variants; default is typename:fullpath
  virtual void		UpdateAfterEdit_impl() {} // this is the preferred place to put all UAE actions, so they all take place before the notify
};

inline istream& operator>>(istream &strm, taBase &obj)
{ obj.Load(strm); return strm; }
inline ostream& operator<<(ostream &strm, taBase &obj)
{ obj.Save(strm); return strm; }

class TA_API taSmartPtr { // ##NO_INSTANCE ##NO_TOKENS "safe" ptr for taBase objects -- automatically does ref counts; designed to be binary compatible with taBase*
public:
  static TypeDef*	GetBaseType(TypeDef* this_typ);
    // returns a best-guess min type -- hacks by looking at the name
    
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
friend class taDataView; // for access to link
public:
  inline taBase*	ptr() const {return m_ptr;}
  void			set(taBase* src) {if (src == m_ptr) return;
    if (m_ptr) m_ptr->RemoveDataClient(this); m_ptr = NULL;
    if (src && src->AddDataClient(this)) m_ptr = src;} 
  
  virtual TypeDef*	GetBaseType() const {return &TA_taBase;}
  
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

  override TypeDef*	GetBaseType() const {return T::StatTypeDef(0);}
  
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

SmartRef_Of(taBase);		// basic ref if you don't know the type

class TA_API taOBase : public taBase {
  // #NO_TOKENS #NO_UPDATE_AFTER owned base class of taBase
INHERITED(taBase)
public:
  TAPtr			owner;	// #READ_ONLY #NO_SAVE pointer to owner

  taDataLink**		addr_data_link() {return &m_data_link;} // #IGNORE
  override taDataLink*	data_link() {return m_data_link;}	// #IGNORE
//  override void		set_data_link(taDataLink* dl) {m_data_link = dl;}

  TAPtr 	GetOwner() const	{ return owner; }
  TAPtr		GetOwner(TypeDef* tp) const { return taBase::GetOwner(tp); }
  TAPtr 	SetOwner(TAPtr ta)	{ owner = ta; return ta; }

  void	CutLinks();
  TA_BASEFUNS(taOBase); //
protected:
  taDataLink*		m_data_link; //
private:
  void 	Initialize()	{ owner = NULL; m_data_link = NULL;}
  void	Destroy();
}; //


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
friend class taOABase;
public:
  taBaseAdapter(taOABase* owner_): QObject(NULL) {owner = owner_;}
  ~taBaseAdapter();
protected:
  taOABase* owner; // #IGNORE
};
#endif // TA_USE_QT


//NOTE: the taOABase object is not used very much...
class TA_API taOABase : public taOBase {
  // #NO_TOKENS #NO_UPDATE_AFTER owned base class with QObject adapter for signals/slots
INHERITED(taOBase)
friend class taBaseAdapter;
public:
#ifdef TA_USE_QT
  taBaseAdapter* 	adapter; // #IGNORE
  void			SetAdapter(taBaseAdapter* adapter_);
  void 	Initialize()	{adapter = NULL;}
#else
  void 	Initialize()	{}
#endif
  void	CutLinks();
  TA_BASEFUNS(taOABase); //
private:
  void	Destroy() {CutLinks();}
};


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
  String		file_name; // #READ_ONLY #NO_SAVE #SHOW the current filename for this object

  override bool		SetFileName(const String& val); // #IGNORE note: we canonicalize name first
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
  // #MENU #MENU_ON_Edit #ARGC_0 #NO_SCRIPT #MENU_CONTEXT #CAT_Modify create n_objs new objects of given type

  String 	GetPath_Long(TAPtr ta=NULL, TAPtr par_stop = NULL) const;
  String 	GetPath(TAPtr ta=NULL, TAPtr par_stop = NULL) const;

  bool 		FindCheck(const String& nm) const // also check for el_typ
  { return ((name == nm) || InheritsFrom(nm) || el_typ->InheritsFrom(nm)); }

  MemberDef* 	FindMembeR(const String& nm, void*& ptr) const;
  // #CAT_Access extended to search in the list
  MemberDef* 	FindMembeR(TypeDef* it, void*& ptr) const;
  // #CAT_Access extended to search in the list

  override void	Close();
  override bool	Close_Child(TAPtr obj);
  override void	ChildUpdateAfterEdit(TAPtr child, bool& handled); 
  override void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); 

  ostream& 	OutputR(ostream& strm, int indent = 0) const;

  override int	Dump_SaveR(ostream& strm, TAPtr par=NULL, int indent=0);
  override int	Dump_Save_PathR(ostream& strm, TAPtr par=NULL, int indent=0);
  virtual int 	Dump_Save_PathR_impl(ostream& strm, TAPtr par, int indent); 
  override int	Dump_Load_Value(istream& strm, TAPtr par=NULL);

  override int	UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  override int	UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  override int	UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  override int	UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr);

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
  virtual int	Find(TypeDef* item) const;
  virtual int	Find(const String& item_nm) const	{ return taPtrList_ta_base::Find(item_nm); }
  // #CAT_Access find item with given name

  virtual bool	Remove(const String& item_nm)	{ return taPtrList_ta_base::Remove(item_nm); }
  virtual bool	Remove(TAPtr item)	{ return taPtrList_ta_base::Remove(item); }
  virtual bool	Remove(int idx);
  // Remove object at given index on list
  virtual bool	RemoveEl(TAPtr item)	{ return Remove(item); }
  // #MENU #ARG_ON_OBJ #CAT_Modify Remove given item from the list

  virtual void	EnforceSize(int sz);
  // #MENU add or remove elements to force list to be of given size
  virtual void	EnforceType();
  // enforce current type (all elements have to be of this type)
  void	EnforceSameStru(const taList_impl& cp);
  // make the two lists identical in terms of size and types of objects

  virtual bool	ChangeType(int idx, TypeDef* new_type);
  // change type of item at index
  virtual bool	ChangeType(TAPtr itm, TypeDef* new_type);
  // #MENU #ARG_ON_OBJ #CAT_Modify #TYPE_ON_el_base change type of item to new type, copying current info
  virtual int	ReplaceType(TypeDef* old_type, TypeDef* new_type);
  // #MENU #USE_RVAL #CAT_Modify #TYPE_ON_el_base replace all items of old type with new type (returns number changed)

  virtual TAPtr	FindType_(TypeDef* item_tp, int& idx) const; 	// #IGNORE

  void	SetBaseType(TypeDef* it); // set base (and default) type to given td

  MemberDef*	ReturnFindMd() const;	// return the find_md variable, initialized if necessary
#ifdef TA_GUI
  override const QPixmap* GetDataNodeBitmap(int bmf, int& flags_supported) const;
#endif
  override int		NumListCols() const {return 3;} // #IGNORE number of columns in a default list view for this list type
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
  void		El_SetIndex_(void* it, int idx) {((TAPtr)it)->SetIndex(idx);}
  String	El_GetName_(void* it) const { return ((TAPtr)it)->GetName(); }
  TALPtr	El_GetOwner_(void* it) const { return (TABLPtr)((TAPtr)it)->GetOwner(); }
  void*		El_SetOwner_(void* it)	{ ((TAPtr)it)->SetOwner(this); return it; }
  bool		El_FindCheck_(void* it, const String& nm) const
  { if (((TAPtr)it)->FindCheck(nm)) {TALPtr own = El_GetOwner_(it);
    return ((!own) || (own == (TALPtr)this));} return false; }

  void*		El_Ref_(void* it)	{ taBase::Ref((TAPtr)it); return it; }
  void*		El_unRef_(void* it)	{ taBase::unRef((TAPtr)it); return it; }
  void		El_Done_(void* it)	{ taBase::Done((TAPtr)it); }
  void*		El_Own_(void* it)	{ taBase::Own((TAPtr)it,this); return it; }
  void		El_disOwn_(void* it)
  { if(El_GetOwner_(it) == this) {((TAPtr)it)->CutLinks(); ((TAPtr)it)->Destroying();}
    El_Done_(El_unRef_(it)); }
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
  override void	CheckChildConfig_impl(bool quiet, bool& rval);
  override String ChildGetColText_impl(taBase* child, const KeyString& key, int itm_idx = -1) const;
};

template<class T> 
class taList: public taList_impl { // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER
public:
  T*		SafeEl(int idx) const		{ return (T*)SafeEl_(idx); }
  // #CAT_Access get element at index
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  // #CAT_Access fast element (no range checking)
  T* 		operator[](int i) const	{ return (T*)el[i]; }

  T*		DefaultEl() const		{ return (T*)DefaultEl_(); }
  // #CAT_Access returns the element specified as the default for this list

  T*		Edit_El(T* item) const		{ return SafeEl(Find((TAPtr)item)); }
  // #MENU #MENU_ON_Edit #USE_RVAL #ARG_ON_OBJ #CAT_Access Edit given list item

  virtual T*	FindName(const String& item_nm, int& idx=Idx) const { return (T*)FindName_(item_nm, idx); }
  // #MENU #USE_RVAL #ARGC_1 #CAT_Access Find element with given name (item_nm)
  virtual T* 	FindType(TypeDef* item_tp, int& idx=Idx) const { return (T*)FindType_(item_tp, idx); }
  // #CAT_Access find given type element (NULL = not here), sets idx

  T*		First() const			{ return (T*)First_(); }
  // #CAT_Access look at the first element; NULL if none
  virtual T*	Pop()				{ return (T*)Pop_(); }
  // #CAT_Modify pop the last element off the stack
  virtual T*	Peek() const			{ return (T*)Peek_(); }
  // #CAT_Access peek at the last element on the stack, if any

  virtual T*	AddUniqNameOld(T* item)		{ return (T*)AddUniqNameOld_((void*)item); }
  // #CAT_Modify add so that name is unique, old used if dupl, returns one used
  virtual T*	LinkUniqNameOld(T* item)	{ return (T*)LinkUniqNameOld_((void*)item); }
  // #CAT_Modify link so that name is unique, old used if dupl, returns one used

  virtual bool	MoveBefore(T* trg, T* item) { return MoveBefore_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just before the target item trg in the list
  virtual bool	MoveAfter(T* trg, T* item) { return MoveAfter_((void*)trg, (void*)item); }
  // #CAT_Modify move item so that it appears just after the target item trg in the list

// when a taList is declared in advance of defining the type within it, this breaks
//  void 	Initialize() 			{ SetBaseType(T::StatTypeDef(1)); }
  void	Initialize()			{ SetBaseType(StatTypeDef(0)); }
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
  void	Initialize() 		{ SetBaseType(&TA_taBase); }
  void 	Destroy()		{ };
  TA_BASEFUNS(taBase_List);
};


/* taDataView -- exemplar base class of a view of an object, of class taOBase or later

   A view is a high-level depiction of an object, ex. "Network", "Graph", etc.

  The IDataViews list object does not own the view object -- some outer controller is
  responsible for lifetime management of dataview objects.

  However, if a dataobject is destroying, it will destroy all its views

*/
class TA_API taDataView: public taOBase, public virtual IDataLinkProxy {
  // #NO_TOKENS  base class for views of an object
INHERITED(taOBase)
friend class DataView_List;
public:
  enum DataViewAction { // #BITS enum used to (safely) manually invoke one or more _impl actions
    CONSTR_POST		= 0x01, // #BIT (only used by DataViewer)
    CLEAR_IMPL		= 0x02, // #BIT (only used by T3DataView)
    RENDER_PRE		= 0x04, // #BIT
    RENDER_IMPL		= 0x08, // #BIT
    RENDER_POST		= 0x10, // #BIT
    CLOSE_WIN_IMPL	= 0x20,  // #BIT (only used by DataViewer)
    RESET_IMPL		= 0x40, // #BIT
    UNBIND_IMPL		= 0x80 // #BIT disconnect everyone from a data source
#ifndef __MAKETA__
    ,CLEAR_ACTS		= CLEAR_IMPL | CLOSE_WIN_IMPL // for Clear
    ,RENDER_ACTS	= CLEAR_IMPL | RENDER_PRE | RENDER_IMPL | RENDER_POST // for Render
    ,RESET_ACTS		= CLEAR_IMPL | CLOSE_WIN_IMPL | RESET_IMPL // for Reset
    ,UNBIND_ACTS	= UNBIND_IMPL
    
    ,CONSTR_MASK	= CONSTR_POST | RENDER_PRE | RENDER_IMPL | RENDER_POST 
      // mask for doing child delegations in forward order
    ,DESTR_MASK		= CLEAR_IMPL | CLOSE_WIN_IMPL | RESET_IMPL 
      // mask for doing child delegations in reverse order
#endif
  };
  
  taSmartRef		m_data;		// #READ_ONLY data -- referent of the item -- the data
  TypeDef*		data_base;	// #READ_ONLY #NO_SAVE Minimum type for data object

  taBase*		data() {return m_data;} // subclasses usually redefine a strongly typed version
  void 			SetData(taBase* ta); // #MENU set the data to which this points -- must be subclass of data_base
  int			dbu_cnt() {return m_dbu_cnt;} // batch update: -ve:data, 0:none, +ve:struct
  inline int		index() const {return m_index;} // convenience accessor
  virtual bool		isMapped() const {return true;} // for DataView classes, or anything w/ separate gui classes that get created distinct from view hierarchy
  virtual MemberDef*	md() const {return NULL;} // ISelectable property member stub
  virtual int		par_dbu_cnt(); // dbu of parent(s); note: only sign is accurate, not necessarily value (optimized)
  inline bool		hasParent() const {return (m_parent);} // encapsulated way to check for a par
  taDataView*		parent() const {return m_parent;} // typically lex override with strong type
  virtual TypeDef*	parentType() const {return &TA_taDataView;} // the controlling parent -- note that when in a list, this is the list owner, not the list; overrride for strong check in SetOwner

  virtual MemberDef*	GetDataMemberDef() {return NULL;} // returns md if known and/or knowable (ex. NULL for list members)
  virtual String	GetLabel() const; // returns a label suitable for tabview tabs, etc.
  virtual void 		ChildAdding(taDataView* child) {} // #IGNORE called from list;
  virtual void 		ChildRemoving(taDataView* child) {} // #IGNORE called from list; 
  virtual void		ChildClearing(taDataView* child) {} // override to implement par's portion of clear
  virtual void		ChildRendered(taDataView* child) {} // override to implement par's portion of render
  virtual void		CloseChild(taDataView* child) {}
  virtual void		Render() {DoActions(RENDER_ACTS);} 
    // renders the visible contents (usually override the _impls) -- MUST BE DEFINED IN SUB
  virtual void		Clear(taDataView* par = NULL) {DoActions(CLEAR_ACTS);} // clears the view (but doesn't delete any components) (usually override _impl)
  virtual void		Reset() {DoActions(RESET_ACTS);} 
    // clears, and deletes any components (usually override _impls)
  virtual void		Unbind() {DoActions(UNBIND_ACTS);} 
    // clears, and deletes any components (usually override _impls)
  virtual void		DoActions(DataViewAction acts); // do the indicated action(s) if safe in this context (ex loading, whether gui exists, etc.); par only needed when a _impl needs it
  
  virtual void		ItemRemoving(taDataView* item) {} // items call this on the root item -- usually used by a viewer to insure item removed from things like sel lists
  
  int	GetIndex() const {return m_index;}
  void	SetIndex(int value) {m_index = value;}
  TAPtr	SetOwner(TAPtr own); // update the parent; nulls it if not of parentType
  void	CutLinks();
  TA_BASEFUNS(taDataView)

public: // IDataLinkProxy
  override void*	This() {return (void*)this;}
//in taBase  virtual TypeDef*	GetTypeDef() const;
#ifndef TA_NO_GUI
  override taiDataLink*	link() const {return m_data.link();}
#else
  override taDataLink*	link() const {return m_data.link();}
#endif
//  virtual TypeDef*	GetDataTypeDef() const; 
  
public: // pseudo-IDataLinkClient-ish interface
  virtual TypeDef*	GetDataTypeDef() const {return m_data->GetTypeDef();} // TypeDef of the data
  virtual void		DataDataChanged(int dcr, void* op1, void* op2);
   // called when the data item has changed, esp. ex lists and groups; dispatches to the DataXxx_impl's
  virtual void		DataDestroying() {}

protected:
  int			m_dbu_cnt; // data batch update count; +ve is Structural, -ve is Parameteric only
  int			m_index; // for when in a list
  // NOTE: all Dataxxx_impl are supressed if dbu_cnt or par_dbu_cnt <> 0 -- see ta_type.h for detailed rules
  mutable taDataView* 	m_parent; // autoset on SetOwner, type checked as well
  
  override void		SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  override void		SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_);
  
  virtual void		DataDataChanged_impl(int dcr, void* op1, void* op2) {}
   // called when the data item has changed, esp. ex lists and groups, *except* UAE -- we also forward the last end of a batch update
  virtual void		DataUpdateAfterEdit_impl() {} // called by data for an UAE, i.e., after editing etc.
  virtual void		DataUpdateView_impl() { if(taMisc::gui_active) Render_impl(); } // called for Update All Views, and at end of a DataUpdate batch
  virtual void		DataStructUpdateEnd_impl() {} // called ONLY at end of a struct update -- derived classes usually do some kind of rebuild or render
  virtual void		DataChanged_Child(TAPtr child, int dcr, void* op1, void* op2) {} 
   // typically from an owned list
  virtual void		DoActionChildren_impl(DataViewAction acts) {} // only one action called at a time, if CONSTR do children in order, if DESTR do in reverse order; call child.DoActions(act)
  virtual void		Constr_post() {DoActionChildren_impl(CONSTR_POST);} 
    // extend to implement post-constr
  virtual void		Clear_impl() {DoActionChildren_impl(CLEAR_IMPL);} 
    // extend to implement clear
  virtual void		CloseWindow_impl() {DoActionChildren_impl(CLOSE_WIN_IMPL);} 
    // extend to implement clear
  virtual void		Render_pre() {DoActionChildren_impl(RENDER_PRE);} 
    // extend with pre-rendering code, if needed
  virtual void		Render_impl() {DoActionChildren_impl(RENDER_IMPL);}
    // extend with code that renders the window contents
  virtual void		Render_post() {DoActionChildren_impl(RENDER_POST);}
    // extend with post-rendering code, if needed
  virtual void		Reset_impl() {DoActionChildren_impl(RESET_IMPL);}
    // extend to implement reset
  virtual void		Unbind_impl() {DoActionChildren_impl(UNBIND_IMPL);}
    // extend to implement unbind
private:
  void			Initialize();
  void			Destroy() {CutLinks();}
};

// for explicit lifetime management
#define TA_DATAVIEWFUNS(b,i) \
  TA_BASEFUNS(b)

// convenience, for declaring the safe strong-typed parent
#define DATAVIEW_PARENT(T) \
  T* parent() const {return (T*)m_parent;} \
  TypeDef* parentType() const {return &TA_ ## T;}
  
// for strongly-typed lists
#define TA_DATAVIEWLISTFUNS(B,I,T) \
  T* SafeEl(int i) const {return (T*)SafeEl_(i);} \
  T* FastEl(int i) const {return (T*)FastEl_(i);} \
  TA_BASEFUNS(B);

class TA_API DataView_List: public taList<taDataView> { // #NO_TOKENS
INHERITED(taList<taDataView>)
public:
  override void 	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);
    // we send to an owner DataView DataChanged_Child
  
  virtual void		DoAction(taDataView::DataViewAction act); 
   // do a single action on all items; we also do self->Reset on Reset_impl
  
  override TAPtr SetOwner(TAPtr); // #IGNORE
  TA_DATAVIEWLISTFUNS(DataView_List, taList<taDataView>, taDataView) //
  
protected:
  taDataView*		data_view; // #IGNORE our owner, when owned by a taDataView, for efficiency

  override void*	El_Own_(void* it);
  override void		El_disOwn_(void* it);

private:
  void 	Initialize() { SetBaseType(&TA_taDataView); data_view = NULL;}
  void	Destroy() {}
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

class TA_API Variant_Array : public taArray<Variant> {
  // #NO_UPDATE_AFTER
public:
  STATIC_CONST Variant blank; // #HIDDEN #READ_ONLY 
  override void*	GetTA_Element(int i, TypeDef*& eltd) 
  { eltd = &TA_Variant; return FastEl_(i); }
  void Initialize()	{err = 0.0;};
  void Destroy()	{ };
  TA_BASEFUNS(Variant_Array);
  TA_ARRAY_FUNS(Variant_Array, Variant)
protected:
  int		El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(*((Variant*)a) > *((Variant*)b)) rval=1; else if(*((Variant*)a) == *((Variant*)b)) rval=0; return rval; }
  bool		El_Equal_(const void* a, const void* b) const
    { return (*((Variant*)a) == *((Variant*)b)); }
  String	El_GetStr_(const void* it) const { return ((Variant*)it)->toString(); }
  void		El_SetFmStr_(void* it, const String& val)
  { Variant tmp = (Variant)val; *((Variant*)it) = tmp; }
};
TA_ARRAY_OPS(Variant_Array)


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


class TA_API UserDataItemBase: public taBase {
  // #INLINE base class for all simple user data
INHERITED(taBase)
public:
  String		name;
  
  virtual bool		isSimple() const {return false;}
    // only true for UserDataItem class
    
  virtual const Variant valueAsVariant() const {return _nilVariant;}
  virtual bool		setValueAsVariant(const Variant& value) {return false;}
  
  bool	SetName(const String& value) {name = value; return true;}
  String GetName() const {return name;}
  TA_BASEFUNS(UserDataItemBase)
private:
  void Initialize() {}
  void Destroy() {}
};

class TA_API UserDataItem: public UserDataItemBase {
  // #INLINE an item of simple user data
INHERITED(UserDataItemBase)
public:
  Variant		value;
  
  override bool		isSimple() const {return true;}
  override const Variant valueAsVariant() const {return value;}
  override bool		setValueAsVariant(const Variant& v) {value = v; return true;}
  
  TA_BASEFUNS(UserDataItem)
private:
  void Initialize() {}
  void Destroy() {}
};

class TA_API UserDataItem_List: public taList<UserDataItemBase> {
// #CHILDREN_INLINE
INHERITED(taList<UserDataItem>)
public:
  
  TA_BASEFUNS(UserDataItem_List)
protected:

private:
  void Initialize() {SetBaseType(&TA_UserDataItemBase);}
  void Destroy() {}
};

SmartPtr_Of(UserDataItem_List) // UserDataItem_ListPtr


// define selectedit if no gui
#ifdef TA_NO_GUI
class TA_API SelectEdit {
  bool	do_nothing;
};
#endif // def TA_NO_GUI

#endif // ta_base_h
