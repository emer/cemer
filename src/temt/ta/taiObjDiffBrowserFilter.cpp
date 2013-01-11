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

#include "taiObjDiffBrowserFilter.h"

taiObjDiffBrowserFilter* taiObjDiffBrowserFilter::New(int font_type,
                                                      QWidget* par_window_) {
  taiObjDiffBrowserFilter* rval = new taiObjDiffBrowserFilter(par_window_);
  rval->setFont(taiM->dialogFont(font_type));
  rval->Constr();
  return rval;
}

taiObjDiffBrowserFilter::taiObjDiffBrowserFilter(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(true);
  setWindowTitle("Set Diff Actions Filter Dialog");
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

taiObjDiffBrowserFilter::~taiObjDiffBrowserFilter() {
}

void taiObjDiffBrowserFilter::accept() {
  action_on = chkActionOn->isChecked();
  add = chkAdd->isChecked();
  del = chkDel->isChecked();
  chg = chkChg->isChecked();
  nm_not = chkNmNot->isChecked();
  val_not = chkValNot->isChecked();
  nm_contains = editNm->text();
  val_contains = editVal->text();
  inherited::accept();
}

void taiObjDiffBrowserFilter::reject() {
  inherited::reject();
}

bool taiObjDiffBrowserFilter::Browse() {
  return (exec() == iDialog::Accepted);
}

void taiObjDiffBrowserFilter::Constr() {
  action_on = true;
  add = del = chg = true;
  nm_not = val_not = false;

  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  String lb_txt = "Select types of diffs records to change, and value to set\n";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  layOuter->addSpacing(taiM->vsep_c);
  layOuter->addSpacing(taiM->vsep_c);

  chkActionOn = new QCheckBox("Action &On", this);
  chkActionOn->setToolTip("State to set for the action flag for diff items -- on or off");
  chkActionOn->setCheckState(Qt::Checked);
  layOuter->addWidget(chkActionOn);

  QHBoxLayout* lay = new QHBoxLayout();

  lbl = new QLabel("Diff types: ");
  lay->addWidget(lbl);

  chkAdd = new QCheckBox("&Add", this);
  chkAdd->setToolTip("Include Add Diffs");
  chkAdd->setCheckState(Qt::Checked);
  lay->addWidget(chkAdd);

  chkDel = new QCheckBox("&Del", this);
  chkDel->setToolTip("Include Del Diffs");
  chkDel->setCheckState(Qt::Checked);
  lay->addWidget(chkDel);

  chkChg = new QCheckBox("&Chg", this);
  chkChg->setToolTip("Include Chg Diffs");
  chkChg->setCheckState(Qt::Checked);
  lay->addWidget(chkChg);
  lay->addStretch();

  layOuter->addLayout(lay);

  lay = new QHBoxLayout();

  lbl = new QLabel("name contains:");
  lbl->setToolTip("Text that the name of diff object must contain (or not)");
  lay->addWidget(lbl);
  editNm = new iLineEdit(this);
  lay->addWidget(editNm);

  chkNmNot = new QCheckBox("not!", this);
  chkNmNot->setToolTip("changes name contains text to NOT contains");
  lay->addWidget(chkNmNot);

  layOuter->addLayout(lay);

  lay = new QHBoxLayout();

  lbl = new QLabel("value contains:");
  lbl->setToolTip("Text that the value of diff object must contain (or not)");
  lay->addWidget(lbl);
  editVal = new iLineEdit(this);
  lay->addWidget(editVal);

  chkValNot = new QCheckBox("not!", this);
  chkValNot->setToolTip("changes value contains text to NOT contains");
  lay->addWidget(chkValNot);

  layOuter->addLayout(lay);

  layOuter->addStretch();

  lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
}
