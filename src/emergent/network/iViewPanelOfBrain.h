// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef iViewPanelOfBrain_h
#define iViewPanelOfBrain_h 1

// parent includes:
#include "network_def.h"
#include <iViewPanel>

// member includes:
#include <voidptr_Matrix>

// declare all other types mentioned but not required to include:
class iColorScaleBar;             // 
class taiWidgetField;                  // 
class taiWidgetFieldRegexp;            // 
class BrainAtlasRegexpPopulator; // 
class iMethodButtonMgr;          // 
class BrainView; //
class taVector3i; //
class QHBoxLayout; // 
class QCheckBox; //
class QTabWidget; //
class QPushButton; //
class QTreeWidget; //
class QTreeWidgetItem; //
class QSpinBox; //
class QComboBox; //
class QSlider; //
class QLabel; //
class ISelectableHost; //
class iTreeListWidget; //
class iTreeListWidgetItem; //


eTypeDef_Of(iViewPanelOfBrain);

class E_API iViewPanelOfBrain : public iViewPanel {
  // frame for gui interface to a BrainView -- usually posted by the netview
  Q_OBJECT
  INHERITED(iViewPanel)
public:
  QWidget*              widg;
  QVBoxLayout*          layTopCtrls;
  QVBoxLayout*          layViewParams;
  QHBoxLayout*          layDispCheck;
  QCheckBox*            chkLayMove;
  QCheckBox*            chkNetText;

  taiWidgetFieldRegexp*       fldBrainColorRegexp;
  taiWidgetFieldRegexp*       fldBrainAtlasRegexp;
  BrainAtlasRegexpPopulator* atlas_regexp_pop;
  QPushButton*          butEditAtlas;

  QVBoxLayout*          layDisplayValues;
  QCheckBox*            chkAutoScale;       // autoscale ck_box
  QPushButton*          butScaleDefault;    // revert to default

  QHBoxLayout*          layiColorBar;
  iColorScaleBar*        cbar;         // colorbar
  QPushButton*          butSetColor;

  QTabWidget*           tw;
  QTreeWidget*          lvDisplayValues;
  iTreeListWidget*      state_values;  // list of network variables that can be displayed and continuously updated in network view

  BrainView*            getBrainView();

  void                  ColorScaleFromData();
  virtual void          GetVars();
  virtual void          GetNetVars();
  virtual void          InitPanel() override;

  iViewPanelOfBrain(BrainView* dv_);
  ~iViewPanelOfBrain();

public: // ISigLinkClient interface
  void*        This() override;
  TypeDef*     GetTypeDef() const override;

protected:
  bool                  req_full_render; // when updating, call Render on netview
  bool                  req_full_build;  // when updating, call Build on netview
  void         UpdatePanel_impl() override;
  void         GetValue_impl() override;
  void         CopyFrom_impl() override;

public slots:
  void          viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview
  void          dynbuttonActivated(int but_no); // for hot member buttons
  void          UpdateViewFromState(int state);

  void          SetDataName(const QString& data_name);
  void          SetDimensions(const taVector3i& dimensions);
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
  void          EmitDimensionsChanged(const taVector3i& d);
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

  void          NetStateItemMoved(int from_index, int to_index);

#ifndef __MAKETA__
signals:
  void          DataNameChanged(const QString& name);
  void          DimensionsChanged(const taVector3i& d);
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
  void                  NetStateValues_itemClicked(QTreeWidgetItem* item, int col);
  void                  NetStateValues_itemChanged(QTreeWidgetItem* item, int col);

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

#endif // iViewPanelOfBrain_h
