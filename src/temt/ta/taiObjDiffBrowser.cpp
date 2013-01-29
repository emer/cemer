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

#include "taiObjDiffBrowser.h"
#include <taObjDiff_List>
#include <taiObjDiffBrowserFilter>
#include <taBase>
#include <MemberDef>
#include <ViewColor_List>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>

taiObjDiffBrowser* taiObjDiffBrowser::New(taObjDiff_List* diffs,
                                          int font_type, QWidget* par_window_) {
  String caption = "DiffCompare_" + diffs->tab_obj_a->GetDisplayName() + "_" +
    diffs->tab_obj_b->GetDisplayName();

  taiObjDiffBrowser* rval = new taiObjDiffBrowser(caption, par_window_);
  rval->setFont(taiM->dialogFont(font_type));
  rval->odl = diffs;
  rval->filter_dlg = NULL;
  rval->Constr();
  return rval;
}

taiObjDiffBrowser::taiObjDiffBrowser(const String& caption_, QWidget* par_window_)
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

taiObjDiffBrowser::~taiObjDiffBrowser() {
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

void taiObjDiffBrowser::accept() {
  inherited::accept();
  // here is where we execute actions!
  if(odl->tab_obj_a) {
    odl->tab_obj_a->DoDiffEdits(*odl);
  }
  delete odl;                   // all done!
  odl = NULL;
}

void taiObjDiffBrowser::reject() {
  inherited::reject();
}

bool taiObjDiffBrowser::Browse() {
  // modal:
  // return (exec() == iDialog::Accepted);
  // non-modal:
  show();
  return true;
}

Q_DECLARE_METATYPE(taObjDiffRec*);

void taiObjDiffBrowser::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  String a_path = odl->tab_obj_a->GetPathNames();
  String b_path = odl->tab_obj_b->GetPathNames();
  String lb_txt = "Differences between object A and object B, shown as changes needed to make A into B\nA is: " + a_path + "\nB is: " + b_path +
    "\nClick actions to actually perform edits on objects, which will take place when Ok is pressed";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  //  layOuter->addSpacing(taiM->vsep_c);

  QHBoxLayout* lay = new QHBoxLayout();
  btnAllA = new QPushButton("Toggle All &A's", this);
  btnAllA->setToolTip("Toggle selection of all the selectable actions for the A item -- i.e., make the A equivalent to the B");
  btnAllA->setDefault(false);
  btnAllA->setAutoDefault(false);
  lay->addWidget(btnAllA);

  btnFiltA = new QPushButton("Set Filtered A's", this);
  btnFiltA->setToolTip("Set actions on or off for filtered subset of the selectable actions for the A item");
  btnFiltA->setDefault(false);
  btnFiltA->setAutoDefault(false);
  lay->addWidget(btnFiltA);
  lay->addStretch();

  btnAllB = new QPushButton("Toggle All &B's", this);
  btnAllB->setToolTip("Toggle selection all the selectable actions for the B item -- i.e., make the B equivalent to the A");
  btnAllB->setDefault(false);
  btnAllB->setAutoDefault(false);
  lay->addWidget(btnAllB);

  btnFiltB = new QPushButton("Set Filtered B's", this);
  btnFiltB->setToolTip("Set actions on or off for filtered subset of the selectable actions for the B item");
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
  items->headerItem()->setToolTip(COL_NEST, "Nesting level below the original A or B object");
  items->headerItem()->setText(COL_A_FLG, "A Action");
  items->headerItem()->setToolTip(COL_A_FLG, "Edit action to perform on the A object -- these actions, if selected, will transform A into B");
  items->headerItem()->setText(COL_A_NM, "A Name");
  items->headerItem()->setToolTip(COL_A_NM, "Name of the item in A -- member name or sub-object name");
  items->headerItem()->setText(COL_A_VAL, "A Value");
  items->headerItem()->setToolTip(COL_A_VAL, "Value of the item in B");
  items->headerItem()->setText(COL_A_VIEW, "View");
  items->headerItem()->setToolTip(COL_A_VIEW, "View A item in project -- selects this object in the appropriate browser in the main project window");

  items->headerItem()->setText(COL_SEP, " | ");

  items->headerItem()->setText(COL_B_FLG, "B Action");
  items->headerItem()->setToolTip(COL_B_FLG, "Edit action to perform on the B object -- these actions, if selected, will transform B into A (i.e., the opposite of the default 'diff' direction)");
  items->headerItem()->setText(COL_B_NM, "B Name");
  items->headerItem()->setToolTip(COL_B_NM, "Name of the item in B -- member name or sub-object name");
  items->headerItem()->setText(COL_B_VAL, "B Value");
  items->headerItem()->setToolTip(COL_B_VAL, "Value of the item in B");
  items->headerItem()->setText(COL_B_VIEW, "View");
  items->headerItem()->setToolTip(COL_B_VIEW, "View B item in project -- selects this object in the appropriate browser in the main project window");
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

  filter_dlg = taiObjDiffBrowserFilter::New(taiMisc::defFontSize, this);
}

void taiObjDiffBrowser::AddItems() {
  if(! odl || odl->size == 0) return;

  int max_width = 40;

  voidptr_PArray        nest_pars;

  int init_nest = 0;
  int a_idx = 0;
  int b_idx = 0;

  int cur_nest = init_nest;
  for(int i=0;i<odl->size; i++) {
    if(a_idx >= odl->src_a->size && b_idx >= odl->src_b->size)
      break;                    // done!  shouldn't happen

    taObjDiffRec* rec = odl->FastEl(i); // from diffs guy itself
    String lbl_a;
    bool chk_a = rec->GetCurAction(0, lbl_a);
    String lbl_b;
    bool chk_b = rec->GetCurAction(1, lbl_b);

    int a_add = 0;
    int b_add = 0;
    if(rec->HasDiffFlag(taObjDiffRec::SRC_A)) {
      a_add = (rec->idx - a_idx) + 1; // add this many to catch up in a
      b_add = (rec->diff_odr->idx - b_idx); // " b
      if(!rec->HasDiffFlag(taObjDiffRec::DIFF_DEL))
        b_add++;                // add the 1
    }
    else {
      // must be an ADD -- ony in B
      b_add = (rec->idx - b_idx) + 1; // add this many to catch up in b
      a_add = (rec->diff_odr->idx - a_idx); // " a -- no +1 here..
    }

    int mxadd = MAX(a_add, b_add);
    int ast = mxadd - a_add;
    int bst = mxadd - b_add;
    for(int ad=0; ad < mxadd; ad++) {
      taObjDiffRec* a_rec = NULL;
      taObjDiffRec* b_rec = NULL;
      if(ad < a_add) {
        a_rec = odl->src_a->SafeEl(a_idx);
        a_idx++;
      }
      if(ad < b_add) {
        b_rec = odl->src_b->SafeEl(b_idx);
        b_idx++;
      }

      int ad_nest = cur_nest;
      if(a_rec)
        ad_nest = a_rec->nest_level; // a dominates
      else if(b_rec)
        ad_nest = b_rec->nest_level;

      bool some_diff = false;
      bool chg_diff = false;
      if((a_rec && a_rec->HasDiffFlag(taObjDiffRec::DIFF_MASK)) ||
         (b_rec && b_rec->HasDiffFlag(taObjDiffRec::DIFF_MASK))) {
        some_diff = true;
        if(a_rec && a_rec->HasDiffFlag(taObjDiffRec::DIFF_CHG))
          chg_diff = true;
      }

      bool condshow_off = false;
      bool a_off = false;
      bool b_off = false;
      // check for condshow off guys
      if(a_rec && a_rec->mdef && a_rec->par_type && a_rec->par_addr) {
        bool anosho = !a_rec->mdef->GetCondOptTest("CONDSHOW", a_rec->par_type,
                                                 a_rec->par_addr);
        bool anoed = !a_rec->mdef->GetCondOptTest("CONDEDIT", a_rec->par_type,
                                                 a_rec->par_addr);
        a_off = (anosho || anoed);
      }
      if(b_rec && b_rec->mdef && b_rec->par_type && b_rec->par_addr) {
        bool bnosho = !b_rec->mdef->GetCondOptTest("CONDSHOW", b_rec->par_type,
                                                 b_rec->par_addr);
        bool bnoed = !b_rec->mdef->GetCondOptTest("CONDEDIT", b_rec->par_type,
                                                 b_rec->par_addr);
        b_off = (bnosho || bnoed);
      }

      if(a_off && b_off) {
        condshow_off = true;
        if(!some_diff)
          continue;             // nothing to see here
      }

      if(some_diff && !chg_diff && (a_off || b_off)) {
        continue;               // if off and add or del, will only show in one, so omit
      }

      QTreeWidgetItem* parw = NULL;
      int par_nest = ad_nest-1;
      if(par_nest > init_nest) {
        if(par_nest < nest_pars.size) {
          parw = (QTreeWidgetItem*)nest_pars[par_nest];
        }
        if(!parw) {  // yes this does happen
          if(a_rec && a_rec->par_odr)
            parw = (QTreeWidgetItem*)a_rec->par_odr->widget;
        }
      }
      QTreeWidgetItem* witm;
      if(parw)
        witm = new QTreeWidgetItem(parw);
      else
        witm = new QTreeWidgetItem(items);
      if(a_rec)
        a_rec->widget = witm;
      if(b_rec)
        b_rec->widget = witm;

      QVariant val_a = qVariantFromValue( a_rec );
      witm->setData(0, Qt::UserRole+1, val_a);
      QVariant val_b = qVariantFromValue( b_rec );
      witm->setData(1, Qt::UserRole+1, val_b);

      nest_pars.SetSize(ad_nest+1);
      nest_pars[ad_nest] = witm;

      witm->setText(COL_NEST, String(ad_nest));
      witm->setText(COL_SEP, " | ");

      if(a_rec) {
        String nm;
        taMisc::IndentString(nm, a_rec->nest_level);
        nm += a_rec->GetDisplayName().elidedTo(max_width);
        witm->setText(COL_A_NM, nm);
        witm->setText(COL_A_VAL, a_rec->value.elidedTo(max_width));
        witm->setToolTip(COL_A_VAL, a_rec->value);

        String dec_key = a_rec->GetTypeDecoKey();
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

        if(a_rec->type->IsActualTaBase() && a_rec->tabref && !a_rec->mdef) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_A_VIEW, Qt::Unchecked);
        }

        if(a_off) {
          witm->setBackground(COL_A_VAL, *off_color);
          witm->setBackground(COL_A_NM, *off_color);
        }
      }
      if(b_rec) {
        String nm;
        taMisc::IndentString(nm, b_rec->nest_level);
        nm += b_rec->GetDisplayName().elidedTo(max_width);
        witm->setText(COL_B_NM, nm);
        witm->setText(COL_B_VAL, b_rec->value.elidedTo(max_width));
        witm->setToolTip(COL_B_VAL, b_rec->value);

        String dec_key = b_rec->GetTypeDecoKey();
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

        if(b_rec->type->IsActualTaBase() && b_rec->tabref && !b_rec->mdef) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_B_VIEW, Qt::Unchecked);
        }

        if(b_off) {
          witm->setBackground(COL_B_VAL, *off_color);
          witm->setBackground(COL_B_NM, *off_color);
        }
      }

      if((a_rec && a_rec->n_diffs > 0) || (b_rec && b_rec->n_diffs > 0)) {
        witm->setExpanded(true);
      }
      else {
        witm->setExpanded(false);
      }

      if(!some_diff) continue;

      if(a_rec && a_rec->HasDiffFlag(taObjDiffRec::DIFF_DEL)) {
        if(chk_a) witm->setBackground(COL_A_FLG, *del_color);
        witm->setBackground(COL_A_VAL, a_off ? *del_color_lt : *del_color);
        witm->setBackground(COL_A_NM, a_off ? *del_color_lt : *del_color);
        if(chk_b) witm->setBackground(COL_B_FLG, *add_color);
        witm->setBackground(COL_B_VAL, b_off ? *add_color_lt : *add_color);
        witm->setBackground(COL_B_NM, b_off ? *add_color_lt : *add_color);
        witm->setExpanded(false); // never expand a del -- only applies to parents anyway..
        if(!a_rec->HasDiffFlag(taObjDiffRec::SUB_NO_ACT)) {
          // only ta base items really feasible here..
          if(rec->type->IsActualTaBase()) {
            witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
            witm->setCheckState(COL_A_FLG, Qt::Unchecked);
            witm->setCheckState(COL_B_FLG, Qt::Unchecked);
            witm->setText(COL_A_FLG, lbl_a);
            witm->setText(COL_B_FLG, lbl_b);
          }
        }
      }
      else if(b_rec && b_rec->HasDiffFlag(taObjDiffRec::DIFF_ADD)) {
        if(chk_a) witm->setBackground(COL_A_FLG, *add_color);
        witm->setBackground(COL_A_VAL, a_off ? *add_color_lt : *add_color);
        witm->setBackground(COL_A_NM, a_off ? *add_color_lt : *add_color);
        if(chk_b) witm->setBackground(COL_B_FLG, *del_color);
        witm->setBackground(COL_B_VAL, b_off ? *del_color_lt : *del_color);
        witm->setBackground(COL_B_NM, b_off ? *del_color_lt : *del_color);
        witm->setExpanded(false);
        if(!b_rec->HasDiffFlag(taObjDiffRec::SUB_NO_ACT)) {
          // only ta base items really feasible here..
          if(rec->type->IsActualTaBase()) {
            witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
            witm->setCheckState(COL_A_FLG, Qt::Unchecked);
            witm->setCheckState(COL_B_FLG, Qt::Unchecked);
            witm->setText(COL_A_FLG, lbl_a);
            witm->setText(COL_B_FLG, lbl_b);
          }
        }
      }
      else if(a_rec && a_rec->HasDiffFlag(taObjDiffRec::DIFF_CHG)) {
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

void taiObjDiffBrowser::ViewItem(taObjDiffRec* rec) {
  taBase* tab = rec->GetOwnTaBase();
  if(!tab) return;
  tab->BrowserSelectMe();
}

void taiObjDiffBrowser::itemClicked(QTreeWidgetItem* itm, int column) {
  QVariant a_val = itm->data(0, Qt::UserRole+1);
  taObjDiffRec* a_rec = a_val.value<taObjDiffRec*>();
  QVariant b_val = itm->data(1, Qt::UserRole+1);
  taObjDiffRec* b_rec = b_val.value<taObjDiffRec*>();

  if(column == COL_A_VIEW || column == COL_B_VIEW) {
    if(column == COL_A_VIEW) {
      itm->setCheckState(COL_A_VIEW, Qt::Unchecked); // never actually click
      ViewItem(a_rec);
      return;
    }
    else {
      itm->setCheckState(COL_A_VIEW, Qt::Unchecked);
      ViewItem(b_rec);
      return;
    }
  }

  if(column != COL_A_FLG && column != COL_B_FLG) return;

  int a_or_b = 0;
  if(column == COL_B_FLG) a_or_b = 1;
  Qt::CheckState chkst = itm->checkState(column);
  bool on = (chkst == Qt::Checked);

  taObjDiffRec* rec;
  if(a_rec)
    rec = a_rec;
  else
    rec = b_rec;
  if(!rec || !rec->ActionAllowed()) return;
  rec->SetCurAction(a_or_b, on);
  UpdateItemDisp(itm, rec, a_or_b);
}

void taiObjDiffBrowser::UpdateItemDisp(QTreeWidgetItem* itm, taObjDiffRec* rec,  int a_or_b) {
  QBrush no_color;

  String lbl;
  bool chk = rec->GetCurAction(a_or_b, lbl);
  int column;
  if(a_or_b == 0) column = COL_A_FLG;
  else column = COL_B_FLG;

  if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL))
    itm->setBackground(column, chk ? (a_or_b ? *add_color : *del_color) : no_color);
  else if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD))
    itm->setBackground(column, chk ? (a_or_b ? *del_color : *add_color) : no_color);
  else if(rec->HasDiffFlag(taObjDiffRec::DIFF_CHG))
    itm->setBackground(column, chk ? *chg_color : no_color);

  itm->setCheckState(column, chk ? Qt::Checked : Qt::Unchecked);
}

void taiObjDiffBrowser::ToggleAll(int a_or_b) {
  for(int i=0;i<odl->size; i++) {
    taObjDiffRec* rec = odl->FastEl(i);

    if(!rec->ActionAllowed()) continue;

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    String lbl;
    bool chk = rec->GetCurAction(a_or_b, lbl);
    rec->SetCurAction(a_or_b, !chk);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void taiObjDiffBrowser::toggleAllA() {
  ToggleAll(0);
}

void taiObjDiffBrowser::toggleAllB() {
  ToggleAll(1);
}

void taiObjDiffBrowser::SetFiltered(int a_or_b, bool on, bool add, bool del, bool chg,
                                    bool nm_not, String nm_contains, bool val_not,
                                    String val_contains) {
  for(int i=0;i<odl->size; i++) {
    taObjDiffRec* rec = odl->FastEl(i);

    if(!rec->ActionAllowed()) continue;

    if(a_or_b == 0) {
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD) && !add) continue;
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL) && !del) continue;
    }
    else {                      // switched for b
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL) && !add) continue;
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD) && !del) continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::DIFF_CHG) && !chg) continue;
    if(nm_contains.nonempty()) {
      if(nm_not) {
        if(rec->name.contains(nm_contains)) continue;
      }
      else {
        if(!rec->name.contains(nm_contains)) continue;
      }
    }
    if(val_contains.nonempty()) {
      if(val_not) {
        if(rec->value.contains(val_contains)) continue;
      }
      else {
        if(!rec->value.contains(val_contains)) continue;
      }
    }

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    rec->SetCurAction(a_or_b, on);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void taiObjDiffBrowser::setFilteredA() {
  if(filter_dlg->Browse()) {
    SetFiltered(0, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}

void taiObjDiffBrowser::setFilteredB() {
  if(filter_dlg->Browse()) {
    SetFiltered(1, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}
