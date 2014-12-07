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

#include "iDialogKeyBindings.h"

#include <taMisc>
#include <taiMisc>
#include <iMainWindowViewer>
#include <EnumSpace>
#include <TypeDef>
#include <KeyBindings>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#if (QT_VERSION >= 0x050200)
#include <QKeySequenceEdit>
#endif
#include <QPushButton>
#include <QGroupBox>
#include <QFile>

iDialogKeyBindings* iDialogKeyBindings::New(iMainWindowViewer* par_window_)
{
  iDialogKeyBindings* rval = new iDialogKeyBindings(par_window_);
//  wflg &= ~Qt::WindowStaysOnTopHint;
//  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iDialogKeyBindings::iDialogKeyBindings(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(true);
  setWindowTitle("Key Binding Preferences");
  setFont(taiM->dialogFont(taiMisc::fonSmall));
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

iDialogKeyBindings::iDialogKeyBindings() {
}

iDialogKeyBindings::~iDialogKeyBindings() {
  delete temp_bindings;
}

void iDialogKeyBindings::Constr() {
  QGroupBox* header_box = new QGroupBox("");
  QGroupBox* body_box = new QGroupBox("");
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  
  // two boxes
  layOuter->addWidget(header_box);
  layOuter->addWidget(body_box);
  
  // instruction box
  QHBoxLayout* header_layout = new QHBoxLayout();
#if (QT_VERSION >= 0x050200)
  String instruction_str = "To Set:    Press a key or key combination and tab out \nTo Clear: Press any single modifier key";
#else
  String instruction_str = "Only available since QT5.2";
#endif
  QLabel* instruction = new QLabel(instruction_str);
  header_layout->addWidget(instruction);
  header_box->setLayout(header_layout);
  
#if (QT_VERSION >= 0x050200)
  // set key bindings box - broken down by tabs
  QHBoxLayout* body_layout = new QHBoxLayout();
  QTabWidget* tabWidget = new QTabWidget;
  body_layout->addWidget(tabWidget);
  body_box->setLayout(body_layout);
  
  String filename = taMisc::GetCustomKeyFilename();

  taMisc::KeyBindingSet current_key_set = taMisc::current_key_bindings;
  taMisc::SetKeyBindingSet(taMisc::KEY_BINDINGS_CUSTOM);

  temp_bindings = new KeyBindings();  // build a list of current bindings for modification and saving
  String context_label;
  String action_label;
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int ctxt=0; ctxt<context_count; ctxt++) {
    taiMisc::BindingContext current_context = taiMisc::BindingContext(ctxt);
    context_label = TA_taiMisc.GetEnumString("BindingContext", ctxt);
    context_label = context_label.before("_CONTEXT");  // strip off "_CONTEXT"
    QWidget* some_tab = new QWidget();
    tabWidget->addTab(some_tab, context_label);
    body_layout->QLayout::addWidget(tabWidget);
    
    // add all of the actions that can be bound to keys
    bindings_layout[ctxt] = new QFormLayout;
    bindings_layout[ctxt]->setLabelAlignment(Qt::AlignLeft);
    some_tab->setLayout(bindings_layout[ctxt]);
    
    QKeySequenceEdit* edit;
    int action_count = static_cast<int>(taiMisc::ACTION_COUNT);
    for (int i=0; i<action_count; i++) {
      action_label = TA_taiMisc.GetEnumString("BoundAction", i);
      if (action_label.startsWith(context_label)) {
        action_label = action_label.after(context_label + "_");
        QLabel* action = new QLabel(action_label);
        taiMisc::BoundAction current_action = taiMisc::BoundAction(i);
        QKeySequence key_seq = taiMisc::GetSequenceFromAction(current_context, current_action);
        QKeySequenceEdit* edit = new QKeySequenceEdit(key_seq);
        bindings_layout[ctxt]->addRow(action, edit);
        temp_bindings->Add(current_context, current_action, key_seq);
      }
    }
  }
  taMisc::SetKeyBindingSet(current_key_set);
#endif
  
  QHBoxLayout* button_layout = new QHBoxLayout();
  button_layout->addStretch();
  button_ok = new QPushButton("&Ok", this);
  button_ok->setDefault(true);
  button_layout->addWidget(button_ok);
  button_layout->addSpacing(taiM->vsep_c);
  button_revert = new QPushButton("&Revert", this);
  button_layout->addWidget(button_revert);
  layOuter->addLayout(button_layout);

  connect(button_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(button_revert, SIGNAL(clicked()), this, SLOT(reject()) );
}

void iDialogKeyBindings::accept() {
  inherited::accept();
  
#if (QT_VERSION >= 0x050200)
  String filename = taMisc::prefs_dir + PATH_SEP + "custom_keys";
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly)) {
    return;
  }
  QDataStream out(&file);
  
  QLabel* action;
  QKeySequenceEdit* key_seq_edit;
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int ctxt=0; ctxt<context_count; ctxt++) {
    for (int row=0; row<bindings_layout[ctxt]->rowCount(); row++) {
      QLayoutItem* label_item = bindings_layout[ctxt]->itemAt(row,  QFormLayout::LabelRole);
      QWidgetItem* label = dynamic_cast<QWidgetItem*>(label_item);
      QLayoutItem* field_item = bindings_layout[ctxt]->itemAt(row,  QFormLayout::FieldRole);
      QWidgetItem* field = dynamic_cast<QWidgetItem*>(field_item);
      if (label) {
        action = dynamic_cast<QLabel*>(label->widget());
      }
      if (field) {
        key_seq_edit = dynamic_cast<QKeySequenceEdit*>(field->widget());
      }
      if (action && key_seq_edit) {
        // add back the context string to the action string
        String context_str = TA_taiMisc.GetEnumString("BindingContext", ctxt);
        String context_prefix = context_str.before("CONTEXT");
        String action_str = (String)action->text();
        action_str = context_prefix + action_str;
        QKeySequence ks = key_seq_edit->keySequence();
        out << (QString)context_str.chars() << (QString)action_str.chars() << ks;
      }
    }
  }
  file.close();
  taiMisc::LoadCustomKeyBindings();
#endif
}

void iDialogKeyBindings::reject() {
  inherited::reject();
}
