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

#ifndef KeyBindings_h
#define KeyBindings_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <KeyActionPair_PArray>
#include <taiMisc>

// declare all other types mentioned but not required to include:
class QKeySequence;

taTypeDef_Of(KeyBindings);

class TA_API KeyBindings : public taNBase {
  // ##INLINE ##NO_TOKENS ##CAT_UserData A set of key bindings for shortcuts on menu items and for keyPressEvent interpretation
INHERITED(taNBase)
public:
  KeyActionPair_PArray       menu_bindings; // #IGNORE
  KeyActionPair_PArray       project_window_bindings; // #IGNORE
  KeyActionPair_PArray       text_edit_bindings; // #IGNORE
  KeyActionPair_PArray       console_bindings; // #IGNORE 
  KeyActionPair_PArray       table_bindings; // #IGNORE
  KeyActionPair_PArray       tree_bindings; // #IGNORE 
  KeyActionPair_PArray       graphics_bindings; // #IGNORE 
  
  virtual bool                  Add(taiMisc::BindingContext context, taiMisc::BoundAction action, QKeySequence key_sequence, String tool_tip = "");
  // #IGNORE add a name value pair to the list of key bindings
  virtual bool                  Remove(taiMisc::BindingContext context, taiMisc::BoundAction action, QKeySequence key_sequence, bool check_action_only = true);
  // #IGNORE remove the pair based on either an action alone (first matching) or on action and key sequence
  virtual void                  Reset();
  // #IGNORE delete key bindings from all contexts
  virtual QKeySequence          KeySequence(taiMisc::BindingContext context, taiMisc::BoundAction action);
  // #IGNORE retrieve the key sequence bound to this context and action (e.g. PROJECTWINDOW_CONTEXT, "view_browse_only")
  virtual taiMisc::BoundAction  Action(taiMisc::BindingContext context,  QKeySequence key_sequence);
  // #IGNORE retrieve the action associated with this key_sequence (if more than one it will find the first - more than one not reasonable)
  virtual KeyActionPair_PArray*   CurrentBindings(taiMisc::BindingContext context);
  // #IGNORE retrieve pointer to list of bindings for the current BindingContext
  
  TA_SIMPLE_BASEFUNS(KeyBindings);

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // KeyBindings_h
