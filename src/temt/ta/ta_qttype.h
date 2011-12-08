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
  void  El_Done_(void* it)      { delete (taiType*)it; }

public:
  ~taiType_List()              { Reset(); }
};


//////////////////////////////
//       taiTypes         //
//////////////////////////////

class TA_API taiIntType : public taiType { // handles numeric int types, up to 32-bits
  TAI_TYPEBASE_SUBCLASS(taiIntType, taiType);
public:
  override bool handlesReadOnly() const { return true; } // uses a RO iSpinBox

  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API taiInt64Type : public taiType { // handles 64-bit numeric int types
  TAI_TYPEBASE_SUBCLASS(taiInt64Type, taiType);
public:
  int           BidForType(TypeDef* td);
};

class TA_API taiRealType : public taiType { // handles real types, provides validators
  TAI_TYPEBASE_SUBCLASS(taiRealType, taiType);
public:
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API taiEnumType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiEnumType, taiType);
public:
  override bool handlesReadOnly() const { return true; } // uses a RO iLineEdit w/ enum name

  inline bool   isCond() const {return m_is_cond;} // true if a BIT, and has any CONDxxxx bits
  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
protected:
  void          Initialize();

  bool          isBit; // true if a BIT type enum
  bool          m_is_cond; // true if a BIT, and has any CONDxxxx bits
};

class TA_API taiBoolType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiBoolType, taiType);
public:
  override bool handlesReadOnly() const { return true; } // uses a RO iCheckBox

  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API taiStringType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiStringType, taiType);
public:
  override bool handlesReadOnly() const { return true; }

  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API taiVariantType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiVariantType, taiType);
public:
  override bool handlesReadOnly() const { return true; }
  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API taiClassType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiClassType, taiType);
public:
  override bool allowsInline() const    { return true; }
  override bool handlesReadOnly() const { return true; } // uses a RO PolyData or RO EditButton
  override bool CanBrowse() const;

  int           BidForType(TypeDef* td);
  override taiData*     GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
         // add in req for inline
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  taiData*      GetDataRepInline_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
};

class TA_API taiMatrixGeomType : public taiClassType { // special editor for matrix geoms
  TAI_TYPEBASE_SUBCLASS(taiMatrixGeomType, taiClassType);
public:
  override bool requiresInline() const { return true; }
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRepInline_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

class TA_API taiColorType : public taiClassType { // special editor for colors, add dialog
  TAI_TYPEBASE_SUBCLASS(taiColorType, taiClassType);
public:
  override bool handlesReadOnly() const { return true; }
  override bool requiresInline() const  { return true; }
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRepInline_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

class TA_API taitaColorType : public taiColorType { // special editor for colors, add dialog
  TAI_TYPEBASE_SUBCLASS(taitaColorType, taiColorType);
public:
  int           BidForType(TypeDef* td);
protected:
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};

class TA_API gpiListType : public taiClassType {
  TAI_TYPEBASE_SUBCLASS(gpiListType, taiClassType);
public:
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

class TA_API gpiGroupType : public gpiListType {
  TAI_TYPEBASE_SUBCLASS(gpiGroupType, gpiListType);
public:
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

class TA_API gpiArray_Type : public taiClassType {
  TAI_TYPEBASE_SUBCLASS(gpiArray_Type, taiClassType);
public:
  override bool allowsInline() const { return false; }
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
};

class TA_API taiTokenPtrType : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiTokenPtrType, taiType);
public:
  enum Mode {
    MD_BASE,            // taBase pointer
    MD_SMART_PTR,       // taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF        // taSmartRef
  };
  override bool handlesReadOnly() const { return true; } // uses a RO tokenptr button
  taBase*       GetTokenPtr(const void* base) const; // depends on mode
  TypeDef*      GetMinType(const void* base);
  int           BidForType(TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

  Mode          mode; // set during first GetDataRep (is garbage until then)
};

class TA_API taiTypePtr : public taiType {
  // typedef pointer
  TAI_TYPEBASE_SUBCLASS(taiTypePtr, taiType);
public:
  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
  void          Initialize();

  TypeDef*      orig_typ;
};

class TA_API taiFilePtrType : public taiType {// ptr to tai_getFiles
  TAI_TYPEBASE_SUBCLASS(taiFilePtrType, taiType);
public:
  int           BidForType(TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
};


//////////////////////////////
//       taiEdits         //
//////////////////////////////

class TA_API taiDefaultEdit : public taiEdit {
  TAI_TYPEBASE_SUBCLASS(taiDefaultEdit, taiEdit);
public:
  int   BidForEdit(TypeDef*) { return 0; }
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


//////////////////////////
//      taiMember       //
//////////////////////////

class TA_API taiMember : public taiType {
public:
  static  void  EndScript(const void* base);
        // calling function has to use this function to end script if neccesary

  enum DefaultStatus { //#BITS  status of default value comparison
    HAS_DEF     = 0x01, // member specified a default value
     IS_DEF     = 0x02, // member's value is the default

     NO_DEF     = 0x00, // #NO_BIT none defined
    NOT_DEF     = 0x01, // #NO_BIT default specified, current is not equal
    EQU_DEF     = 0x03, // #NO_BIT default specified, current is default
  };

  MemberDef*    mbr;

  taiMember*            sub_types() {return (taiMember*)m_sub_types;}
  taiMember**           addr_sub_types() {return (taiMember**)&m_sub_types;}
  override bool         handlesReadOnly() const;
  bool                  isCondEdit() const;
  bool                  isCondShow() const;

  int                   BidForType(TypeDef*) { return 0; }
  // none of the member specific ones should apply types
  virtual int           BidForMember(MemberDef*, TypeDef*) { return 1; }
  // bid for (appropriateness) for given type of member (and members owner type)

//
//   // default member action is to pass thru to the type
//
  override taiData*     GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  // get taiData rep of type -- delegates to mbr's it
  DefaultStatus         GetDefaultStatus(String memb_val);
    // get status of value, if is default value or not
  override void         GetImage(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void          GetMbrValue(taiData* dat, void* base, bool& first_diff);
  // this is the one to call to get a member value (GetValue is not used)
  // TODO: should this class override void GetValue(taiData* dat, void * base); as a noop?

  virtual TypeDef*      GetTargetType(const void* base);
  // for XxxDef* and token ptrs, returns the target type, which can be specified in several ways, or this can be overridden

  // script-generation code
  virtual void  GetOrigVal(taiData* dat, const void* base);
        // gets original value and adds it to the orig_vals list
  virtual void  StartScript(const void* base);
  virtual void  CmpOrigVal(taiData* dat, const void* base, bool& first_diff);
        // compares to original value and generates script code to change it

  taiMember(MemberDef* mb, TypeDef* td) : taiType(td)   { mbr = mb; }
  taiMember()                                           { mbr = NULL; }
  ~taiMember()                                          { };

  virtual void          AddMember(MemberDef* md);       // add an iv to a member
  virtual taiMember*    MembInst(MemberDef* md, TypeDef* td) const {
    return new taiMember(md, td);
  }
  TypeDef*      GetTypeDef() const {return &TA_taiMember;}

protected:
  // the "Arbitrate routines all apply the same logic of ro, and subtype, to call
  // either that guy, or our own -- only SpecPtr overrides these
  virtual taiData*      GetArbitrateDataRep(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr); // gets sub or this, and factors ro
  virtual void          GetArbitrateImage(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void          GetArbitrateMbrValue(taiData* dat, void* base, bool& first_diff);

  override taiData*     GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void         GetImage_impl(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void          GetMbrValue_impl(taiData* dat, void* base);
  override bool         isReadOnly(taiData* dat, IDataHost* host_ = NULL); // used dlg, par, and member directives to determine if RO
  void                  CheckProcessCondEnum(taiEnumType* et, taiData* dat, const void* base);
};

#define TAI_MEMBER_SUBCLASS(x, y)        \
    INHERITED(y)                         \
  public:                                \
    x(MemberDef* md, TypeDef* td)        \
      : y(md, td)                        \
    {                                    \
      Initialize();                      \
    }                                    \
    x()                                  \
    {                                    \
      Initialize();                      \
    }                                    \
    ~x()                                 \
    {                                    \
      Destroy();                         \
    }                                    \
    override TypeDef* GetTypeDef() const \
    {                                    \
      return &TA_##x;                    \
    }                                    \
    override x* MembInst(MemberDef* md, TypeDef* td) const \
    {                                    \
      return new x(md, td);              \
    }

//////////////////////////////
//       taiMembers       //
//////////////////////////////

// these have BidforMember() functions which may depend on the opts
// of a member

class TA_API taiTokenPtrMember : public taiMember {
  // for taBase pointer members (allows scoping by owner obj)
  TAI_MEMBER_SUBCLASS(taiTokenPtrMember, taiMember);
public:
  enum Mode {
    MD_BASE,            // taBase pointer
    MD_SMART_PTR,       // taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF        // taSmartRef
  };

  override bool handlesReadOnly() const { return true; }
  TypeDef*      GetMinType(const void* base);
  taBase*       GetTokenPtr(const void* base) const; // depends on mode
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
  Mode          mode; // set during first GetDataRep (is garbage until then)
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiDefaultToken : public taiTokenPtrMember {
  // Special edit menu for the TDefault's token member
  TAI_MEMBER_SUBCLASS(taiDefaultToken, taiTokenPtrMember);
public:
  TypeDefault*  tpdflt;

  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize()    { tpdflt = NULL; }
  void          Destroy() {}
};

class TA_API taiSubTokenPtrMember : public taiMember {
  // a token ptr that points to sub-objects of current object
  TAI_MEMBER_SUBCLASS(taiSubTokenPtrMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiTypePtrMember : public taiMember {
  // typedef ptrs that have member-comment directives
  TAI_MEMBER_SUBCLASS(taiTypePtrMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiEnumTypePtrMember : public taiTypePtrMember {
  // typedef ptrs that have member-comment directives, Enums
  TAI_MEMBER_SUBCLASS(taiEnumTypePtrMember, taiTypePtrMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
   taiData*     GetDataRep_impl(IDataHost* host_, taiData* par,
     QWidget* gui_parent_, int flags_, MemberDef* mbr);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiMemberDefPtrMember : public taiMember {
  // pointer to a member-def
  TAI_MEMBER_SUBCLASS(taiMemberDefPtrMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
   taiData*     GetDataRep_impl(IDataHost* host_, taiData* par,
     QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiMethodDefPtrMember : public taiMember {
  // pointer to a MethodDef
  TAI_MEMBER_SUBCLASS(taiMethodDefPtrMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
   taiData*     GetDataRep_impl(IDataHost* host_, taiData* par,
     QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiFunPtrMember : public taiMember {
  // pointer to a function
  TAI_MEMBER_SUBCLASS(taiFunPtrMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiFileDialogMember : public taiMember {
  // string with file dialog directive
  TAI_MEMBER_SUBCLASS(taiFileDialogMember, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TypeDefault;

class TA_API taiTDefaultMember : public taiMember {
  // special for the TypeDefault member (add the "active" box) -- this doesn't use the default handling, and just provides its own directly
  TAI_MEMBER_SUBCLASS(taiTDefaultMember, taiMember);
public:
  TypeDefault*  tpdflt;

  virtual int           BidForMember(MemberDef* md, TypeDef* td);//
  override taiData*     GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  override void         GetImage(taiData* dat, const void* base);
  override void         GetMbrValue(taiData* dat, void* base, bool& first_diff);

private:
  void          Initialize()    { tpdflt = NULL; }
  void          Destroy()       { m_sub_types = NULL; } // prevent from being destroyed
};

//////////////////////////
//      taiMethod       //
//////////////////////////

// the default method handles requests for Menu or Button instances
// the client needs to check the MethodDef directives to decide if
// the type is supposed to have buttons and/or menus in a given context

class TA_API taiMethod : public taiType {
INHERITED(taiType)
public:
  MethodDef*    meth;

  taiMethod*            sub_types() {return (taiMethod*)m_sub_types;}
  taiMethod**           addr_sub_types() {return (taiMethod**)&m_sub_types;}

  int           BidForType(TypeDef*)                    { return 0; }
  // none of the method specific ones should apply to types
  virtual int   BidForMethod(MethodDef*, TypeDef*)      { return 0; }
  // bid for (appropriateness) for given type of method (default is not at all approp.)


  taiMethodData*        GetGenericMethodRep(void* base, taiData* par); // this is just for CallFun
  taiMethodData*        GetButtonMethodRep(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_);
  taiMethodData*        GetMenuMethodRep(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_); // covers MENU and MENU_BUTTON types
  USING(inherited::GetImage)
  override void         GetImage(taiData*, const void*) { }
  override void         GetValue(taiData*, void*)       { }

  taiMethod(MethodDef* mb, TypeDef* td) : taiType(td)   { meth = mb; }
  taiMethod()                                           { meth = NULL; }
  ~taiMethod()                                          { };

  virtual void          AddMethod(MethodDef* md);       // add an iv to a member

  virtual taiMethod*    MethInst(MethodDef* md, TypeDef* td) const
        { return new taiMethod(md,td);}
  TypeDef*      GetTypeDef() const {return &TA_taiMethod;}
protected:
  virtual taiMethodData* GetButtonMethodRep_impl(void* base, IDataHost* host_,
    taiData* par, QWidget* gui_parent_, int flags_) {return NULL;}
  virtual taiMethodData* GetMenuMethodRep_impl(void* base, IDataHost* host_,
    taiData* par, QWidget* gui_parent_, int flags_) {return NULL;}
};

#define TAI_METHOD_SUBCLASS(x, y)        \
    INHERITED(y)                         \
  public:                                \
    x(MethodDef* md, TypeDef* td)        \
      : y(md, td)                        \
    {                                    \
      Initialize();                      \
    }                                    \
    x()                                  \
    {                                    \
      Initialize();                      \
    }                                    \
    ~x()                                 \
    {                                    \
      Destroy();                         \
    }                                    \
    override TypeDef* GetTypeDef() const \
    {                                    \
      return &TA_##x;                    \
    }                                    \
    override x* MethInst(MethodDef* md, TypeDef* td) const \
    {                                    \
      return new x(md, td);              \
    }

//////////////////////////////
//  taiActuatorMethod       //
//////////////////////////////

class TA_API taiActuatorMethod : public taiMethod {
  TAI_METHOD_SUBCLASS(taiActuatorMethod, taiMethod);
public:
  int                   BidForMethod(MethodDef* md, TypeDef* td);

protected:
  taiMethodData*        GetButtonMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  taiMethodData*        GetMenuMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


//////////////////////////
//      taiArgType      //
//////////////////////////

class TA_API taiArgType : public taiType {
  // unlike taiTypes, these are created and destroyed each time
  // thus, they cache their values
public:
  MethodDef*    meth;           // method that has the args
  int           arg_idx;        // index of argument in list of args
  TypeDef*      arg_typ;        // which arg typedef this one is
  bool          err_flag;       // true if one of the args was improperly set

  taiType*      use_it;         // alternate iv type to use
  void*         arg_base;       // base value is computed for typedef
  cssEl*        arg_val;        // argument value (as a css element)
  taBase*       obj_inst;       // instance of taBase object for ptr=0 args

  int           BidForType(TypeDef*)                    { return 0; }
  // none of the argtype specific ones should apply to types
  virtual int   BidForArgType(int, TypeDef*, MethodDef*, TypeDef*)      { return 1; }
  // bid for (appropriateness) for given type of method and argument type

  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);

  virtual cssEl* GetElFromArg(const char* arg_nm, void* base);
  // this is responsible for setting arg_base and arg_val (base is parent base)

  // base passed here is of the parent object(!)
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

  virtual bool  GetHasOption(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // check the meth->HasOption, also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx
  virtual String GetOptionAfter(const String& opt, MethodDef* md = NULL, int aidx = -1);
  // get the meth->OptionAfter(opt), also taking into account a possible restriction to a particular argument index, specified as opt_n_xxx where n = arg idx

  taiArgType(int aidx, TypeDef* argt, MethodDef* mb, TypeDef* td);
  taiArgType();
  ~taiArgType();

  virtual taiArgType*  ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) const
  { return new taiArgType(aidx, argt,md,td);}
  TypeDef*      GetTypeDef() const {return &TA_taiArgType;}
private:
  void          Initialize() {}
  void          Destroy() {}
};

#define TAI_ARGTYPE_SUBCLASS(x, y)       \
    INHERITED(y)                         \
  public:                                \
    x(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) \
      : y(aidx, argt, md, td)            \
    {                                    \
      Initialize();                      \
    }                                    \
    x()                                  \
    {                                    \
      Initialize();                      \
    }                                    \
    ~x()                                 \
    {                                    \
      Destroy();                         \
    }                                    \
    override TypeDef* GetTypeDef() const \
    {                                    \
      return &TA_##x;                    \
    }                                    \
    override x* ArgTypeInst(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) const \
    {                                    \
      return new x(aidx, argt, md, td);  \
    }

class TA_API taiStreamArgType : public taiArgType {
  // for ios derived args (uses a file-requestor)
  TAI_ARGTYPE_SUBCLASS(taiStreamArgType, taiArgType);
public:
  taFiler*      gf;

  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
  virtual void  GetValueFromGF(); // actually get the value from the getfile

private:
  void Initialize();
  void Destroy();
};

class TA_API taiTokenPtrArgType : public taiArgType {
  // for pointers to tokens (uses appropriate scoping)
  TAI_ARGTYPE_SUBCLASS(taiTokenPtrArgType, taiArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiTypePtrArgType : public taiArgType {
  // for typedef ptr types
  TAI_ARGTYPE_SUBCLASS(taiTypePtrArgType, taiArgType);
public:
  TypeDef*      base_type;      // base type for type selector

  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void          Initialize() { base_type = NULL; }
  void          Destroy() {}
};

class TA_API taiMemberPtrArgType : public taiArgType {
  // for memberdef ptr types
  TAI_ARGTYPE_SUBCLASS(taiMemberPtrArgType, taiArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiMethodPtrArgType : public taiArgType {
  // for methoddef ptr types
  TAI_ARGTYPE_SUBCLASS(taiMethodPtrArgType, taiArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API taiFileDialogArgType : public taiArgType {
  // for string args with FILE_DIALOG_xxx
  TAI_ARGTYPE_SUBCLASS(taiFileDialogArgType, taiArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void Initialize();
  void Destroy();
};

class TA_API gpiDefaultEl : public taiMember {
  TAI_MEMBER_SUBCLASS(gpiDefaultEl, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue(taiData* dat, void* base, bool& first_diff);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API gpiLinkGP : public taiMember {
  TAI_MEMBER_SUBCLASS(gpiLinkGP, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue(taiData* dat, void* base, bool& first_diff);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API gpiLinkList : public taiMember {
  TAI_MEMBER_SUBCLASS(gpiLinkList, taiMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue(taiData* dat, void* base, bool& first_diff);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API gpiFromGpTokenPtrMember : public taiTokenPtrMember {
  TAI_MEMBER_SUBCLASS(gpiFromGpTokenPtrMember, taiTokenPtrMember);
public:
  int           BidForMember(MemberDef* md, TypeDef* td);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue(taiData* dat, void* base, bool& first_diff);

  virtual MemberDef*    GetFromMd();
  virtual TABLPtr       GetList(MemberDef* from_md, const void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};



//////////////////////////////////////////////////
//       taiArgTypes:   Lists and Groups        //
//////////////////////////////////////////////////

class TA_API gpiTAPtrArgType : public taiTokenPtrArgType {
  // for taBase pointers in groups, sets the typedef to be the right one..
  TAI_ARGTYPE_SUBCLASS(gpiTAPtrArgType, taiTokenPtrArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API gpiInObjArgType : public gpiTAPtrArgType {
  // for taBase pointers in groups with ARG_IN_OBJ
  TAI_ARGTYPE_SUBCLASS(gpiInObjArgType, gpiTAPtrArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

class TA_API gpiFromGpArgType : public taiTokenPtrArgType {
  // for taBase pointers with FROM_GROUP_xxx
  TAI_ARGTYPE_SUBCLASS(gpiFromGpArgType, taiTokenPtrArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

  virtual MemberDef*    GetFromMd();
  virtual TABLPtr       GetList(MemberDef* from_md, const void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

//////////////////////////////////////////////////////////
//              taiType:        Array Edits             //
//////////////////////////////////////////////////////////

class TA_API gpiListEdit : public taiEdit {
  TAI_TYPEBASE_SUBCLASS(gpiListEdit, taiEdit);
public:
  int           BidForEdit(TypeDef* td);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class TA_API gpiArrayEdit : public taiEdit {
  TAI_TYPEBASE_SUBCLASS(gpiArrayEdit, taiEdit);
public:
  int           BidForEdit(TypeDef* td);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


class TA_API SArgEdit : public gpiArrayEdit {
  TAI_TYPEBASE_SUBCLASS(SArgEdit, gpiArrayEdit);
public:
  int           BidForEdit(TypeDef* td);
protected:
  override taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance
};


//////////////////////////
//   Viewer Types       //
//////////////////////////

class TA_API tabViewType: public taiViewType { // for taBase and descendants
  TAI_TYPEBASE_SUBCLASS(tabViewType, taiViewType)
public:
  override int          BidForView(TypeDef*);
  override const iColor GetEditColorInherit(taiDataLink* dl, bool& ok) const;// #IGNORE background color for edit
  override iDataPanel*  CreateDataPanel(taiDataLink* dl_);
  override taiDataLink* GetDataLink(void* data_, TypeDef* el_typ);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  virtual taiDataLink*  CreateDataLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiDataLink* dl_);
private:
  EditDataPanel*        edit_panel; // cached during construction
};

class TA_API tabOViewType: public tabViewType { // for taOBase and descendants
  TAI_TYPEBASE_SUBCLASS(tabOViewType, tabViewType)
public:
  override bool         needSet() const {return true;} // always, so we can do dyn panels
  override int          BidForView(TypeDef*);
  override taiDataLink* GetDataLink(void* data_, TypeDef* el_typ); // optimized version of tabViewType
  override void         CheckUpdateDataPanelSet(iDataPanelSet* pan);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  override taiDataLink* CreateDataLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API tabListViewType: public tabOViewType { // for taList
  TAI_TYPEBASE_SUBCLASS(tabListViewType, tabOViewType)
public:
  override int          BidForView(TypeDef*);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  override taiDataLink* CreateDataLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API tabGroupViewType: public tabListViewType { // for taGroup
  TAI_TYPEBASE_SUBCLASS(tabGroupViewType, tabListViewType)
public:
  override int          BidForView(TypeDef*);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  override taiDataLink* CreateDataLink_impl(taBase* data_);
};

#endif // tai_type_h

