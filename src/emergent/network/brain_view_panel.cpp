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
 
  BrainViewState* bvs(&(dv_->bv_state));
  // set a safe default if no BrainViewState yet...
  int max_slices(1);
  if (NULL != bvs) {
    max_slices = bvs->MaxSlices();
  }
  
  int font_spec = taiMisc::fonMedium;
  req_full_render = false;
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
  //viewPlane_ = dl.Add(new taiComboBox(true, TA_BrainView.sub_types.FindName("BrainPlaneView"),this, NULL, widg, taiData::flgAutoApply));
  view_plane_comb_ = new QComboBox(widg);
  view_plane_comb_->addItem("AXIAL");
  view_plane_comb_->addItem("SAGITTAL");
  view_plane_comb_->addItem("CORONAL");
  connect(view_plane_comb_, SIGNAL(currentIndexChanged(int)), bvs, SLOT(SetViewPlane(int)) );
  connect(bvs, SIGNAL(ViewPlaneChanged(int)), view_plane_comb_, SLOT(setCurrentIndex(int)) );
  bvControls->addWidget(view_plane_comb_);
  bvControls->addSpacing(taiM->hspc_c); 
  
  label = taiM->NewLabel("Starting\nSlice:", widg, font_spec);
  label->setToolTip("The starting slice number to view."); 
  bvControls->addWidget(label);
  slice_strt_sbox_ = new QSpinBox(widg);
  slice_strt_sbox_->setRange(1,max_slices);
  slice_strt_sbox_->setValue(1);
  connect(slice_strt_sbox_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceStart(int)) );
  connect(bvs, SIGNAL(SliceStartChanged(int)), slice_strt_sbox_, SLOT(setValue(int)) );
  bvControls->addWidget(slice_strt_sbox_);
  bvControls->addSpacing(taiM->hsep_c);
  
  slice_strt_slid_ =  new QSlider(Qt::Horizontal, widg);
  slice_strt_slid_->setRange(1, max_slices);
  slice_strt_slid_->setValue(1);
  slice_strt_slid_->setMinimumWidth(min_slider);
  connect(slice_strt_slid_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceStart(int)) );
  connect(bvs, SIGNAL(SliceStartChanged(int)), slice_strt_slid_, SLOT(setValue(int)) );
  bvControls->addWidget(slice_strt_slid_);
  bvControls->addSpacing(taiM->hspc_c);  
  
  label = taiM->NewLabel("Lock\n#Slices:", widg, font_spec);
  label->setToolTip("Lock slice sliders to maintain number of slices."); 
  bvControls->addWidget(label);
  lock_slices_chbox_ = new QCheckBox(widg);
  lock_slices_chbox_->setCheckState(Qt::Unchecked);
  connect(lock_slices_chbox_, SIGNAL(stateChanged(int)), bvs, SLOT(SetLockSlices(int)) );
  bvControls->addWidget(lock_slices_chbox_);  
  bvControls->addSpacing(taiM->hspc_c); 
  
  label = taiM->NewLabel("Ending\nSlice:", widg, font_spec);
  label->setToolTip("The ending slice number to view."); 
  bvControls->addWidget(label);
  slice_end_sbox_ = new QSpinBox(widg);
  slice_end_sbox_->setRange(1, max_slices);
  slice_end_sbox_->setValue(max_slices);
  connect(bvs, SIGNAL(SliceEndChanged(int)), slice_end_sbox_, SLOT(setValue(int)) );
  connect(slice_end_sbox_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceEnd(int)) );
  bvControls->addWidget(slice_end_sbox_);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  slice_end_slid_ =  new QSlider(Qt::Horizontal, widg);
  slice_end_slid_->setTickPosition(QSlider::NoTicks);
  slice_end_slid_->setRange(1, max_slices);
  slice_end_slid_->setValue(max_slices);
  slice_end_slid_->setMinimumWidth(min_slider);
  connect(bvs, SIGNAL(SliceEndChanged(int)), slice_end_slid_, SLOT(setValue(int)) );
  connect(slice_end_slid_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceEnd(int)) );
  bvControls->addWidget(slice_end_slid_);
  bvControls->addStretch();	// need final stretch to prevent full stretching
  
  bvControls = new QHBoxLayout();  layViewParams->addLayout(bvControls);
  
  label = taiM->NewLabel("Unit Values\nTransparency:", widg, font_spec);
  label->setToolTip("The transparency value of unit values."); 
  bvControls->addWidget(label);
  unit_val_tran_sbox_ = new QSpinBox(widg);
  unit_val_tran_sbox_->setRange(1, 100);
  unit_val_tran_sbox_->setValue(98 );
  unit_val_tran_sbox_->setSuffix("%");
  connect(unit_val_tran_sbox_, SIGNAL(valueChanged(int)), bvs, SLOT(SetUnitValuesTransparency(int)) );
  connect(bvs, SIGNAL(UnitValuesTransparencyChanged(int)), unit_val_tran_sbox_, SLOT(setValue(int)) );
  bvControls->addWidget(unit_val_tran_sbox_);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  unit_val_tran_slid_ =  new QSlider(Qt::Horizontal, widg);
  unit_val_tran_slid_->setTickPosition(QSlider::NoTicks);
  unit_val_tran_slid_->setRange(1,100);
  unit_val_tran_slid_->setValue(98);
  unit_val_tran_slid_->setMinimumWidth(min_slider);
  connect(unit_val_tran_slid_, SIGNAL(valueChanged(int)), bvs, SLOT(SetUnitValuesTransparency(int)) );
  connect(bvs, SIGNAL(UnitValuesTransparencyChanged(int)), unit_val_tran_slid_, SLOT(setValue(int)) );
  bvControls->addWidget(unit_val_tran_slid_);
  bvControls->addSpacing(taiM->hspc_c); 
  
  label = taiM->NewLabel("Slice\nTransparency:", widg, font_spec);
  label->setToolTip("The transparency value of brain slices."); 
  bvControls->addWidget(label);
  slice_trans_sbox_ = new QSpinBox(widg);
  slice_trans_sbox_->setRange(1, 100);
  slice_trans_sbox_->setValue(98 );
  slice_trans_sbox_->setSuffix("%");
  connect(slice_trans_sbox_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceTransparency(int)) );
  connect(bvs, SIGNAL(SliceTransparencyChanged(int)), slice_trans_sbox_, SLOT(setValue(int)) );
  bvControls->addWidget(slice_trans_sbox_);  
  bvControls->addSpacing(taiM->hsep_c); 
  
  slice_tran_slid_ =  new QSlider(Qt::Horizontal, widg);
  slice_tran_slid_->setTickPosition(QSlider::NoTicks);
  slice_tran_slid_->setRange(1,100);
  slice_tran_slid_->setValue(98);
  slice_tran_slid_->setMinimumWidth(min_slider);
  connect(slice_tran_slid_, SIGNAL(valueChanged(int)), bvs, SLOT(SetSliceTransparency(int)) );
  connect(bvs, SIGNAL(SliceTransparencyChanged(int)), slice_tran_slid_, SLOT(setValue(int)) );
  bvControls->addWidget(slice_tran_slid_);
  bvControls->addStretch(); 
   
    // listen for BrainViewState state changed
  connect(bvs, SIGNAL(StateChanged(int)), this, SLOT(UpdateViewFromState(int)) );
  

  ////////////////////////////////////////////////////////////////////////////
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layColorBar = new QHBoxLayout();  layDisplayValues->addLayout(layColorBar);

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
  layTopCtrls->addStretch();

  //MakeButtons(layOuter);
}

BrainViewPanel::~BrainViewPanel() 
{
  BrainView* bv_ = bv();
  if (bv_) {
    bv_->bvp = NULL;
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
  
  unit_val_tran_sbox_->setValue((int)(bv->view_params.unit_trans*100));
  ColorScaleFromData();
  
  --updating;
}

void BrainViewPanel::UpdateViewFromState(int state) 
{
  // BrainViewState has changed...determine if full render
  // is required and make call to apply the GUI change

  if (req_full_render == true) return; // already have an update to do
  if (state & BrainViewState::MAJOR) {
    req_full_render = true;
  }
  else {
    req_full_render = false;
  }
  Apply_Async();
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
  //req_full_render = true; // everything requires a re-render
  
  bv->view_params.unit_trans = ((float)(unit_val_tran_sbox_->value())/100.0f);

  if (false == req_full_render) {
    // just need update, not full rebuild/render
    bv->AsyncRenderUpdate();
  }
}

void BrainViewPanel::CopyFrom_impl() 
{
  BrainView* bv_; if (!(bv_ = bv())) return;
  bv_->CallFun("CopyFromView");
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
  --updating;
}

void BrainViewPanel::GetVars() 
{
//  BrainView* bv_;
//  if (!(bv_ = bv())) return;
//
//  lvDisplayValues->clear();
//  if (bv_->membs.size == 0) return;
//
//  MemberDef* md;
//  QTreeWidgetItem* lvi = NULL;
//  for (int i=0; i < bv_->membs.size; i++) {
//    md = bv_->membs[i];
//    QStringList itm;
//    itm << md->name << md->desc;
//    lvi = new QTreeWidgetItem(lvDisplayValues, itm);
//  }
//  lvDisplayValues->resizeColumnToContents(0);
}

void BrainViewPanel::InitPanel() 
{
//  BrainView* bv_;
//  if (!(bv_ = bv())) return;
//  ++updating;
//  // fill monitor values
////  GetVars();
//  --updating;
}

void BrainViewPanel::lvDisplayValues_selectionChanged() 
{
//  if (updating) return;
//  BrainView* bv_;
//  if (!(bv_ = bv())) return;
//  // redo the list each time, to guard against stale values
//  bv_->cur_unit_vals.Reset(); 
//  QList<QTreeWidgetItem*> items(lvDisplayValues->selectedItems());
//  QTreeWidgetItem* item = NULL;
//  for (int j = 0; j < items.size(); ++j) {
//    item = items.at(j);
//    bv_->cur_unit_vals.Add(item->text(0));
//  }
//  MemberDef* md = (MemberDef*)bv_->membs.FindName(bv_->cur_unit_vals.SafeEl(0));
//  if (md) {
//    bv_->setUnitDispMd(md); 
//    bv_->UpdateViewerModeForMd(md);
//  }
//  ColorScaleFromData();
//  bv_->UpdateDisplay(false);
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
  BrainViewState& bvs = bv()->bv_state;
  int max_slices(bvs.MaxSlices());
  int num_slices(bvs.NumSlices());
  
  // if user wants number of slices to remain fixed
  // we need to be sure limits are adjusted appropriately
  if (bvs.NumSlicesAreLocked()) { 
    slice_strt_sbox_->setRange(1, max_slices - num_slices);
    slice_strt_slid_->setRange(1, max_slices - num_slices);
    slice_end_sbox_->setRange(num_slices + 1, max_slices);
    slice_end_slid_->setRange(num_slices + 1, max_slices);
  }
  else {
    slice_strt_sbox_->setRange(1, max_slices);
    slice_strt_slid_->setRange(1, max_slices);
    slice_end_sbox_->setRange(1, max_slices);    
    slice_end_slid_->setRange(1, max_slices);
  }
}

