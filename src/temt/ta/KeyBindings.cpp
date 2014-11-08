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


#include <NameVar>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(KeyBindings);

NameVar_PArray* KeyBindings::CurrentBindings(Binding_Context context) {
  switch (context) {
    case MAIN_WINDOW_CONTEXT:
      return &main_window_bindings;
      break;
    case LINE_EDIT_CONTEXT:
      return &line_edit_bindings;
    default:
      return NULL;
      break;
  }
}

bool KeyBindings::Add(Binding_Context context, String action, String key_sequence) {
  bool rval = false;
  NameVar_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    // if neither action nor value are already defined add to list
    if ((context_bindings->FindName(action) == -1) && (context_bindings->FindValue(key_sequence) == -1)) {
      context_bindings->Add(NameVar(action, key_sequence));
      rval = true;
    }
  }
  return rval;
}

String KeyBindings::KeySequence(Binding_Context context, String action) {
  NameVar_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    Variant sequence = context_bindings->GetVal(action);
    return sequence.toString();
  }
  else {
    return String("");
  }
}

String KeyBindings::Action(Binding_Context context, String key_sequence) {
  NameVar_PArray* context_bindings = CurrentBindings(context);
  if (context_bindings) {
    Variant action = context_bindings->GetName(key_sequence);
    return action.toString();
  }
  else {
    return String("");
  }
}