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

#ifndef taiMember_h
#define taiMember_h 1

// parent includes:
#include <taiType>

// member includes:
#include <taString>
#include <MemberDef>


// declare all other types mentioned but not required to include:
class taiTypeOfEnum; //

taTypeDef_Of(taiMember);

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

  taiMember*            LowerBidder() { return static_cast<taiMember*>(next_lower_bidder); }
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
  override taiWidget*     GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  // get taiWidget rep of type -- delegates to mbr's it
  DefaultStatus         GetDefaultStatus(String memb_val);
    // get status of value, if is default value or not
  override void         GetImage(taiWidget* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetWidgetRep
  virtual void          GetMbrValue(taiWidget* dat, void* base, bool& first_diff);
  // this is the one to call to get a member value (GetValue is not used)
  // TODO: should this class override void GetValue(taiWidget* dat, void * base); as a noop?

  virtual TypeDef*      GetTargetType(const void* base);
  // for XxxDef* and token ptrs, returns the target type, which can be specified in several ways, or this can be overridden

  // script-generation code
  virtual void  GetOrigVal(taiWidget* dat, const void* base);
        // gets original value and adds it to the orig_vals list
  virtual void  StartScript(const void* base);
  virtual void  CmpOrigVal(taiWidget* dat, const void* base, bool& first_diff);
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
  virtual taiWidget*      GetArbitrateDataRep(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr); // gets sub or this, and factors ro
  virtual void          GetArbitrateImage(taiWidget* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetWidgetRep
  virtual void          GetArbitrateMbrValue(taiWidget* dat, void* base, bool& first_diff);

  override taiWidget*     GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void         GetImage_impl(taiWidget* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetWidgetRep
  virtual void          GetMbrValue_impl(taiWidget* dat, void* base);
  override bool         isReadOnly(taiWidget* dat, IWidgetHost* host_ = NULL); // used dlg, par, and member directives to determine if RO
  void                  CheckProcessCondEnum(taiTypeOfEnum* et, taiWidget* dat, const void* base);
};


#define TAI_MEMBER_SUBCLASS(x, y) \
  INHERITED(y) \
  public: \
  x(MemberDef* md, TypeDef* td) : y(md, td) { \
    Initialize(); \
  } \
  x() { Initialize(); } \
  ~x() { Destroy(); } \
  override TypeDef* GetTypeDef() const { \
    return &TA_##x; \
  } \
  override x* MembInst(MemberDef* md, TypeDef* td) const { \
    return new x(md, td); \
  }

// macro for doing safe casts of types -- helps to uncover errors
// when the actual class is not the expected class
// (of course this NEVER happens... uh, huh... ;) )
#define QCAST_MBR_SAFE_EXIT(qtyp, rval, dat) \
  qtyp rval = qobject_cast<qtyp>(dat); \
  if (!rval) { \
    taMisc::Error("QCAST_MBR_SAFE_EXIT: expect " #qtyp "for mbr", \
    mbr->name, "but was:", \
      dat->metaObject()->className()); \
    return; \
  }

#endif // taiMember_h

