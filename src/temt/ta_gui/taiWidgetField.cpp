// Copyright 2013-2017, Regents of the University of Colorado,
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

#include <taMisc>
#include <MemberDef>
#include <MethodDef>
#include <BuiltinTypeDefs>
#include <Completions>
#include <DataTable>
#include <ProgExprBase>

#include <iDialogWidgetField>
#include <iLineEdit>
#include <iTextEdit>
#include <iCodeCompleter>
#include <taiWidgetCompletionChooser>

#include <css_qtdialog.h>


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
    edit_dialog->setText(rep()->text(), leText->cursorPosition());
    edit_dialog->setWindowTitle(wintxt);
    QObject::connect(edit_dialog->txtText, SIGNAL(lookupKeyPressed()), this, SLOT(lookupKeyPressed_dialog()) );
    QObject::connect(edit_dialog->txtText, SIGNAL(characterEntered()), this, SLOT(characterEntered_dialog()) );
  }
  edit_dialog->show();
  edit_dialog->raise();
}

void taiWidgetField::lookupKeyPressed() {
  cssiArgDialog* cssi_arg_dlg = dynamic_cast<cssiArgDialog*>(host);
  if (cssi_arg_dlg) {
    arg_completions.Reset();
    String reference_arg;  // the arg that holds a pointer to the object from which we can get a list
    taBase* class_base = (taBase*)host->Root();
    String cur_text = leText->text();
    if (class_base) {
      if (label()) {
        reference_arg = class_base->GetArgForCompletion(cssi_arg_dlg->md->name, label()->text());
        
        taBase* arg_obj = NULL;
        if (reference_arg) {
          arg_obj = cssi_arg_dlg->GetBaseForArg(reference_arg);
        }
        String pre_text = cur_text;
        if (cur_text.contains('.')) {
          pre_text = cur_text.before('.');
        }
        class_base->GetArgCompletionList(cssi_arg_dlg->md->name, label()->text(), arg_obj, pre_text, arg_completions);
        rep()->GetCompleter()->SetCompletions(&arg_completions);
      }
    }
    if (arg_completions.HasCompletions()) {
      taiWidgetCompletionChooser* chooser = new taiWidgetCompletionChooser(NULL, NULL, NULL, NULL, 0, &arg_completions.seed);
      chooser->SetCompletions(&arg_completions);
      bool ok_choice = chooser->OpenChooser();
      
      if (ok_choice) {
        String selection_text = chooser->GetSelectionText();
        rep()->GetCompleter()->chooser_selection = selection_text;
        rep()->setText(selection_text);
        rep()->GetCompleter()->chooser_selection = _nilString;
      }
      delete chooser;
    }
    return;
  }

  // wasn't a cssi_arg_dialog
  if(!lookupfun_md || !lookupfun_base) return;
  
  taBase* tab = (taBase*)lookupfun_base;
  int cur_pos = rep()->cursorPosition();
  int new_pos = -1;
  Completions* expr_completions = NULL;
  iCodeCompleter* completer = rep()->GetCompleter();
  if (completer) {
    member_completions.Reset();
    if (completer->field_type == iCodeCompleter::SIMPLE) {
      String text = rep()->text();
      String pre_text = text;
      if (text.contains('.')) {
        pre_text = text.before('.');
      }
      tab->GetMemberCompletionList(lookupfun_md, pre_text, member_completions);
      if (pre_text != text) {
        member_completions.pre_text = pre_text + ".";
      }
      rep()->GetCompleter()->SetCompletions(&member_completions);
    }
    else {  // iCodeCompleter::EXPRESSION
      expr_completions = tab->StringFieldLookupForCompleter(rep()->text(), cur_pos, lookupfun_md->name, new_pos);
      rep()->GetCompleter()->SetCompletions(expr_completions);
    }
  }

#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
  
  if (expr_completions) {
    taiWidgetCompletionChooser* chooser = new taiWidgetCompletionChooser(NULL, NULL, NULL, NULL, 0, expr_completions->seed);
    chooser->SetCompletions(expr_completions);
    bool ok_choice = chooser->OpenChooser();
    
    if (ok_choice) {
      String selection_text = chooser->GetSelectionText();
      rep()->GetCompleter()->chooser_selection = selection_text;
      rep()->setText(expr_completions->pre_text + selection_text + expr_completions->append_text);
      rep()->GetCompleter()->chooser_selection = _nilString;
    }
  }
  else if (member_completions.HasCompletions()) {
    taiWidgetCompletionChooser* chooser = new taiWidgetCompletionChooser(NULL, NULL, NULL, NULL, 0, &member_completions.seed);
    chooser->SetCompletions(&member_completions);
    bool ok_choice = chooser->OpenChooser();
    
    if (ok_choice) {
      String selection_text = chooser->GetSelectionText();
      rep()->setText(selection_text);
    }
    delete chooser;
  }
}

void taiWidgetField::lookupKeyPressed_dialog() {
  if(!lookupfun_md || !lookupfun_base) return;
  if(!edit_dialog) return;

  QTextCursor cursor(edit_dialog->txtText->textCursor());

  taBase* tab = (taBase*)lookupfun_base;
  int cur_pos = cursor.position();
  int new_pos = -1;
  
  Completions* completions = NULL;
  
  if (rep()->GetCompleter()) {
    completions = tab->StringFieldLookupForCompleter(edit_dialog->txtText->toPlainText(), cur_pos, lookupfun_md->name, new_pos);
    rep()->GetCompleter()->SetCompletions(completions);
  }
  
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
  
  taiWidgetCompletionChooser* chooser = new taiWidgetCompletionChooser(NULL, NULL, NULL, NULL, 0, completions->seed);
  chooser->SetCompletions(completions);
  bool ok_choice = chooser->OpenChooser();
  
  if (ok_choice) {
    String selection_text = chooser->GetSelectionText();
    edit_dialog->txtText->setPlainText(completions->pre_text + selection_text + completions->append_text);
    QTextCursor cur2(edit_dialog->txtText->textCursor());
    new_pos = cur_pos + selection_text.length() + completions->seed.length();
    cur2.setPosition(new_pos);
    edit_dialog->txtText->setTextCursor(cur2);
  }
  delete chooser;
}

void taiWidgetField::characterEntered() {
  arg_completions.Reset();
  member_completions.Reset();
  cssiArgDialog* cssi_arg_dlg = dynamic_cast<cssiArgDialog*>(host);
  if (cssi_arg_dlg) {
    String reference_arg;  // the arg that holds a pointer to the object from which we can get a list
    String cur_text = leText->text();
    taBase* class_base = (taBase*)host->Root();
    if (class_base) {
      if (label()) { // why isn't the label always set - conditional field might be the issue
        if (!cur_text.contains('.')) {
          reference_arg = class_base->GetArgForCompletion(cssi_arg_dlg->md->name, label()->text());
          taBase* arg_obj = NULL;
          if (reference_arg) {
            arg_obj = cssi_arg_dlg->GetBaseForArg(reference_arg);
          }
          // this is a hack for SetMember to show enum values in value field
          // could generalize if there are other similar cases
          if (cssi_arg_dlg->md->name == "SetMember" && label()->text() == "value") {
            String member_name = cssi_arg_dlg->GetArgValue(0);
            arg_completions.string_completions.Add(member_name);
          }
          class_base->GetArgCompletionList(cssi_arg_dlg->md->name, label()->text(), arg_obj, cur_text, arg_completions);
          rep()->GetCompleter()->SetCompletions(&arg_completions);
          return;
        }
        else {
          MemberDef* md = NULL;
          String member_name = cur_text.before('.', -1);
          class_base->FindMembeR(member_name, md);
          TypeDef* td = NULL;
          if (md) {
            td = md->type;
          }
          else {  // special case?
            td = ProgExprBase::GetSpecialCaseType(member_name);
          }
          if (td) {
            for (int i=0; i<td->members.size; i++) {
              MemberDef* member_md = td->members.FastEl(i);
              if (!member_md->IsEditorHidden()) {
//                if (!member_md->IsGuiReadOnly() && !member_md->IsEditorHidden()) {
                arg_completions.member_completions.Link(member_md);
              }
            }
            arg_completions.pre_text = member_name + '.';
            rep()->GetCompleter()->SetCompletions(&arg_completions);
          }
        }
        return;
      }
    }
  }
  
  // wasn't a cssi_arg_dialog
  if(!lookupfun_md || !lookupfun_base) return;
  
  taBase* tab = (taBase*)lookupfun_base;
  int cur_pos = rep()->cursorPosition();
  int new_pos = -1;
  
  iCodeCompleter* completer = rep()->GetCompleter();
  if (completer) {
    if (completer->field_type == iCodeCompleter::SIMPLE) {
      String text = rep()->text();
      String pre_text = text;
      if (text.contains('.')) {
        pre_text = text.before('.');
      }
      tab->GetMemberCompletionList(lookupfun_md, pre_text, member_completions);
      if (pre_text != text) {
        member_completions.pre_text = pre_text + ".";
      }
      rep()->GetCompleter()->SetCompletions(&member_completions);
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

void taiWidgetField::characterEntered_dialog() {
  if(!lookupfun_md || !lookupfun_base) return;
  
  taBase* tab = (taBase*)lookupfun_base;

  if (!edit_dialog) return;
  
  int cur_pos = edit_dialog->txtText->textCursor().position();
  int new_pos = -1;
  iCodeCompleter* completer = edit_dialog->txtText->GetCompleter();
  if (completer) {
    arg_completions.Reset();
    member_completions.Reset();
    if (completer->field_type == iCodeCompleter::SIMPLE) {
      tab->GetMemberCompletionList(lookupfun_md, edit_dialog->txtText->toPlainText(), member_completions);
      edit_dialog->txtText->GetCompleter()->SetCompletions(&member_completions);
    }
    else {  // iCodeCompleter::EXPRESSION
      Completions* completions = tab->StringFieldLookupForCompleter(edit_dialog->txtText->toPlainText(), cur_pos, lookupfun_md->name, new_pos);
      edit_dialog->txtText->GetCompleter()->SetCompletions(completions);
    }
  }
  
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
}
