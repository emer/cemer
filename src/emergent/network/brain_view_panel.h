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
#include "ta_qtdata.h"
class QString;

#ifndef __MAKETA__
# include <QItemDelegate>
#endif


////////////////////////////////////////////////////
//   BrainViewPanel
////////////////////////////////////////////////////

class EMERGENT_API BrainViewPanel : public iViewPanelFrame
{
  // frame for gui interface to a BrainView -- usually posted by the netview
  Q_OBJECT
  INHERITED(iViewPanelFrame)

public:
  QWidget*              widg;
  QVBoxLayout*          layTopCtrls;
  QVBoxLayout*          layViewParams;
  QHBoxLayout*          layDispCheck;
  QCheckBox*            chkDisplay;
  QCheckBox*            chkLayMove;
  QCheckBox*            chkNetText;

  QHBoxLayout*          layFontsEtc;
  QLabel*               lblUnitTrans;
  taiField*             fldUnitTrans;

  taiRegexpField*       fldBrainColorRegexp;
  taiRegexpField*       fldBrainAtlasRegexp;
  BrainAtlasRegexpPopulator* atlas_regexp_pop;
  QPushButton*          butEditAtlas;

  QVBoxLayout*          layDisplayValues;
  QHBoxLayout*          layColorScaleCtrls;
  QCheckBox*            chkAutoScale;       // autoscale ck_box
  QPushButton*          butScaleDefault;    // revert to default

  QHBoxLayout*          layColorBar;
  ScaleBar*             cbar;         // colorbar
  QPushButton*          butSetColor;

  QTabWidget*           tw;
  QTreeWidget*          lvDisplayValues;

  iMethodButtonMgr*     meth_but_mgr;
  QWidget*              widCmdButtons;

  BrainView*            getBrainView();

  void                  ColorScaleFromData();
  virtual void          GetVars();
  virtual void          InitPanel();

  BrainViewPanel(BrainView* dv_);
  ~BrainViewPanel();

public: // IDataLinkClient interface
  override void*        This();
  override TypeDef*     GetTypeDef() const;

protected:
  int                   cmd_x; // current coords of where to place next button/ctrl
  int                   cmd_y;
  bool                  req_full_render; // when updating, call Render on netview
  bool                  req_full_build;  // when updating, call Build on netview
  override void         UpdatePanel_impl();
  override void         GetValue_impl();
  override void         CopyFrom_impl();

public slots:
  void          viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void          dynbuttonActivated(int but_no); // for hot member buttons
  void          UpdateViewFromState(int state);

  void          SetDataName(const QString& data_name);
  void          SetDimensions(const TDCoord& dimensions);
  void          SetViewPlane( int plane );
  void          SetSliceStart(int start);
  void          SetSliceEnd(int end);
  void          SetLockSlices(int state);
  void          SetSliceSpacing(int spacing);
  void          SetSliceTransparency(int transparency);
  void          SetUnitValuesTransparency(int transparency);
  void          SetColorBrain(int state);
  void          ColorBrainRegexpEdited();
  void          SetColorBrainRegexp(const QString& regexp);
  void          SetViewAtlas(int state);
  void          ViewAtlasRegexpEdited();
  void          SetViewAtlasRegexp(const QString& regexp);

  void          EmitDataNameChanged(const QString& name);
  void          EmitDimensionsChanged(const TDCoord& d);
  void          EmitViewPlaneChanged(int plane);
  void          EmitNumSlicesChanged(int nSlices);
  void          EmitSliceStartChanged(int start);
  void          EmitSliceEndChanged(int end);
  void          EmitSliceSpacingChanged(int spacing);
  void          EmitSliceTransparencyChanged(int transparency);
  void          EmitUnitValuesTransparencyChanged(int transparency);
  void          EmitStateChanged(int state);
  void          EmitColorBrainAreaRegexpChanged(const QString& regexp);
  void          EmitViewAtlasRegexpChanged(const QString& regexp);

#ifndef __MAKETA__
signals:
  void          DataNameChanged(const QString& name);
  void          DimensionsChanged(const TDCoord& d);
  void          ViewPlaneChanged(int plane);
  void          NumSlicesChanged(int nSlices);
  void          SliceStartChanged(int start);
  void          SliceEndChanged(int end);
  void          SliceSpacingChanged(int spacing);
  void          SliceTransparencyChanged(int transparency);
  void          UnitValuesTransparencyChanged(int transparency);
  void          StateChanged(int state);
  void          BrainColorRegexpChanged(const QString& regexp);
  void          ViewAtlasRegexpChanged(const QString& regexp);
#endif

protected slots:
  void                  butScaleDefault_pressed();
  void                  butEditAtlas_pressed();
  void                  butSetColor_pressed();
  void                  lvDisplayValues_selectionChanged();

private:
  QComboBox*        m_view_plane_comb;
  QSpinBox*         m_slice_strt_sbox;
  QSlider*          m_slice_strt_slid;
  QCheckBox*        m_lock_slices_chbox;
  QSpinBox*         m_slice_end_sbox;
  QSlider*          m_slice_end_slid;
  QSpinBox*         m_unit_val_tran_sbox;
  QSlider*          m_unit_val_tran_slid;
  QSpinBox*         m_slice_trans_sbox;
  QSlider*          m_slice_tran_slid;
  QCheckBox*        m_chk_color_brain;
  QCheckBox*        m_chk_atlas;
  void              UpdateWidgetLimits();
};

// this is here for moc'ing

class EMERGENT_API iBrainViewEditDialog : public iRegexpDialog
{
  Q_OBJECT
  INHERITED(iRegexpDialog)

public:
  iBrainViewEditDialog(
    taiRegexpField* regexp_field,
    const String& field_name,
    iRegexpDialogPopulator *re_populator,
    const void *fieldOwner,
    bool read_only,
    bool editor_mode = true
  );

protected:
  virtual void          AddButtons();
  virtual void          SetColors();

protected slots:
  void                  itemClicked(const QModelIndex & index);
  override void         btnApply_clicked();
  override void         btnRandomColors_clicked();
  override void         btnColorsFromScale_clicked();
};

#endif // BRAIN_VIEW_PANEL_H
