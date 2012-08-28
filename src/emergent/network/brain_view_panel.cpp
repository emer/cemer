//    // Copyright, 1995-2011, Regents of the University of Colorado,
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

#include "brain_view_panel.h"
#include "brain_view.h"
#include "iflowlayout.h"

// Probably don't need all of these Qt headers:
#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <QGroupBox>
#include <qlayout.h>
#include <QScrollArea>
#include <qpushbutton.h>
#include <QTreeWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <qwidget.h>
#include "nifti_reader.h"
#include "brainstru.h"
#include <QStandardItemModel>
#include <QColorDialog>
#include <QSortFilterProxyModel>
#include <QDialogButtonBox>
#include <QHeaderView>

////////////////////////////////////////////////////
//  BrainViewPanel
////////////////////////////////////////////////////
BrainView* BrainViewPanel::bv() 
{
  return (BrainView*) m_dv;
}

void* BrainViewPanel::This() 
{
  return (void*) this;
}

TypeDef* BrainViewPanel::GetTypeDef() const 
{
  return &TA_BrainViewPanel;
}

BrainViewPanel::BrainViewPanel(BrainView* dv_)
  : inherited(dv_)
{
 
  // set a safe default if no BrainViewState yet...
  int max_slices(1);
  Network* net = NULL;
  if (NULL != dv_) {
    max_slices = dv_->MaxSlices();
    net = dv_->net();
  }

  
  int font_spec = taiMisc::fonMedium;
  req_full_render = true;
  req_full_build = false;

  const int min_slider = 80;

  T3ExaminerViewer* vw = dv_->GetViewer();
  if(vw)
    connect(vw, SIGNAL(dynbuttonActivated(int)), this, SLOT(dynbuttonActivated(int)) );
  
  QWidget* widg = new QWidget();
  layTopCtrls = new QVBoxLayout(widg); //layWidg->addLayout(layTopCtrls);
  layTopCtrls->setSpacing(2);
  layTopCtrls->setMargin(2);

  layViewParams = new QVBoxLayout(); layTopCtrls->addLayout(layViewParams);
  layViewParams->setSpacing(2);
  layViewParams->setMargin(0);

  /////////////////////////////////////////////////////////////////////
  QHBoxLayout* bvControls = new QHBoxLayout();  layViewParams->addLayout(bvControls);
  
  QLabel* label = taiM->NewLabel("View\nPlane:", widg, font_spec);
  label->setToolTip("Which of the anatomical planes to view.");
  bvControls->addWidget(label);
  m_view_plane_comb = new QComboBox(widg);
  m_view_plane_comb->addItem("AXIAL");
  m_view_plane_comb->addItem("SAGITTAL");
  m_view_plane_comb->addItem("CORONAL");
  connect(m_view_plane_comb, SIGNAL(currentIndexChanged(int)), this, SLOT(SetViewPlane(int)) );
  connect(this, SIGNAL(ViewPlaneChanged(int)), m_view_plane_comb, SLOT(setCurrentIndex(int)) );
  bvControls->addWidget(m_view_plane_comb);
  bvControls->addSpacing(taiM->hspc_c); 
  
  label = taiM->NewLabel("Starting\nSlice:", widg, font_spec);
  label->setToolTip("The starting slice number to view."); 
  bvControls->addWidget(label);
  m_slice_strt_sbox = new QSpinBox(widg);
  m_slice_strt_sbox->setRange(1,max_slices);
  m_slice_strt_sbox->setValue(1);
  connect(m_slice_strt_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceStart(int)) );
  connect(this, SIGNAL(SliceStartChanged(int)), m_slice_strt_sbox, SLOT(setValue(int)) );
  bvControls->addWidget(m_slice_strt_sbox);
  bvControls->addSpacing(taiM->hsep_c);
  
  m_slice_strt_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_strt_slid->setRange(1, max_slices);
  m_slice_strt_slid->setValue(1);
  m_slice_strt_slid->setMinimumWidth(min_slider);
  connect(m_slice_strt_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceStart(int)) );
  connect(this, SIGNAL(SliceStartChanged(int)), m_slice_strt_slid, SLOT(setValue(int)) );
  bvControls->addWidget(m_slice_strt_slid);
  bvControls->addSpacing(taiM->hspc_c);  
  
  label = taiM->NewLabel("Lock\n#Slices:", widg, font_spec);
  label->setToolTip("Lock slice sliders to maintain number of slices."); 
  bvControls->addWidget(label);
  m_lock_slices_chbox = new QCheckBox(widg);
  m_lock_slices_chbox->setCheckState(Qt::Unchecked);
  connect(m_lock_slices_chbox, SIGNAL(stateChanged(int)), this, SLOT(SetLockSlices(int)) );
  bvControls->addWidget(m_lock_slices_chbox);  
  bvControls->addSpacing(taiM->hspc_c); 
  
  label = taiM->NewLabel("Ending\nSlice:", widg, font_spec);
  label->setToolTip("The ending slice number to view."); 
  bvControls->addWidget(label);
  m_slice_end_sbox = new QSpinBox(widg);
  m_slice_end_sbox->setRange(1, max_slices);
  m_slice_end_sbox->setValue(max_slices);
  connect(this, SIGNAL(SliceEndChanged(int)), m_slice_end_sbox, SLOT(setValue(int)) );
  connect(m_slice_end_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceEnd(int)) );
  bvControls->addWidget(m_slice_end_sbox);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  m_slice_end_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_end_slid->setTickPosition(QSlider::NoTicks);
  m_slice_end_slid->setRange(1, max_slices);
  m_slice_end_slid->setValue(max_slices);
  m_slice_end_slid->setMinimumWidth(min_slider);
  connect(this, SIGNAL(SliceEndChanged(int)), m_slice_end_slid, SLOT(setValue(int)) );
  connect(m_slice_end_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceEnd(int)) );
  bvControls->addWidget(m_slice_end_slid);
  bvControls->addStretch();	// need final stretch to prevent full stretching
  
  bvControls = new QHBoxLayout();  layViewParams->addLayout(bvControls);
  
  const int unit_trans(60);
  label = taiM->NewLabel("Unit Values\nTransparency:", widg, font_spec);
  label->setToolTip("The transparency value of unit values."); 
  bvControls->addWidget(label);
  m_unit_val_tran_sbox = new QSpinBox(widg);
  m_unit_val_tran_sbox->setRange(1, 100);
  m_unit_val_tran_sbox->setValue(unit_trans);
  m_unit_val_tran_sbox->setSuffix("%");
  connect(m_unit_val_tran_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetUnitValuesTransparency(int)) );
  connect(this, SIGNAL(UnitValuesTransparencyChanged(int)), m_unit_val_tran_sbox, SLOT(setValue(int)) );
  bvControls->addWidget(m_unit_val_tran_sbox);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  m_unit_val_tran_slid =  new QSlider(Qt::Horizontal, widg);
  m_unit_val_tran_slid->setTickPosition(QSlider::NoTicks);
  m_unit_val_tran_slid->setRange(1,100);
  m_unit_val_tran_slid->setValue(unit_trans);
  m_unit_val_tran_slid->setMinimumWidth(min_slider);
  connect(m_unit_val_tran_slid, SIGNAL(valueChanged(int)), this, SLOT(SetUnitValuesTransparency(int)) );
  connect(this, SIGNAL(UnitValuesTransparencyChanged(int)), m_unit_val_tran_slid, SLOT(setValue(int)) );
  bvControls->addWidget(m_unit_val_tran_slid);
  bvControls->addSpacing(taiM->hspc_c); 
  
  const int slice_trans(90);
  label = taiM->NewLabel("Slice\nTransparency:", widg, font_spec);
  label->setToolTip("The transparency value of brain slices."); 
  bvControls->addWidget(label);
  m_slice_trans_sbox = new QSpinBox(widg);
  m_slice_trans_sbox->setRange(1, 100);
  m_slice_trans_sbox->setValue(slice_trans);
  m_slice_trans_sbox->setSuffix("%");
  connect(m_slice_trans_sbox, SIGNAL(valueChanged(int)), this, SLOT(SetSliceTransparency(int)) );
  connect(this, SIGNAL(SliceTransparencyChanged(int)), m_slice_trans_sbox, SLOT(setValue(int)) );
  bvControls->addWidget(m_slice_trans_sbox);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  m_slice_tran_slid =  new QSlider(Qt::Horizontal, widg);
  m_slice_tran_slid->setTickPosition(QSlider::NoTicks);
  m_slice_tran_slid->setRange(1,100);
  m_slice_tran_slid->setValue(slice_trans);
  m_slice_tran_slid->setMinimumWidth(min_slider);
  connect(m_slice_tran_slid, SIGNAL(valueChanged(int)), this, SLOT(SetSliceTransparency(int)) );
  connect(this, SIGNAL(SliceTransparencyChanged(int)), m_slice_tran_slid, SLOT(setValue(int)) );
  bvControls->addWidget(m_slice_tran_slid);
  bvControls->addStretch();
  
 
  // the populator for the brain colorization and atlas widgets
  atlas_regexp_pop = new BrainAtlasRegexpPopulator();
  
  ////////////////////////////////////////////////////////////////////////////
  // Brain colorization widgets
  bvControls = new QHBoxLayout();  layViewParams->addLayout(bvControls);  
  label = taiM->NewLabel("Color brain:", widg, font_spec);
  bvControls->addWidget(label);
  m_chk_color_brain = new QCheckBox(widg);
  m_chk_color_brain->setCheckState(Qt::Unchecked);
  m_chk_color_brain->setEnabled(false);
  connect(m_chk_color_brain, SIGNAL(stateChanged(int)), this, SLOT(SetColorBrain(int)) );
  bvControls->addWidget(m_chk_color_brain);  
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Areas (regexp):", widg, font_spec);
  label->setToolTip("Select brain areas using a regular expression (wild card) to color according to their color in the atlas -- use the full regexp .*/.*/.*/.*/.* to color all areas."); 
  bvControls->addWidget(label);

  fldBrainColorRegexp = dl.Add(new taiRegexpField(&TA_taString, this, dynamic_cast<taiData*>(this), widg,0, dynamic_cast<iRegexpDialogPopulator*>(atlas_regexp_pop)));
  fldBrainColorRegexp->SetFieldOwner(net);
  bvControls->addWidget(fldBrainColorRegexp->GetRep());
  ((iLineEdit*)fldBrainColorRegexp->GetRep())->setCharWidth(40);
  connect(fldBrainColorRegexp->rep(), SIGNAL(returnPressed()), this, SLOT(ColorBrainRegexpEdited()));
  bvControls->addSpacing(taiM->hsep_c); 
  bvControls->addSpacing(taiM->hsep_c); 

  butEditAtlas = new QPushButton("Edit Atlas", widg);
  butEditAtlas->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butEditAtlas->setMaximumWidth(taiM->maxButtonWidth() / 2);
  bvControls->addWidget(butEditAtlas);
  connect(butEditAtlas, SIGNAL(pressed()), this, SLOT(butEditAtlas_pressed()) );
  bvControls->addStretch();
  
  ////////////////////////////////////////////////////////////////////////////
  // Brain atlas widgets  
  bvControls = new QHBoxLayout();  layViewParams->addLayout(bvControls);  
  label = taiM->NewLabel("View atlas:", widg, font_spec);
  bvControls->addWidget(label);
  m_chk_atlas = new QCheckBox(widg);
  m_chk_atlas->setCheckState(Qt::Unchecked);
  m_chk_atlas->setEnabled(false);
  connect(m_chk_atlas, SIGNAL(stateChanged(int)), this, SLOT(SetViewAtlas(int)) );
  bvControls->addWidget(m_chk_atlas);  
  bvControls->addSpacing(taiM->hspc_c);

  label = taiM->NewLabel("Atlas label (rexgexp)", widg, font_spec);
  label->setToolTip("Select brain areas to draw in opaque square regions -- the same as the display of unit values -- using a regular expression (wild card) -- works best with a small number of areas, and do NOT select all .*/.*/.*/.*/.* -- very slow");
  bvControls->addWidget(label);

  fldBrainAtlasRegexp = dl.Add(new taiRegexpField(&TA_taString, this, dynamic_cast<taiData*>(this), widg,0, dynamic_cast<iRegexpDialogPopulator*>(atlas_regexp_pop)));
  fldBrainAtlasRegexp->SetFieldOwner(net);
  bvControls->addWidget(fldBrainAtlasRegexp->GetRep());
  ((iLineEdit*)fldBrainAtlasRegexp->GetRep())->setCharWidth(40);
  connect(fldBrainAtlasRegexp->rep(), SIGNAL(returnPressed()), this, SLOT(ViewAtlasRegexpEdited()));
  bvControls->addStretch();
  
  // listen for BrainViewState state changed
  connect(this, SIGNAL(StateChanged(int)), this, SLOT(UpdateViewFromState(int)) );


  layDispCheck = new QHBoxLayout();  layViewParams->addLayout(layDispCheck);
  chkNetText = new QCheckBox("Net\nTxt", widg);
  chkNetText->setToolTip("Turn on the network text display at the base of the network, showing the current state of various counters and stats");
  connect(chkNetText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkNetText);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkLayMove = new QCheckBox("Manip", widg);
  chkLayMove->setToolTip("Turn on the manipulation (moving, scaling, rotating the brain view) controls when in the manipulation mode (red arrow) of viewer");
  connect(chkLayMove, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkLayMove);
  layDispCheck->addSpacing(taiM->hsep_c);
  
  chkAutoScale = new QCheckBox("Auto\nScale", widg);
  chkAutoScale->setToolTip("Automatically scale min and max values of colorscale based on values of variable being displayed");
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkAutoScale);
  layDispCheck->addSpacing(taiM->hsep_c);
  layDispCheck->addStretch();
  
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layColorBar = new QHBoxLayout();  layDisplayValues->addLayout(layColorBar);
  
  butScaleDefault = new QPushButton("Defaults", widg);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butScaleDefault->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layColorBar->addWidget(butScaleDefault);
  layColorBar->addSpacing(taiM->hsep_c); 
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()) );
  
  cbar = new HCScaleBar(&(dv_->scale), ScaleBar::RANGE, true, true, widg);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
  layColorBar->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  layColorBar->addSpacing(taiM->hsep_c); 
  
  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layColorBar->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

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
  connect(lvDisplayValues, SIGNAL(itemSelectionChanged()), this, SLOT(lvDisplayValues_selectionChanged()) );
  
  layTopCtrls->addWidget(tw);
}

BrainViewPanel::~BrainViewPanel() 
{
  BrainView* bv_ = bv();
  if (bv_) {
    bv_->bvp = NULL;
  }
  if (atlas_regexp_pop != NULL){
    delete atlas_regexp_pop;
  }
}

void BrainViewPanel::UpdatePanel_impl() 
{
  inherited::UpdatePanel_impl();
  ++updating;
  BrainView* bv = this->bv(); // cache
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

void BrainViewPanel::UpdateViewFromState(int state) 
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
void BrainViewPanel::SetDataName(const QString& data_name)
{
  bv()->SetDataName(data_name);
}
void BrainViewPanel::SetDimensions(const TDCoord& dimensions)
{
  bv()->SetDimensions(dimensions);
}
void BrainViewPanel::SetViewPlane( int plane )
{
  bv()->SetViewPlane(plane);
}
void BrainViewPanel::SetSliceStart(int start)
{
  bv()->SetSliceStart(start);
}
void BrainViewPanel::SetSliceEnd(int end)
{
  bv()->SetSliceEnd(end);
}
void BrainViewPanel::SetLockSlices(int state)
{
  bv()->SetLockSlices(state);
}
void BrainViewPanel::SetSliceSpacing(int spacing)
{
  bv()->SetSliceSpacing(spacing);
}
void BrainViewPanel::SetSliceTransparency(int transparency)
{
  bv()->SetSliceTransparency(transparency);
}
void BrainViewPanel::SetUnitValuesTransparency(int transparency)
{
  bv()->SetUnitValuesTransparency(transparency);
}
void BrainViewPanel::SetColorBrain(int state)
{
  bv()->SetColorBrain(state);
}
void BrainViewPanel::ColorBrainRegexpEdited()
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
void BrainViewPanel::SetColorBrainRegexp(const QString& regexp)
{
  bv()->SetColorBrainRegexp(regexp);
}

void BrainViewPanel::SetViewAtlas(int state)
{
  bv()->SetViewAtlas(state);
}

void BrainViewPanel::ViewAtlasRegexpEdited()
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
void BrainViewPanel::SetViewAtlasRegexp(const QString& regexp)
{
  bv()->SetViewAtlasRegexp(regexp);
}
void BrainViewPanel::EmitDataNameChanged(const QString& name)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit DataNameChanged(name);
}
void BrainViewPanel::EmitDimensionsChanged(const TDCoord& d)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit DimensionsChanged(d);
}
void BrainViewPanel::EmitViewPlaneChanged(int plane)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit ViewPlaneChanged(plane);
}
void BrainViewPanel::EmitNumSlicesChanged(int nSlices)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit NumSlicesChanged(nSlices);
}
void BrainViewPanel::EmitSliceStartChanged(int start)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit SliceStartChanged(start);
}
void BrainViewPanel::EmitSliceEndChanged(int end)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit SliceEndChanged(end);
}
void BrainViewPanel::EmitSliceSpacingChanged(int spacing)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit SliceSpacingChanged(spacing);
}
void BrainViewPanel::EmitSliceTransparencyChanged(int transparency)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit SliceTransparencyChanged(transparency);
}
void BrainViewPanel::EmitUnitValuesTransparencyChanged(int transparency)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit UnitValuesTransparencyChanged(transparency);
}
void BrainViewPanel::EmitStateChanged(int state)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit StateChanged(state);
}
void BrainViewPanel::EmitColorBrainAreaRegexpChanged(const QString& regexp)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit BrainColorRegexpChanged(regexp);
}
void BrainViewPanel::EmitViewAtlasRegexpChanged(const QString& regexp)
{
  // calling this method allows us to raise a signal, notifying 
  // any widgets that may be connected to it
  emit ViewAtlasRegexpChanged(regexp);
}

void BrainViewPanel::GetValue_impl() {
  inherited::GetValue_impl();
  BrainView* bv = this->bv(); // cache
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
  
  if (false == req_full_render) {
    // just need update, not full rebuild/render
    bv->AsyncRenderUpdate();
  }
  bv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void BrainViewPanel::CopyFrom_impl() 
{
  BrainView* bv_; if (!(bv_ = bv())) return;
  bv_->CallFun("CopyFromView");
}

void BrainViewPanel::butScaleDefault_pressed() 
{
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  
  bv_->SetScaleDefault();
  bv_->UpdateDisplay(true);
}

void BrainViewPanel::butEditAtlas_pressed() 
{
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  Network* net = bv_->net();
  if(!net || !net->brain_atlas) return;
  net->brain_atlas->EditAtlas();
  bv_->UpdateDisplay(true);
}

void BrainViewPanel::butSetColor_pressed() 
{
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  bv_->CallFun("SetColorSpec");
}

void BrainViewPanel::ColorScaleFromData() 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  ++updating;
  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(bv_->scale.auto_scale); //note: raises signal on widget! (grr...)
  --updating;
}

void BrainViewPanel::GetVars() 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  lvDisplayValues->clear();
  if (bv_->membs.size == 0) return;

  MemberDef* md;
  QTreeWidgetItem* lvi = NULL;
  for (int i=0; i < bv_->membs.size; i++) {
    md = bv_->membs[i];
    QStringList itm;
    itm << md->name << md->desc;
    lvi = new QTreeWidgetItem(lvDisplayValues, itm);
  }
  lvDisplayValues->resizeColumnToContents(0);
}

void BrainViewPanel::InitPanel() 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  ++updating;
  // fill monitor values
  GetVars();
  --updating;
}

void BrainViewPanel::lvDisplayValues_selectionChanged() 
{
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  // redo the list each time, to guard against stale values
  bv_->cur_unit_vals.Reset(); 
  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
  QTreeWidgetItem* item = NULL;
  for (int j = 0; j < items.size(); ++j) {
    item = items.at(j);
    bv_->cur_unit_vals.Add(item->text(0));
  }
  MemberDef* md = (MemberDef*)bv_->membs.FindName(bv_->cur_unit_vals.SafeEl(0));
  if (md) {
    bv_->setUnitDispMd(md); 
    bv_->UpdateViewerModeForMd(md);
  }
  ColorScaleFromData();
  bv_->UpdateDisplay(false);
}

void BrainViewPanel::dynbuttonActivated(int but_no) 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  T3ExaminerViewer* vw = bv_->GetViewer();
  if(!vw) return;
  taiAction* dyb = vw->getDynButton(but_no);
  if(!dyb) return;
  String nm = dyb->text();
  bv_->cur_unit_vals.Reset(); 
  bv_->cur_unit_vals.Add(nm);
  MemberDef* md = (MemberDef*)bv_->membs.FindName(nm);
  if(md) {
    bv_->setUnitDispMd(md); 
    bv_->UpdateViewerModeForMd(md);
    vw->setDynButtonChecked(but_no, true, true); // mutex
  }
  ColorScaleFromData();
  bv_->UpdateDisplay(true);     // update panel
}

void BrainViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  bv_->viewWin_NotifySignal(src, op);
}

void BrainViewPanel::UpdateWidgetLimits() 
{
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  
  int max_slices(bv_->MaxSlices());
  int num_slices(bv_->NumSlices());
  
  // if user wants number of slices to remain fixed
  // we need to be sure limits are adjusted appropriately
  if (bv_->NumSlicesAreLocked()) { 
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


//////////////////////////////////////////////
//		iNetwork_Group

iBrainViewEditDialog::iBrainViewEditDialog(taiRegexpField* regexp_field, const String& field_name, iRegexpDialogPopulator *re_populator, const void *fieldOwner, bool read_only, bool editor_mode)
  : inherited(regexp_field, field_name, re_populator, fieldOwner, read_only, editor_mode) {

  AddButtons();
  SetColors();

  connect(m_table_view, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(itemClicked(const QModelIndex&)));
}

void iBrainViewEditDialog::SetColors() {
  BrainAtlasRegexpPopulator* bepop = (BrainAtlasRegexpPopulator*)m_populator;

  QList<QColor>	clrs = bepop->getColors();
  int rows = clrs.size();

  int col = m_num_parts + NUM_EXTRA_COLS;
  for (int row = 0; row < rows; ++row) {
    QColor clr = clrs[row];
    QStandardItem* item = new QStandardItem(clr.name());
    item->setBackground(QBrush(clr));
    item->setEditable(false);
    m_table_model->setItem(row, col, item);
  }

  m_table_view->horizontalHeader()->setToolTip("Double-click on Color to edit with a color editor dialog");
}

void iBrainViewEditDialog::itemClicked(const QModelIndex & index) {
  QModelIndex src_idx = m_proxy_model->mapToSource(index);
  int row = src_idx.row();
  int col = src_idx.column();
  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  if(col != clr_col) return;
  QStandardItem* item = m_table_model->item(row, clr_col);
  QColor clr(item->text());
  QColor nwclr = QColorDialog::getColor(clr, this);
  item->setText(nwclr.name());
  item->setBackground(QBrush(nwclr));
}

void iBrainViewEditDialog::btnApply_clicked() {
  inherited::btnApply_clicked();
  if(!m_editor_mode) return;

  BrainAtlasRegexpPopulator* bepop = (BrainAtlasRegexpPopulator*)m_populator;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;

  QList<QColor> colors;
  int rows = m_table_model->rowCount();
  for(int row=0; row < rows; ++row) {
    QStandardItem* item = m_table_model->item(row, clr_col);
    QColor clr(item->text());
    colors.append(clr);
  }
  bepop->setColors(colors);
}

void iBrainViewEditDialog::AddButtons() {
  if(!m_editor_mode) return;

  QPushButton* btnRandomColors = m_button_box->addButton("RandomColors",
							 QDialogButtonBox::ActionRole);
  btnRandomColors->setToolTip("generates random colors for the currently-selected items (according to the current filter) from a selected color scale");

  QPushButton* btnColorsFromScale = m_button_box->addButton("ColorsFromScale",
							    QDialogButtonBox::ActionRole);
  btnColorsFromScale->setToolTip("generates colors for the currently-selected items (according to the current filter) from a selected color scale");

  // Connect the button-box buttons to our SLOTs.
  connect(btnRandomColors, SIGNAL(clicked()), this, SLOT(btnRandomColors_clicked()));
  connect(btnColorsFromScale, SIGNAL(clicked()), this, SLOT(btnColorsFromScale_clicked()));
}

void iBrainViewEditDialog::btnColorsFromScale_clicked() {
  taiObjChooser* chs = taiObjChooser::createInstance(&TA_ColorScaleSpec,
						     "select a colorscale to apply to the currently-selected labels");
  bool rval = chs->Choose();
  if(!rval) return;
  ColorScaleSpec* cspec = (ColorScaleSpec*)chs->sel_obj();
  delete chs;
  if(!cspec) return;

  int rows = m_proxy_model->rowCount();
  if(rows == 0) return;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  TAColor_List cls;
  int extra = 0;
  do {				// it doesn't always generate enough..
    cspec->GenRanges(&cls, rows+extra);
    extra += 2;
  } while (cls.size < rows);

  m_table_model->blockSignals(true);
  for(int row = 0; row < rows; row++) {
    QModelIndex pidx = m_proxy_model->index(row, clr_col);
    QModelIndex sidx = m_proxy_model->mapToSource(pidx);
    QStandardItem* itm = m_table_model->item(sidx.row(), clr_col);
    String rgb = String("#") + cls[row]->color().toString();
    itm->setText(rgb);
    itm->setBackground(QBrush(QColor(QString(rgb.chars()))));
  }
  m_table_model->blockSignals(false);
  m_table_view->viewport()->update();
}

void iBrainViewEditDialog::btnRandomColors_clicked() {
  taiObjChooser* chs = taiObjChooser::createInstance(&TA_ColorScaleSpec,
						     "select a colorscale to select random colors from -- Rainbow is generally a good choice");
  bool rval = chs->Choose();
  if(!rval) return;
  ColorScaleSpec* cspec = (ColorScaleSpec*)chs->sel_obj();
  delete chs;
  if(!cspec) return;

  int rows = m_proxy_model->rowCount();
  if(rows == 0) return;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  TAColor_List cls;
  int extra = 0;
  do {				// it doesn't always generate enough..
    cspec->GenRanges(&cls, rows+extra);
    extra += 2;
  } while (cls.size < rows);
  int_Array prmt;
  prmt.SetSize(rows);
  prmt.FillSeq();
  prmt.Permute();

  m_table_model->blockSignals(true);
  for(int row = 0; row < rows; row++) {
    QModelIndex pidx = m_proxy_model->index(row, clr_col);
    QModelIndex sidx = m_proxy_model->mapToSource(pidx);
    QStandardItem* itm = m_table_model->item(sidx.row(), clr_col);
    TAColor* clr = cls[prmt[row]];
    String rgb = String("#") + clr->color().toString();
    itm->setText(rgb);
    itm->setBackground(QBrush(QColor(QString(rgb.chars()))));
  }
  m_table_model->blockSignals(false);
  m_table_view->viewport()->update();
}
