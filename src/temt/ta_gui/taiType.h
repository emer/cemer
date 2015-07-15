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

#ifndef taiType_h
#define taiType_h 1

// parent includes:
#include <taiTypeBase>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidget;
class IWidgetHost;


taTypeDef_Of(taiType);

class TA_API taiType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
  TAI_TYPEBASE_SUBCLASS(taiType, taiTypeBase);
public:
  virtual bool          allowsInline() const    { return false; } // the type allows inline reps, obtained GetWidgetRepInline
  virtual bool          requiresInline() const  { return false; } // only inline reps allowed, no member-by-member reps
  virtual bool          handlesReadOnly() const { return false; } // for types like taiString and taiInt whose editors handle readOnly
  virtual bool          isCompound() const      { return false; } // true if requires multiple edit fields
  virtual bool          CanBrowse() const       { return false; } // only things from taBase classes up can be browse nodes

  taiType*              LowerBidder() { return static_cast<taiType*>(next_lower_bidder); }

  void                  AddToType(TypeDef* td); // add an instance to a type
  virtual int           BidForType(TypeDef*) { return 1; }
  // bid for (appropriateness) for given type

  virtual taiWidget*      GetWidgetRep(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  // get taiWidget rep of type -- delegates to _impl of type, except if readonly and the type can't handle ro

  virtual void          GetImage(taiWidget* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetWidgetRep
  virtual void          GetValue(taiWidget* dat, void* base);
  // get the value from the representation -- same rules as GetWidgetRep

  void                  SetCurParObjType(void* par_obj_base, TypeDef* pob_typ) {
    m_par_obj_base = par_obj_base;
    m_par_obj_type = pob_typ;
  }
  // called by taiMember::GetImage_impl and GetValue_impl prior to calling respective functions

  void                  ClearCurParObjType() {
    m_par_obj_base = NULL;
    m_par_obj_type = NULL;
  }
  // called by taiMember::GetImage_impl and GetValue_impl after calling respective functions

  void*                 GetCurParObjBase() { return m_par_obj_base; }
  void*                 GetCurParObjType() { return m_par_obj_type; }

  void                  Initialize();
  void                  Destroy();

protected:
  virtual bool          isReadOnly(taiWidget* dat, IWidgetHost* host_ = NULL);
  // works in both GetWidgetRep, passing par=dat, as well as GetImage/GetValue, passing dat=dat and dlg=NULL
  virtual taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent, int flags_, MemberDef* mbr_);
  // default behavior uses a taiWidgetField type
  virtual taiWidget*      GetWidgetRepInline_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent, int flags_, MemberDef* mbr_);
  // default behavior same as GetWidgetRep_impl

  virtual void          GetImage_impl(taiWidget* dat, const void* base);
  // generate the gui representation of the data --  default behavior uses a taiWidgetField type
  virtual void          GetValue_impl(taiWidget* dat, void* base);
  // get the value from the representation --  default behavior uses a taiWidgetField type

  void*                 m_par_obj_base;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base
  TypeDef*              m_par_obj_type;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base type
};

#endif // taiType_h
