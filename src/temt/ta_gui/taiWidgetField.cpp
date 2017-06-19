
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

#include "taiWidgetField.h"
#include <iDialogWidgetField>
#include <iLineEdit>
#include <MemberDef>
#include <iTextEdit>
#include <BuiltinTypeDefs>
#include <taMisc>
#include <iCodeCompleter>
#include <Completions>
#include <ProgExprBase>
#include <taiWidgetCompletionChooser>

#define completion_chooser

taiWidgetField::taiWidgetField(TypeDef* typ_, IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef* md, taBase* base)
  : taiWidgetText(typ_, host_, par, gui_parent_, flags_, (flags_ & flgEditDialog),
            "Edit this field in a multi-line dialog.", md, base)
  , edit_dialog(0)
{
  // min width for some popular types
  if (typ) {
    if (typ->DerivesFrom(TA_float))
      setMinCharWidth(8);
    else if (typ->DerivesFrom(TA_double))
      setMinCharWidth(16);
    else // just put a lower pixel bound on size, since charwidth is exp to compute
      rep()->setMinimumWidth(100);
  }
}

taiWidgetField::~taiWidgetField() {
  delete edit_dialog;
  edit_dialog = 0;
}

void taiWidgetField::btnEdit_clicked(bool) {
  if (!edit_dialog) { // has to be modeless
    String wintxt;
    String desc;
    //TODO: we could in theory trap the raw GetImage and derive the object parent
    // to provide additional information, such as the object name (if base)
    if (mbr) {
      wintxt = "Editing field: " + mbr->name;
      desc = mbr->desc;
    }
    else {
      wintxt = "Editing field";
      //desc =
    }
    edit_dialog = new iDialogWidgetField(false, readOnly(), desc, this);
    // mismatch between modal=true and calling show does NOT work in 5.4.1 -- is now
    // modeless
    edit_dialog->setText(rep()->text());
    edit_dialog->setWindowTitle(wintxt);
    QObject::connect(edit_dialog->txtText, SIGNAL(lookupKeyPressed()),
                     this, SLOT(lookupKeyPressed_dialog()) );
  }
  edit_dialog->show();
  edit_dialog->raise();
}

void taiWidgetField::lookupKeyPressed() {
  if(!lookupfun_md || !lookupfun_base) return;
  
  taBase* tab = (taBase*)lookupfun_base;
  int cur_pos = rep()->cursorPosition();
  int new_pos = -1;
  Completions* completions = NULL;

#ifdef completion_chooser
  completions = tab->StringFieldLookupForCompleter(rep()->text(), cur_pos, lookupfun_md->name, new_pos);
  rep()->GetCompleter()->SetCompletions(completions);
#else
  String rval = tab->StringFieldLookupFun(rep()->text(), cur_pos, lookupfun_md->name, new_pos);
#endif

#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
  
#ifdef completion_chooser
  // Experiment to test out chooser before converting over to full use
  taiWidgetCompletionChooser* chooser = new taiWidgetCompletionChooser(NULL, NULL, NULL, NULL, 0, completions->seed);
  chooser->SetCompletions(completions);
  bool ok_choice = chooser->OpenChooser();
  
  if (ok_choice) {
    String selection_text = chooser->GetSelectionText();
    rep()->setText(selection_text + ProgExprBase::completion_append_text);
  }
#else
  if(rval.nonempty()) {
    rep()->setText(rval);
    if(new_pos >= 0)
      rep()->setCursorPosition(new_pos); // go back to orig pos
    else
      rep()->setCursorPosition(cur_pos); // go back to orig pos
  }
#endif  // completion_chooser
}

void taiWidgetField::lookupKeyPressed_dialog() {
  if(!lookupfun_md || !lookupfun_base) return;
  if(!edit_dialog) return;

  QTextCursor cursor(edit_dialog->txtText->textCursor());

  taBase* tab = (taBase*)lookupfun_base;
  int cur_pos = cursor.position();
  int new_pos = -1;
  String rval = tab->StringFieldLookupFun(edit_dialog->txtText->toPlainText(), cur_pos,
                                          lookupfun_md->name, new_pos);
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
  if(rval.nonempty()) {
    edit_dialog->txtText->setPlainText(rval);
    QTextCursor cur2(edit_dialog->txtText->textCursor());
    if(new_pos >= 0)
      cur2.setPosition(new_pos);
    else
      cur2.setPosition(cur_pos);
    edit_dialog->txtText->setTextCursor(cur2);
  }
}

void taiWidgetField::characterEntered() {
  if(!lookupfun_md || !lookupfun_base) return;
  taBase* tab = (taBase*)lookupfun_base;
  
  int cur_pos = rep()->cursorPosition();
  int new_pos = -1;
  iCodeCompleter* completer = rep()->GetCompleter();
  if (completer) {
    completion_list.Reset();
    if (completer->field_type == iCodeCompleter::SIMPLE) {
      tab->GetListForCompletion(lookupfun_md, completion_list);
      rep()->GetCompleter()->SetModelList(&completion_list);
    }
    else {  // iCodeCompleter::EXPRESSION
      Completions* completions = tab->StringFieldLookupForCompleter(rep()->text(), cur_pos, lookupfun_md->name, new_pos);
      rep()->GetCompleter()->SetCompletions(completions);
    }
  }
  
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
}
