
// Copyright 2017, Regents of the University of Colorado,
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

#include "taiWidgetText.h"
#include <iLineEdit>
#include <iClipData>

#include <taMisc>
#include <taiMisc>
#include <MemberDef>
#include <Completions>
#include <css_qtdialog.h>

#include <QHBoxLayout>
#include <QToolButton>
#include <QFont>

taiWidgetText::taiWidgetText(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_,
                 bool needs_edit_button, const char *tooltip, MemberDef* md, taBase* base)
  : taiWidget(typ_, host_, par, gui_parent_, flags_)
  , lookupfun_md(md)
  , lookupfun_base(base)
  , leText()
  , btnEdit()

{
  cssiArgDialog* cssi_arg_dlg = dynamic_cast<cssiArgDialog*>(host_);
  
  if (needs_edit_button) {
    QWidget* act_par = MakeLayoutWidget(gui_parent_);
    QHBoxLayout* lay = new QHBoxLayout(act_par);
    lay->setMargin(0);
    lay->setSpacing(1);
    
    bool add_completer = (lookupfun_md && (md->HasOption("ADD_COMPLETER_EXPR") || lookupfun_md->HasOption("ADD_COMPLETER_SIMPLE")));
    if (add_completer) {
      leText = new iLineEdit(act_par, true);
      if (leText->GetCompleter()) {
        leText->GetCompleter()->SetHostType(iCodeCompleter::DIALOG_HOST);
        if (lookupfun_md->HasOption("ADD_COMPLETER_SIMPLE")) {
          leText->GetCompleter()->SetFieldType(iCodeCompleter::SIMPLE);
        }
        else {
          leText->GetCompleter()->SetFieldType(iCodeCompleter::EXPRESSION);
        }
      }
    }
    else {
      leText = new iLineEdit(act_par, false);
    }
    lay->addWidget(leText, 1);
    
    btnEdit = new QToolButton(act_par);
    btnEdit->setText("...");
    btnEdit->setToolTip(taiMisc::ToolTipPreProcess(tooltip));
    btnEdit->setFixedHeight(taiM->text_height(defSize()));
    lay->addWidget(btnEdit);
    
    SetRep(act_par);
    connect(btnEdit, SIGNAL(clicked(bool)), this, SLOT(btnEdit_clicked(bool)) );
  }
  else {
    bool add_completer = (lookupfun_md && (md->HasOption("ADD_COMPLETER_EXPR") || lookupfun_md->HasOption("ADD_COMPLETER_SIMPLE")));
    if (add_completer) {
      leText = new iLineEdit(gui_parent_, true);
      leText->GetCompleter()->SetHostType(iCodeCompleter::DIALOG_HOST);
      if (lookupfun_md->HasOption("ADD_COMPLETER_SIMPLE")) {
        leText->GetCompleter()->SetFieldType(iCodeCompleter::SIMPLE);
      }
      else {
        leText->GetCompleter()->SetFieldType(iCodeCompleter::EXPRESSION);
      }
    }
    else if (cssi_arg_dlg) {
      leText = new iLineEdit(gui_parent_, true);
      if (leText->GetCompleter()) {
        cssi_arg_dlg->code_completer = leText->GetCompleter();
        leText->GetCompleter()->SetHostType(iCodeCompleter::DIALOG_HOST);
        leText->GetCompleter()->SetFieldType(iCodeCompleter::SIMPLE);
      }
    }
    else {
      leText = new iLineEdit(gui_parent_, false);
    }
    SetRep(leText);
  }
  
  QFont font = rep()->font();
  font.setPointSize(taMisc::GetCurrentFontSize("labels"));
  rep()->setFont(font);
  
  rep()->setFixedHeight(taiM->text_height(defSize()));
  if (readOnly()) {
    rep()->setReadOnly(true);
  }
  else {
    QObject::connect(rep(), SIGNAL(textChanged(const QString&) ), this, SLOT(repChanged() ) );
  }
  
  // cliphandling connections
  QObject::connect(rep(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged() ) );
  QObject::connect(rep(), SIGNAL(lookupKeyPressed(iLineEdit*)), this, SLOT(lookupKeyPressed()) );
  QObject::connect(rep(), SIGNAL(characterEntered(iLineEdit*)), this, SLOT(characterEntered()) );
}

iLineEdit* taiWidgetText::rep() const { return leText; }

void taiWidgetText::GetImage(const String& val) {
  if(!rep()) return;
  rep()->setText(val);
  // the field may not show the full text if it is too small
  if (GetValue().nonempty()) {
    rep()->setToolTip(GetValue());
  }
  else {
    if (Base()) {
      rep()->setToolTip(Base()->GetDesc());
    }
  }
}

String taiWidgetText::GetValue() const {
  if(!rep()) return _nilString;
  return rep()->text();
}

void taiWidgetText::selectionChanged() {
  emit_UpdateUi();
}

void taiWidgetText::setMinCharWidth(int num) {
  rep()->setMinCharWidth(num);
}

void taiWidgetText::setCharWidth(int num) {
  rep()->setCharWidth(num);
}

void taiWidgetText::this_GetEditActionsEnabled(int& ea) {
  if(!rep()) return;
  if (!readOnly())
    ea |= iClipData::EA_PASTE;
  if (rep()->hasSelectedText()) {
    ea |= (iClipData::EA_COPY);
    if (!readOnly())
      ea |= (iClipData::EA_CUT |  iClipData::EA_DELETE);
  }
}

void taiWidgetText::this_EditAction(int ea) {
  if(!rep()) return;
  if (ea & iClipData::EA_CUT) {
    rep()->cut();
  } else if (ea & iClipData::EA_COPY) {
    rep()->copy();
  } else if (ea & iClipData::EA_PASTE) {
    rep()->paste();
  } else if (ea & iClipData::EA_DELETE) {
    rep()->del(); //note: assumes we already qualified with hasSelectedText, otherwise it is a BS
  }
}

void taiWidgetText::this_SetActionsEnabled() {
  //TODO: UNDO/REDO
}

