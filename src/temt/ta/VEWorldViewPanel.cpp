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

#include "VEWorldViewPanel.h"
#include <VEWorld>
#include <VEWorldView>
#include <taiField>
#include <iFlowLayout>
#include <iMethodButtonMgr>
#include <BuiltinTypeDefs>

#include <taMisc>
#include <taiMisc>


#include <QVBoxLayout>
#include <QCheckBox>


VEWorldViewPanel::VEWorldViewPanel(VEWorldView* dv_)
:inherited(dv_)
{
  req_full_redraw = false;

  int font_spec = taiMisc::fonMedium;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vsep_c);

  layDispCheck = new QHBoxLayout; layOuter->addLayout(layDispCheck);
  chkDisplay = new QCheckBox("Display", widg);
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDisplay);
  layDispCheck->addSpacing(taiM->hspc_c);

  chkDragObjs = new QCheckBox("Drag Objs", widg);
  connect(chkDragObjs, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkDragObjs);
  layDispCheck->addSpacing(taiM->hspc_c);

  chkShowJoints = new QCheckBox("Show Joints", widg);
  connect(chkShowJoints, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layDispCheck->addWidget(chkShowJoints);
  layDispCheck->addSpacing(taiM->hspc_c);

  lblDragSize = taiM->NewLabel("Drag Size: ", widg, font_spec);
  lblDragSize->setToolTip("Size of the 'dragger' controls for manipulating the position, size, and rotation of objects.");
  layDispCheck->addWidget(lblDragSize);
  fldDragSize = dl.Add(new taiField(&TA_float, this, NULL, widg));
  layDispCheck->addWidget(fldDragSize->GetRep());
  //  layDispCheck->addSpacing(taiM->hspc_c);

  layDispCheck->addStretch();

  layCams = new QHBoxLayout; layOuter->addLayout(layCams);

  layCam0 = new QVBoxLayout; layCams->addLayout(layCam0);
  layCam0->setMargin(0);
  labcam0 = new QLabel(widg);
  layCam0->addWidget(labcam0);
  labcam0_txt = new QLabel(widg);
  labcam0_txt->setText("Camera 0");
  layCam0->addWidget(labcam0_txt);

  layCam1 = new QVBoxLayout; layCams->addLayout(layCam1);
  layCam1->setMargin(0);
  labcam1 = new QLabel(widg);
  layCam1->addWidget(labcam1);
  labcam1_txt = new QLabel(widg);
  labcam1_txt->setText("Camera 1");
  layCam1->addWidget(labcam1_txt);

  // Command Buttons
  widCmdButtons = new QWidget(widg);
  iFlowLayout* fl = new iFlowLayout(widCmdButtons);
//  layTopCtrls->addWidget(widCmdButtons);
  layOuter->addWidget(widCmdButtons);

  meth_but_mgr = new iMethodButtonMgr(widCmdButtons, fl, widCmdButtons);
  meth_but_mgr->Constr(wv()->World());

  setCentralWidget(widg);
  MakeButtons(layOuter);
}

VEWorldViewPanel::~VEWorldViewPanel() {
  VEWorldView* wv_ = wv();
  if (wv_) {
    wv_->m_wvp = NULL;
  }
}

void VEWorldViewPanel::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  VEWorldView* wv_ = wv();
  if(!wv_) return;

  VEWorld* wl = wv_->World();
  if(!wl) return;

  if (req_full_redraw) {
    req_full_redraw = false;
    wv_->Reset();
    wv_->BuildAll();
    wv_->Render();
  }

  chkDisplay->setChecked(wv_->display_on);
  chkDragObjs->setChecked(wv_->drag_objs);
  chkShowJoints->setChecked(wv_->show_joints);
  fldDragSize->GetImage((String)wv_->drag_size);

  if(wv_->display_on) {
    if(wl->camera_0) {
      QImage img = wv_->GetCameraImage(0);
      if(!img.isNull()) {
        QPixmap pm = QPixmap::fromImage(img);
        labcam0->setPixmap(pm);
      }
      else {
        labcam0->setText("Render Failed!");
      }
    }
    else {
      labcam0->setText("Not Set");
    }

    if(wl->camera_1) {
      QImage img = wv_->GetCameraImage(1);
      if(!img.isNull()) {
        QPixmap pm = QPixmap::fromImage(img);
        labcam1->setPixmap(pm);
      }
      else {
        labcam1->setText("Render Failed!");
      }
    }
    else {
      labcam1->setText("Not Set");
    }
  }
}

void VEWorldViewPanel::GetValue_impl() {
  inherited::GetValue_impl();
  VEWorldView* wv_ = wv();
  if (!wv_) return;

  wv_->display_on = chkDisplay->isChecked();
  wv_->drag_objs = chkDragObjs->isChecked();
  wv_->show_joints = chkShowJoints->isChecked();
  wv_->drag_size = (float)fldDragSize->GetValue();

  req_full_redraw = true;       // not worth micro-managing: MOST changes require full redraw!
}
