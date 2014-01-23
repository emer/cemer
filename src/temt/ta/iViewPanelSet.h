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
#include <iPanelSetBase>

// member includes:

// declare all other types mentioned but not required to include:
class iTabBarBase;  //
class iViewPanel; //


taTypeDef_Of(iViewPanelSet);

class TA_API iViewPanelSet: public iPanelSetBase { //  contains 0 or more sub-view-panels, and btm hor tab for selecting panels
  Q_OBJECT
INHERITED(iPanelSetBase)
public:
  iTabBarBase*            tbSubPanels;

  bool         lockInPlace() const override {return true;}

  void                  AddSubPanel(iViewPanel* pn);

  void         UpdatePanel() override; // update tab names too
  void                  PanelDestroying(iViewPanel* pn); // so we remove tab

  iViewPanelSet(taiSigLink* dl_);
  ~iViewPanelSet();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
//  void               SigLinkDestroying(taSigLink* dl) override {} // nothing for us; subpanels handle
  TypeDef*     GetTypeDef() const override {return &TA_iViewPanelSet;}

protected:
  void         setCurrentPanelId_impl(int id) override;
};

#endif // iViewPanelSet_h
