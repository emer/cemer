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

#ifndef iViewPanelSet_h
#define iViewPanelSet_h 1

// parent includes:
#include <iDataPanelSetBase>

// member includes:

// declare all other types mentioned but not required to include:
class iTabBarBase;  //
class iViewPanelFrame; //


TypeDef_Of(iViewPanelSet);

class TA_API iViewPanelSet: public iDataPanelSetBase { //  contains 0 or more sub-view-panels, and btm hor tab for selecting panels
  Q_OBJECT
INHERITED(iDataPanelSetBase)
public:
  iTabBarBase*            tbSubPanels;

  override bool         lockInPlace() const {return true;}

  void                  AddSubPanel(iViewPanelFrame* pn);

  override void         UpdatePanel(); // update tab names too
  void                  PanelDestroying(iViewPanelFrame* pn); // so we remove tab

  iViewPanelSet(taiSigLink* dl_);
  ~iViewPanelSet();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
//  override void               SigLinkDestroying(taSigLink* dl) {} // nothing for us; subpanels handle
  override TypeDef*     GetTypeDef() const {return &TA_iViewPanelSet;}

protected:
  override void         setCurrentPanelId_impl(int id);
};

#endif // iViewPanelSet_h
