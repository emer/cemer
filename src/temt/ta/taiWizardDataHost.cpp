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

#include "taiWizardDataHost.h"
#include <iTextBrowser>
#include <NumberedTextView>
#include <MemberDef>
#include <iTextEdit>
#include <iDialog>
#include <iHostDialog>
#include <iStripeWidget>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextEdit>
#include <QDialog>
#include <QTabWidget>


taiWizardDataHost::taiWizardDataHost(taWizard* base_, TypeDef* typ_,
             bool read_only_, bool modal_, QObject* parent)
:inherited(typ_ ,read_only_, modal_, parent)
{
  root = base_;
  tabs = NULL;
}

taiWizardDataHost::~taiWizardDataHost() {
}

/*void taiWizardDataHost::Constr(const char* prompt, const char* win_title) {
  inherited::Constr(prompt, win_title);
}*/

void taiWizardDataHost::Constr_Box() {
  tabs = new QTabWidget(widget());
  vblDialog->addWidget(tabs, 1);
}

void taiWizardDataHost::Constr_RegNotifies() {
/*TODO  taBase* rbase = Base_(); // cache
  if (rbase) {
    rbase->AddSigClient(this);
  }*/
}


void taiWizardDataHost::Constr_Buttons() {
  inherited::Constr_Buttons();
/*TODO  btnPrint = new QPushButton("&Print", widget());
  layButtons->addSpacing(16);
  layButtons->addWidget(btnPrint, 0, (Qt::AlignVCenter));
  connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );*/
}

void taiWizardDataHost::Constr_Data_Labels()
{
  // assert all the tabs -- note that the number can never change
  if (tabs->count() == 0) {
    for (int i = 0; i < membs.size; ++i) {
      iStripeWidget* tab = new iStripeWidget();
      tabs->addTab(tab, page_names.SafeEl(i));
      QPalette pal = tab->palette();
      pal.setColor(QPalette::Background, bg_color);
      tab->setPalette(pal); 
      tab->setHiLightColor(bg_color_dark);
      tab->setStripeHeight(row_height + (2 * 1));
    }
  }
}

void taiWizardDataHost::Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl)
{
}

void taiWizardDataHost::Constr_Strings() {
/*TODO*/
}

void taiWizardDataHost::Enum_Members() {
  if (!typ) return; // class browser or such
  page_names.Reset();
  membs.Reset();

  MemberSpace& ms = typ->members;
  int page_no = 0; // always contains the most recent explicitly marked page
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
    // we skip the normal taNBase members
    if ((md->name == "name") || (md->name == "auto_open"))
      continue;

    // we only show Normal guys in Wizards, sorry charlie!
    if (!md->ShowMember(~TypeItem::IS_NORMAL, TypeItem::SC_EDIT, TypeItem::IS_NORMAL)) {
      continue;
    }

    // parse page directive -- if none, values are 0, ""
    String pd = md->OptionAfter("PAGE_");
    String page_name; // we only set if included
    if (pd.nonempty()) {
      if (pd.contains("_")) {
        page_name = pd.after("_"); // "" if empty
        pd = pd.before("_");
      }
      page_no = pd.toInt();
    }
    // add to lists if needed -- both have same counts
    if (page_no >= page_names.size) {
      page_names.SetSize(page_no + 1);
      membs.SetMinSize(page_no + 1);
    }
    if (page_name.nonempty()) {
      page_names[page_no] = page_name;
    }
    memb_el(page_no).Add(md);
  }
}

/*void taiWizardDataHost::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, widget());
  if (pd.exec() != iDialog::Accepted) return;
  // print ...
  edit->document()->print(&pr);
}*/

/*void taiWizardDataHost::GetImage() {
}

void taiWizardDataHost::GetValue() {
}*/

void taiWizardDataHost::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, etc. etc.
  if (modified) {
    GetValue();
  }
}

void taiWizardDataHost::Ok_impl() { //note: only used for Dialogs
  inherited::Ok_impl();
  //  if (modified) {
    GetValue();
    Unchanged();
    //  }
}
