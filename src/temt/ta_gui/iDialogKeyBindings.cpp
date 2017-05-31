// Copyright 2016, Regents of the University of Colorado,
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
#include <KeyBindings_List>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#if (QT_VERSION >= 0x050200)
#include <QKeySequenceEdit>
#endif
#include <QPushButton>
#include <QGroupBox>
#include <QFile>
#include <QScrollArea>
#include <QSize>

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
//  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

iDialogKeyBindings::iDialogKeyBindings() {
  for (int i=0; i<10; i++) {
    bindings_layout[i] = NULL;
  }
}

iDialogKeyBindings::~iDialogKeyBindings() {
}

void iDialogKeyBindings::Constr() {
  QGroupBox* header_box = new QGroupBox("");
  QGroupBox* body_box = new QGroupBox("");
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  
  layOuter->setSizeConstraint(QLayout::SetDefaultConstraint);
  setMinimumSize(400, 700 );

  // two boxes
  layOuter->addWidget(header_box);
  layOuter->addWidget(body_box);
  
  // instruction box
  QHBoxLayout* header_layout = new QHBoxLayout();
#if (QT_VERSION >= 0x050200)
  String instruction_str = "To Set:    Press a key or key combination and tab out \nTo Clear: Press any single modifier key \nHover over label for user friendly version of key sequence";
#else
  String instruction_str = "Only available since QT5.2";
#endif
  QLabel* instruction = new QLabel(instruction_str);
  header_layout->addWidget(instruction);
  header_box->setLayout(header_layout);
  
#if (QT_VERSION >= 0x050200)
  // set key bindings box - broken down by tabs
  QHBoxLayout* body_layout = new QHBoxLayout();
  QTabWidget* tab_widget = new QTabWidget;
  body_layout->addWidget(tab_widget);
  body_box->setLayout(body_layout);
  
  String filename = taMisc::GetCustomKeyFilename();  // this file contains the current user set bindings
  KeyBindings* default_bindings = taMisc::key_binding_lists->SafeEl(static_cast<int>(taMisc::KEY_BINDINGS_DEFAULT));
  taMisc::KeyBindingSet current_key_set = taMisc::current_key_bindings;
  taMisc::SetKeyBindingSet(taMisc::KEY_BINDINGS_CUSTOM);

  String context_label;
  String action_label;
  int context_count = static_cast<int>(taiMisc::CONTEXT_COUNT);
  for (int ctxt=0; ctxt<context_count; ctxt++) {
    taiMisc::BindingContext current_context = taiMisc::BindingContext(ctxt);
    context_label = TA_taiMisc.GetEnumString("BindingContext", ctxt);
    context_label = context_label.before("_CONTEXT");  // strip off "_CONTEXT"
    QWidget* a_tab = new QWidget();
    body_layout->QLayout::addWidget(tab_widget);
    
    QScrollArea *scroll_area = new QScrollArea(tab_widget);  // tab_widget is the parent
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(a_tab);  // each tab has a scrollarea
    
    tab_widget->addTab(scroll_area, context_label);
    
    // add all of the actions that can be bound to keys
    bindings_layout[ctxt] = new QFormLayout;
    bindings_layout[ctxt]->setLabelAlignment(Qt::AlignLeft);
    a_tab->setLayout(bindings_layout[ctxt]);
    
    int action_count = static_cast<int>(taiMisc::ACTION_COUNT);
    for (int i=0; i<action_count; i++) {
      action_label = TA_taiMisc.GetEnumString("BoundAction", i);
      if (action_label.startsWith(context_label)) {
        action_label = action_label.after(context_label + "_");
        QLabel* action = new QLabel(action_label);
        taiMisc::BoundAction current_action = taiMisc::BoundAction(i);
        QKeySequence key_seq = taiMisc::GetSequenceFromAction(current_context, current_action);
        QKeySequenceEdit* edit = new QKeySequenceEdit(key_seq);
        
        // if different from default - highlight - would be nice if it also worked when you edit, probably need to subclass QKeySequenceEdit
        KeyActionPair_PArray* default_pairs = default_bindings->CurrentBindings(static_cast<taiMisc::BindingContext>(ctxt));
        QKeySequence default_key_seq = default_pairs->GetKeySequence(current_action);
        if (key_seq.matches(default_key_seq) == 0) {
          edit->setStyleSheet("background-color:yellow;");
        }
        else {
          edit->setStyleSheet("background-color:white;");
        }
        
        // get the most readable form of the key sequence
        int kb_set = taMisc::KEY_BINDINGS_DEFAULT;
        String key_seq_str = taiMisc::GetSequenceFromActionFriendly(current_context, current_action, kb_set);
        KeyActionPair* pair = default_pairs->GetPairFromAction(current_action);
        String help_str;
        if (pair) {
          help_str = pair->tooltip;
          if (help_str.nonempty()) {
            help_str = "   (" + help_str  + ")";
          }
        }
        action->setToolTip("Default: " + key_seq_str + help_str);
        bindings_layout[ctxt]->addRow(action, edit);
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
  button_cancel = new QPushButton("&Cancel", this);
  button_layout->addWidget(button_cancel);
  layOuter->addLayout(button_layout);
  
  connect(button_ok, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(button_cancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

void iDialogKeyBindings::accept() {
  inherited::accept();
  
#if (QT_VERSION >= 0x050200)
  String filename = taMisc::prefs_dir + PATH_SEP + "custom_keys";
  QFile file(filename);
  if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
    return;
  }
  QDataStream out(&file);
  
  QLabel* action = NULL;
  QKeySequenceEdit* key_seq_edit = NULL;
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