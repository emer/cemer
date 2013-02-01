// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#ifndef NetViewPanel_h
#define NetViewPanel_h 1

// parent includes:
#include "network_def.h"
#include <iViewPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class NetView; //
class ColorScaleBar; //
class taiField; //
class taiComboBox; //
class taiGroupElsButton; //
class iTreeView; //
class iTreeViewItem; //
class iMethodButtonMgr; //
class taiRegexpField; //
class BaseSpec; //
class QHBoxLayout; // 
class QCheckBox; //
class QToolBar; //
class QLabel; //
class QPushButton; //
class QAction; //
class QTabWidget; //
class QTreeWidget; //
class ISelectableHost; //


TypeDef_Of(NetViewPanel);

class EMERGENT_API NetViewPanel: public iViewPanelFrame {
  // frame for gui interface to a NetView -- usually posted by the netview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:
  QWidget*              widg;
//  QVBoxLayout*                layWidg;

  QVBoxLayout*          layTopCtrls;
  QVBoxLayout*          layViewParams;
  QHBoxLayout*            layDispCheck;
  QCheckBox*                chkDisplay;
  QCheckBox*                chkLayMove;
  QCheckBox*                chkNetText;
  QLabel*                   lblLayLayout;
  taiComboBox*              cmbLayLayout;
  QLabel*                   lblTextRot;
  taiField*                 fldTextRot;
  QLabel*                   lblUnitText;
  taiComboBox*              cmbUnitText;
  QLabel*                   lblDispMode;
  taiComboBox*              cmbDispMode;
  QLabel*                   lblPrjnDisp;
  taiComboBox*              cmbPrjnDisp;

  QHBoxLayout*            layFontsEtc;
  QCheckBox*                chkShowIconified;
  QLabel*                   lblPrjnWdth;
  taiField*                 fldPrjnWdth;
  QLabel*                   lblUnitTrans;
  taiField*                 fldUnitTrans;
  QLabel*                   lblUnitFont;
  taiField*                 fldUnitFont;
  QLabel*                   lblLayFont;
  taiField*                 fldLayFont;
  QLabel*                   lblMinLayFont;
  taiField*                 fldMinLayFont;
  QCheckBox*                chkXYSquare;
  QCheckBox*                chkLayGp;

  QVBoxLayout*          layDisplayValues;
  QHBoxLayout*            layColorScaleCtrls;
  QCheckBox*                chkAutoScale;       // autoscale ck_box
  QPushButton*              butScaleDefault;    // revert to default
  QCheckBox*                chkWtLines;
  QCheckBox*                chkWtLineSwt;
  QLabel*                   lblWtLineWdth;
  taiField*                 fldWtLineWdth;
  QLabel*                   lblWtLineThr;
  taiField*                 fldWtLineThr;
  QLabel*                   lblWtPrjnKUn;
  taiField*                 fldWtPrjnKUn;
  QLabel*                   lblWtPrjnKGp;
  taiField*                 fldWtPrjnKGp;
  QLabel*                   lblWtPrjnLay;
  taiGroupElsButton*        gelWtPrjnLay;

  QHBoxLayout*           layColorBar;
  QCheckBox*                chkSnapBord;
  QLabel*                   lblSnapBordWdth;
  taiField*                 fldSnapBordWdth;
  QLabel*                   lblUnitSpacing;
  taiField*                 fldUnitSpacing;
  ColorScaleBar*           cbar;              // colorbar
  QPushButton*             butSetColor;

  QHBoxLayout*           layHistory;
  QToolBar*                histTB;
  QCheckBox*               chkHist;
  taiField*                fldHistMax;
  taiField*                fldHistFF;
  QAction*                 actBack_All;
  QAction*                 actBack_F;
  QAction*                 actBack;
  QAction*                 actFwd;
  QAction*                 actFwd_F;
  QAction*                 actFwd_All;
  QLabel*                  lblHist;
  QAction*                 actMovie;

  QTabWidget*           tw;
  QTreeWidget*            lvDisplayValues;
  iTreeView*              tvSpecs;

  iMethodButtonMgr*     meth_but_mgr;
  QWidget*              widCmdButtons;

  NetView*              getNetView();

  void                  ColorScaleFromData();
  virtual void          GetVars();
  virtual void          InitPanel();

  NetViewPanel(NetView* dv_);
  ~NetViewPanel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;} //
//  override void               SigLinkDestroying(taSigLink* dl);
//  override void               SigLinkClientRemoving(taSigLink* dl, ISigLinkClient* dlc);
  override TypeDef*     GetTypeDef() const {return &TA_NetViewPanel;}

protected:
  int                   cmd_x; // current coords of where to place next button/ctrl
  int                   cmd_y;
  BaseSpec*             m_cur_spec; // cur spec chosen -- only compared, so ok if stale
  bool                  req_full_render; // when updating, call Render on netview
  bool                  req_full_build;  // when updating, call Build on netview
  override void         UpdatePanel_impl();
  override void         GetValue_impl();
  override void         CopyFrom_impl();
  void                  setHighlightSpec(BaseSpec* spec, bool force = false);

public slots:
  void                  viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void                  dynbuttonActivated(int but_no); // for hot member buttons
  void                  unTrappedKeyPressEvent(QKeyEvent* e);
  // gets signal of same name from T3ExaminerViewer -- used for keyboard arrow nav

protected slots:
  void                  butScaleDefault_pressed();
  void                  butSetColor_pressed();
  void                  hist_back();
  void                  hist_back_f();
  void                  hist_back_all();
  void                  hist_fwd();
  void                  hist_fwd_f();
  void                  hist_fwd_all();
  void                  hist_movie();
  void                  lvDisplayValues_selectionChanged();
  void                  tvSpecs_CustomExpandFilter(iTreeViewItem* item,
                                                   int level, bool& expand);
  void                  tvSpecs_ItemSelected(iTreeViewItem* item);
  // note: this one seems a bit defunct for the iTreeView -- replaced with Notify below
  void                  tvSpecs_Notify(ISelectableHost* src, int op);

};

#endif // NetViewPanel_h
