// Copyright 2017, Regents of the University of Colorado,
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

#include "iViewPanelOfBrain.h"
#include <Network>
#include <BrainView>
#include <BrainAtlasRegexpPopulator>
#include <NiftiReader>
#include <iFlowLayout>
#include <T3ExaminerViewer>
#include <taiWidgetFieldRegexp>
#include <iLineEdit>
#include <iHColorScaleBar>

#include <taMisc>
#include <taiMisc>

// Probably don't need all of these Qt headers:
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QTreeWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QStandardItemModel>
#include <QColorDialog>
#include <QSortFilterProxyModel>
#include <QDialogButtonBox>
#include <QHeaderView>

////////////////////////////////////////////////////
//  iViewPanelOfBrain
////////////////////////////////////////////////////
BrainView* iViewPanelOfBrain::getBrainView()
{
  return static_cast<BrainView *>(m_dv);
}

void* iViewPanelOfBrain::This()
{
  return this;
}

TypeDef* iViewPanelOfBrain::GetTypeDef() const
{
  return &TA_iViewPanelOfBrain;
}

iViewPanelOfBrain::iViewPanelOfBrain(BrainView* dv_)
  : inherited(dv_)
{
  // set a safe default if no BrainViewState yet...
  int max_slices(1);
  Network* net = NULL;
  max_slices = dv_->MaxSlices();
  net = dv_->net();

  bool ok;
  this->setStyleSheet("iViewPanel { background-color: #" +
                      net->GetEditColor(ok).toString() + "; }");
  
  int font_spec = taiMisc::fonMedium;
  req_full_render = true;
  req_full_build = false;

  const int min_slider = 80;

  T3ExaminerViewer* vw = dv_->GetViewer();
  if(vw)
    connect(vw, SIGNAL(dynbuttonActivated(int)), this, SLOT(dynbuttonActivated(int)));

  widg = new QWidget();
  layTopCtrls = new QVBoxLayout(widg);
  layTopCtrls->setSpacing(2);
  layTopCtrls->setMargin(2);

  layViewParams = new QVBoxLayout();
  layTopCtrls->addLayout(layViewParams);
  layViewParams->setSpacing(2);
  layViewParams->setMargin(0);

  /////////////////////////////////////////////////////////////////////
  QHBoxLayout* bvControls = new QHBoxLayout();
  layViewParams->addLayout(bvControls);

  QLabel* label = taiM->NewLabel("View\nPlane:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("Which of the anatomical planes to view."));
  bvControls->addWidget(label);
  m_view_plane_comb = new QComboBox(widg);
  m_view_plane_comb->addItem("AXIAL");
  m_view_plane_comb->addItem("SAGITTAL");
  m_view_plane_comb->addItem("CORONAL");
  connect(m_view_plane_comb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetViewPlane(int)));
  connect(this, SIGNAL(ViewPlaneChanged(int)), m_view_plane_comb, SLOT(setCurrentIndex(int)));
  bvControls->addWidget(m_view_plane_comb);
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Starting\nSlice:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("The starting slice number to view."));
  bvControls->addWidget(label);
  m_slice_strt_sbox = new QSpinBox(widg);
  m_slice_strt_sbox->setRange(1,max_slices);
  m_slice_strt_sbox->setValue(1);
  connect(m_slice_strt_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceStart(int)));
  connect(this, SIGNAL(SliceStartChanged(int)), m_slice_strt_sbox, SLOT(setValue(int)));
  bvControls->addWidget(m_slice_strt_sbox);
  bvControls->addSpacing(taiM->hsep_c);

  m_slice_strt_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_strt_slid->setRange(1, max_slices);
  m_slice_strt_slid->setValue(1);
  m_slice_strt_slid->setMinimumWidth(min_slider);
  connect(m_slice_strt_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceStart(int)));
  connect(this, SIGNAL(SliceStartChanged(int)), m_slice_strt_slid, SLOT(setValue(int)));
  bvControls->addWidget(m_slice_strt_slid);
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Lock\n#Slices:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("Lock slice sliders to maintain number of slices."));
  bvControls->addWidget(label);
  m_lock_slices_chbox = new QCheckBox(widg);
  m_lock_slices_chbox->setCheckState(Qt::Unchecked);
  connect(m_lock_slices_chbox, SIGNAL(stateChanged(int)), this, SLOT(SetLockSlices(int)));
  bvControls->addWidget(m_lock_slices_chbox);
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Ending\nSlice:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("The ending slice number to view."));
  bvControls->addWidget(label);
  m_slice_end_sbox = new QSpinBox(widg);
  m_slice_end_sbox->setRange(1, max_slices);
  m_slice_end_sbox->setValue(max_slices);
  connect(this, SIGNAL(SliceEndChanged(int)), m_slice_end_sbox, SLOT(setValue(int)));
  connect(m_slice_end_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceEnd(int)));
  bvControls->addWidget(m_slice_end_sbox);
  bvControls->addSpacing(taiM->hsep_c);

  m_slice_end_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_end_slid->setTickPosition(QSlider::NoTicks);
  m_slice_end_slid->setRange(1, max_slices);
  m_slice_end_slid->setValue(max_slices);
  m_slice_end_slid->setMinimumWidth(min_slider);
  connect(this, SIGNAL(SliceEndChanged(int)), m_slice_end_slid, SLOT(setValue(int)));
  connect(m_slice_end_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceEnd(int)));
  bvControls->addWidget(m_slice_end_slid);
  bvControls->addStretch();     // need final stretch to prevent full stretching

  bvControls = new QHBoxLayout();
  layViewParams->addLayout(bvControls);

  const int unit_trans(60);
  label = taiM->NewLabel("Unit Values\nTransparency:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("The transparency value of unit values."));
  bvControls->addWidget(label);
  m_unit_val_tran_sbox = new QSpinBox(widg);
  m_unit_val_tran_sbox->setRange(1, 100);
  m_unit_val_tran_sbox->setValue(unit_trans);
  m_unit_val_tran_sbox->setSuffix("%");
  connect(m_unit_val_tran_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetUnitValuesTransparency(int)));
  connect(this, SIGNAL(UnitValuesTransparencyChanged(int)), m_unit_val_tran_sbox, SLOT(setValue(int)));
  bvControls->addWidget(m_unit_val_tran_sbox);
  bvControls->addSpacing(taiM->hsep_c);

  m_unit_val_tran_slid =  new QSlider(Qt::Horizontal, widg);
  m_unit_val_tran_slid->setTickPosition(QSlider::NoTicks);
  m_unit_val_tran_slid->setRange(1,100);
  m_unit_val_tran_slid->setValue(unit_trans);
  m_unit_val_tran_slid->setMinimumWidth(min_slider);
  connect(m_unit_val_tran_slid, SIGNAL(valueChanged(int)), this, SLOT(SetUnitValuesTransparency(int)));
  connect(this, SIGNAL(UnitValuesTransparencyChanged(int)), m_unit_val_tran_slid, SLOT(setValue(int)));
  bvControls->addWidget(m_unit_val_tran_slid);
  bvControls->addSpacing(taiM->hspc_c);

  const int slice_trans(90);
  label = taiM->NewLabel("Slice\nTransparency:", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("The transparency value of brain slices."));
  bvControls->addWidget(label);
  m_slice_trans_sbox = new QSpinBox(widg);
  m_slice_trans_sbox->setRange(1, 100);
  m_slice_trans_sbox->setValue(slice_trans);
  m_slice_trans_sbox->setSuffix("%");
  connect(m_slice_trans_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceTransparency(int)));
  connect(this, SIGNAL(SliceTransparencyChanged(int)), m_slice_trans_sbox, SLOT(setValue(int)));
  bvControls->addWidget(m_slice_trans_sbox);
  bvControls->addSpacing(taiM->hsep_c);

  m_slice_tran_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_tran_slid->setTickPosition(QSlider::NoTicks);
  m_slice_tran_slid->setRange(1,100);
  m_slice_tran_slid->setValue(slice_trans);
  m_slice_tran_slid->setMinimumWidth(min_slider);
  connect(m_slice_tran_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceTransparency(int)));
  connect(this, SIGNAL(SliceTransparencyChanged(int)), m_slice_tran_slid, SLOT(setValue(int)));
  bvControls->addWidget(m_slice_tran_slid);
  bvControls->addStretch();


  // the populator for the brain colorization and atlas widgets
  atlas_regexp_pop = new BrainAtlasRegexpPopulator();

  ////////////////////////////////////////////////////////////////////////////
  // Brain colorization widgets
  bvControls = new QHBoxLayout();
  layViewParams->addLayout(bvControls);
  label = taiM->NewLabel("Color brain:", widg, font_spec);
  bvControls->addWidget(label);
  m_chk_color_brain = new QCheckBox(widg);
  m_chk_color_brain->setCheckState(Qt::Unchecked);
  m_chk_color_brain->setEnabled(false);
  connect(m_chk_color_brain, SIGNAL(stateChanged(int)), this, SLOT(SetColorBrain(int)));
  bvControls->addWidget(m_chk_color_brain);
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Areas (regexp):", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("Select brain areas using a regular expression (wild card) to color according to their color in the atlas -- use the full regexp .*/.*/.*/.*/.* to color all areas."));
  bvControls->addWidget(label);

  fldBrainColorRegexp = dl.Add(new taiWidgetFieldRegexp(&TA_taString, this, dynamic_cast<taiWidget*>(this), widg,0, dynamic_cast<iDialogRegexpPopulator*>(atlas_regexp_pop)));
  fldBrainColorRegexp->SetFieldOwner(net);

  // GetRep() returns the widget that holds the line edit and the edit button.
  // (See code in taiWidgetText::taiWidgetText(), if (needs_edit_button), for details.)
  // That is the correct widget to add to the BV panel.
  bvControls->addWidget(fldBrainColorRegexp->GetRep());
  // rep() returns the line edit itself, which is what we want to set the
  // width of and what we want to connect to.
  if (iLineEdit* theLineEdit = dynamic_cast<iLineEdit*>(fldBrainColorRegexp->rep())) {
    theLineEdit->setCharWidth(40);
  }

  connect(fldBrainColorRegexp->rep(), SIGNAL(returnPressed()), this, SLOT(ColorBrainRegexpEdited()));
  bvControls->addSpacing(taiM->hsep_c);
  bvControls->addSpacing(taiM->hsep_c);

  butEditAtlas = new QPushButton("Edit Atlas", widg);
  butEditAtlas->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butEditAtlas->setMaximumWidth(taiM->maxButtonWidth() / 2);
  bvControls->addWidget(butEditAtlas);
  connect(butEditAtlas, SIGNAL(pressed()), this, SLOT(butEditAtlas_pressed()));
  bvControls->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  // Brain atlas widgets
  bvControls = new QHBoxLayout();
  layViewParams->addLayout(bvControls);
  label = taiM->NewLabel("View atlas:", widg, font_spec);
  bvControls->addWidget(label);
  m_chk_atlas = new QCheckBox(widg);
  m_chk_atlas->setCheckState(Qt::Unchecked);
  m_chk_atlas->setEnabled(false);
  connect(m_chk_atlas, SIGNAL(stateChanged(int)), this, SLOT(SetViewAtlas(int)));
  bvControls->addWidget(m_chk_atlas);
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Atlas label (rexgexp)", widg, font_spec);
  label->setToolTip(taiMisc::ToolTipPreProcess("Select brain areas to draw in opaque square regions -- the same as the display of unit values -- using a regular expression (wild card) -- works best with a small number of areas, and do NOT select all .*/.*/.*/.*/.* -- very slow"));
  bvControls->addWidget(label);

  fldBrainAtlasRegexp = dl.Add(new taiWidgetFieldRegexp(&TA_taString, this, dynamic_cast<taiWidget*>(this), widg,0, dynamic_cast<iDialogRegexpPopulator*>(atlas_regexp_pop)));
  fldBrainAtlasRegexp->SetFieldOwner(net);
  bvControls->addWidget(fldBrainAtlasRegexp->GetRep());
  if (iLineEdit* theLineEdit = dynamic_cast<iLineEdit*>(fldBrainAtlasRegexp->rep())) {
    theLineEdit->setCharWidth(40);
  }
  connect(fldBrainAtlasRegexp->rep(), SIGNAL(returnPressed()), this, SLOT(ViewAtlasRegexpEdited()));
  bvControls->addStretch();

  // listen for BrainViewState state changed
  connect(this, SIGNAL(StateChanged(int)), this, SLOT(UpdateViewFromState(int)));


  layDispCheck = new QHBoxLayout();
  layViewParams->addLayout(layDispCheck);
  chkNetText = new QCheckBox("Net\nTxt", widg);
  chkNetText->setToolTip(taiMisc::ToolTipPreProcess("Turn on the network text display at the base of the network, showing the current state of various counters and stats"));
  connect(chkNetText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()));
  layDispCheck->addWidget(chkNetText);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkLayMove = new QCheckBox("Manip", widg);
  chkLayMove->setToolTip(taiMisc::ToolTipPreProcess("Turn on the manipulation (moving, scaling, rotating the brain view) controls when in the manipulation mode (red arrow) of viewer"));
  connect(chkLayMove, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()));
  layDispCheck->addWidget(chkLayMove);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkAutoScale = new QCheckBox("Auto\nScale", widg);
  chkAutoScale->setToolTip(taiMisc::ToolTipPreProcess("Automatically scale min and max values of colorscale based on values of variable being displayed"));
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()));
  layDispCheck->addWidget(chkAutoScale);
  layDispCheck->addSpacing(taiM->hsep_c);
  layDispCheck->addStretch();

  layDisplayValues = new QVBoxLayout();
  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layiColorBar = new QHBoxLayout();
  layDisplayValues->addLayout(layiColorBar);

  butScaleDefault = new QPushButton("Defaults", widg);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butScaleDefault->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layiColorBar->addWidget(butScaleDefault);
  layiColorBar->addSpacing(taiM->hsep_c);
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()));

  cbar = new iHColorScaleBar(&(dv_->scale), iColorScaleBar::RANGE, true, true, widg);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()));
  layiColorBar->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  layiColorBar->addSpacing(taiM->hsep_c);

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layiColorBar->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()));

  ////////////////////////////////////////////////////////////////////////////
  setCentralWidget(widg);
  tw = new QTabWidget(this);
  ////////////////////////////////////////////////////////////////////////////
  lvDisplayValues = new QTreeWidget();
  tw->addTab(lvDisplayValues, "Unit Display Values");
  lvDisplayValues->setRootIsDecorated(false); // makes it look like a list
  QStringList hdr;
  hdr << "Value" << "Description";
  lvDisplayValues->setHeaderLabels(hdr);
  lvDisplayValues->setSortingEnabled(false);
  lvDisplayValues->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(lvDisplayValues, SIGNAL(itemSelectionChanged()), this, SLOT(lvDisplayValues_selectionChanged()));

  layTopCtrls->addWidget(tw);
}

iViewPanelOfBrain::~iViewPanelOfBrain()
{
  if (BrainView *bv = getBrainView()) {
    bv->bvp = NULL;
  }

  delete atlas_regexp_pop;
  atlas_regexp_pop = 0;
}

void iViewPanelOfBrain::UpdatePanel_impl()
{
  inherited::UpdatePanel_impl();
  ++updating;
  BrainView *bv = getBrainView(); // cache
  if (!bv) return;

  if(req_full_build) {
    req_full_build = false;
    bv->Reset();
    bv->BuildAll();
  }
  if(req_full_render) {
    req_full_render = false;
    bv->Render();
  }

  m_unit_val_tran_sbox->setValue((int)(bv->view_params.unit_trans*100));
  chkLayMove->setChecked(bv->lay_mv);

  // update the brain coloring widgets
  if (bv->net()->brain_atlas.ptr() == NULL) {
    m_chk_color_brain->setEnabled(false);
    m_chk_color_brain->setChecked(false);
    fldBrainColorRegexp->setVisible(false); // @TODO - would be nice but seems to do nothing
  }
  else {
    m_chk_color_brain->setEnabled(true);
    m_chk_color_brain->setChecked(bv->ColorBrain());
    fldBrainColorRegexp->setVisible(true); // @TODO - would be nice but seems to do nothing
  }
  fldBrainColorRegexp->rep()->setText(bv->ColorBrainRegexp());

  // update the atlas viewing widgets
  if (bv->net()->brain_atlas.ptr() == NULL) {
    m_chk_atlas->setEnabled(false);
    m_chk_atlas->setChecked(false);
    fldBrainAtlasRegexp->setVisible(false); // @TODO - would be nice but seems to do nothing
  }
  else {
    m_chk_atlas->setEnabled(true);
    m_chk_atlas->setChecked(bv->ViewAtlas());
    fldBrainAtlasRegexp->setVisible(true); // @TODO - would be nice but seems to do nothing
  }
  fldBrainAtlasRegexp->rep()->setText(bv->brain_area_regexp);


  // update var selection
  int i = 0;
  QTreeWidgetItemIterator it(lvDisplayValues);
  QTreeWidgetItem* item = NULL;
  while (*it) {
    item = *it;
    bool is_selected = (bv->cur_unit_vals.FindEl(item->text(0)) >= 0);
    item->setSelected(is_selected);
    // if list is size 1 make sure that there is a scale_range entry for this one
    ++it;
    ++i;
  }
  ColorScaleFromData();

  --updating;
}

void iViewPanelOfBrain::UpdateViewFromState(int state)
{
  // BrainViewState has changed...determine if full render
  // is required and make call to apply the GUI change

  if (req_full_render == true) {
    return; // already have an update to do
  }
  if (state & BrainView::MAJOR) {
    req_full_render = true;
  }
  else {
    req_full_render = false;
  }
  Apply_Async();
}

//////////////////////////////////////////////////////////////////
void iViewPanelOfBrain::SetDataName(const QString& data_name)
{
  getBrainView()->SetDataName(data_name);
}
void iViewPanelOfBrain::SetDimensions(const taVector3i& dimensions)
{
  getBrainView()->SetDimensions(dimensions);
}
void iViewPanelOfBrain::SetViewPlane(int plane)
{
  getBrainView()->SetViewPlane(plane);
}
void iViewPanelOfBrain::SetSliceStart(int start)
{
  getBrainView()->SetSliceStart(start);
}
void iViewPanelOfBrain::SetSliceEnd(int end)
{
  getBrainView()->SetSliceEnd(end);
}
void iViewPanelOfBrain::SetLockSlices(int state)
{
  getBrainView()->SetLockSlices(state);
}
void iViewPanelOfBrain::SetSliceSpacing(int spacing)
{
  getBrainView()->SetSliceSpacing(spacing);
}
void iViewPanelOfBrain::SetSliceTransparency(int transparency)
{
  getBrainView()->SetSliceTransparency(transparency);
}
void iViewPanelOfBrain::SetUnitValuesTransparency(int transparency)
{
  getBrainView()->SetUnitValuesTransparency(transparency);
}
void iViewPanelOfBrain::SetColorBrain(int state)
{
  getBrainView()->SetColorBrain(state);
}
void iViewPanelOfBrain::ColorBrainRegexpEdited()
{
  QString regexp = fldBrainColorRegexp->GetValue().toQString();
  QRegExp re(regexp);
  if (re.isValid()) {
    SetColorBrainRegexp(regexp);
    m_chk_color_brain->setCheckable(true);
  }
  else {
    m_chk_color_brain->setCheckable(false);
  }
}
void iViewPanelOfBrain::SetColorBrainRegexp(const QString& regexp)
{
  getBrainView()->SetColorBrainRegexp(regexp);
}

void iViewPanelOfBrain::SetViewAtlas(int state)
{
  getBrainView()->SetViewAtlas(state);
}

void iViewPanelOfBrain::ViewAtlasRegexpEdited()
{
  QString regexp = fldBrainAtlasRegexp->GetValue().toQString();
  QRegExp re(regexp);
  if (re.isValid()) {
    if(regexp == ".*/.*/.*/.*/.*") {
      taMisc::Warning("You cannot set the match-all regular expression .*/.*/.*/.*/.* -- fills in the whole brain with squares and takes forever.");
      m_chk_atlas->setCheckable(false);
    }
    SetViewAtlasRegexp(regexp);
    m_chk_atlas->setCheckable(true);
  }
  else {
    m_chk_atlas->setCheckable(false);
  }
}
void iViewPanelOfBrain::SetViewAtlasRegexp(const QString& regexp)
{
  getBrainView()->SetViewAtlasRegexp(regexp);
}
void iViewPanelOfBrain::EmitDataNameChanged(const QString& name)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit DataNameChanged(name);
}
void iViewPanelOfBrain::EmitDimensionsChanged(const taVector3i& d)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit DimensionsChanged(d);
}
void iViewPanelOfBrain::EmitViewPlaneChanged(int plane)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit ViewPlaneChanged(plane);
}
void iViewPanelOfBrain::EmitNumSlicesChanged(int nSlices)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit NumSlicesChanged(nSlices);
}
void iViewPanelOfBrain::EmitSliceStartChanged(int start)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit SliceStartChanged(start);
}
void iViewPanelOfBrain::EmitSliceEndChanged(int end)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit SliceEndChanged(end);
}
void iViewPanelOfBrain::EmitSliceSpacingChanged(int spacing)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit SliceSpacingChanged(spacing);
}
void iViewPanelOfBrain::EmitSliceTransparencyChanged(int transparency)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit SliceTransparencyChanged(transparency);
}
void iViewPanelOfBrain::EmitUnitValuesTransparencyChanged(int transparency)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit UnitValuesTransparencyChanged(transparency);
}
void iViewPanelOfBrain::EmitStateChanged(int state)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit StateChanged(state);
}
void iViewPanelOfBrain::EmitColorBrainAreaRegexpChanged(const QString& regexp)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit BrainColorRegexpChanged(regexp);
}
void iViewPanelOfBrain::EmitViewAtlasRegexpChanged(const QString& regexp)
{
  // calling this method allows us to raise a signal, notifying
  // any widgets that may be connected to it
  emit ViewAtlasRegexpChanged(regexp);
}

void iViewPanelOfBrain::GetValue_impl()
{
  inherited::GetValue_impl();
  BrainView *bv = getBrainView(); // cache
  if (!bv) return;
  req_full_build = false;

  // update widget limits based on BrainViewState
  UpdateWidgetLimits();

  // It used to be we would re-render no matter what...but
  // this proved slow for some of the brain view controls,
  // so now we by default do NOT set that flag and let
  // UpdateFromViewState set it as necessary
  // req_full_render = true; // everything requires a re-render

  bv->view_params.unit_trans = ((float)(m_unit_val_tran_sbox->value())/100.0f);

  // net text requires rebuild since it is controlled in BV::Render
  if (bv->net_text != chkNetText->isChecked()) {
    req_full_render = true;
  }
  bv->net_text = chkNetText->isChecked();
  bv->lay_mv = chkLayMove->isChecked();

  bv->color_brain_regexp = fldBrainColorRegexp->GetValue();
  bv->brain_area_regexp = fldBrainAtlasRegexp->GetValue();

  if (!req_full_render) {
    // just need update, not full rebuild/render
    bv->AsyncRenderUpdate();
  }
  bv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void iViewPanelOfBrain::CopyFrom_impl()
{
  if (BrainView *bv = getBrainView()) {
    bv->CallFun("CopyFromView");
  }
}

void iViewPanelOfBrain::butScaleDefault_pressed()
{
  if (updating) return;
  if (BrainView *bv = getBrainView()) {
    bv->SetScaleDefault();
    bv->UpdateDisplay(true);
  }
}

void iViewPanelOfBrain::butEditAtlas_pressed()
{
  if (updating) return;
  if (BrainView *bv = getBrainView()) {
    Network *net = bv->net();
    if (!net || !net->brain_atlas) return;
    net->brain_atlas->EditAtlas();
    bv->UpdateDisplay(true);
  }
}

void iViewPanelOfBrain::butSetColor_pressed()
{
  if (updating) return;
  if (BrainView *bv = getBrainView()) {
    bv->CallFun("SetColorSpec");
  }
}

void iViewPanelOfBrain::ColorScaleFromData()
{
  if (BrainView *bv = getBrainView()) {
    ++updating;
    cbar->UpdateScaleValues();
    chkAutoScale->setChecked(bv->scale.auto_scale); //note: raises signal on widget! (grr...)
    --updating;
  }
}

void iViewPanelOfBrain::GetVars()
{
  BrainView *bv = getBrainView();
  if (!bv) return;

  lvDisplayValues->clear();
  if (bv->membs.size == 0) return;

  MemberDef* md;
  for (int i = 0; i < bv->membs.size; i++) {
    md = bv->membs[i];
    QStringList itm;
    itm << md->name << md->desc;
    new QTreeWidgetItem(lvDisplayValues, itm);
  }
  lvDisplayValues->resizeColumnToContents(0);
}

void iViewPanelOfBrain::InitPanel()
{
  if (BrainView *bv = getBrainView()) {
    ++updating;
    // fill monitor values
    GetVars();
    --updating;
  }
}

void iViewPanelOfBrain::lvDisplayValues_selectionChanged()
{
  if (updating) return;

  BrainView *bv = getBrainView();
  if (!bv) return;

  // redo the list each time, to guard against stale values
  bv->cur_unit_vals.Reset();
  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
  QTreeWidgetItem* item = NULL;
  for (int j = 0; j < items.size(); ++j) {
    item = items.at(j);
    bv->cur_unit_vals.Add(item->text(0));
  }
  MemberDef* md = (MemberDef*) bv->membs.FindName(bv->cur_unit_vals.SafeEl(0));
  if (md) {
    bv->setUnitDispMd(md);
    bv->UpdateViewerModeForMd(md);
  }
  ColorScaleFromData();
  bv->UpdateDisplay(false);
}

void iViewPanelOfBrain::dynbuttonActivated(int but_no)
{
  BrainView *bv = getBrainView();
  if (!bv) return;

  T3ExaminerViewer* vw = bv->GetViewer();
  if (!vw) return;

  iAction* dyb = vw->getDynButton(but_no);
  if (!dyb) return;

  String nm = dyb->text();
  bv->cur_unit_vals.Reset();
  bv->cur_unit_vals.Add(nm);
  if (MemberDef *md = (MemberDef*) bv->membs.FindName(nm)) {
    bv->setUnitDispMd(md);
    bv->UpdateViewerModeForMd(md);
    vw->setDynButtonChecked(but_no, true, true); // mutex
  }
  ColorScaleFromData();
  bv->UpdateDisplay(true);     // update panel
}

void iViewPanelOfBrain::viewWin_NotifySignal(ISelectableHost* src, int op)
{
  if (BrainView *bv = getBrainView()) {
    bv->viewWin_NotifySignal(src, op);
  }
}

void iViewPanelOfBrain::UpdateWidgetLimits()
{
  BrainView *bv = getBrainView();
  if (!bv) return;

  int max_slices(bv->MaxSlices());
  int num_slices(bv->NumSlices());

  // if user wants number of slices to remain fixed
  // we need to be sure limits are adjusted appropriately
  if (bv->NumSlicesAreLocked()) {
    m_slice_strt_sbox->setRange(1, max_slices - num_slices);
    m_slice_strt_slid->setRange(1, max_slices - num_slices);
    m_slice_end_sbox->setRange(num_slices + 1, max_slices);
    m_slice_end_slid->setRange(num_slices + 1, max_slices);
  }
  else {
    m_slice_strt_sbox->setRange(1, max_slices);
    m_slice_strt_slid->setRange(1, max_slices);
    m_slice_end_sbox->setRange(1, max_slices);
    m_slice_end_slid->setRange(1, max_slices);
  }
}
