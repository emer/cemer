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

#ifndef taBase_h
#define taBase_h 1

#include <taString>
#include <Variant>
#include <TypeDef>

#ifndef __MAKETA__
#ifdef TA_GUI
#include <QObject>
#endif
#endif

// parent includes:

// member includes:

// declare all other types mentioned but not required to include:
class taMatrix; // 
class taBaseItr; // 
class MemberDef; // 
class taBase_PtrList; // 
class taObjDiff_List; // 
class taDoc; // 
class ControlPanel; // 
class MethodDef; // 
class UserDataItem_List; // 
class UserDataItemBase; // 
class UserDataItem; // 
class taSmartRef; //
class taSmartPtr; //
class taList_impl; //
class taFiler; //
class taiMimeSource; //
class ISigLinkClient; //
class QVariant; // #IGNORE
class iColor; //
class MainWindowViewer; //


//////////////////////////////////////////////////////////////////////////////////
// Copy logic (this is complicated for dealing with constructors and = assignment
//
// void Copy_(const type& cp)   non-virtual private method that only copies data
//                              for THIS type does NOT call parent, nor copy parent data
// void Copy__(const type& cp)  simple COPYING flag wrapper around Copy_
// copy constructor             calls Copy__ to just copy stuff for this type
// void Copy_impl(const type& cp) non-virtual protected method defined in BASEFUNS
//                              that calls parent copy_impl, and then this copy -- this
//                              is the master copy function for assignment =
// void Copy(const type& cp)    public interface to call Copy_impl for a given guy
//
// then there is, confusingly, an entirely separate set of copy mechanisms that
// use the TA system to do more flexible copying using generic taBase* objects
// these all take pointer args * instead of reference args
//
// void UnSafeCopy(const taBase* cp) virtual protected defined in BASEFUNS,
//                              calls Copy_impl if source inherits from us, and
//                              CastCopyTo if we inherit from source, so we get
//                              common elements of subclass copied
// void CastCopyTo(taBase* cp)  virtual protected defined in BASEFUNS,
//                              casts source to our type, and calls Copy_impl on
//                              that guy from us
// bool Copy(const taBase* cp)  virtual public function that is master interface
//                              to this generic interface
// bool CanDoCopy_impl          master routine called by Copy that checks if it is
//                              OK to copy (Can = CanCopy_impl), and if it is ok, then it
//                              actually does the copy (Do = UnSafeCopy) -- this provides
//                              the high level of safe checking and flexibility
// CopyFromCustom_impl          for special cases where objects of divergent inheritence
//                              branches can still copy -- example is Matrix objects
//                              that can still copy from different types using Variant
//                              conversion interface (e.g. float from string etc)
//
// The bottom line is: define Copy_ in each class to copy just that class's members
// and use ->Copy(cp) to copy a generic taBase* guy -- only use = when the types are
// known and correct


// common defs used by ALL taBase types: Type and Copy guys
#define TA_BASEFUNS_MAIN_(y) \
private: \
  void Copy__(const y& cp); \
protected: \
  void Copy_impl(const y& cp); \
  void UnSafeCopy(const taBase* cp); \
  void CastCopyTo(taBase* cp) const; \
public: \
  static TypeDef* StatTypeDef(int) { return &TA_##y; } \
  TypeDef* GetTypeDef() const { return &TA_##y; } \
  void Copy(const y& cp); \
  bool Copy(const taBase* cp); \
  y& operator=(const y& cp);

#define TA_BASEFUNS_MAIN_DEFN_(y) \
  void y::Copy__(const y& cp) {  \
    SetBaseFlag(COPYING); \
      Copy_(cp); \
    ClearBaseFlag(COPYING);} \
  void y::Copy_impl(const y& cp) { \
    StructUpdate(true); \
      inherited::Copy_impl(cp); \
      Copy__(cp); \
    StructUpdate(false);} \
  void y::UnSafeCopy(const taBase* cp) { \
    if(cp->InheritsFrom(&TA_##y)) Copy_impl(*((y*)cp)); \
    else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this);} \
  void y::CastCopyTo(taBase* cp) const { y& rf = *((y*)cp); rf.Copy_impl(*this); } \
  void y::Copy(const y& cp) { Copy_impl(cp);} \
  bool y::Copy(const taBase* cp) {return taBase::Copy(cp);} \
  y& y::operator=(const y& cp) { Copy(cp); return *this;}

#define TA_TMPLT_BASEFUNS_MAIN_(y,T) \
private: \
  inline void Copy__(const y<T>& cp) { \
    SetBaseFlag(COPYING); \
      Copy_(cp); \
    ClearBaseFlag(COPYING);} \
protected: \
  void Copy_impl(const y<T>& cp) { \
    StructUpdate(true); \
      inherited::Copy_impl(cp); \
      Copy__(cp); \
    StructUpdate(false);} \
  void  UnSafeCopy(const taBase* cp) { if(cp->InheritsFrom(&TA_##y)) Copy_impl(*((y<T>*)cp)); \
    else if(InheritsFrom(cp->GetTypeDef())) cp->CastCopyTo(this); } \
  void  CastCopyTo(taBase* cp) const { y<T>& rf = *((y<T>*)cp); rf.Copy_impl(*this); } \
public: \
  static TypeDef* StatTypeDef(int) { return &TA_##y; } \
  TypeDef* GetTypeDef() const { return &TA_##y; } \
  void Copy(const y<T>& cp) { Copy_impl(cp);} \
  inline bool Copy(const taBase* cp) {return taBase::Copy(cp);} \
  y<T>& operator=(const y<T>& cp) { Copy(cp); return *this; }

// common defs used to make instances: Cloning and Tokens
#define TA_BASEFUNS_INST_(y) \
  taBase* Clone() const { return new y(*this); }  \
  taBase* MakeToken() const { return (taBase*)(new y); } \
  taBase* MakeTokenAry(int n) const { return (taBase*)(new y[n]); }

#define TA_TMPLT_BASEFUNS_INST_(y,T) \
  taBase* Clone() const { return new y<T>(*this); } \
  taBase* MakeToken() const { return (taBase*)(new y<T>); }  \
  taBase* MakeTokenAry(int n) const { return (taBase*)(new y<T>[n]); }

// ctors -- one size fits all (where used) thanks to Initialize__

#define TA_BASEFUNS_CTORS_(y) \
  explicit y (bool reg = true); \
  y (const y& cp, bool reg = true); 

#define TA_TMPLT_BASEFUNS_CTORS_(y,T) \
  explicit y (bool reg = true):inherited(false) { Initialize__(reg); } \
  y (const y<T>& cp, bool reg = true):inherited(cp, false) { Initialize__(reg); Copy__(cp);}

// definition of constructors and destructors: you must put these into .cpp files!
#define TA_BASEFUNS_CTORS_DEFN(y) \
  y::y(bool reg):inherited(false) { Initialize__(reg); } \
  y::y(const y& cp, bool reg):inherited(cp, false) { Initialize__(reg); Copy__(cp); } \
  y::~y () { CheckDestroyed(); unRegister(); Destroying(); Destroy(); } \
  TA_BASEFUNS_MAIN_DEFN_(y)

#define TA_BASEFUNS_CTORS_LITE_DEFN(y) \
  y::y(bool reg):inherited(false) { Initialize__(reg); } \
  y::y(const y& cp, bool reg):inherited(cp, false) { Initialize__(reg); Copy__(cp); } \
  y::~y() { CheckDestroyed(); Destroying(); Destroy(); } \
  TA_BASEFUNS_MAIN_DEFN_(y)

// common dtor/init, when using tokens (same for TMPLT)
#define TA_BASEFUNS_TOK_(y) \
  private: \
  void Initialize__(bool reg) {if (reg) Register(); Initialize(); \
    if (reg) InitSetDefaultName(); } \
  public: \
  ~y ();

#define TA_TMPLT_BASEFUNS_TOK_(y,T) \
  private: \
  inline void Initialize__(bool reg) {if (reg) Register(); Initialize(); \
    if (reg) InitSetDefaultName(); } \
  public: \
  ~y () { CheckDestroyed(); unRegister(); Destroying(); Destroy(); }


// common dtor/init when not using tokens (the LITE guys)
#define TA_BASEFUNS_NTOK_(y) \
  private: \
  inline void Initialize__(bool) {Initialize();}  \
  public: \
  ~y ()

#define TA_TMPLT_BASEFUNS_NTOK_(y,T) \
  private: \
  inline void Initialize__(bool) {Initialize();}  \
  public: \
  ~y () { CheckDestroyed(); Destroying(); Destroy(); }


// normal set of funs, for tokens, except ctors; you can use this yourself
// when you have consts in your class and can't use the generic ctors
#define TA_BASEFUNS_TOK_NCTORS(y) \
  TA_BASEFUNS_TOK_(y) \
  TA_BASEFUNS_MAIN_(y) \
  TA_BASEFUNS_INST_(y)

#define TA_TMPLT_BASEFUNS_TOK_NCTORS(y,T) \
  TA_TMPLT_BASEFUNS_TOK_(y,T) \
  TA_TMPLT_BASEFUNS_MAIN_(y,T) \
  TA_TMPLT_BASEFUNS_INST_(y,T)

// dummy, for when nothing to copy in this class
#define NOCOPY(y) \
  void Copy_(const y& cp){}

#define TMPLT_NOCOPY(y,T) \
  void Copy_(const y<T>& cp){}

// this is the typical guy to use for most classes, esp if they keep Tokens
// the 2 versions bake in the inherited guy, so you don't need to do that
// the ncopy version includes a dummy Copy_ func (typical for template instances)
#define TA_BASEFUNS(y) \
  TA_BASEFUNS_CTORS_(y) \
  TA_BASEFUNS_TOK_NCTORS(y)

#define TA_BASEFUNS_NOCOPY(y) \
  private: NOCOPY(y) public: \
  TA_BASEFUNS(y)

#define TA_BASEFUNS2(y,x) \
  private: INHERITED(x) public: \
  TA_BASEFUNS(y)

#define TA_BASEFUNS2_NOCOPY(y,x) \
  private: INHERITED(x) NOCOPY(y) public: \
  TA_BASEFUNS(y)


// for templates (single parameter)
#define TA_TMPLT_BASEFUNS(y,T) \
  TA_TMPLT_BASEFUNS_CTORS_(y,T) \
  TA_TMPLT_BASEFUNS_TOK_NCTORS(y,T)

#define TA_TMPLT_BASEFUNS_NOCOPY(y,T) \
  private: TMPLT_NOCOPY(y,T) public: \
  TA_TMPLT_BASEFUNS(y,T)

#define TA_TMPLT_BASEFUNS2(y,T,x) \
  private: INHERITED(x) public: \
  private: TMPLT_NOCOPY(y,T) public: \
  TA_TMPLT_BASEFUNS(y,T)

#define TA_TMPLT_BASEFUNS2_NOCOPY(y,T,x) \
  private: INHERITED(x) TMPLT_NOCOPY(y,T) public: \
  TA_TMPLT_BASEFUNS(y,T)


// this is the typical guy to use for "helper" or "value" classes
// that do not keep Tokens -- it avoids registration overhead
#define TA_BASEFUNS_LITE(y) \
  TA_BASEFUNS_CTORS_(y) \
  TA_BASEFUNS_MAIN_(y) \
  TA_BASEFUNS_INST_(y) \
  TA_BASEFUNS_NTOK_(y)

#define TA_BASEFUNS_LITE_NOCOPY(y) \
  private: NOCOPY(y) public: \
  TA_BASEFUNS_LITE(y)

#define TA_BASEFUNS2_LITE(y,x) \
  private: INHERITED(x) public: \
  TA_BASEFUNS_LITE(y)

#define TA_BASEFUNS2_LITE_NOCOPY(y,x) \
  private: INHERITED(x) NOCOPY(y) public: \
  TA_BASEFUNS_LITE(y)

// template versions, ex. for smart ptrs, and similar class with no reg
#define TA_TMPLT_BASEFUNS_LITE(y,T) \
  TA_TMPLT_BASEFUNS_CTORS_(y,T) \
  TA_TMPLT_BASEFUNS_MAIN_(y,T) \
  TA_TMPLT_BASEFUNS_INST_(y,T) \
  TA_TMPLT_BASEFUNS_NTOK_(y,T)

// macro for abstract base classes (with pure virtual methods, and no instance)
#define TA_ABSTRACT_BASEFUNS(y) \
  TA_BASEFUNS_CTORS_(y) \
  TA_BASEFUNS_MAIN_(y) \
  TA_BASEFUNS_TOK_(y)

#define TA_ABSTRACT_BASEFUNS_NOCOPY(y) \
  private: NOCOPY(y) public: \
  TA_ABSTRACT_BASEFUNS(y)


#define TA_TMPLT_ABSTRACT_BASEFUNS(y,T) \
  TA_TMPLT_BASEFUNS_CTORS_(y,T) \
  TA_TMPLT_BASEFUNS_MAIN_(y,T) \
  TA_TMPLT_BASEFUNS_TOK_(y,T)

// for use with templates
#define TA_TMPLT_TYPEFUNS(y,T) \
  static TypeDef* StatTypeDef(int) {  return &TA_##y##_##T; } \
  TypeDef* GetTypeDef() const { return &TA_##y##_##T; }

// this guy is your friend for most simple classes! esp good in plugins
#define SIMPLE_COPY(T) \
  void Copy_(const T& cp) {T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); }

// for when you need to give it a diff name
#define SIMPLE_COPY_EX(T,NAME) \
  void NAME(const T& cp) {T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); }

// this calls UpdatePointers_NewPar based on a major scoping owning parent, only if that
// parent is not already copying, and the parent is different than the copy parent
#define SIMPLE_COPY_UPDT_PTR_PAR(T,P) \
  void Copy_(const T& cp) {T::StatTypeDef(0)->CopyOnlySameType((void*)this, (void*)&cp); \
    UpdatePointers_NewPar_IfParNotCp(&cp, &TA_##P); }

// automated Init/Cut links guys -- esp good for code in Plugins
#define SIMPLE_INITLINKS(T) \
  void InitLinks() { inherited::InitLinks(); InitLinks_taAuto(&TA_##T); }

#define SIMPLE_CUTLINKS(T) \
  void CutLinks() { CutLinks_taAuto(&TA_##T); inherited::CutLinks(); }

#define SIMPLE_LINKS(T) \
  SIMPLE_INITLINKS(T); \
  SIMPLE_CUTLINKS(T)

// this is Sweetness and Light1(TM) -- everything is automatic and simple!
#define TA_BASEFUNS_SC(T) \
  SIMPLE_COPY(T); \
  TA_BASEFUNS(T)

// this is Sweetness and Light2(TM) -- everything is automatic and simple!
#define TA_SIMPLE_BASEFUNS(T) \
  SIMPLE_COPY(T); \
  SIMPLE_LINKS(T); \
  TA_BASEFUNS(T)

// for guys that have pointers to outside objects -- need to update if
// not within PAR scope
#define TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(T,P) \
  SIMPLE_COPY_UPDT_PTR_PAR(T,P); \
  SIMPLE_LINKS(T); \
  TA_BASEFUNS(T)

#define TA_SIMPLE_BASEFUNS2(y,x) \
  SIMPLE_COPY(y); \
  SIMPLE_LINKS(y); \
  TA_BASEFUNS2(y,x)

#define TA_SIMPLE_BASEFUNS_LITE(y) \
  SIMPLE_COPY(y); \
  SIMPLE_LINKS(y); \
  TA_BASEFUNS_LITE(y)

#define TA_SIMPLE_BASEFUNS2_LITE(y,x) \
  SIMPLE_COPY(y); \
  SIMPLE_LINKS(y); \
  TA_BASEFUNS2_LITE(y,x)

// simplified Get owner functions B = ta_base object, T = class name
#define GET_MY_OWNER(T) ((T *) GetOwner(&TA_##T))
#define GET_OWNER(B,T)  ((T *) ((B)->GetOwner(T::StatTypeDef(0))))

#define SET_POINTER(var,obj) (taBase::SetPointer((taBase**)&(var), (taBase*)(obj)))
#define DEL_POINTER(var) (taBase::DelPointer((taBase**)&(var)))

// standard smart refs and ptrs -- you should use this for every class
#define TA_SMART_PTRS(API, y) SMARTPTR_OF(y); SMARTREF_OF(API, y);

// note: the following iterators have dummy if and for loops to control the 
// allocation of the iterator, in a manner that works with break etc stmts

// generic iterator over items in taBase containers
#define TA_FOREACH(ELEM_VAR_NAME, LIST)   \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr; !FOREACH_done; FOREACH_done = true) \
      for(Variant ELEM_VAR_NAME = (LIST).IterFirst(FOREACH_itr); \
          FOREACH_itr.More(); \
          ELEM_VAR_NAME = (LIST).IterNext(FOREACH_itr))

// generic iterator over items in taBase containers, index version
#define TA_FOREACH_INDEX(IDX_VAR_NAME, LIST)                   \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr; !FOREACH_done; FOREACH_done = true) \
      for(int IDX_VAR_NAME = (LIST).IterFirstIndex(FOREACH_itr); \
          FOREACH_itr.More();                                    \
          IDX_VAR_NAME = (LIST).IterNextIndex(FOREACH_itr))

// generic reverse iterator over items in taBase containers
#define TA_FOREACH_REV(ELEM_VAR_NAME, LIST)                        \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr; !FOREACH_done; FOREACH_done = true) \
      for(Variant ELEM_VAR_NAME = (LIST).IterLast(FOREACH_itr);     \
          FOREACH_itr.More();                                       \
          ELEM_VAR_NAME = (LIST).IterPrev(FOREACH_itr))

// generic reverse iterator over items in taBase containers, index version
#define TA_FOREACH_INDEX_REV(IDX_VAR_NAME, LIST)                   \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr; !FOREACH_done; FOREACH_done = true) \
      for(int IDX_VAR_NAME = (LIST).IterLastIndex(FOREACH_itr);     \
          FOREACH_itr.More();                                       \
          IDX_VAR_NAME = (LIST).IterPrevIndex(FOREACH_itr))

// generic iterator over items in two taBase containers, index version, only checks first
#define TA_FOREACH_INDEX_TWO(IDX_VAR_NAME_a, LIST_a, IDX_VAR_NAME_b, LIST_b) \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr_a; !FOREACH_done; FOREACH_done = true)  \
      for (taBaseItr FOREACH_itr_b; !FOREACH_done; FOREACH_done = true) \
        for(int IDX_VAR_NAME_a = (LIST_a).IterFirstIndex(FOREACH_itr_a), \
              IDX_VAR_NAME_b = (LIST_b).IterFirstIndex(FOREACH_itr_b);  \
            FOREACH_itr_a.More();                                       \
            IDX_VAR_NAME_a = (LIST_a).IterNextIndex(FOREACH_itr_a),     \
              IDX_VAR_NAME_b = (LIST_b).IterNextIndex(FOREACH_itr_b))

// generic iterator over items in two taBase containers, index version, checks both
#define TA_FOREACH_INDEX_TWO_CHK(IDX_VAR_NAME_a, LIST_a, IDX_VAR_NAME_b, LIST_b) \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr_a; !FOREACH_done; FOREACH_done = true)  \
      for (taBaseItr FOREACH_itr_b; !FOREACH_done; FOREACH_done = true) \
        for(int IDX_VAR_NAME_a = (LIST_a).IterFirstIndex(FOREACH_itr_a), \
              IDX_VAR_NAME_b = (LIST_b).IterFirstIndex(FOREACH_itr_b);  \
            FOREACH_itr_a.More() && FOREACH_itr_b.More();               \
            IDX_VAR_NAME_a = (LIST_a).IterNextIndex(FOREACH_itr_a),     \
              IDX_VAR_NAME_b = (LIST_b).IterNextIndex(FOREACH_itr_b))

// generic reverse iterator over items in two taBase containers, index version, only checks first
#define TA_FOREACH_INDEX_REV_TWO(IDX_VAR_NAME_a, LIST_a, IDX_VAR_NAME_b, LIST_b) \
  if (bool FOREACH_done = false) { } else \
    for (taBaseItr FOREACH_itr_a; !FOREACH_done; FOREACH_done = true)   \
      for (taBaseItr FOREACH_itr_b; !FOREACH_done; FOREACH_done = true) \
        for(int IDX_VAR_NAME_a = (LIST_a).IterLastIndex(FOREACH_itr_a), \
              IDX_VAR_NAME_b = (LIST_b).IterLastIndex(FOREACH_itr_b);   \
            FOREACH_itr_a.More();                                       \
            IDX_VAR_NAME_a = (LIST_a).IterPrevIndex(FOREACH_itr_a),     \
              IDX_VAR_NAME_b = (LIST_b).IterPrevIndex(FOREACH_itr_b))

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

////////////////////////////////////////////////////////////////////////////////////
//              ta Base   ---   The Base of all type-aware classes

taTypeDef_Of(taBase);

class TA_API taBase {
  // #NO_TOKENS #INSTANCE #NO_UPDATE_AFTER Base type for all type-aware classes
  // has auto instances for all taBases unless NO_INSTANCE
friend class taSmartRef;
friend class taDataView;
friend class taBase_PtrList;
friend class taList_impl;

// Order of includes problem -- the Windows header file "Nb30.h" defines this
// as a macro.  If some file indirectly includes that file and then this file,
// it causes a compilation error on Windows.
// See also the comment in src/emergent/network/netdata.h
#ifdef REGISTERED
  //#define REGISTERED // uncomment to see previous macro definition
  #pragma message("Warning: undefining REGISTERED macro")
  #undef REGISTERED
#endif

  ///////////////////////////////////////////////////////////////////////////
  //    Types
public:

  enum Orientation { // must be same values as Qt::Orientation
    Horizontal = 0x1,
    Vertical = 0x2
  };

  enum ValType { // the basic data types widely supported by data-handling api's, esp. matrices
    VT_STRING,          // #LABEL_String an ANSI string of any length
    VT_DOUBLE,          // #LABEL_double a 8-byte floating point value (aprox 15 sig decimal digits)
    VT_FLOAT,           // #LABEL_float a 4-byte floating point value (aprox 7 sig decimal digits)
    VT_INT,             // #LABEL_int a signed integer in machine native size (32 or 64 bit)
    VT_BYTE,            // #LABEL_byte an unsigned 8-bit integer; used mostly for image components (rgb)
    VT_VARIANT,         // #LABEL_Variant a Variant, which can hold scalars, strings, matrices, and objects
    VT_VOIDPTR,         // #LABEL_void* a generic pointer (void*)
  };

  enum IndexMode {              // what kind of index is specified in the Elem access function
    IDX_UNK,                    // unknown -- use the argument to figure it out based on the specs for each index type (string defaults to IDX_NAME, IDX_SLICE favored over IDX_COORDS where ambiguous)
    IDX_IDX,                    // single scalar numerical index (flat index) -- negative numbers count from the back of the list, initial index is 0 -- ignores any existing view filtering on container and uses original dimensions
    IDX_NAME,                   // single string value -- find element object by name or string value (if applicable) -- simple wildcard expressions are supported (*, ? [..]) -- returns single value of first match or NULL if none -- obeys any existing view filtering on container
    IDX_NAMES,                  // String_Matrix -- find element objects by name or string value (if applicable) -- simple wildcard expressions are supported (*, ? [..]) -- returns container of items that match any of the names in the matrix (empty if none) -- obeys any existing view filtering on container
    IDX_COORD,                  // 1D int_Matrix with size = number of dims in container -- get item from an explicit coordinate along each dimension, any of which can be negative to get from the end (for multidimensional containers) -- returns a single item -- ignores any existing view filtering on container and uses original dimensions
    IDX_COORDS,                 // 1D or 2D int_Matrix with first (inner) dim = number of dims in container specifying coordinates of items in container (if container is 1D then int_Matrix can be 1D), second (outer) dim of arbitrary size (< size of container) to hold any number of elements to select -- returns container of items (could be the original container with a selector filter set to access only these items, depending on the container type) -- ignores any existing view filtering on container and uses original dimensions
    IDX_FRAMES,                // for matrix objects, 1D int_Matrix that specifies indexes of frames to access for matrix -- used for datatable rows
    IDX_SLICE,                  // 2D slice_Matrix or int_Matrix with first (inner) dim of size 3 = start:stop:skip slice ranges for each dimension (any of which can be negative = start from end or iterate backward), outer dim = number of dims of container (one slice spec per dimension) -- ignores any existing view filtering on container and uses original dimensions
    IDX_MASK,                   // byte_Matrix with same shape as container = bool mask of items to include (0 = don't include, 1 = include) -- any existing mask is logical AND'd to produce intersection
    IDX_MISC,                  // miscellaneous case-specific indexing format, typically taking a Variant_Matrix and doing things like typedef matching or other specific cases
  };

  enum BaseFlags { // #BITS control flags
    BF_0                = 0, // #IGNORE
    THIS_INVALID        = 0x0001, // #EXPERT CheckThisConfig_impl has detected a problem
    CHILD_INVALID       = 0x0002, // #EXPERT CheckChildConfig_impl returns issue with a child
    COPYING             = 0x0004, // #EXPERT this object is currently within a Copy function
    USE_STALE           = 0x0008, // #EXPERT calls setStale on appropriate changes; usually set in Initialize
    BF_READ_ONLY        = 0x0010, // #EXPERT this object should be considered readonly by most code (except controlling objs) and by CSS -- note that ro is a property -- use that to query the ro status
    BF_GUI_READ_ONLY    = 0x0020, // #EXPERT a less restrictive form of ro intended to prevent users from modifying an object, but still permit programmatic access; RO ==> GRO
    DESTROYING          = 0x0040, // #EXPERT Set in Destroying at the very beginning of destroy
    DESTROYED           = 0x0080, // #EXPERT set in base destroy (DEBUG only); lets us detect multi destroys
    NAME_READONLY       = 0x0100, // #EXPERT set to disable editing of name
    REGISTERED          = 0x0200, // #EXPERT set when registered (must unreg)
    BF_MISC1            = 0x1000, // #EXPERT miscellaneous user flag -- useful for internal temp flags that user does not need to see (e.g., marking an object as used or not in a given context)
    BF_MISC2            = 0x2000, // #EXPERT miscellaneous user flag -- useful for internal temp flags that user does not need to see (e.g., marking an object as used or not in a given context)
    BF_MISC3            = 0x4000, // #EXPERT miscellaneous user flag -- useful for internal temp flags that user does not need to see (e.g., marking an object as used or not in a given context)
    BF_MISC4            = 0x8000, // #EXPERT miscellaneous user flag -- useful for internal temp flags that user does not need to see (e.g., marking an object as used or not in a given context)
    INVALID_MASK        = THIS_INVALID | CHILD_INVALID,
    COPY_MASK          = THIS_INVALID | CHILD_INVALID | NAME_READONLY, // flags to copy when doing an object copy
    EDITABLE_MASK      = BF_READ_ONLY | BF_GUI_READ_ONLY, // flags in the Editable group
  };

  enum DumpQueryResult { // #IGNORE Dump_QuerySaveMember response
    DQR_NO_SAVE,        // #EXPERT definitely do not save
    DQR_SAVE,           // #EXPERT definitely save
    DQR_DEFAULT         // #EXPERT do default for this member (this is the base result)
  };

  ///////////////////////////////////////////////////////////////////////////
  //    Reference counting mechanisms, all static just for consistency..
public:

  static int            GetRefn(taBase* it)     { return it->refn; } // #IGNORE
#ifdef DEBUG
  static void           Ref(taBase& it);             // #IGNORE
  static void           Ref(taBase* it);             // #IGNORE
#else
  static void           Ref(taBase& it)         { it.refn.ref(); }           // #IGNORE
  static void           Ref(taBase* it)         { it->refn.ref(); }          // #IGNORE
#endif
  static void           UnRef(taBase* it) {unRef(it); Done(it);} // #IGNORE
  static void           Own(taBase& it, taBase* onr);   // #IGNORE note: also does a RefStatic() on first ownership
  static void           Own(taBase* it, taBase* onr);   // #IGNORE note: also does a Ref() on new ownership
  static void           Own(taSmartRef& it, taBase* onr);       // #IGNORE for semantic compat with other Owns
protected:
  // legacy ref counting routines, for compatability -- do not use for new code
  // note that guards/tests in these are "defensive", not "by design"
#ifdef DEBUG
  static void           unRef(taBase* it); // #IGNORE
  static void           Done(taBase* it); // #IGNORE
#else
  static void           unRef(taBase* it)
    { it->refn.deref(); }  // #IGNORE
  static void           Done(taBase* it)
    { if ((it->refn == 0) && (!it->HasBaseFlag(DESTROYED))) delete it;}
    // #IGNORE -- we check the flag for similar reasons as given in unRef
#endif
  static void           unRefDone(taBase* it)
    { if (!it->refn.deref() && (!it->HasBaseFlag(DESTROYED))) delete it;}
         // #IGNORE
  ///////////////////////////////////////////////////////////////////////////
  //    Pointer management routines (all pointers should be ref'd!!)
public:

  static void           InitPointer(taBase** ptr) { *ptr = NULL; } // #IGNORE
  static void           SetPointer(taBase** ptr, taBase* new_val);       // #IGNORE
  static void           OwnPointer(taBase** ptr, taBase* new_val, taBase* onr); // #IGNORE
  static void           DelPointer(taBase** ptr);                                 // #IGNORE

  ///////////////////////////////////////////////////////////////////////////
  //    Basic constructor/destructor ownership/initlink etc interface
public:

  virtual void          InitLinks()             { };
  // #IGNORE initialize links to other objs and do more elaborate object initialization, called after construction & SetOwner (added to object hierarchy).  ALWAYS CALL PARENT InitLinks!!!
  virtual void          CutLinks();
  // #IGNORE cut any links to other objs, called upon removal from a group or owner.  ALWAYS CALL PARENT CutLinks!!!  MIGHT BE CALLED MULTIPLE TIMES
  virtual void          InitLinks_taAuto(TypeDef* td);
  // #IGNORE automatic TA-based initlinks: calls inherited and goes through only my members & owns them
  virtual void          CutLinks_taAuto(TypeDef* td);
  // #IGNORE automatic TA-based cutlinks: goes through only my members & calls cutlinks and calls inherited

  void                  Register();
  // #IGNORE non-virtual, called in constructors to register token in token list
  void                  unRegister();
  // #IGNORE non-virtual, called in destructors to unregister token in token list
  virtual void          SetTypeDefaults();
  // #IGNORE initialize modifiable default initial values stored with the typedef -- see TypeDefault object in ta_defaults.  currently not used; was called in taBase::Own
  virtual void          SetTypeDefaults_impl(TypeDef* ttd, taBase* scope); // #IGNORE
  virtual void          SetTypeDefaults_parents(TypeDef* ttd, taBase* scope); // #IGNORE


protected:  // Impl
  void                  InitSetDefaultName();
  // #IGNORE called only in Initialize__ -- checks for NOT is_loading and is_duplicating and then calls SetDefaultName()
#ifdef DEBUG
  void                  CheckDestroyed();// issues error msg or assertion if destroyed
#else
  inline void           CheckDestroyed() {} // should get optimized out
#endif
  void                  Destroying(); // non-virtual called at beginning of destroy

  ///////////////////////////////////////////////////////////////////////////
  // actual constructors/destructors and related: defined in TA_BASEFUNS for derived classes
public:
  static  TypeDef*      StatTypeDef(int);       // #IGNORE
  static taBase*        MakeToken(TypeDef* td);
  // #IGNORE static version to make a token of the given type
  static taBase*        MakeTokenAry(TypeDef* td, int no);
  // #IGNORE static version to make an array of tokens of the given type

  explicit taBase(bool=false)   { Register(); Initialize(); }
  taBase(const taBase& cp, bool=false)  { Register(); Initialize(); Copy_impl(cp); }
  virtual ~taBase()     { Destroy(); } //

  virtual taBase*       Clone() const           { return new taBase(*this); } // #IGNORE
  virtual taBase*       MakeToken() const       { return new taBase; }  // #IGNORE
  virtual taBase*       MakeTokenAry(int no) const { return new taBase[no]; } // #IGNORE
//  taBase&             operator=(const taBase& cp) { Copy(cp); return *this;}
  virtual TypeDef*      GetTypeDef() const;     // #IGNORE
  taBase*               New(int n_objs=1, TypeDef* type=NULL,
    const String& name = "(default name)")
    { return New_impl(n_objs, type, name);}
  // #CAT_ObjectMgmt Create n_objs objects of given type (type is optional)

  virtual taBase*       ChooseNew(taBase* origin) { return NULL; }
  // #IGNORE called by taiWidgetTokenChooser to create a new object of this type from the chooser, if the user so chooses
  virtual bool          HasChooseNew()            { return false; }
  // #IGNORE return true if this class defines a ChooseNew function
protected:
  virtual taBase*       New_impl(int n_objs, TypeDef* type,
    const String& nm) { return NULL; }

  ////////////////////////////////////////////////////////////////////,T///////
  //    Object managment flags (taBase supports up to 8 flags for basic object mgmt purposes)
public:

  bool                  HasBaseFlag(int flag) const;
  // #CAT_ObjectMgmt true if flag set, or if multiple, any set
  void                  SetBaseFlag(int flag);
  // #CAT_ObjectMgmt sets the flag(s)
  void                  ClearBaseFlag(int flag);
  // #CAT_ObjectMgmt clears the flag(s)
  void                  ChangeBaseFlag(int flag, bool set)
    {if (set) SetBaseFlag(flag); else ClearBaseFlag(flag);}
  // #CAT_ObjectMgmt sets or clears the flag(s)
  int                   baseFlags() const {return base_flags;}
  // #IGNORE flag values; see also HasBaseFlag
  inline bool           useStale() const {return HasBaseFlag(USE_STALE);}
    // #IGNORE
  inline void           setUseStale(bool val)
    {if (val) SetBaseFlag(USE_STALE); else ClearBaseFlag(USE_STALE);}
    // #IGNORE
  int                   GetEditableState(int mask) const;
  // #IGNORE returns READ_ONLY and GUI_READ_ONLY, which also (in default behavior) factors in the owner's state supercursively until/unless not found; WARNING: result may include other flags, so you must &
  bool                  isDestroying() const;
    // #IGNORE returns true if this object or ANY of its owners is currently destroying -- if an owner is going, we're going too eventually..
  bool                  isReadOnly() const
    {return (GetEditableState(BF_READ_ONLY) & BF_READ_ONLY);}
    // #IGNORE true if the object is (supercursively) strongly read-only
  bool                  isGuiReadOnly() const
    {return (GetEditableState((BF_READ_ONLY | BF_GUI_READ_ONLY))
       & (BF_READ_ONLY | BF_GUI_READ_ONLY));}
    // #IGNORE true if the object is (supercursively) read-only in the gui
protected:
  virtual int           GetThisEditableState_impl(int mask) const;
    // extend this guy to factor in special purpose flags or runtime conditions
  virtual int           GetOwnerEditableState_impl(int mask) const;
   // you can stub this one out to prevent supercursively searching for the flag

  ///////////////////////////////////////////////////////////////////////////
  //    Basic object properties: index in list, owner, name, description, etc
public:

  virtual int           GetIndex() const {return -1;}
  // #CAT_ObjectMgmt object's index within an owner list.  cached by some objs.
  virtual void          SetIndex(int value) {};
  // #IGNORE set the objects index value.  note: typically don't do a notify, because list itself will take care of notifying gui clients
  virtual int           GetEnabled() const {return -1;}
  // #IGNORE for items that support an enabled/disabled state; -1=n/a, 0=disabled, 1=enabled (note: (bool)-1 = true)
  virtual void          SetEnabled(bool value) {};
  // #IGNORE
  virtual int           GetSpecialState() const { return -1; }
  // #IGNORE for items that support an alternative special state that should be communicated to users via a subtle background color (e.g., a flag of some sort is set that alters behavior in an important way) -- -1 = n/a, 0 = in non-special (default) state, 1..4 = special states -- multiple levels are supported with different colors: 1 = lavender, 2 = light yellow, 3 = light green, 4 = light red (pink)
  virtual bool          HasName() const { return false; }
  // #CAT_ObjectMgmt does the object have a name field that can be set?
  virtual bool          SetName(const String& nm) {return false;}
  // #CAT_ObjectMgmt #SET_name Set the object's name
  virtual String        GetName() const         { return _nilString; }
  // #CAT_ObjectMgmt #GET_name Get the name of the object
  virtual String        GetDisplayName() const;
  // #IGNORE can be overridden to provide a more elaborate or cleaned-up user-visible name for display purposes (default is just GetName())
  virtual void          MakeNameUnique();
  // #CAT_ObjectMgmt make sure my name is unique relative to names of objects associated with my owner (e.g., if it is a list object), typically because my name has changed, and owner needs to ensure that all names are unique
  virtual String        GetDesc() const {return _nilString;}
  // #IGNORE a type-specific description of the specific functionality/properties of the object
  virtual void          SetDefaultName() {} // #IGNORE note: called non-virtually in every constructor
  void                  SetDefaultName_(); // #IGNORE default behavior for >=taNBase -- you can call this manually for taOBase (or others that implement Name)
  virtual void          SetDefaultName_impl(int idx); // #IGNORE called from within, or by list -- NAME_TYPE will determine what we do with idx
  virtual String        GetTypeDecoKey() const { return _nilString; }
  // #IGNORE lookup key for visual decoration of an item reflecting its overall type information, used for font colors in the gui browser, for example
  virtual bool          GetQuiet() const {return false;}
  // #IGNORE general-purpose fuzzy flag for suppressing Warning messages from an object, maybe because it is special, user shuts them off, etc.
  virtual String        GetStateDecoKey() const;
  // #IGNORE lookup key for visual decoration of an item reflecting current state information, used for background colors in the gui browser, for example

  //////////////////////////////////////////////////////////////
  //    Container element access

  virtual bool          IsContainer()   { return false; }
  // #CAT_Access is this item a container object or not?
  virtual taBase*       ElemPath(const String& path, TypeDef* expect_type = NULL,
                                 bool err_msg = true) const;
  // #CAT_Access returns taBase item at given path under this object -- path can skip over steps -- a search is conducted to find items -- this provides the same kind of flexibility that css provides in accessing elements of the structural hierarchy -- also allows for type checking of result -- it is just a wrapper around FindFromPath with optional error checking
  taBase*               operator[](const String& path) const
  { return ElemPath(path, NULL, true); }
  // #IGNORE returns taBase item at given path under this object -- path can skip over steps -- a search is conducted to find items -- this provides the same kind of flexibility that css provides in accessing elements of the structural hierarchy -- it is just a call to ElemPath with err_msg set to true, so you can use directly, in chained expressions etc, for cases where you know the items should exist -- use ElemPath in code where it may not exist
  virtual Variant       Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return _nilVariant; }
  // #CAT_Access get element(s) from container -- return can be a single item or a Matrix of multiple items, depending on the index -- see IndexMode for all the possible indexing modes and associated return values (some of which are not applicable to some containers, as documented for that container)
    virtual IndexMode   IndexModeDecode(const Variant& idx, int cont_dims) const;
    // #CAT_Access #EXPERT decode index mode from variant index, and number of dimensions in the container object
    virtual bool        IndexModeValidate(const Variant& idx, IndexMode md, int cont_dims) const;
    // #CAT_Access #EXPERT validate that the index is of an appopriate configuration for given index mode -- issues appropriate error messages if not good
    virtual taMatrix*   ElView() const  { return NULL; }
    // #CAT_Access #EXPERT View of list -- matrix that specifies a subset of items to view, for display and other kinds of functions
    virtual IndexMode   ElViewMode() const  { return IDX_UNK; }
    // #CAT_Access #EXPERT View mode for view of list -- specifies what kind of view data there is, for display and other kinds of functions -- only valid cases are IDX_COORDS or IDX_MASK, or IDX_UNK if no view specified
    virtual int         ElemCount() const  { return 0; }
    // #CAT_Access #EXPERT number of elements in the container -- used for iteration control
  virtual int           IterCount() const;
  // #CAT_Access how many items will be iterated over if we iterate through this container -- this is fast for no view and IDX_COORDS views and slow (requiring summing over mask) for IDX_MASK views
  virtual Variant       IterFirst(taBaseItr& itr) const;
  // #CAT_Access get iterator for this container and start it at the first item for iterating through items in this container -- if no valid first item, iterator is deleted: e.g., taBaseItr itr; for(Variant itm = cont.IterFirst(itr); itr.More(); itm = cont.IterNext(itr)) { ... } -- see also TA_FOREACH macro
  virtual Variant       IterNext(taBaseItr& itr) const;
  // #CAT_Access iterate to next item in container using given iterator -- when the end is reached, the iterator pointer is automatically deleted and the pointer is set to NULL (and returns _nilVariant) -- use itr as test to see if there is a next item -- see IterFirst() for more docs
  virtual int           IterFirstIndex(taBaseItr& itr) const;
  // #CAT_Access get iterator for this container and start index at the first item for iterating through items in this container, returns index if iterator was set to a valid index, -1 if not, in which case iterator is deleted: e.g., taBaseItr itr; for(Variant itm = cont.IterFirstIndex(itr); itr.More(); itm = cont.IterNextIndex(itr)) { ... } -- see also TA_FOREACH_INDEX macro
  virtual int           IterNextIndex(taBaseItr& itr) const;
  // #CAT_Access iterate to index of next item in container using given iterator -- when the end is reached, the iterator pointer is automatically deleted and the pointer is set to NULL (and returns _nilVariant) -- use itr as test to see if there is a next item -- see IterFirstIndex() for more docs
  virtual Variant       IterLast(taBaseItr& itr) const;
  // #CAT_Access get reverse iterator for this container and start it at the last item for iterating through items in this container -- if no valid last item, iterator is deleted: e.g., taBaseItr itr; for(Variant itm = cont.IterLast(itr); itr.More(); itm = cont.IterNextRev(itr)) { ... } -- see also TA_FOREACH_REV macro
  virtual Variant       IterPrev(taBaseItr& itr) const;
  // #CAT_Access iterate to previous item in container using given iterator -- when the end is reached, the iterator pointer is automatically deleted and the pointer is set to NULL (and returns _nilVariant) -- use itr as test to see if there is a prev item -- see IterLast() for more docs
  virtual int           IterLastIndex(taBaseItr& itr) const;
  // #CAT_Access get iterator for this container and start index at the last item for iterating through items in this container in reverse, returns index if iterator was set to a valid index, -1 if not, in which case iterator is deleted: e.g., taBaseItr itr; for(Variant itm = cont.IterLastIndex(itr); itr.More(); itm = cont.IterPrevIndex(itr)) { ... } -- see also TA_FOREACH_INDEX_REV macro
  virtual int           IterPrevIndex(taBaseItr& itr) const;
  // #CAT_Access iterate to index of previous item in container using given iterator -- when the end is reached, the iterator pointer is automatically deleted and the pointer is set to NULL (and returns _nilVariant) -- use itr as test to see if there is a prev item -- see IterLastIndex() for more docs

    //////////////   Iteration Implementation, lower-level functions   //////////////

    virtual bool        IterValidate(taMatrix* vmat, IndexMode mode, int cont_dims) const;
    // #IGNORE validate view matrix and mode for suitability as iterators -- only IDX_COORDS or IDX_MASK are supported for iteration view modes
    virtual bool        IterFirst_impl(taBaseItr& itr) const;
    // #IGNORE implementation function to actually get first item -- returns true if there is a valid first item in iterator, false otherwise.  default should work for most cases
    virtual bool        IterNext_impl(taBaseItr& itr) const;
    // #IGNORE implementation function to actually get next item -- returns true if there is a valid next item in iterator, false otherwise.  does NOT call DelItr if not valid.  default should work for most cases

    virtual bool        IterLast_impl(taBaseItr& itr) const;
    // #IGNORE implementation function to actually get last item -- returns true if there is a valid last item in iterator, false otherwise.  does NOT call DelItr if not valid. default should work for most cases
    virtual bool        IterPrev_impl(taBaseItr& itr) const;
    // #IGNORE implementation function to actually get prev item -- returns true if there is a valid prev item in iterator, false otherwise.  does NOT call DelItr if not valid.  default should work for most cases

    virtual Variant     IterElem(taBaseItr& itr) const   { return _nilVariant; }
    // #IGNORE access current item according to iterator -- this is used by IterFirst and IterNext, etc and not typically required for end users
    virtual bool        FixSliceValsFromSize(int& start, int& end, int size) const;
    // #IGNORE fix the start and end slice values based on actual size -- returns false if values are invalid after all that

  virtual taList_impl*  children_() {return NULL;}
  // #IGNORE for lists, and for taOBase w/ default children
  const taList_impl*    children_() const
  { return const_cast<const taList_impl*>(const_cast<taBase*>(this)->children_());}
  // #IGNORE mostly for testing if has children
  virtual taBase*       SetOwner(taBase*)               { return(NULL); } // #IGNORE
  virtual taBase*       GetOwner() const        { return(NULL); } // #CAT_ObjectMgmt
  virtual taBase*       GetOwner(TypeDef* td) const; // #CAT_ObjectMgmt
  virtual taBase*       GetThisOrOwner(TypeDef* td); // #IGNORE get this obj or first owner that is of type td
  virtual taBase*       GetMemberOwner(bool highest = false) const;
  // #CAT_ObjectMgmt get the object that owns this object as a member -- if this object is not a member of its Owner (or has no owner) then it returns NULL -- if highest then it proceeds recursively up to the highest member owner and returns that -- i.e., if this is a member of a member (which is an object) of larger object, this will return the largest such object that contains this as a member at some level
  virtual taBase*       GetParent() const;
  // #CAT_ObjectMgmt typically the first non-list/group owner above this one
  bool                  IsParentOf(const taBase* obj) const; // #CAT_ObjectMgmt true if this object is a direct or indirect parent of the obj (or is the obj)
  bool                  IsChildOf(const taBase* obj) const; // #CAT_ObjectMgmt true if this object is a direct or indirect child of the obj (or is the obj)

  ///////////////////////////////////////////////////////////////////////////
  //    Paths in the structural hierarchy
public:

  virtual String        GetPath_Long(taBase* ta=NULL, taBase* par_stop=NULL) const;
  // #IGNORE get path from root (default), but stop at par_stop if non-null  -- ta is used for recursion and should be NULL for any end-user calls
  virtual String        GetPath(taBase* ta=NULL, taBase* par_stop=NULL) const;
  // #CAT_ObjectMgmt get path without name information, stop at par_stop if non-null -- ta is used for recursion and should be NULL for any end-user calls
  virtual String        GetPathNames(taBase* ta=NULL, taBase* par_stop=NULL) const;
  // #CAT_ObjectMgmt get paths using string names instead of indexes, which is good for portability, stop at par_stop if non-null -- ta is used for recursion and should be NULL for any end-user calls
  virtual taBase*       FindFromPath(const String& path, MemberDef*& ret_md, int start=0) const;
  // #CAT_ObjectMgmt find object from path (starting from this, and position start of the path -- ret_md is return member def: if NULL and return is !NULL, then it is a member of a list or group, not a member in object
  virtual Variant       GetValFromPath(const String& path, MemberDef*& ret_md, bool warn_not_found=false) const;
  // #CAT_ObjectMgmt get a member value from given path -- only follows direct members (of members) of this object -- does not look into items in lists or groups

  // utility functions for doing path stuff
  static int            GetNextPathDelimPos(const String& path, int start);
  // #IGNORE get the next delimiter ('.' or '[') position in the path
  static int            GetLastPathDelimPos(const String& path);
  // #IGNORE get the last delimiter ('.' or '[') position in the path

  virtual TypeDef*      GetScopeType();
  // #IGNORE gets my scope type (if NULL, it means no scoping, or root)
  virtual taBase*       GetScopeObj(TypeDef* scp_tp=NULL);
  // #IGNORE gets the object that is at the scope type above me (uses GetScopeType() or scp_tp)
  virtual bool          SameScope(taBase* ref_obj, TypeDef* scp_tp=NULL);
  // #IGNORE determine if this is in the same scope as given ref_obj (uses my scope type)
  static int            NTokensInScope(TypeDef* type, taBase* ref_obj, TypeDef* scp_tp=NULL);
  // #IGNORE number of tokens of taBase objects of given type in same scope as ref_obj
  virtual taBase*       GetUndoBarrier();
  // #IGNORE get the nearest owner of this object that is marked as an UNDO_BARRIER -- any changes under that object should not affect anything outside its own scope, and thus undo changes can be saved strictly within that scope

  //////////////////////////////////////////////////////////////////////
  //    Saving and Loading to/from files
public:
  virtual bool          SetFileName(const String& val)  {return false;}
  // #CAT_File set file name for object
  virtual String        GetFileName() const     { return _nilString; }
  // #CAT_File get file name object was last saved with
  virtual String        GetFileNameFmProject(const String& ext, const String& tag = "",
                                  const String& subdir = "", bool dmem_proc_no = false);
  // #CAT_File get file name from project file name -- useful for saving files associated with the project; ext = extension; tag = additional tag; subdir = additional directory after any existing in project name; fname = proj->base_name (subdir) + tag + ext; if dmem_proc_no, add dmem proc no to file name.  empty if project not found
  virtual String        GetProjDir() const;
  // #CAT_File get the directory path associated with the project that this object lives within

  static taFiler*       StatGetFiler(TypeItem* td, String exts= _nilString,
    int compress=-1, String filetypes =_nilString);
  // #IGNORE gets file dialog for the TypeItem -- clients must ref/unrefdone; ext is for non-default extension (otherwise looks up EXT_); compress -1=default, 0=none, 1=yes
  taFiler*              GetFiler(TypeItem* td = NULL, const String& exts = _nilString,
    int compress=-1, const String& filetypes = _nilString);
  // #IGNORE gets filer for this object (or TypeItem if non-null) -- clients must ref/unrefdone; ext is for non-default extension (otherwise looks up EXT_); compress -1=default, 0=none, 1=yes; exts/ft's must match, and are ,-separated lists

  virtual int           Load_strm(std::istream& strm, taBase* par=NULL, taBase** loaded_obj_ptr = NULL);
  // #EXPERT #CAT_File Load object data from a file -- sets pointer to loaded obj if non-null: could actually load a different object than this (e.g. if this is a list or group)
  virtual int           Save_strm(std::ostream& strm, taBase* par=NULL, int indent=0);
  // #EXPERT #CAT_File Save object data to a file stream
  taFiler*              GetLoadFiler(const String& fname, String exts = _nilString,
    int compress=-1, String filetypes = _nilString, bool getset_file_name = true);
  // #IGNORE get filer with istrm opened for loading for file fname; if empty, prompts user with filer chooser.  NOTE: must unRefDone the filer when done with it in calling function!
  virtual int           Load(const String& fname="", taBase** loaded_obj_ptr = NULL);
  // #MENU #MENU_ON_Object #ARGC_0 #CAT_File Load object data from given file name (if empty, prompt user for a name) -- sets pointer to loaded obj if non-null: could actually load a different object than this (e.g. if this is a list or group)
  virtual int           Load_cvt(taFiler*& flr);
  // #IGNORE convert stream from old to new format (if needed)

  taFiler*              GetSaveFiler(const String& fname, String ext = _nilString,
    int compress=-1, String filetypes=_nilString, bool getset_file_name = true, bool make_copy = false);
  // #IGNORE get filer with ostrm opened for saving for file fname; if empty, prompts user with filer chooser.  NOTE: must unRefDone the filer when done with it in calling function!
  taFiler*              GetAppendFiler(const String& fname, const String& ext="",
    int compress=-1, String filetypes=_nilString, bool getset_file_name = true);
  // #IGNORE get filer with ostrm opened for appending for file fname; if empty, prompts user with filer chooser.  NOTE: must unRefDone the filer when done with it in calling function!
  virtual int           Save();
  // #MENU #MENU_ON_Object #EDIT_READ_ONLY #CAT_File saves the object to a file using current file name
  virtual int           SaveAs(const String& fname = "");
  // #MENU #ARGC_0 #EDIT_READ_ONLY #CAT_File Saves object data to a new file -- if fname is empty, the user is prompted with a file dialog

  virtual int           Save_String(String& save_str, taBase* par=NULL, int indent=0);
  // #EXPERT #CAT_File dump full object save information to a string, which contains the exact information that would be saved to a file for Save_strm -- just a string-stream (sstream) wrapper around Save_strm
  virtual int           Load_String(const String& load_str, taBase* par=NULL, taBase** loaded_obj_ptr = NULL);
  // #EXPERT #CAT_File load full object information from a string, which should have been generated through a corresponding Save_String call -- must contain the exact information that would be saved to a file for Save_strm -- just a string-stream (sstream) wrapper around Load_strm

  virtual String GetValStr(void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false) const;
  // #IGNORE get a value string for this object (ptr=0) -- called by TypeDef GetValStr -- default for inline is just to iterate over members and output values just as in TypeDef code -- can overload for more complex classes for inlines
  static String GetValStr_ptr(const TypeDef* td, const void* base, void* par = NULL,
                              MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                              bool force_inline = false);
  // #IGNORE get a value string for pointer to ta base object (ptr=1) -- called by TypeDef GetValStr
  virtual bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                          TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                          bool force_inline = false);
  // #IGNORE set value from a string for this object (ptr=0) -- called by TypeDef SetValStr -- default for inline is just to iterate over members and output values just as in TypeDef code -- can overload for more complex classes that might still be inlinable
  static bool   SetValStr_ptr(const String& val, TypeDef* td, void* base,
                              void* par = NULL, MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                              bool force_inline = false);
  // #IGNORE set value from a string for ptr to taBase (ptr=1) -- called by TypeDef SetValStr
  virtual int   ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                              void* par = NULL, TypeDef* par_typ=NULL, MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT);
  // #IGNORE replace string value -- does a GetValStr, replace srch with repl in that string, then does a SetValStr -- always iterates over members of classes instead of doing inline to prevent replacing member names -- returns number replaced (0 = none) -- mbr_filt = filter for members to replace in -- if non-empty, member name for terminal value members where replace actually occurs (as opposed to owner class objects) must contain this string

  virtual taObjDiffRec*  GetObjDiffVal(taObjDiff_List& odl, int nest_lev,
                               MemberDef* memb_def=NULL, const void* par=NULL,
                                       TypeDef* par_typ=NULL, taObjDiffRec* par_od=NULL) const;
  // #IGNORE add this object and all its members and sub-objects to the object diff list

  //////////////////////////////////////////////////////////////////////
  //    Low-level dump load/save
public:

  virtual int           Dump_Load_impl(std::istream& strm, taBase* par=NULL) // #IGNORE
  { return GetTypeDef()->Dump_Load_impl(strm, (void*)this, par); }
  virtual taBase*       Dump_Load_Path_ptr(const String& el_path, TypeDef* ld_el_typ);
  // #IGNORE load a new object that is an owned pointer that lives on 'this' object, of given type and path -- returns ptr to new obj or NULL if failure
  virtual taBase*       Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ);
  // #IGNORE 'this' is the parent, and el_path specifies path to child -- create appropriate obj -- returns ptr to new obj or NULL if failure -- only really defined for taList..
  virtual void          Dump_Load_pre() {};
  // #IGNORE -- called just before single-object Load_strm -- use to reset stuff prior to loading
  virtual int           Dump_Load_Value(std::istream& strm, taBase* par=NULL) // #IGNORE
  { return GetTypeDef()->Dump_Load_Value(strm, (void*)this, par); }
  virtual void          Dump_Load_post() {}
  // #IGNORE called after load, in normal (non loading) context if has DUMP_LOAD_POST directive

  virtual void          Dump_Save_GetPluginDeps();
  // #IGNORE called prior to saving, to build the plugin deps in .plugin_deps
  // note: this routine (and overrides) is in ta_dump.cpp
  virtual void          Dump_Save_pre() {}
  // #IGNORE called before _Path, enables jit updating before save
  virtual int           Dump_Save_impl(std::ostream& strm, taBase* par=NULL, int indent=0)
  { Dump_Save_pre();
    return GetTypeDef()->Dump_Save_impl(strm, (void*)this, par, indent); } // #IGNORE
  virtual int           Dump_Save_inline(std::ostream& strm, taBase* par=NULL, int indent=0)
  { Dump_Save_pre();
    return GetTypeDef()->Dump_Save_inline(strm, (void*)this, par, indent); } // #IGNORE
  virtual int           Dump_Save_Path(std::ostream& strm, taBase* par=NULL, int indent=0)
  { return GetTypeDef()->Dump_Save_Path(strm, (void*)this, par, indent); } // #IGNORE
  virtual int           Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent=0)
  { return GetTypeDef()->Dump_Save_Value(strm, (void*)this, par, indent); } // #IGNORE

  virtual int           Dump_SaveR(std::ostream& strm, taBase* par=NULL, int indent=0)
  { return GetTypeDef()->Dump_SaveR(strm, (void*)this, par, indent); }  // #IGNORE
  virtual int           Dump_Save_PathR(std::ostream& strm, taBase* par=NULL, int indent=0)
  { return GetTypeDef()->Dump_Save_PathR(strm, (void*)this, par, indent); } // #IGNORE
  virtual DumpQueryResult Dump_QuerySaveMember(MemberDef* md);
  // #IGNORE default checks NO_SAVE directive; override to make save decision at runtime
  virtual bool          Dump_QuerySaveChildren()
  {return true;} // #IGNORE override to make save decision at runtime

  ///////////////////////////////////////////////////////////////////////////
  //    Updating of object properties
public:

  virtual void          UpdateAfterEdit();
  // #CAT_ObjectMgmt (aka UAE) called after editing, or any user change to members (eg. in the interface, script)
  virtual void          UpdateAfterEdit_NoGui();
  // #CAT_ObjectMgmt does any object-related updates based on changes, but does NOT trigger any gui updates -- use this for updating members inside of an object's UAE
  virtual bool          UAEProgramDefault() { return false; }
  // #IGNORE what is default setting of update_after flag for programs that modify fields on this object -- the base default is false, to produce reasonable speed -- only use where essential
  virtual void          ChildUpdateAfterEdit(taBase* child, bool& handled);
  // #IGNORE called by a child in its UAE routine; provides child notifications  NOTE: only member objects are detected; subclasses that want to notify on owned taBase* members must override and check for those instances manually
  virtual void          UpdateAfterMove(taBase* old_owner);
  // #IGNORE called after object has been moved from one location to another in the object hierarchy (i.e., list Transfer fun) -- actual functions should be put in the _impl version which should call inherited:: etc just as for UAE -- use for updating pointers etc
  virtual void          UpdateAllViews();
  // #CAT_Display called after data changes, to update views
  virtual void          RebuildAllViews();
  // #CAT_Display call after data changes, to rebuild views, typically when a child is added
  virtual void          SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  // #IGNORE sends the indicated notification to all siglink clients, if any; virtual so we can override to trap/monitor
  void                  SigEmitUpdated();
  // #CAT_ObjectMgmt send the ITEM_UPDATED data changed signal to indicate to gui elements etc that this item has been updated
  void                  StructUpdate(bool begin) { BatchUpdate(begin, true); }
  // #CAT_ObjectMgmt bracket structural changes with (nestable) true/false calls;
  void                  DataUpdate(bool begin) { BatchUpdate(begin, false); }
  // #CAT_ObjectMgmt bracket data value changes with (nestable) true/false calls;
  bool                  InStructUpdate();
  // #CAT_ObjectMgmt is object currently within a struct update already?

  virtual bool          isDirty() const {return false;}
  // #IGNORE implemented by very few, esp. Project -- Dirty is used to indicate the need to resave an object
  virtual void          setDirty(bool value);
  // #CAT_ObjectMgmt set the dirty flag indicating a change in object values; 'true' gets forwarded up; 'false' does nothing

  virtual bool          isStale() const {return false;}
  // #IGNORE implemented by few, esp. Program and Network -- Stale indicates the need to "rebuild" an object with such semantics (ex regen script, rebuild a net, etc.)
  virtual void          setStale();
  // #CAT_ObjectMgmt set the stale flag indicating a change in object values; gets forwarded up ("true" is implied, only the impl obj defines when it is cleared)

protected:  // Impl
  virtual void          UpdateAfterEdit_impl() {}
  // this is the preferred place to put all UAE actions, so they all take place before the notify
  virtual void          UpdateAfterMove_impl(taBase* old_owner) {}
  // for actions that should be performed after object has been moved from one location to another in the structure hierarchy

  ///////////////////////////////////////////////////////////////////////////
  //    SigLinks -- notify other guys when you change
public:

  virtual taSigLink*   sig_link() {return NULL;} // #IGNORE link for viewer system created when needed, deleted when 0 clients -- all delegated functions must be of form: if(sig_link()) sig_link->SomeFunc(); NOT autocreated by call to this func -- call GetSigLink() to force creation
  virtual taSigLink*   GetSigLink(); // #IGNORE forces creation; can still be NULL if the type doesn't support siglinks
  bool                  AddSigClient(ISigLinkClient* dlc); // #IGNORE note: only applicable for classes that implement siglinks
  bool                  RemoveSigClient(ISigLinkClient* dlc); // #IGNORE WARNING: link is undefined after this
  virtual String&       ListSigClients(String& strm, int indent = 0);
  // #CAT_ObjectMgmt list all the data clients for this object to string
protected:      // Impl
  virtual void          BatchUpdate(bool begin, bool struc);
  // #IGNORE bracket changes with (nestable) true/false calls; data clients can use it to supress change updates
  virtual void          SmartRef_SigDestroying(taSmartRef* ref, taBase* obj);
  // #IGNORE the obj (to which we had a ref) is about to destroy (the ref has already been NULL'ed)
  virtual void          SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
    int sls, void* op1_, void* op2_) {}
  // #IGNORE the obj (to which we have a ref) has signalled the indicated data change
  virtual void          SmartRef_SigChanging(taSmartRef* ref,
    taBase* obj, bool setting) {}
  // #IGNORE the obj ref has either been removed (smartref now null) or added (smartref already set to that object)


  ///////////////////////////////////////////////////////////////////////////
  //    Checking the configuration of objects prior to using them
public:

  bool   TestError(bool test, const char* fun_name,
                          const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                   const char* g=0, const char* h=0) const;
  // #CAT_ObjectMgmt if test, then report error, including object name, type, and path information; returns test -- use e.g. if(TestError((condition), "fun", "msg")) return    false;
  bool   TestWarning(bool test, const char* fun_name,
                     const char* a, const char* b=0, const char* c=0,
                     const char* d=0, const char* e=0, const char* f=0,
                     const char* g=0, const char* h=0) const;
  // #CAT_ObjectMgmt if test, then report warning, including object name, type, and path information; returns test -- use e.g. if(TestWarning((condition), "fun", "msg")) return false;
  virtual void  DebugInfo(const char* fun_name,
                          const char* a, const char* b=0, const char* c=0,
                          const char* d=0, const char* e=0, const char* f=0,
                          const char* g=0, const char* h=0) const;
  // #CAT_ObjectMgmt reports object identity in addition to the error message -- use this for any debug messages that get logged in reference to this object

#ifndef __MAKETA__
  void                  CheckConfig(bool quiet, bool& rval)
    {if (!CheckConfig_impl(quiet)) rval = false;}
    // this one is typically used in CheckXxx_impl routines; we don't do gui wrap stuff
#endif
  bool                  CheckConfig(bool quiet = false)
  { return CheckConfig_Gui(false, quiet);}
  // #CAT_ObjectMgmt check the configuration of this object and all its children (defaults to no confirm of success)
  bool                  CheckConfig_Gui(bool confirm_success = true, bool quiet = false);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt #ARGC_0 #LABEL_CheckConfig check the configuration of this object and all its children -- failed items highlighted in red, items with failed children in yellow
  void                  ClearCheckConfig(); // #IGNORE this can be called when a CheckConfig_impl routine blindly assert ok, ex. for an object that has an "off" or "disable" state; this routine updates the gui if the state has now changed

  virtual void  CheckError_msg(const char* a, const char* b=0, const char* c=0,
                               const char* d=0, const char* e=0, const char* f=0,
                               const char* g=0, const char* h=0) const;
  // #IGNORE generate error message

  inline bool   CheckError(bool test, bool quiet, bool& rval,
                           const char* a, const char* b=0, const char* c=0,
                           const char* d=0, const char* e=0, const char* f=0,
                           const char* g=0, const char* h=0) const {
    if(!test) return false;
    rval = false;
    if(!quiet) CheckError_msg(a,b,c,d,e,f,g,h);
    return test;
  }
  // #CAT_ObjectMgmt for CheckConfig routines: if test, then report config error, including object name, type, and path information; returns test & sets rval to false if test is true -- use e.g. CheckError((condition), quiet, rval, "msg"));

protected: // impl
  virtual bool          CheckConfig_impl(bool quiet);
  // #IGNORE usually not overridden, see Check[This/Child]_impl
  virtual void          CheckThisConfig_impl(bool quiet, bool& ok) {}
  // impl for us; can include embedded objects (but don't incl them in Child check); only clear ok (if invalid), don't set
  virtual void          CheckChildConfig_impl(bool quiet, bool& ok) {}
  // impl for checking children; only clear ok (if invalid), don't set

  ///////////////////////////////////////////////////////////////////////////
  //   Clipboard Queries and Edit Actions (for drag-n-drop, cut/paste etc)
  //   ms=NULL indicates SRC ops/context, else DST ops
  //  child="item here" for item, else NULL for "into" parent
#ifdef TA_GUI
public:
  void                  QueryEditActions(taiMimeSource* ms, int& allowed, int& forbidden);
  // #IGNORE
  int                   EditAction(taiMimeSource* ms, int ea); // #IGNORE

  void                  ChildQueryEditActions(const MemberDef* md, const taBase* child,
        taiMimeSource* ms, int& allowed, int& forbidden);
  // #IGNORE gives ops allowed on child, md valid if obj is a member of query parent, o/w NULL; child is sel item, or NULL if querying parent only; ms is clipboard or drop contents
  virtual int           ChildEditAction(const MemberDef* md, taBase* child,
        taiMimeSource* ms, int ea);
  // #IGNORE note: multi source ops will have child=NULL

protected: //  note: all impl code is in ta_qtclipdata.cpp
  virtual void          QueryEditActionsS_impl(int& allowed, int& forbidden);
    // called once per src item, by controller
  virtual void          QueryEditActionsD_impl(taiMimeSource* ms,
    int& allowed, int& forbidden);
    // gives ops allowed on child, with ms being clipboard or drop contents, md valid if we are a member, o/w NULL
  virtual int           EditActionS_impl(int ea);
    // called once per src item, by controller
  virtual int           EditActionD_impl(taiMimeSource* ms, int ea);
    // called once per ms item, in 0:N order, by ourself

  virtual void          ChildQueryEditActions_impl(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden);
    // called in
  virtual int           ChildEditAction_impl(const MemberDef* md, taBase* child,
        taiMimeSource* ms, int ea);
    // op implementation (non-list/grp)
#endif // TA_USE_GUI
  ///////////////////////////////////////////////////////////////////////////
  //    Browser gui
public:
  static const KeyString key_name; // #IGNORE "name" -- Name, note: can easily be empty
  static const KeyString key_type; // #IGNORE "type" -- def to typename, but some like progvar append their own subtype
  static const KeyString key_type_desc; // #IGNORE "type_desc" -- static type description
  static const KeyString key_desc; // #IGNORE "desc" -- per-instance desc if available (def to type)
  static const KeyString key_disp_name; // #IGNORE "disp_name" -- DisplayName, never empty
  static const KeyString key_unique_name; // #IGNORE "unique_name" -- DottedName, for token lists, DisplayName if empty

  virtual const String  statusTip(const KeyString& key = "") const;
  // #IGNORE the default text returned for StatusTipRole (key usually not needed)
  virtual const String  GetToolTip(const KeyString& key) const;
  // #IGNORE the default text returned for ToolTipRole
  virtual String        GetColText(const KeyString& key, int itm_idx = -1) const;
  // #IGNORE default keys are: name, type, desc, disp_name
  virtual const QVariant GetColData(const KeyString& key, int role) const;
  // #IGNORE typ roles are ToolTipRole, StatusTipRole; key can be blank if not col-specific

protected:
  virtual String        ChildGetColText_impl(taBase* child, const KeyString& key,
    int itm_idx = -1) const {return _nilKeyString;}

  ///////////////////////////////////////////////////////////////////////////
  //    Edit Dialog gui
public:
  // NOTE: this plain Edit seems weird and vestigial and should be nuked:
  virtual bool          Edit();
  // #CAT_Display Edit this object using the gui -- this will be an edit dialog or an edit panel depending on ...???
  virtual bool          EditDialog(bool modal = false);
  // #MENU #ARGC_0 #MENU_ON_Object #MENU_SEP_BEFORE #MENU_CONTEXT #NO_SCRIPT #EDIT_READ_ONLY #CAT_Display Edit this object in a popup dialog using the gui (if modal == true, the edit dialog blocks all other gui operations until the user closes it)
  virtual bool          EditPanel(bool new_tab = false, bool pin_tab = false);
  // #MENU #ARGC_0 #MENU_ON_Object #NO_SCRIPT #EDIT_READ_ONLY #CAT_Display Edit this object in a panel in the gui browser (if new_tab == true, then a new edit panel tab is opened for it, if pin_tab == true then the new tab is pinned in place (option ignored for new_tab == false))
  virtual MainWindowViewer* GetMyBrowser();
  // #CAT_Display get the project browser for this object's project owner
  virtual bool          BrowserSelectMe();
  // #CAT_Display select this item in the main project browser (only works if gui is active, etc) -- returns success
  virtual bool          BrowserExpandAll();
  // #CAT_Display expand all sub-leaves under this item in the browser
  virtual bool          BrowserCollapseAll();
  // #CAT_Display collapse all sub-leaves under this item in the browser
  virtual void          BrowseMe();
  // #MENU #MENU_ON_Object #MENU_CONTEXT #EDIT_READ_ONLY #CAT_Display show this object in its own browser
  virtual bool          BrowserEditEnable() { return false; }
  // #IGNORE is this item editable in the tree browser interface?
  virtual String        BrowserEditString() { return GetDisplayName(); }
  // #IGNORE the string representation to use for editing this item in the browser
  virtual bool          BrowserEditSet(const String& new_val_str, int move_after = 0)
  { return false; }
  // #IGNORE browser edit calls this with new value string to update value of item when editing is applied -- return true if edit is successful -- move_after is direction to move after editing  (+1 - down 1, -1 up 1) -- only needed if the edit causes something to interfere with the normal flow of the editor (e.g., a new object is created), where the object then needs to recapitulate that movement
  virtual bool          GuiFindFromMe(const String& find_str="");
  // #CAT_Display activate the gui find dialog starting from this object, with given find string
  // #CAT_Display reshows any open edit dialogs for this object
  virtual const iColor GetEditColor(bool& ok); // #IGNORE background color for edit dialog
  virtual const iColor GetEditColorInherit(bool& ok);
  // #IGNORE background color for edit dialog, include inherited colors from parents
  virtual void          WindowShowHook() {};
  // #CAT_Display #EXPERT this is called when a top-level window that represents this object becomes visible
  virtual void          WindowHideHook() {};
  // #CAT_Display #EXPERT this is called when a top-level window that represents this object becomes invisible

#if defined(TA_GUI) && !defined(__MAKETA__)
  virtual const QPixmap* GetDataNodeBitmap(int, int& flags_supported) const
    {return NULL; } // #IGNORE gets the NodeBitmapFlags for the tree or list node -- see ta_qtbrowse_def.h
#endif
  virtual String        StringFieldLookupFun(const String& cur_txt, int cur_pos,
                                             const String& mbr_name, int& new_pos)
  { return _nilString; }
  // #IGNORE special lookup function called when Ctrl-L is pressed for string members -- is passed current text and position of cursor, and name of member, and it must return the replacement text for the entire edit (if rval is empty, nothing happens)

  virtual void          CallFun(const String& fun_name);
  // #CAT_ObjectMgmt call function (method) of given name on this object, prompting for args using gui interface
  static  void          CallObjFun(taBase* obj, const String& fun_name);
  // #CAT_ObjectMgmt #CSS_LIST_EXPAND_1 call function (method) of given name on given object, prompting for args using gui interface
  static  void          SetMemberStr(taBase* obj, const String& memb_name,
                                     const String& str);
  // #CAT_ObjectMgmt #CSS_LIST_EXPAND_1 set member value based on string value -- memb_name can be an arbitrary full path below the obj
  static  void          SetMemberVar(taBase* obj, const String& memb_name,
                                     const Variant& val);
  // #CAT_ObjectMgmt #CSS_LIST_EXPAND_1 set member value based on variant -- memb_name can be an arbitrary full path below the obj
  virtual void          MemberUpdateAfterEdit(MemberDef* md, bool edit_dialog = false) { };
  // #CAT_ObjectMgmt the given member was just edited to a new value -- apply any member-specific changes before the global UpdateAfterEdit function is called.  if called from a gui edit dialog interface, the edit_dialog flag is set, so that behavior can be appropriately differentiated

  virtual Variant       GetGuiArgVal(const String& fun_name, int arg_idx);
  // #IGNORE overload this to get default initial arg values for given function and arg index -- function must be marked with ARG_VAL_FM_FUN[_n] comment directive, and _nilVariant rval will be ignored (NOTE: definitely call inherited:: because this is used for ChangeMyType!

  ///////////////////////////////////////////////////////////////////////////
  //    Copying and changing type
public:

  bool                  CanCopy(const taBase* cp, bool quiet = true) const;
    // #IGNORE the retail version
  void                  CanCopy(const taBase* cp, bool quiet, bool& ok) const
    {if (CanCopy(cp, quiet)) return; ok = false;}
    // #IGNORE convenience, for nested calls

  virtual bool          Copy(const taBase* cp);
  // #IGNORE this is a generic copy, that enables common-subclass copying, or even copying from disparate clases that might have a sensible copy semantic
  virtual void          CopyFromSameType(void* src_base)
  { GetTypeDef()->CopyFromSameType((void*)this, src_base); }
  // #IGNORE copy values from object of same type
  virtual void          CopyOnlySameType(void* src_base)
  { GetTypeDef()->CopyOnlySameType((void*)this, src_base); }
  // #IGNORE copy only those members from same type (no inherited members)
  virtual void          MemberCopyFrom(int memb_no, void* src_base)
  { GetTypeDef()->MemberCopyFrom(memb_no, (void*)this, src_base); }
   // #IGNORE copy given member index no from source object of same type
//note: CopyFrom/To should NOT be virtual -- specials should be handled in the impl routines, or the Copy_ routines
  bool                  CopyFrom(taBase* cpy_from);
  // #TYPE_ON_this #PROJ_SCOPE #CAT_ObjectMgmt Copy from given object into this object (this is a safe interface to UnSafeCopy)
  bool                  CopyTo(taBase* cpy_to);
  // #TYPE_ON_this #PROJ_SCOPE #CAT_ObjectMgmt Copy to given object from this object
  // need both directions to more easily handle scoping of types on menus
  virtual bool          CanAppend(const taBase* apendee) const;
  // #IGNORE can you append an object to this object
  virtual bool          Append(taBase* appendee);
  // #IGNORE

  virtual bool          ChildCanDuplicate(const taBase* chld, bool quiet = true) const;
    // #IGNORE
  virtual taBase*       ChildDuplicate(const taBase* chld);
    // #IGNORE duplicate given child, returning the new one (NULL if can't do it)

  bool                  DuplicateMe();
  // #CONFIRM #CAT_ObjectMgmt Make another copy of myself (done through owner)
  virtual bool          ChangeMyType(TypeDef* new_type);
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #TYPE_this #CAT_ObjectMgmt #ARG_VAL_FM_FUN Change me into a different type of object, copying current info (done through owner)
  virtual void          UnSafeCopy(const taBase*) {} // #IGNORE custom version made for each class: if cp->Inherits(me) Copy(cp); else if me->Inherits(cp) cp.CastCopyTo(me) else CopyOther_impl(cp)
  virtual void          CastCopyTo(taBase*) const {}; // #IGNORE custom version made for every class, casts
  void                  CopyToCustom(taBase* src) const; // #IGNORE DO NOT CALL -- this is only a public, static wrapper for the _impl
  void                  CopyFromCustom(const taBase* cp); // #IGNORE DO NOT CALL -- this is only a public, static wrapper for the _impl

protected: // impl
  void                  Copy_impl(const taBase& cp);
  virtual bool          CanDoCopy_impl(const taBase* cp, bool quiet, bool copy);
    // intertwines the checks and copy, so it can be used for checking, or copying
  virtual void          CanCopy_impl(const taBase* cpy_from, bool quiet,
    bool& ok, bool virt) const;
    // basic query interface impl, only passed frm >= our class; may get called repeatedly, so subs are allowed to add an empty stub
  virtual void          CopyFromCustom_impl(const taBase* cp) {} // this is the generic copy, that enables common subclass or disparate class copying; follow pattern of Copy_impl, except we are always called in a Struct bracket
  virtual void          CopyToCustom_impl(taBase* targ) const {} // this is a fairly rarely used one for the case where the src actually does the copy; follow pattern of Copy_impl, except we are always called in a Struct bracket

  virtual void          CanCopyCustom_impl(bool to, const taBase* cp,
    bool quiet, bool& allowed, bool& forbidden) const {}
    // we need an allowed/forbidden paradigm here, so we can always call inherited -- only issue msg on forbidden; caller will supply msg if not allowed -- this routine is called for self (to=0), and we also call the proposed buddy (to=1) -- either one can forbid; us forbidding trumps cp allowing; since cp-controlled is so unusual, it is given priority

  ///////////////////////////////////////////////////////////////////////////
  //    Type information
public:

  bool          InheritsFrom(const TypeDef& it) const
  { return GetTypeDef()->InheritsFrom(it); }
  bool          InheritsFrom(TypeDef* it) const
  { return GetTypeDef()->InheritsFrom(it); }
  // #CAT_ObjectMgmt does this inherit from given type
  bool          InheritsFromName(const char* nm) const
  { return GetTypeDef()->InheritsFromName(nm); }
  // #CAT_ObjectMgmt does this inherit from given type name?

  TypeDef*      GetStemBase() const;
  // #IGNORE get first (from me) parent with STEM_BASE directive -- defines equivalence class -- if not found, then taBase is returned

  virtual MemberDef*    FindMember(const String& nm) const // #IGNORE
  { return GetTypeDef()->members.FindName(nm); }
  virtual MemberDef*    FindMember(TypeDef* it) const   // #IGNORE
  { return GetTypeDef()->members.FindType(it); }
  virtual MemberDef*    FindMember(void* mbr) const     // #IGNORE
  { int idx; return GetTypeDef()->members.FindAddr((void*)this, mbr, idx); }
  virtual MemberDef*    FindMemberPtr(void* mbr) const  // #IGNORE
  { int idx; return GetTypeDef()->members.FindAddrPtr((void*)this, mbr, idx); } // #IGNORE

  virtual void*         FindMembeR(const String& nm, MemberDef*& ret_md) const;
  // #CAT_ObjectMgmt find member based on name or type, recursive -- does breadth-first then depth search -- returns pointer of member item, and ret_md is filled in if avail -- if NULL it indicates that it is an item in a list of type taBase and not a proper member

  virtual bool          FindCheck(const String& nm) const // #IGNORE check this for the name
  { return (GetName() == nm); }

  virtual void          Search(const String& srch, taBase_PtrList& items,
                               taBase_PtrList* owners = NULL,
                               bool contains = true, bool case_sensitive = false,
                               bool obj_name = true, bool obj_type = true,
                               bool obj_desc = true, bool obj_val = true,
                               bool mbr_name = true, bool type_desc = false);
  // #CAT_ObjectMgmt search for objects using srch string, from this point down the structural hierarchy (my members, and their members and objects in lists, etc).  items are linked into items list, and all owners of items found are linked into owners list (if present -- can be used as a lookup table for expanding owners to browse found items).  contains = use "contains" for all matches instead of exact match, rest are values to search in (obj_desc includes DisplayName as well as any explicit description), obj_val is only for value members and inline members

  virtual void          Search_impl(const String& srch, taBase_PtrList& items,
                                    taBase_PtrList* owners = NULL,
                                    bool contains = true, bool case_sensitive = false,
                                    bool obj_name = true, bool obj_type = true,
                                    bool obj_desc = true, bool obj_val = true,
                                    bool mbr_name = true, bool type_desc = false);
  // #IGNORE implementation -- only first Search() is externally called

  virtual bool          SearchTestStr_impl(const String& srch,  String tst,
                                           bool contains, bool case_sensitive);
  // #IGNORE Search test string according to searching criteria

  virtual bool          SearchTestItem_impl(taBase* obj, const String& srch,
                                            bool contains, bool case_sensitive,
                                            bool obj_name, bool obj_type,
                                            bool obj_desc, bool obj_val,
                                            bool mbr_name, bool type_desc);
  // #IGNORE Search test for just this one taBase item according to criteria

  virtual void          CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
                           voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                           taBase* cp_base,
                           int show_forbidden=TypeItem::NO_HIDDEN,
                           int show_allowed=TypeItem::SHOW_CHECK_MASK, 
                           bool no_ptrs = true);
  // #IGNORE compare all member values from object of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists -- recursive -- will also check members of lists/groups that I own

  virtual String        GetEnumString(const String& enum_tp_nm, int enum_val) const
  { return GetTypeDef()->GetEnumString(enum_tp_nm, enum_val); }
  // #CAT_ObjectMgmt get the name corresponding to given enum value in enum type enum_tp_nm
  virtual int           GetEnumVal(const String& enum_nm, String& enum_tp_nm) const
  { return GetTypeDef()->GetEnumVal(enum_nm, enum_tp_nm); }
  // #CAT_ObjectMgmt get the enum value corresponding to the given enum name (-1 if not found), and sets enum_tp_nm to name of type this enum belongs in (empty if not found)
  virtual uint          GetSize() const         { return GetTypeDef()->size; }  // #IGNORE

  virtual String        GetTypeName() const                     // #IGNORE
  { return GetTypeDef()->name; }
  virtual String&       PrintType(String& strm) const         // #IGNORE
  { return GetTypeDef()->PrintType(strm); }
  virtual String&       PrintInherit(String& strm) const      // #IGNORE
  { return GetTypeDef()->PrintInherit(strm); }
  virtual String&       PrintTokens(String& strm) const       // #IGNORE
  { return GetTypeDef()->PrintTokens(strm); }

  static const String   ValTypeToStr(ValType vt);
  // #IGNORE get the value type as a standard type string
  static ValType        ValTypeForType(TypeDef* td);
  // #IGNORE return the appropriate ValType for given typedef

  ///////////////////////////////////////////////////////////////////////////
  //    Printing out object state values
public:

  virtual String&       Print(String& strm, int indent = 0) const
  { return GetTypeDef()->Print(strm, (void*)this, indent); }
  // #CAT_Display print the value of the object to given string
  String                PrintStr(int indent=0) const
  { String rval; return Print(rval, indent); }
  // #CAT_Display print the value of the object to a string

  /////////////////////////////////////////////////////////////////////
  //            Select For Edit
public:
  virtual bool          DiffCompare(taBase* cmp_obj);
  // #MENU #MENU_ON_Object #CAT_ObjectMgmt #TYPE_ON_0_this #NO_SCOPE #NO_BUSY compare this object with selected comparison object using a structured hierarchical diff operation -- pulls up a diff editor display to allow the user to view and merge the differences between objects
  virtual bool          DoDiffEdits(taObjDiff_List& diffs);
  // #CAT_ObjectMgmt actually perform edit actions (copy, add, delete) selected on given list of diffs

  virtual String        DiffCompareString(taBase* cmp_obj, taDoc*& doc);
  // #NULL_OK_1  #NULL_TEXT_1_NewDoc  #CAT_Display #TYPE_ON_0_this #NO_SCOPE compare this object with selected comparison object using a diff operation on their save file representations -- more robust to large differences than the select-for-edit version (if doc is NULL, a new one is created in .docs).  returns diff string as well.

  virtual bool          AddToControlPanel(MemberDef* member, ControlPanel* ctrl_panel,
      const String& extra_label = "", const String& sub_gp_nm = "");
  // #MENU #MENU_ON_ControlPanel #MENU_SEP_BEFORE #CAT_Display #NULL_OK_1 #NULL_TEXT_1_NewCtrlPanel select an object member to be added to a project control_panel - a panel that is a user chosen collection of members and methods from one or more objects (if ctrl_panel is NULL, a new one is created in .ctrl_panels).  returns false if member was already selected.  extra_label is prepended to item name, and if sub_gp_nm is specified, item will be put in this sub-group (new one will be made if it does not yet exist)
  virtual bool          AddToControlPanelNm(const String& memb_nm, ControlPanel* ctrl_panel,
                const String& extra_label = _nilString, const String& sub_gp_nm = _nilString,
                const String& desc = _nilString);
  // #CAT_Display select a  member (by name) for adding to a project control_panel - a panel that is a user chosen collection of members and methods from one or more objects (if ctrl_panel is NULL, a new one is created in .ctrl_panels).  returns false if member was already selected.  extra_label is prepended to item name, and if sub_gp_nm is specified, item will be put in this sub-group (new one will be made if it does not yet exist).  desc is a custom description -- will show up as tooltip for user (default is info from member)
  virtual int           AddToControlPanelSearch(const String& memb_contains, ControlPanel*& ctrl_panel);
  // #MENU #NULL_OK_1 #NULL_TEXT_1_NewCtrlPanel #CAT_Display search among this object and any sub-objects for members containing given string, and add to given control_panel (if NULL, a new one is created in .ctrl_panels).  returns number found
  virtual int           AddToControlPanelCompare(taBase* cmp_obj, ControlPanel*& ctrl_panel, bool no_ptrs = true);
  // #MENU #NULL_OK_1  #NULL_TEXT_1_NewCtrlPanel  #CAT_Display #TYPE_ON_0_this #NO_SCOPE compare this object with selected comparison object, adding any differences to given control_panel (if NULL, a new one is created in .ctrl_panels).  returns number of differences.  no_ptrs = ignore differences in pointer fields
  virtual bool          AddFunToControlPanel(MethodDef* function, ControlPanel* ctrl_panel,
         const String& extra_label = "", const String& sub_gp_nm = "");
  // #MENU #NULL_OK_1  #NULL_TEXT_1_NewCtrlPanel  #CAT_Display select a function (method) for calling from a project control_panel - a panel that is a user chosen collection of members and methods from one or more objects (if ctrl_panel is NULL, a new one is created in .ctrl_panels). returns false if method was already selected.  extra_label is prepended to item name, and if sub_gp_nm is specified, item will be put in this sub-group (new one will be made if it does not yet exist)
  virtual bool          AddFunToControlPanelNm(const String& function_nm, ControlPanel* ctrl_panel,
           const String& extra_label = _nilString, const String& sub_gp_nm = _nilString,
           const String& desc = _nilString);
  // #CAT_Display select a method (by name) for use in a control_panel that is a collection of selected members and methods from different objects (if ctrl_panel is NULL, a new one is created in .crtl_panels)  returns false if method was already selected.   extra_label is prepended to item name, and if sub_gp_nm is specified, item will be put in this sub-group (new one will be made if it does not yet exist)  desc is a custom description -- will show up as tooltip for user (default is info from member)
  virtual void          GetSelectText(MemberDef* mbr, String extra_label,
    String& lbl, String& desc) const; // #IGNORE supply extra_label (optional); provides the canonical lbl and (if empty) desc -- NOTE: routine is in ta_seledit.cpp

  ///////////////////////////////////////////////////////////////////////////
  //    User Data: optional configuration settings for objects
public:
  virtual UserDataItem_List* GetUserDataList(bool force_create = false) const
    {return NULL;}
  // #CAT_UserData #EXPERT gets the userdatalist for this class
  bool                  HasUserDataList() const
    {return (GetUserDataList(false) != NULL);}
  // #CAT_UserData #EXPERT returns true if UserData exists at all
  bool                  HasUserData(const String& key) const;
  // #CAT_UserData returns true if UserData exists for this key (case sens)
  const Variant         GetUserData(const String& key) const;
  // #CAT_UserData get specified user data; returns class default value if not present, or nilVariant if no default user data or class doesn't support UserData
  const Variant         GetUserDataDef(const String& key, const Variant& def)
    {if (HasUserData(key)) return GetUserData(key); else return def;}
  // #CAT_UserData #EXPERT return value if exists, or default if doesn't
  UserDataItemBase*     GetUserDataOfType(TypeDef* typ, const String& key,
                                          bool force_create);
  // #CAT_UserData #EXPERT #ARGC_2 gets specified user data of given type, making one if doesn't exist and fc=true
  UserDataItemBase*     GetUserDataOfTypeC(TypeDef* typ, const String& key) const;
  // #IGNORE const non-forced version, for convenience
  UserDataItemBase*     GetUserDataItem(const String& key) const;
  // #IGNORE const non-forced version, for convenience
  inline bool           GetUserDataAsBool(const String& key) const
    {return GetUserData(key).toBool();} // #CAT_UserData #EXPERT get specified user data as bool (see GetUserData)
  inline int            GetUserDataAsInt(const String& key) const
    {return GetUserData(key).toInt();} // #CAT_UserData #EXPERT get specified user data as int (see GetUserData)
  inline float          GetUserDataAsFloat(const String& key) const
    {return GetUserData(key).toFloat();} // #CAT_UserData #EXPERT get specified user data as float (see GetUserData)
  inline double         GetUserDataAsDouble(const String& key) const
    {return GetUserData(key).toDouble();} // #CAT_UserData #EXPERT get specified user data as double (see GetUserData)
  inline const String   GetUserDataAsString(const String& key) const
    {return GetUserData(key).toString();} // #CAT_UserData #EXPERT get specified user data as String (see GetUserData)
  UserDataItem*         SetUserData(const String& key, const Variant& value);
  // #CAT_UserData make new (or change existing) simple user data entry with given name and value; returns item, which can be ignored
  void                  SetUserData_Gui(const String& key, const Variant& value,
    const String& desc);
  // #CAT_UserData #MENU #MENU_ON_Object #MENU_CONTEXT #LABEL_Set_User_Data make new (or change existing) simple user data entry with given name and value (desc optional) -- this is how to get User Data editor panel to show up the first time
  bool                  RemoveUserData(const String& key);
  // #CAT_UserData removes data for indicated key; returns true if it existed
  taDoc*                GetDocLink() const;
  // #CAT_UserData #EXPERT gets a linked Doc, if any; you can use this to test for existence
  void                  SetDocLink(taDoc* doc);
  // #CAT_UserData #MENU #MENU_CONTEXT #DROP1 #NULL_OK set a link to a doc from the .docs collection -- the doc will then show up automatically in a panel for this obj -- set to NULL to remove it
  void                  PrintMyPath();
  // #CAT_ObjectMgmt #MENU #MENU_CONTEXT print out the path to this object to the css console -- can then be copied for use in a program or other such things

  bool          HasOption(const char* op) const
  { return GetTypeDef()->HasOption(op); }
  // #IGNORE hard-coded options for this type
  bool          CheckList(const String_PArray& lst) const
  { return GetTypeDef()->CheckList(lst); }
  // #IGNORE

  ///////////////////////////////////////////////////////////////////////////
  //    Closing

  virtual void          CloseLater();
  // #NO_REVERT_AFTER #LABEL_Close_(Destroy) #CAT_ObjectMgmt PERMANENTLY Destroy this object!  This is not Iconify or close window..
  virtual void          Close();
  // #IGNORE an immediate version of Close for use in code (no waitproc delay)
  virtual bool          Close_Child(taBase* obj);
  // #IGNORE actually closes a child object (should be immediate child)
  virtual bool          CloseLater_Child(taBase* obj);
  // #IGNORE actually closes a child object (should be immediate child) but defers deletion to loop

  virtual void          Help();
  // #MENU #MENU_ON_Object #MENU_SEP_BEFORE #CAT_Display get help on using this object

  ///////////////////////////////////////////////////////////////////////////
  //    Misc container functionality

  virtual taBase*       CopyChildBefore(taBase* src, taBase* child_pos) { return NULL; }
  // #CAT_ListMgmt implemented by container (list) objects: make a copy of the src object and insert it at the position of the child_pos object within this list -- copies the name of the object too


  ///////////////////////////////////////////////////////////////////////////
  //    Updating pointers (when objects change type or are copied)

  virtual taBase* UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par,
                                                taBase* new_par);
  // #IGNORE find a new pointer to replace old_guy in new_par
  virtual bool  UpdatePointers_NewPar_Ptr(taBase** ptr, taBase* old_par, taBase* new_par,
                                          bool null_not_found = true);
  // #IGNORE update pointer if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated
  virtual bool  UpdatePointers_NewPar_PtrNoSet(taBase** ptr, taBase* old_par, taBase* new_par,
                                               bool null_not_found = true);
  // #IGNORE update pointer if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated (NO_SET_POINTER version)
  virtual bool  UpdatePointers_NewPar_SmPtr(taSmartPtr& ptr, taBase* old_par, taBase* new_par,
                                            bool null_not_found = true);
  // #IGNORE update pointer if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated
  virtual bool  UpdatePointers_NewPar_Ref(taSmartRef& ref, taBase* old_par,
                                          taBase* new_par, bool null_not_found = true);
  // #IGNORE update reference if it used to point to an object under old_par parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.  used for updating after a copy operation: returns true if updated

  virtual int   UpdatePointers_NewPar(taBase* old_par, taBase* new_par);
  // #IGNORE update pointers for a new parent (e.g., after a copy operation): and anything that lives under old_par and points to something else that lives under old_par is updated to point to new_par: this default impl uses TA info to walk the members and find the guys to change; returns number changed
  virtual int   UpdatePointers_NewPar_IfParNotCp(const taBase* cp, TypeDef* par_type);
  // #IGNORE for use during a copy operation: call UpdatePointers_NewPar with parent (GET_OWNER) of given par_type, only if that parent is not already COPYING (according to base flag)

  virtual bool  UpdatePointers_NewParType_Ptr(taBase** ptr, TypeDef* par_typ, taBase* new_par,
                                          bool null_not_found = false);
  // #IGNORE update pointer if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.
  virtual bool  UpdatePointers_NewParType_PtrNoSet(taBase** ptr, TypeDef* par_typ, taBase* new_par,
                                          bool null_not_found = false);
  // #IGNORE update pointer if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set. (NO_SET_POINTER version)
  virtual bool  UpdatePointers_NewParType_SmPtr(taSmartPtr& ptr, TypeDef* par_typ,
                                                taBase* new_par, bool null_not_found = false);
  // #IGNORE update pointer if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.
  virtual bool  UpdatePointers_NewParType_Ref(taSmartRef& ref, TypeDef* par_typ,
                                          taBase* new_par, bool null_not_found = false);
  // #IGNORE update reference if it used to point to an object under par_typ parent, have it point to the corresponding object under new_par (based on path) -- set to null if not found if option set.

  virtual int   UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par);
  // #IGNORE update pointers for a new parent of given type -- any pointer under this object that points to an object that lives under an object of par_typ (type of new_par): and anything that lives under old_par and points to something else that lives under old_par is updated to point to new_par: this default impl uses TA info to walk the members and find the guys to change; returns number changed

  static bool   UpdatePointers_NewObj_Ptr(taBase** ptr, taBase* ptr_owner,
                                          taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner
  static bool   UpdatePointers_NewObj_PtrNoSet(taBase** ptr, taBase* ptr_owner,
                                          taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner -- NO_SET_POINTER version
  static bool   UpdatePointers_NewObj_SmPtr(taSmartPtr& ptr, taBase* ptr_owner,
                                            taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner
  static bool   UpdatePointers_NewObj_Ref(taSmartRef& ref, taBase* ptr_owner,
                                          taBase* old_ptr, taBase* new_ptr);
  // #IGNORE update pointer to new_ptr if it used to point to old_ptr; call UAE on ptr_owner

  virtual int   UpdatePointersToMe(taBase* new_ptr);
  // #IGNORE replace all pointers that might have pointed to old version of this object (and all of its kids!) to new_ptr -- called during ChangeType: default is to call UpdatePointers_NewObj on taMisc::default_scope; returns number changed
  virtual int   UpdatePointersToMe_impl(taBase* scope_obj, taBase* new_ptr);
  // #IGNORE actually does the work: above just passes in the scoper
  virtual int   UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr);
  // #IGNORE override this for list/group guys that contain sub-objs -- update all the sub objs that we own!

  virtual int   UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr);
  // #IGNORE replace all pointers to old_ptr with new_ptr: walks the entire structure (members, lists, etc) and iteratively calls; returns number changed

  ///////////////////////////////////////////////////////////////////////////
  //    DMem -- distributed memory (MPI)

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

public:
  ///////////////////////////////////////////////////////////////////////////
  //            Misc Impl stuff

  mutable BaseFlags     base_flags; // #NO_SHOW #NO_SAVE #READ_ONLY #CAT_taBase base_flags for ta system
protected:
  taAtomicInt            refn;   // number of references to this object
private:
// Initialize and Destroy are always private because they should only be called in ctor/dtor
  void                  Initialize() {base_flags = BF_0;}
  // #IGNORE constructor implementation to initialize members of class.  every class should define this initializer.  cannot refer to anything outside of the object itself (use InitLinks for when it gets added into the object hierarchy)
  void                  Destroy();
  // #IGNORE destructor implementation -- free any allocated memory and reset pointers to null, etc. -- set to null and check for null!

};


/////////////////////////////////////////////
//	Operators

#ifndef __MAKETA__
inline std::istream& operator>>(std::istream &strm, taBase &obj)
{ obj.Load_strm(strm); return strm; }
inline std::ostream& operator<<(std::ostream &strm, taBase &obj)
{ obj.Save_strm(strm); return strm; }
#endif

#endif // taBase_h
