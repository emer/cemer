// Copyright 2017-2017, Regents of the University of Colorado,
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

#ifndef Completions_h
#define Completions_h 1

// parent includes:
#include <taNBase>
#include <String_Array>
#include <taBase_List>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(Completions);

class TA_API Completions : public taNBase {
  // An object that holds all of the various lists that might be shown for code completion
INHERITED(taNBase)
public:
  taBase_List             object_completions;   // objects that derive from taBase
  MemberSpace             member_completions;   // members of objects
  MethodSpace             method_completions;   // methods of objects
  EnumSpace               enum_completions;     // enums
  TypeSpace               static_completions;   // statics - list is generated in taMisc
  TypeSpace               misc_completions;     // classes that have end user enums - list is generated in taMisc; other classes of misc nature could also be on this list
  String_Array            string_completions;   // these are strings like "NULL", true/false, etc that are not taBase objects
  String                  seed;                 // the text that starts the lookup
  String                  pre_text;             // text preceding completion
  String                  append_text;          // text after completion
  String                  pre_cursor_text;    // all text up to current cursor position
  
  void                    Reset();              // remove items from all lists
  bool                    HasCompletions();     // at least one list has items
  
  TA_SIMPLE_BASEFUNS(Completions);
  
protected:

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // Completions_h
