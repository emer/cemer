// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef BRAIN_VIEW_PANEL_H
#define BRAIN_VIEW_PANEL_H

#include "netstru_qtso.h" // overkill #include

class EMERGENT_API BrainViewPanel: public iViewPanelFrame {
  // frame for gui interface to a BrainView -- usually posted by the netview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  QWidget*		widg;

  QVBoxLayout*		layTopCtrls;
  QVBoxLayout*		layViewParams;
  QHBoxLayout*		  layDispCheck;
  QCheckBox*		    chkDisplay;
  QCheckBox*		    chkLayMove;
  QCheckBox*		    chkNetText;
  QLabel*		    lblTextRot;
  taiField*		    fldTextRot;
  QLabel*		    lblUnitText;
  taiComboBox*		    cmbUnitText;
  QLabel*		    lblDispMode;
  taiComboBox*		    cmbDispMode;
  QLabel*		    lblPrjnDisp;
  taiComboBox*		    cmbPrjnDisp;
  QLabel*		    lblPrjnWdth;
  taiField*		    fldPrjnWdth;

  QHBoxLayout*		  layFontsEtc;
  QLabel*		    lblUnitTrans;
  taiField*		    fldUnitTrans;
  QLabel*		    lblUnitFont;
  taiField*		    fldUnitFont;
  QLabel*		    lblLayFont;
  taiField*		    fldLayFont;
  QLabel*		    lblMinLayFont;
  taiField*		    fldMinLayFont;
  QCheckBox*		    chkXYSquare;
  QCheckBox*		    chkLayGp;

  QVBoxLayout*		layDisplayValues;
  QHBoxLayout*		  layColorScaleCtrls;
  QCheckBox*		    chkAutoScale;       // autoscale ck_box
  QPushButton*		    butScaleDefault;    // revert to default  
  QCheckBox*		    chkWtLines;
  QCheckBox*		    chkWtLineSwt;
  QLabel*		    lblWtLineWdth;
  taiField*		    fldWtLineWdth;
  QLabel*		    lblWtLineThr;
  taiField*		    fldWtLineThr;
  QLabel*		    lblWtPrjnKUn;
  taiField*		    fldWtPrjnKUn;
  QLabel*		    lblWtPrjnKGp;
  taiField*		    fldWtPrjnKGp;
  QLabel*		    lblWtPrjnLay;
  taiGroupElsButton*	    gelWtPrjnLay;

  QHBoxLayout*		 layColorBar;
  QCheckBox*		    chkSnapBord;
  QLabel*		    lblSnapBordWdth;
  taiField*		    fldSnapBordWdth;
  QLabel*		    lblUnitSpacing;
  taiField*		    fldUnitSpacing;
  ScaleBar*		   cbar;	      // colorbar
  QPushButton*		   butSetColor;

  QTabWidget* 		tw; 
  QTreeWidget*		  lvDisplayValues;
  iTreeView*		  tvSpecs;
  
  iMethodButtonMgr*	meth_but_mgr;
  QWidget*		widCmdButtons;

  BrainView*		bv();

  void 			ColorScaleFromData();
  virtual void		GetVars();
  virtual void		InitPanel();

  BrainViewPanel(BrainView* dv_);
  ~BrainViewPanel();

public: // IDataLinkClient interface
  override void*	This();
  override TypeDef*     GetTypeDef() const;

protected:
  int			cmd_x; // current coords of where to place next button/ctrl
  int			cmd_y;
  BaseSpec*		m_cur_spec; // cur spec chosen -- only compared, so ok if stale
  bool			req_full_render; // when updating, call Render on netview
  bool			req_full_build;	 // when updating, call Build on netview
  override void		UpdatePanel_impl();
  override void		GetValue_impl();
  override void		CopyFrom_impl();
  void 			setHighlightSpec(BaseSpec* spec, bool force = false);

public slots:
  void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void			dynbuttonActivated(int but_no); // for hot member buttons

protected slots:
  void			butScaleDefault_pressed();
  void 			butSetColor_pressed();
  void			lvDisplayValues_selectionChanged();
  void			tvSpecs_CustomExpandFilter(iTreeViewItem* item,
						   int level, bool& expand);
  void			tvSpecs_ItemSelected(iTreeViewItem* item); 
  // note: this one seems a bit defunct for the iTreeView -- replaced with Notify below
  void			tvSpecs_Notify(ISelectableHost* src, int op);

};

#endif // BRAIN_VIEW_PANEL_H
