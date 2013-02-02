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

#ifndef taiViewType_h
#define taiViewType_h 1

// parent includes:
#include <taiTypeBase>

// member includes:
#include <iColor>

// declare all other types mentioned but not required to include:
class iPanelSet; //
class iPanel; //
class iPanelBase; //
class taiSigLink; //

TypeDef_Of(taiViewType);

class TA_API taiViewType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS viewer services for the type
  TAI_TYPEBASE_SUBCLASS(taiViewType, taiTypeBase)
public:
  static taiSigLink*   StatGetSigLink(void* el, TypeDef* el_typ); // get correct one

  taiViewType*          LowerBidder() { return static_cast<taiViewType*>(next_lower_bidder); }
  virtual bool          needSet() const {return m_need_set;} // only valid during constr of panels
  void                  AddView(TypeDef* td);   // add an instance to a type
  virtual int           BidForView(TypeDef*) {return 1;}
  virtual iPanelBase*   CreateDataPanel(taiSigLink* dl_); // creates a new data panel; normally override _impl
  virtual void          CheckUpdateDataPanelSet(iPanelSet* pan) {} // dynamically updates a data panel set; currently only called when set gets a USER_DATA_UPDATED notify
  virtual taiSigLink*  GetSigLink(void* data_, TypeDef* el_typ) {return NULL;}
    // get an existing, or create new if needed
  virtual const iColor  GetEditColorInherit(taiSigLink* dl, bool& ok) const {ok = false; return def_color;} // #IGNORE background color for edit dialog, include inherited colors from parents

  virtual iPanelSet* GetDataPanelSet() { return m_dps; }
  // return the data panel set that contains multiple data panels if present -- use to control flipping between them..

  void                  Initialize();
  void                  Destroy() {}

protected:
  void                  DataPanelCreated(iPanel* dp); // call in CreateDataPanel_impl for each dp frame created
  virtual void          CreateDataPanel_impl(taiSigLink* dl_) {} // create one or more dp's

  bool                  m_need_set;
  iPanelSet*        m_dps; // #IGNORE created automatically if more than one datapanel is created

private:
  iPanel*      m_dp; // #IGNORE single instance created for a call
};

#endif // taiViewType_h
