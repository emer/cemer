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

#ifndef iViewPanelOfVEWorld_h
#define iViewPanelOfVEWorld_h 1

// parent includes:
#include <iViewPanel>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetField; //
class iMethodButtonMgr; //
class VEWorldView; //
class QHBoxLayout; // 
class QCheckBox; //
class QLabel; //


taTypeDef_Of(iViewPanelOfVEWorld);

class TA_API iViewPanelOfVEWorld: public iViewPanel {
  // frame for gui interface to a VEWorldView -- usually posted by the worldview
INHERITED(iViewPanel)
  Q_OBJECT
public:

  QVBoxLayout*		layOuter;
  QHBoxLayout*		 layDispCheck;
  QCheckBox*		  chkDisplay;
  QCheckBox*		  chkDragObjs;
  QCheckBox*		  chkShowJoints;
  QLabel*                   lblDragSize;
  taiWidgetField*                 fldDragSize;

  QHBoxLayout*		 layCams;
  QVBoxLayout*		  layCam0;
  QLabel*		  labcam0;
  QLabel*		  labcam0_txt;

  QVBoxLayout*		  layCam1;
  QLabel*		  labcam1;
  QLabel*		  labcam1_txt;

  iMethodButtonMgr*	meth_but_mgr;
  QWidget*		widCmdButtons;

  VEWorldView*		wv() {return (VEWorldView*)m_dv;} //

  iViewPanelOfVEWorld(VEWorldView* dv_);
  ~iViewPanelOfVEWorld();

public: // ISigLinkClient interface
  void*	This() CPP11_OVERRIDE {return (void*)this;} //
  TypeDef*	GetTypeDef() const CPP11_OVERRIDE {return &TA_iViewPanelOfVEWorld;}

protected:
  bool			req_full_redraw;
  void		UpdatePanel_impl() CPP11_OVERRIDE;
  void		GetValue_impl() CPP11_OVERRIDE;

// public slots:
//   void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

  //protected slots:
};

#endif // iViewPanelOfVEWorld_h
