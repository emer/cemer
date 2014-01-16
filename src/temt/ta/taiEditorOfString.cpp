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

#include "taiEditorOfString.h"
#include <iTextBrowser>
#include <iNumberedTextView>
#include <iDiffTextView>
#include <MemberDef>
#include <iTextEdit>
#include <iDialog>
#include <iDialogEditor>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextEdit>
#include <QDialog>
#include <QKeyEvent>


taiEditorOfString::taiEditorOfString(MemberDef* mbr_, void* base_, TypeDef* typ_,
     bool read_only_, bool modal_, QObject* parent, bool line_nos_,
                                     bool rich_text_, bool diffs_)
:inherited(typ_ ,read_only_, modal_, parent)
{
  root = base_;
  mbr = mbr_;
  edit = NULL;
  btnPrint = NULL;
  line_nos = line_nos_;
  rich_text = rich_text_;
  diffs = diffs_;
  fancy_edit = NULL;
}

taiEditorOfString::~taiEditorOfString() {
}

void taiEditorOfString::Constr(const char* prompt, const char* win_title) {
  inherited::Constr(prompt, win_title);
}

void taiEditorOfString::Constr_Box() {
  if(diffs) {
    iDiffTextView* ntv = new iDiffTextView(widget());
    vblDialog->addWidget(ntv, 1);
    fancy_edit = ntv;
  }
  else if(line_nos) {
    iNumberedTextView* ntv = new iNumberedTextView(widget());
    edit = ntv->textEdit();
    vblDialog->addWidget(ntv, 1);
    fancy_edit = ntv;
  }
  else {
    if(read_only) {
      edit = new iTextBrowser(widget());
      ((iTextBrowser*)edit)->setOpenExternalLinks(true);
    }
    else {
      edit = new iTextEdit(widget());
    }
    vblDialog->addWidget(edit, 1);
  }
  if(edit) {
    edit->installEventFilter(this); // hopefully everyone below body will get it too!
    edit->setFontPointSize(taMisc::font_size);
  }
}

void taiEditorOfString::Constr_RegNotifies() {
  taBase* rbase = Base_(); // cache
  if (rbase) {
    rbase->AddSigClient(this);
  }
}

void taiEditorOfString::Constr_Strings() {
//NO  inherited::Constr_Strings(prompt_str_, win_str_); // for if non-empty
  taBase* rbase = Base_(); // cache
  if (rbase && mbr) {
    win_str = "Editing " + rbase->GetPathNames() + ":" + mbr->GetLabel();
  }
  if (mbr) {
    prompt_str = mbr->GetLabel() + ": " + mbr->desc;
  }

}

void taiEditorOfString::Constr_Buttons() {
  inherited::Constr_Buttons();
  // printing is now declared silly..
//   btnPrint = new QPushButton("&Print", widget());
//   layButtons->addSpacing(16);
//   layButtons->addWidget(btnPrint, 0, (Qt::AlignVCenter));
//   connect(btnPrint, SIGNAL(clicked()), this, SLOT(btnPrint_clicked()) );
}


void taiEditorOfString::btnPrint_clicked() {
  QPrinter pr;
  QPrintDialog pd(&pr, widget());
  if (pd.exec() != iDialog::Accepted) return;
  // print ...
  edit->document()->print(&pr);
}

void taiEditorOfString::SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {
}

void taiEditorOfString::DoConstr_Dialog(iDialogEditor*& dlg) {
  inherited::DoConstr_Dialog(dlg);
  if(diffs) {
    iSize sz = taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgVer);
    sz.w *= 2;
    dlg->resize( sz );
  }
  else {
    dlg->resize( taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgVer) );
  }
}


void taiEditorOfString::GetImage() {
  const String val = mbr->type->GetValStr(mbr->GetOff(root), root, mbr);
  if(diffs) {
    ((iDiffTextView*)fancy_edit)->setDiffString(val);
  }
  else if(rich_text) {
    edit->setHtml(val);
  }
  else {
    edit->setPlainText(val);
  }
}

void taiEditorOfString::GetValue() {
  if(!diffs) {
    String val = edit->toPlainText();
    mbr->type->SetValStr(val, mbr->GetOff(root), root, mbr);
  }
}

void taiEditorOfString::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, etc. etc.
  if (modified) {
    GetValue();
  }
}

void taiEditorOfString::Ok_impl() { //note: only used for Dialogs
  inherited::Ok_impl();
  //  if (modified) {
    GetValue();
    Unchanged();
    //  }
}

void taiEditorOfString::SelectLines(int st_line, int end_line) {
  if(!edit) return;
  QTextCursor tc = edit->textCursor();
  tc.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
  if(st_line > 1) {
    tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, st_line-1);
  }
  if(end_line >= st_line) {
    tc.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, (end_line-st_line)+1);
  }
  edit->setTextCursor(tc);
}

bool taiEditorOfString::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* e = static_cast<QKeyEvent *>(event);
    bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
    if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
      Ok();                     // do it!
      return true;
    }
    if(e->key() == Qt::Key_Escape) {
      Cancel();                 // do it!
      return true;
    }
  }
  return QObject::eventFilter(obj, event);
}

