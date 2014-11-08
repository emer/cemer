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
#include <NameVar_PArray>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(KeyBindings);

class TA_API KeyBindings : public taNBase {
  // ##INLINE ##NO_TOKENS ##CAT_UserData A set of key bindings for shortcuts on menu items and for keyPressEvent interpretation
INHERITED(taNBase)
public:
  enum Binding_Context {
    MAIN_WINDOW_CONTEXT,             // bindings for main menubar
    LINE_EDIT_CONTEXT                // bindings for single line editing, inline editing of code
  };
  
  NameVar_PArray            main_window_bindings;
  NameVar_PArray            line_edit_bindings;
  
  virtual bool              Add(Binding_Context context, String action, String key_sequence);
  // add a name value pair to the list of key bindings
  virtual String            KeySequence(Binding_Context context, String action);
  // retrieve the key sequence bound to this context and action (e.g. MAIN_WINDOW_CONTEXT, "view_browse_only")
  virtual String            Action(Binding_Context context, String key_sequence);
  
  TA_SIMPLE_BASEFUNS(KeyBindings);

protected:
  virtual NameVar_PArray*   CurrentBindings(Binding_Context context);
  // #IGNORE retrieve pointer to list of bindings for the current Binding_Context
  
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // KeyBindings_h
