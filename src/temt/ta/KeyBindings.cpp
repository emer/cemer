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

#include "KeyBindings.h"


#include <KeyActionPair>
#include <KeyActionPair_PArray>
#include <taMisc>
#include <taiMisc>


TA_BASEFUNS_CTORS_DEFN(KeyBindings);

KeyActionPair_PArray* KeyBindings::CurrentBindings(taiMisc::BindingContext context) {
  switch (context) {
    case taiMisc::MAIN_WINDOW_CONTEXT:
      return &main_window_bindings;
    case taiMisc::TEXT_EDIT_CONTEXT:
      return &line_edit_bindings;
    case taiMisc::CONSOLE_CONTEXT:
      return &console_bindings;
    case taiMisc::DIALOG_CONTEXT:
      return &dialog_bindings;
    case taiMisc::PANEL_CONTEXT:
      return &panel_bindings;
    case taiMisc::TABLE_CONTEXT:
      return &table_bindings;
    case taiMisc::TREE_CONTEXT:
      return &tree_bindings;
    case taiMisc::GRAPHICS_CONTEXT:
      return &graphics_bindings;
    default:
      return NULL;
      break;
  }
}

bool KeyBindings::Add(taiMisc::BindingContext context, taiMisc::BoundAction action,  QKeySequence key_sequence) {
  bool rval = false;
  KeyActionPair_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    // okay to bind 2 key sequences to the same action
    // NOT okay to have the same key sequence bound to 2 actions
    if ((context_bindings->FindKeySequence(key_sequence) == -1)) {
      context_bindings->Add(KeyActionPair(key_sequence, action));
      rval = true;
    }
  }
  return rval;
}

QKeySequence KeyBindings::KeySequence(taiMisc::BindingContext context, taiMisc::BoundAction action) {
  KeyActionPair_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    return context_bindings->GetKeySequence(action);
  }
  return QKeySequence("");
}

taiMisc::BoundAction KeyBindings::Action(taiMisc::BindingContext context,  QKeySequence key_sequence) {
  KeyActionPair_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    return context_bindings->GetAction(key_sequence);
  }
  else {
    taMisc::Error("KeyActionPair_PArray not found, did you forget to add the context to KeyBindings::CurrentBindings()");
  }
  return taiMisc::NULL_ACTION;
}