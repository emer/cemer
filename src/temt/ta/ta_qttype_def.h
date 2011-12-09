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


// ta_qttype_def.h: easily extendable interface support for all types

#ifndef TAI_QTTYPE_DEF_H
#define TAI_QTTYPE_DEF_H

#include "ta_type.h"
#include "icolor.h"
#include "ta_TA_type.h"

// externals
class taBase;
class taiData;
class IDataHost;
class taiEditDataHost;
class iDataPanel;
class iDataPanelFrame;
class iDataPanelSet;
class EditDataPanel;
class iDocDataPanel;
class iDataViewer;
class taiDataLink;
class iMainWindowViewer;
class taiDataList;

class taiToken;
class taiTypeHier; //

//////////////////////////
//      taiType         //
//////////////////////////

class TA_API taiTypeBase: public taRefN {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
public:
  static void InitializeTypes(bool gui);  // called at startup to initialize the type system

  taiTypeBase(TypeDef* typ_);
  taiTypeBase();
  virtual ~taiTypeBase();

  bool HasLowerBidder() const {
    return next_lower_bidder;
  }
  virtual TypeDef* GetTypeDef() const {
    return &TA_taiTypeBase;
  }
  virtual taiTypeBase* TypeInst(TypeDef* td) const {
    return new taiTypeBase(td);
  }

private:
  void InsertThisIntoBidList_impl(taiTypeBase *&pHead);
  // Insert this object into a list of taiTypeBase objects bidding for some type.  The list is sorted in order of the objects' bids.

protected:
#ifndef __MAKETA__ // maketa doesn't like template methods
  template<typename T>
  void InsertThisIntoBidList(T *&pHead) {
    // Need this little helper function since Der** isn't-a Base**.
    taiTypeBase *pHeadBase = pHead;
    InsertThisIntoBidList_impl(pHeadBase);
    pHead = static_cast<T*>(pHeadBase);
  }
#endif

public:
  static const iColor   def_color;      // passed as a default, or explicitly, to indicate using default
  TypeDef*              typ;            // typedef of base object
  int                   bid;            // this object's bid for typ.
  taiTypeBase*          next_lower_bidder; // points to the next bidder in the linked-list
  bool                  no_setpointer;  // don't use SetPointer for taBase pointers (ie., for css or other secondary pointers)
};

// Implement common routines for taiTypeBase subclasses.
// The overridden TypeInst() function has a covariant return type.
// See also TAI_MEMBER_SUBCLASS, TAI_ARGTYPE_SUBCLASS, TAI_METHOD_SUBCLASS.
#define TAI_TYPEBASE_SUBCLASS(x, y)         \
    INHERITED(y)                            \
  public:                                   \
    x(TypeDef* td)                          \
      : y(td)                               \
    {                                       \
      Initialize();                         \
    }                                       \
    x()                                     \
    {                                       \
      Initialize();                         \
    }                                       \
    ~x()                                    \
    {                                       \
      Destroy();                            \
    }                                       \
    override TypeDef* GetTypeDef() const    \
    {                                       \
      return &TA_##x;                       \
    }                                       \
    override x* TypeInst(TypeDef* td) const \
    {                                       \
      return new x(td);                     \
    }

//////////////////////////
//      taiType         //
//////////////////////////

class TA_API taiType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
  TAI_TYPEBASE_SUBCLASS(taiType, taiTypeBase);
public:
  virtual bool          allowsInline() const    { return false; } // the type allows inline reps, obtained GetDataRepInline
  virtual bool          requiresInline() const  { return false; } // only inline reps allowed, no member-by-member reps
  virtual bool          handlesReadOnly() const { return false; } // for types like taiString and taiInt whose editors handle readOnly
  virtual bool          isCompound() const      { return false; } // true if requires multiple edit fields
  virtual bool          CanBrowse() const       { return false; } // only things from taBase classes up can be browse nodes

  taiType*              LowerBidder() { return static_cast<taiType*>(next_lower_bidder); }

  void                  AddToType(TypeDef* td); // add an instance to a type
  virtual int           BidForType(TypeDef*) { return 1; }
  // bid for (appropriateness) for given type

  virtual taiData*      GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
                                   taiType* parent_type_ = NULL, int flags = 0, MemberDef* mbr = NULL);
  // get taiData rep of type -- delegates to _impl of type, except if readonly and the type can't handle ro

  virtual void          GetImage(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void          GetValue(taiData* dat, void* base);
  // get the value from the representation -- same rules as GetDataRep

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
  virtual bool          isReadOnly(taiData* dat, IDataHost* host_ = NULL);
  // works in both GetDataRep, passing par=dat, as well as GetImage/GetValue, passing dat=dat and dlg=NULL
  virtual taiData*      GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_, MemberDef* mbr_);
  // default behavior uses a taiField type
  virtual taiData*      GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_, MemberDef* mbr_);
  // default behavior same as GetDataRep_impl

  virtual void          GetImage_impl(taiData* dat, const void* base);
  // generate the gui representation of the data --  default behavior uses a taiField type
  virtual void          GetValue_impl(taiData* dat, void* base);
  // get the value from the representation --  default behavior uses a taiField type

  void*                 m_par_obj_base;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base
  TypeDef*              m_par_obj_type;
  // if GetImage/Value is called by a taiMember, it will set this to point to parent object's base type
};

//////////////////////////
//   taiViewType        //
//////////////////////////

class TA_API taiViewType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS viewer services for the type
  TAI_TYPEBASE_SUBCLASS(taiViewType, taiTypeBase)
public:
  static taiDataLink*   StatGetDataLink(void* el, TypeDef* el_typ); // get correct one

  taiViewType*          LowerBidder() { return static_cast<taiViewType*>(next_lower_bidder); }
  virtual bool          needSet() const {return m_need_set;} // only valid during constr of panels
  void                  AddView(TypeDef* td);   // add an instance to a type
  virtual int           BidForView(TypeDef*) {return 1;}
  virtual iDataPanel*   CreateDataPanel(taiDataLink* dl_); // creates a new data panel; normally override _impl
  virtual void          CheckUpdateDataPanelSet(iDataPanelSet* pan) {} // dynamically updates a data panel set; currently only called when set gets a USER_DATA_UPDATED notify
  virtual taiDataLink*  GetDataLink(void* data_, TypeDef* el_typ) {return NULL;}
    // get an existing, or create new if needed
  virtual const iColor  GetEditColorInherit(taiDataLink* dl, bool& ok) const {ok = false; return def_color;} // #IGNORE background color for edit dialog, include inherited colors from parents

  virtual iDataPanelSet* GetDataPanelSet() { return m_dps; }
  // return the data panel set that contains multiple data panels if present -- use to control flipping between them..

  void                  Initialize();
  void                  Destroy() {}

protected:
  void                  DataPanelCreated(iDataPanelFrame* dp); // call in CreateDataPanel_impl for each dp frame created
  virtual void          CreateDataPanel_impl(taiDataLink* dl_) {} // create one or more dp's

  bool                  m_need_set;
  iDataPanelSet*        m_dps; // #IGNORE created automatically if more than one datapanel is created

private:
  iDataPanelFrame*      m_dp; // #IGNORE single instance created for a call
};

//////////////////////////
//   taiEdit            //
//////////////////////////

// offloads lots of the work to the edit panel...

class TA_API taiEdit : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiEdit, taiType);
public:
  taiEdit*              LowerBidder() { return static_cast<taiEdit*>(next_lower_bidder); }

  virtual const iColor  GetBackgroundColor(void* base, bool& ok); // gets for taBase
  virtual int           Edit(void* base=NULL, bool read_only=false, const iColor& bgcol = def_color); //edit wherever found (note: rarely overridden)
  virtual int           EditDialog(void* base, bool read_only = false,
                                   bool modal = false, const iColor& bgcol = def_color,
                                   int min_width=-1, int min_height=-1);
  // edit in a Dialog (note: rarely overridden) -- finds existing if non-modal else makes new

  virtual EditDataPanel* EditNewPanel(taiDataLink* link, void* base=NULL,
    bool read_only = false, const iColor& bgcol = def_color);
  //edit in a new panel (note: rarely overridden)

  virtual EditDataPanel* EditPanel(taiDataLink* link, void* base=NULL,
    bool read_only = false, iMainWindowViewer* not_in_win = NULL,
    const iColor& bgcol = def_color);
  //edit in a panel, prefer existing, else created new (note: rarely overridden)

  virtual int           BidForType(TypeDef*) { return 0;}
  virtual int           BidForEdit(TypeDef*) { return 1;}
  virtual void          AddEdit(TypeDef* td); // add an edit to a type

  void                  Initialize() {}
  void                  Destroy() {}

protected:
  virtual taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance, overridden by subclasses
};

#endif // TAI_QTTYPE_DEF_H
