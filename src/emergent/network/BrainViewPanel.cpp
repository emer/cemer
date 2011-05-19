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

#include "BrainViewPanel.h"
#include "BrainView.h"
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
#include <qwidget.h>

BrainView*
BrainViewPanel::bv() {
  return (BrainView*) m_dv;
}

void*
BrainViewPanel::This() {
  return (void*) this;
}

TypeDef*
BrainViewPanel::GetTypeDef() const {
  return &TA_BrainViewPanel;
}

BrainViewPanel::BrainViewPanel(BrainView* dv_)
  : inherited(dv_)
{
  int font_spec = taiMisc::fonMedium;
  m_cur_spec = NULL;
  req_full_render = false;
  req_full_build = false;

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

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout();  layViewParams->addLayout(layDispCheck);
  chkDisplay = new QCheckBox("Display", widg);
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkLayMove = new QCheckBox("Lay\nMv", widg);
  chkLayMove->setToolTip("Turn on the layer moving controls when in the manipulation mode (red arrow) of viewer -- these can sometimes interfere with viewing weights, so you can turn them off here (but then you won't be able to move layers around in the GUI)");
  connect(chkLayMove, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkLayMove);
  layDispCheck->addSpacing(taiM->hsep_c);

  chkNetText = new QCheckBox("Net\nTxt", widg);
  chkNetText->setToolTip("Turn on the network text display at the base of the network, showing the current state of various counters and stats");
  connect(chkNetText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkNetText);
  layDispCheck->addSpacing(taiM->hsep_c);

  lblTextRot = taiM->NewLabel("Txt\nRot", widg, font_spec);
  lblTextRot->setToolTip("Rotation of the network text in the Z axis -- set to -90 if text overall is rotated upright in the display");
  layDispCheck->addWidget(lblTextRot);
  fldTextRot = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layDispCheck->addWidget(fldTextRot->GetRep());
  ((iLineEdit*)fldTextRot->GetRep())->setCharWidth(6);
  layDispCheck->addSpacing(taiM->hsep_c);

  lblUnitText = taiM->NewLabel("Unit:\nText", widg, font_spec);
  lblUnitText->setToolTip("What text to display for each unit (values, names)");
  layDispCheck->addWidget(lblUnitText);
  cmbUnitText = dl.Add(new taiComboBox(true, TA_BrainView.sub_types.FindName("UnitTextDisplay"),
                                this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbUnitText->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);

  lblDispMode = taiM->NewLabel("Style", widg, font_spec);
  lblDispMode->setToolTip("How to display unit values.  3d Block (default) is optimized\n\
 for maximum speed.");
  layDispCheck->addWidget(lblDispMode);
  cmbDispMode = dl.Add(new taiComboBox(true, TA_BrainView.sub_types.FindName("UnitDisplayMode"),
    this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbDispMode->GetRep());
  layDispCheck->addSpacing(taiM->hsep_c);
  
  lblPrjnDisp = taiM->NewLabel("Prjn\nDisp", widg, font_spec);
  lblPrjnDisp->setToolTip("How to display projections between layers:\n\
L_R_F: Left = sender, Right = receiver, all arrows at the Front of the layer\n\
L_R_B: Left = sender, Right = receiver, all arrows at the Back of the layer\n\
B_F: Back = sender, Front = receiver, all arrows in the middle of the layer");
  layDispCheck->addWidget(lblPrjnDisp);
  cmbPrjnDisp = dl.Add(new taiComboBox(true, TA_NetViewParams.sub_types.FindName("PrjnDisp"),
                                this, NULL, widg, taiData::flgAutoApply));
  layDispCheck->addWidget(cmbPrjnDisp->GetRep());
  layDispCheck->addStretch();
  
  ////////////////////////////////////////////////////////////////////////////
  layFontsEtc = new QHBoxLayout();  layViewParams->addLayout(layFontsEtc);

  lblUnitTrans = taiM->NewLabel("Trans\nparency", widg, font_spec);
  lblUnitTrans->setToolTip("Unit maximum transparency level: 0 = all units opaque; 1 = inactive units are completely invisible.\n .6 = default; transparency is inversely related to value magnitude.");
  layFontsEtc->addWidget(lblUnitTrans);
  fldUnitTrans = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitTrans->GetRep());
  ((iLineEdit*)fldUnitTrans->GetRep())->setCharWidth(6);  layFontsEtc->addSpacing(taiM->hsep_c);

  lblUnitFont = taiM->NewLabel("Font\nSize", widg, font_spec);
  lblUnitFont->setToolTip("Unit text font size (as a proportion of entire network display). .02 is default.");
  layFontsEtc->addWidget(lblUnitFont);
  fldUnitFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldUnitFont->GetRep());
  ((iLineEdit*)fldUnitFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblLayFont = taiM->NewLabel("Layer\nFont Sz", widg, font_spec);
  lblLayFont->setToolTip("Layer name font size (as a proportion of entire network display). .04 is default.");
  layFontsEtc->addWidget(lblLayFont);
  fldLayFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldLayFont->GetRep());
  ((iLineEdit*)fldLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  lblMinLayFont = taiM->NewLabel("Min Sz", widg, font_spec);
  lblMinLayFont->setToolTip("Minimum layer name font size (as a proportion of entire network display) -- prevents font from shrinking too small for small layers. .01 is default.");
  layFontsEtc->addWidget(lblMinLayFont);
  fldMinLayFont = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layFontsEtc->addWidget(fldMinLayFont->GetRep());
  ((iLineEdit*)fldMinLayFont->GetRep())->setCharWidth(6);
  layFontsEtc->addSpacing(taiM->hsep_c);

  chkXYSquare = new QCheckBox("XY\nSquare", widg);
  chkXYSquare->setToolTip("Make the X and Y size of network the same, so that unit cubes are always square (but can waste a certain amount of display space).");
  connect(chkXYSquare, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkXYSquare);

  chkLayGp = new QCheckBox("Lay\nGp", widg);
  chkLayGp->setToolTip("Display boxes around layer groups.");
  connect(chkLayGp, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layFontsEtc->addWidget(chkLayGp);

  layFontsEtc->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layDisplayValues = new QVBoxLayout();  layTopCtrls->addLayout(layDisplayValues); //gbDisplayValues);
  layDisplayValues->setSpacing(2);
  layDisplayValues->setMargin(0);

  layColorScaleCtrls = new QHBoxLayout();  layDisplayValues->addLayout(layColorScaleCtrls);
  
  lblUnitSpacing = taiM->NewLabel("Unit\nSpace", widg, font_spec);
  lblUnitSpacing->setToolTip("Spacing between units, as a proportion of the total width of the unit box"); 
  layColorScaleCtrls->addWidget(lblUnitSpacing);
  fldUnitSpacing = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layColorScaleCtrls->addWidget(fldUnitSpacing->GetRep());
  ((iLineEdit*)fldUnitSpacing->GetRep())->setCharWidth(6);
  layColorScaleCtrls->addSpacing(taiM->hsep_c);


  layColorScaleCtrls->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layColorBar = new QHBoxLayout();  layDisplayValues->addLayout(layColorBar);

  chkAutoScale = new QCheckBox("Auto\nScale", widg);
  chkAutoScale->setToolTip("Automatically scale min and max values of colorscale based on values of variable being displayed");
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorBar->addWidget(chkAutoScale);

  butScaleDefault = new QPushButton("Defaults", widg);
  butScaleDefault->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butScaleDefault->setMaximumWidth(taiM->maxButtonWidth() / 2);
  layColorBar->addWidget(butScaleDefault);
  connect(butScaleDefault, SIGNAL(pressed()), this, SLOT(butScaleDefault_pressed()) );
  
  cbar = new HCScaleBar(&(dv_->scale), ScaleBar::RANGE, true, true, widg);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
  layColorBar->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  
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
  //layDisplayValues->addWidget(lvDisplayValues, 1);
  connect(lvDisplayValues, SIGNAL(itemSelectionChanged()), this, SLOT(lvDisplayValues_selectionChanged()) );
  
  layTopCtrls->addWidget(tw);
  
  ////////////////////////////////////////////////////////////////////////////
  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
  layOuter->addWidget(widCmdButtons);
  
  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons); 
  meth_but_mgr->Constr(bv()->net());

  MakeButtons(layOuter);
}

BrainViewPanel::~BrainViewPanel() {
  BrainView* bv_ = bv();
  if (bv_) {
    bv_->bvp = NULL;
  }
}

void BrainViewPanel::UpdatePanel_impl() {
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
  
  chkDisplay->setChecked(bv->display);
  chkLayMove->setChecked(bv->lay_mv);
  chkNetText->setChecked(bv->net_text);
  fldTextRot->GetImage((String)bv->net_text_rot);
  cmbUnitText->GetEnumImage(bv->unit_text_disp);
  cmbDispMode->GetEnumImage(bv->unit_disp_mode);
  cmbPrjnDisp->GetEnumImage(bv->view_params.prjn_disp);
  fldPrjnWdth->GetImage((String)bv->view_params.prjn_width);

  chkSnapBord->setChecked(bv->snap_bord_disp);
  fldSnapBordWdth->GetImage((String)bv->snap_bord_width);
  fldUnitSpacing->GetImage((String)bv->view_params.unit_spacing);

  chkWtLines->setChecked(bv->wt_line_disp);
  chkWtLineSwt->setChecked(bv->wt_line_swt);
  fldWtLineWdth->GetImage((String)bv->wt_line_width);
  fldWtLineThr->GetImage((String)bv->wt_line_thr);
  fldWtPrjnKUn->GetImage((String)bv->wt_prjn_k_un);
  fldWtPrjnKGp->GetImage((String)bv->wt_prjn_k_gp);
  gelWtPrjnLay->GetImage(&(bv->net()->layers), bv->wt_prjn_lay.ptr());

  fldUnitTrans->GetImage((String)bv->view_params.unit_trans);
  fldUnitFont->GetImage((String)bv->font_sizes.unit);
  fldLayFont->GetImage((String)bv->font_sizes.layer);
  fldMinLayFont->GetImage((String)bv->font_sizes.layer_min);
  chkXYSquare->setChecked(bv->view_params.xy_square);
  chkLayGp->setChecked(bv->view_params.show_laygp);

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
  // spec highlighting
  BaseSpec* cspc = m_cur_spec; // to see if it changes, if not, we force redisplay
  iTreeViewItem* tvi = dynamic_cast<iTreeViewItem*>(tvSpecs->currentItem());
  tvSpecs_ItemSelected(tvi); // manually invoke slot
  if (cspc == m_cur_spec)
    setHighlightSpec(m_cur_spec, true);

  ColorScaleFromData();
  --updating;
}

void BrainViewPanel::GetValue_impl() {
  inherited::GetValue_impl();
  BrainView* bv = this->bv(); // cache
  if (!bv) return;
  req_full_render = true;       // everything requires a re-render
  req_full_build = false;

  bv->display = chkDisplay->isChecked();
  bv->lay_mv = chkLayMove->isChecked();
  bv->net_text = chkNetText->isChecked();
  bv->net_text_rot = (float)fldTextRot->GetValue();

  int i; 
  cmbUnitText->GetEnumValue(i);
  bv->unit_text_disp = (BrainView::UnitTextDisplay)i;

  // unit disp mode is only guy requiring full build!
  cmbDispMode->GetEnumValue(i);
  req_full_build = req_full_build || (bv->unit_disp_mode != i);
  bv->unit_disp_mode = (BrainView::UnitDisplayMode)i;
  
  cmbPrjnDisp->GetEnumValue(i);
  bv->view_params.prjn_disp = (NetViewParams::PrjnDisp)i;
  
  bv->view_params.prjn_width = (float)fldPrjnWdth->GetValue();

  bv->view_params.unit_trans = (float)fldUnitTrans->GetValue();
  bv->font_sizes.unit = (float)fldUnitFont->GetValue();
  bv->font_sizes.layer = (float)fldLayFont->GetValue();
  bv->font_sizes.layer_min = (float)fldMinLayFont->GetValue();

  bv->snap_bord_disp = chkSnapBord->isChecked();
  bv->snap_bord_width = (float)fldSnapBordWdth->GetValue();
  bv->view_params.unit_spacing = (float)fldUnitSpacing->GetValue();

  bv->view_params.xy_square = chkXYSquare->isChecked();
  bv->view_params.show_laygp = chkLayGp->isChecked();

  bv->SetScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max(), false);
}

void BrainViewPanel::CopyFrom_impl() {
  BrainView* bv_; if (!(bv_ = bv())) return;
  bv_->CallFun("CopyFromView");
}

void BrainViewPanel::butScaleDefault_pressed() {
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  bv_->SetScaleDefault();
  bv_->UpdateDisplay(true);
}

void BrainViewPanel::butSetColor_pressed() {
  if (updating) return;
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  bv_->CallFun("SetColorSpec");
}

void BrainViewPanel::ColorScaleFromData() {
  BrainView* bv_;
  if (!(bv_ = bv())) return;

  ++updating;
  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(bv_->scale.auto_scale); //note: raises signal on widget! (grr...)
  --updating;
}

void BrainViewPanel::GetVars() {
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

void BrainViewPanel::InitPanel() {
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  ++updating;
  // fill monitor values
  GetVars();
  --updating;
}

void BrainViewPanel::lvDisplayValues_selectionChanged() {
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

void BrainViewPanel::viewWin_NotifySignal(ISelectableHost* src, int op) {
  BrainView* bv_;
  if (!(bv_ = bv())) return;
  bv_->viewWin_NotifySignal(src, op);
}

void BrainViewPanel::dynbuttonActivated(int but_no) {
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
