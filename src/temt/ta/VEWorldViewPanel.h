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

#ifndef VEWorldViewPanel_h
#define VEWorldViewPanel_h 1

// parent includes:
#include <iViewPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API VEWorldViewPanel: public iViewPanelFrame {
  // frame for gui interface to a VEWorldView -- usually posted by the worldview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:

  QVBoxLayout*		layOuter;
  QHBoxLayout*		 layDispCheck;
  QCheckBox*		  chkDisplay;
  QCheckBox*		  chkDragObjs;
  QCheckBox*		  chkShowJoints;
  QLabel*                   lblDragSize;
  taiField*                 fldDragSize;

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

  VEWorldViewPanel(VEWorldView* dv_);
  ~VEWorldViewPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
  override TypeDef*	GetTypeDef() const {return &TA_VEWorldViewPanel;}

protected:
  bool			req_full_redraw;
  override void		UpdatePanel_impl();
  override void		GetValue_impl();

// public slots:
//   void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

  //protected slots:
};

#endif // VEWorldViewPanel_h
