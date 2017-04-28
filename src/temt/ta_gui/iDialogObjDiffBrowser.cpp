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

#include "iDialogObjDiffBrowser.h"
#include <ObjDiff>
#include <ObjDiffRec>
#include <ObjDiffRec_List>
#include <iDialogObjDiffFilter>
#include <taBase>
#include <MemberDef>
#include <ViewColor_List>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>

iDialogObjDiffBrowser* iDialogObjDiffBrowser::New(ObjDiff* diffs,
                                          int font_type, QWidget* par_window_) {
  String caption = "DiffCompare_" + diffs->a_top->GetDisplayName() + "_" +
    diffs->b_top->GetDisplayName();

  iDialogObjDiffBrowser* rval = new iDialogObjDiffBrowser(caption, par_window_);
  rval->setFont(taiM->dialogFont(font_type));
  rval->odl = diffs;
  rval->filter_dlg = NULL;
  rval->Constr();
  return rval;
}

iDialogObjDiffBrowser::iDialogObjDiffBrowser(const String& caption_, QWidget* par_window_)
:inherited(par_window_)
{
  caption = caption_;
  setModal(false);
  setWindowTitle(caption);
  add_color = new QBrush(QColor("pale green"));
  del_color = new QBrush(QColor("pink"));
  chg_color = new QBrush(Qt::yellow);

  int darkness = 125;
  add_color_lt = new QBrush(add_color->color().darker(darkness));
  del_color_lt = new QBrush(del_color->color().darker(darkness));
  chg_color_lt = new QBrush(chg_color->color().darker(darkness));

  off_color = new QBrush(Qt::lightGray);

  resize(taiM->dialogSize(taiMisc::hdlg_b));
}

iDialogObjDiffBrowser::~iDialogObjDiffBrowser() {
  if(filter_dlg)
    delete filter_dlg;
  filter_dlg = NULL;
  if(odl)
    delete odl;
  odl = NULL;
  delete add_color;
  delete del_color;
  delete chg_color;
  delete add_color_lt;
  delete del_color_lt;
  delete chg_color_lt;
  delete off_color;
}

void iDialogObjDiffBrowser::accept() {
  inherited::accept();
  // here is where we execute actions!
  if(!isModal()) {
    // odl->>DoDiffEdits(NULL, NULL);
    delete odl;                   // all done!
  }
  odl = NULL;
}

void iDialogObjDiffBrowser::reject() {
  inherited::reject();
}

bool iDialogObjDiffBrowser::Browse(bool modal) {
  if(modal) {
    setModal(true);
    return (exec() == iDialog::Accepted);
  }
  setModal(false);
  show();
  return true;
}

Q_DECLARE_METATYPE(ObjDiffRec*);

void iDialogObjDiffBrowser::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  String a_path = odl->a_top->GetPathNames();
  String b_path = odl->b_top->GetPathNames();
  String lb_txt = "Differences between object A and object B, shown as changes needed to make A into B\nA is: " + a_path + "\nB is: " + b_path +
    "\nClick actions to actually perform edits on objects, which will take place when Ok is pressed";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  //  layOuter->addSpacing(taiM->vsep_c);

  QHBoxLayout* lay = new QHBoxLayout();
  btnAllA = new QPushButton("Toggle All &A's", this);
  btnAllA->setToolTip(taiMisc::ToolTipPreProcess("Toggle selection of all the selectable actions for the A item -- i.e., make the A equivalent to the B"));
  btnAllA->setDefault(false);
  btnAllA->setAutoDefault(false);
  lay->addWidget(btnAllA);

  btnFiltA = new QPushButton("Set Filtered A's", this);
  btnFiltA->setToolTip(taiMisc::ToolTipPreProcess("Set actions on or off for filtered subset of the selectable actions for the A item"));
  btnFiltA->setDefault(false);
  btnFiltA->setAutoDefault(false);
  lay->addWidget(btnFiltA);
  lay->addStretch();

  btnAllB = new QPushButton("Toggle All &B's", this);
  btnAllB->setToolTip(taiMisc::ToolTipPreProcess("Toggle selection all the selectable actions for the B item -- i.e., make the B equivalent to the A"));
  btnAllB->setDefault(false);
  btnAllB->setAutoDefault(false);
  lay->addWidget(btnAllB);

  btnFiltB = new QPushButton("Set Filtered B's", this);
  btnFiltB->setToolTip(taiMisc::ToolTipPreProcess("Set actions on or off for filtered subset of the selectable actions for the B item"));
  btnFiltB->setDefault(false);
  btnFiltB->setAutoDefault(false);
  lay->addWidget(btnFiltB);
  lay->addStretch();

  layOuter->addLayout(lay);

  items = new QTreeWidget(this);
  layOuter->addWidget(items, 1); // list is item to expand in host

  items->setColumnCount(COL_N);
  items->setSortingEnabled(false);// only 1 order possible
  items->setEditTriggers(QAbstractItemView::DoubleClicked);
  items->headerItem()->setText(COL_NEST, "Nesting");
  items->headerItem()->setToolTip(COL_NEST, taiMisc::ToolTipPreProcess("Nesting level below the original A or B object"));
  items->headerItem()->setText(COL_A_FLG, "A Action");
  items->headerItem()->setToolTip(COL_A_FLG, taiMisc::ToolTipPreProcess("Edit action to perform on the A object -- these actions, if selected will transform A into B"));
  items->headerItem()->setText(COL_A_NM, "A Name");
  items->headerItem()->setToolTip(COL_A_NM, taiMisc::ToolTipPreProcess("Name of the item in A -- member name or sub-object name"));
  items->headerItem()->setText(COL_A_VAL, "A Value");
  items->headerItem()->setToolTip(COL_A_VAL, taiMisc::ToolTipPreProcess("Value of the item in B"));
  items->headerItem()->setText(COL_A_VIEW, "View");
  items->headerItem()->setToolTip(COL_A_VIEW, taiMisc::ToolTipPreProcess("View A item in project -- selects this object in the appropriate browser in the main project window"));

  items->headerItem()->setText(COL_SEP, " | ");

  items->headerItem()->setText(COL_B_FLG, "B Action");
  items->headerItem()->setToolTip(COL_B_FLG, taiMisc::ToolTipPreProcess("Edit action to perform on the B object -- these actions, if selected, will transform B into A (i.e., the opposite of the default 'diff' direction)"));
  items->headerItem()->setText(COL_B_NM, "B Name");
  items->headerItem()->setToolTip(COL_B_NM, taiMisc::ToolTipPreProcess("Name of the item in B -- member name or sub-object name"));
  items->headerItem()->setText(COL_B_VAL, "B Value");
  items->headerItem()->setToolTip(COL_B_VAL, taiMisc::ToolTipPreProcess("Value of the item in B"));
  items->headerItem()->setText(COL_B_VIEW, "View");
  items->headerItem()->setToolTip(COL_B_VIEW, taiMisc::ToolTipPreProcess("View B item in project -- selects this object in the appropriate browser in the main project window"));
  items->setUniformRowHeights(true);
  items->setIndentation(taMisc::tree_indent);

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

  connect(btnAllA, SIGNAL(clicked()), this, SLOT(toggleAllA()) );
  connect(btnAllB, SIGNAL(clicked()), this, SLOT(toggleAllB()) );

  connect(btnFiltA, SIGNAL(clicked()), this, SLOT(setFilteredA()) );
  connect(btnFiltB, SIGNAL(clicked()), this, SLOT(setFilteredB()) );

  connect(items, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
          SLOT(itemClicked(QTreeWidgetItem*,int)));

  AddItems();

  filter_dlg = iDialogObjDiffFilter::New(taiMisc::defFontSize, this);
}

void iDialogObjDiffBrowser::AddItems() {
  if(!odl || odl->diffs.size == 0) return;

  int max_width = 40;

  for(int i=0;i<odl->diffs.size; i++) {
    ObjDiffRec* rec = odl->diffs.FastEl(i);

    String lbl_a;
    bool chk_a = rec->GetCurAction(0, lbl_a);
    String lbl_b;
    bool chk_b = rec->GetCurAction(1, lbl_b);
    
    bool some_diff = false;
    bool chg_diff = false;
    if(rec->IsDiff()) {
      some_diff = true;
      if(rec->IsABDiff()) {
        chg_diff = true;
      }
    }

    bool condshow_off = false;
    bool a_off = (rec->AMemberNoShow() || rec->AMemberNoEdit());
    bool b_off = (rec->BMemberNoShow() || rec->BMemberNoEdit());

    if(a_off && b_off) {
      condshow_off = true;
      // if(!some_diff)
      //   continue;             // nothing to see here
    }

    // if(some_diff && !chg_diff && (a_off || b_off)) {
    //   continue;               // if off and add or del, will only show in one, so omit
    // }

    QTreeWidgetItem* parw = NULL;
    if(rec->par_rec) {
      parw = (QTreeWidgetItem*)rec->par_rec->widget;
    }
    
    QTreeWidgetItem* witm;
    if(parw)
      witm = new QTreeWidgetItem(parw);
    else
      witm = new QTreeWidgetItem(items);

    rec->widget = witm;

    QVariant qval = qVariantFromValue(rec);
    witm->setData(0, Qt::UserRole+1, qval);

    witm->setText(COL_NEST, String(rec->nest_level));
    witm->setText(COL_SEP, " | ");

    if(!rec->IsBnotA()) {
      String nm;
      taMisc::IndentString(nm, rec->nest_level);
      nm += rec->AName().elidedTo(max_width);
      witm->setText(COL_A_NM, nm);
      witm->setText(COL_A_VAL, rec->a_val.elidedTo(max_width));
      witm->setToolTip(COL_A_VAL, rec->a_val);

      String dec_key = rec->ADecoKey();
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        iColor clr;
        if(vc->use_fg)
          clr = vc->fg_color.color();
        else if(vc->use_bg)
          clr = vc->bg_color.color();
        witm->setTextColor(COL_A_NM, clr);
        witm->setTextColor(COL_A_VAL, clr);
      }

      if(rec->IsObjects() || rec->IsParent()) {
        witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        witm->setCheckState(COL_A_VIEW, Qt::Unchecked);
      }

      if(a_off) {
        witm->setBackground(COL_A_VAL, *off_color);
        witm->setBackground(COL_A_NM, *off_color);
      }
    }
    if(!rec->IsAnotB()) {
      String nm;
      taMisc::IndentString(nm, rec->nest_level);
      nm += rec->BName().elidedTo(max_width);
      witm->setText(COL_B_NM, nm);
      witm->setText(COL_B_VAL, rec->b_val.elidedTo(max_width));
      witm->setToolTip(COL_B_VAL, rec->b_val);

      String dec_key = rec->BDecoKey();
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        iColor clr;
        if(vc->use_fg)
          clr = vc->fg_color.color();
        else if(vc->use_bg)
          clr = vc->bg_color.color();
        witm->setTextColor(COL_B_NM, clr);
        witm->setTextColor(COL_B_VAL, clr);
      }

      if(rec->IsObjects() || rec->IsParent()) {
        witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        witm->setCheckState(COL_B_VIEW, Qt::Unchecked);
      }

      if(b_off) {
        witm->setBackground(COL_B_VAL, *off_color);
        witm->setBackground(COL_B_NM, *off_color);
      }
    }

    // if(rec->n_diffs > 0) {
      witm->setExpanded(true);
    // }
    // else {
    //   witm->setExpanded(false);
    // }

    if(!some_diff) continue;

    if(rec->IsAnotB()) {
      if(chk_a) witm->setBackground(COL_A_FLG, *del_color);
      witm->setBackground(COL_A_VAL, a_off ? *del_color_lt : *del_color);
      witm->setBackground(COL_A_NM, a_off ? *del_color_lt : *del_color);
      if(chk_b) witm->setBackground(COL_B_FLG, *add_color);
      witm->setBackground(COL_B_VAL, b_off ? *add_color_lt : *add_color);
      witm->setBackground(COL_B_NM, b_off ? *add_color_lt : *add_color);
      witm->setExpanded(false); // never expand a del -- only applies to parents anyway..
      if(!rec->HasDiffFlag(ObjDiffRec::SUB_NO_ACT)) {
        // only ta base items really feasible here..
        if(rec->IsObjects()) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_A_FLG, Qt::Unchecked);
          witm->setCheckState(COL_B_FLG, Qt::Unchecked);
          witm->setText(COL_A_FLG, lbl_a);
          witm->setText(COL_B_FLG, lbl_b);
        }
      }
    }
    else if(rec->IsBnotA()) {
      if(chk_a) witm->setBackground(COL_A_FLG, *add_color);
      witm->setBackground(COL_A_VAL, a_off ? *add_color_lt : *add_color);
      witm->setBackground(COL_A_NM, a_off ? *add_color_lt : *add_color);
      if(chk_b) witm->setBackground(COL_B_FLG, *del_color);
      witm->setBackground(COL_B_VAL, b_off ? *del_color_lt : *del_color);
      witm->setBackground(COL_B_NM, b_off ? *del_color_lt : *del_color);
      witm->setExpanded(false);
      if(!rec->HasDiffFlag(ObjDiffRec::SUB_NO_ACT)) {
        // only ta base items really feasible here..
        if(rec->IsObjects()) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_A_FLG, Qt::Unchecked);
          witm->setCheckState(COL_B_FLG, Qt::Unchecked);
          witm->setText(COL_A_FLG, lbl_a);
          witm->setText(COL_B_FLG, lbl_b);
        }
      }
    }
    else if(rec->IsABDiff()) {
      if(chk_a) witm->setBackground(COL_A_FLG, *chg_color);
      witm->setBackground(COL_A_VAL, a_off ? *chg_color_lt : *chg_color);
      witm->setBackground(COL_A_NM, a_off ? *chg_color_lt : *chg_color);
      if(chk_b) witm->setBackground(COL_B_FLG, *chg_color);
      witm->setBackground(COL_B_VAL, b_off ? *chg_color_lt : *chg_color);
      witm->setBackground(COL_B_NM, b_off ? *chg_color_lt : *chg_color);

      witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
      witm->setCheckState(COL_A_FLG, Qt::Unchecked);
      witm->setCheckState(COL_B_FLG, Qt::Unchecked);
      witm->setText(COL_A_FLG, lbl_a);
      witm->setText(COL_B_FLG, lbl_b);
    }
  }

  items->resizeColumnToContents(COL_NEST);
  items->resizeColumnToContents(COL_A_FLG);
  items->resizeColumnToContents(COL_A_NM);
  items->resizeColumnToContents(COL_A_VAL);
  items->resizeColumnToContents(COL_A_VIEW);
  items->resizeColumnToContents(COL_SEP);
  items->resizeColumnToContents(COL_B_FLG);
  items->resizeColumnToContents(COL_B_NM);
  items->resizeColumnToContents(COL_B_VAL);
  items->resizeColumnToContents(COL_B_VIEW);
}

void iDialogObjDiffBrowser::ViewItem(ObjDiffRec* odr, int a_or_b) {
  if(!odr) return;
  if(a_or_b == 0) {
    if(odr->a_obj) {
      odr->a_obj->BrowserSelectMe();
    }
  }
  else {
    if(odr->b_obj) {
      odr->b_obj->BrowserSelectMe();
    }
  }
}

void iDialogObjDiffBrowser::itemClicked(QTreeWidgetItem* itm, int column) {
  QVariant qval = itm->data(0, Qt::UserRole+1);
  ObjDiffRec* rec = qval.value<ObjDiffRec*>();
  if(!rec) return;

  if(column == COL_A_VIEW || column == COL_B_VIEW) {
    if(column == COL_A_VIEW) {
      itm->setCheckState(COL_A_VIEW, Qt::Unchecked); // never actually click
      ViewItem(rec, 0);
      return;
    }
    else {
      itm->setCheckState(COL_B_VIEW, Qt::Unchecked);
      ViewItem(rec, 1);
      return;
    }
  }

  if(column != COL_A_FLG && column != COL_B_FLG) return;

  int a_or_b = 0;
  if(column == COL_B_FLG) a_or_b = 1;
  Qt::CheckState chkst = itm->checkState(column);
  bool on = (chkst == Qt::Checked);

  if(!rec->ActionAllowed()) return;
  rec->SetCurAction(a_or_b, on);
  UpdateItemDisp(itm, rec, a_or_b);
}

void iDialogObjDiffBrowser::UpdateItemDisp(QTreeWidgetItem* itm, ObjDiffRec* rec,  int a_or_b) {
  QBrush no_color;

  String lbl;
  bool chk = rec->GetCurAction(a_or_b, lbl);
  int column;
  if(a_or_b == 0) column = COL_A_FLG;
  else column = COL_B_FLG;

  if(rec->HasDiffFlag(ObjDiffRec::B_NOT_A))
    itm->setBackground(column, chk ? (a_or_b ? *add_color : *del_color) : no_color);
  else if(rec->HasDiffFlag(ObjDiffRec::A_NOT_B))
    itm->setBackground(column, chk ? (a_or_b ? *del_color : *add_color) : no_color);
  else if(rec->HasDiffFlag(ObjDiffRec::A_B_DIFF))
    itm->setBackground(column, chk ? *chg_color : no_color);

  itm->setCheckState(column, chk ? Qt::Checked : Qt::Unchecked);
}

void iDialogObjDiffBrowser::ToggleAll(int a_or_b) {
  for(int i=0;i<odl->diffs.size; i++) {
    ObjDiffRec* rec = odl->diffs.FastEl(i);

    if(!rec->ActionAllowed()) continue;

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    String lbl;
    bool chk = rec->GetCurAction(a_or_b, lbl);
    rec->SetCurAction(a_or_b, !chk);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void iDialogObjDiffBrowser::toggleAllA() {
  ToggleAll(0);
}

void iDialogObjDiffBrowser::toggleAllB() {
  ToggleAll(1);
}

void iDialogObjDiffBrowser::SetFiltered(int a_or_b, bool on, bool add, bool del, bool chg,
                                    bool nm_not, String nm_contains, bool val_not,
                                    String val_contains) {
  for(int i=0;i<odl->diffs.size; i++) {
    ObjDiffRec* rec = odl->diffs.FastEl(i);

    if(!rec->ActionAllowed()) continue;

    if(a_or_b == 0) {
      if(rec->HasDiffFlag(ObjDiffRec::B_NOT_A) && !add) continue;
      if(rec->HasDiffFlag(ObjDiffRec::A_NOT_B) && !del) continue;
    }
    else {                      // switched for b
      if(rec->HasDiffFlag(ObjDiffRec::A_NOT_B) && !add) continue;
      if(rec->HasDiffFlag(ObjDiffRec::B_NOT_A) && !del) continue;
    }

    if(rec->HasDiffFlag(ObjDiffRec::A_B_DIFF) && !chg) continue;
    if(nm_contains.nonempty()) {
      bool cont = rec->NameContains(nm_contains);
      if(nm_not) {
        if(cont) continue;
      }
      else {
        if(!cont) continue;
      }
    }
    if(val_contains.nonempty()) {
      bool cont = rec->ValueContains(val_contains);
      if(val_not) {
        if(cont) continue;
      }
      else {
        if(!cont) continue;
      }
    }

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    rec->SetCurAction(a_or_b, on);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void iDialogObjDiffBrowser::setFilteredA() {
  if(filter_dlg->Browse()) {
    SetFiltered(0, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}

void iDialogObjDiffBrowser::setFilteredB() {
  if(filter_dlg->Browse()) {
    SetFiltered(1, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}
