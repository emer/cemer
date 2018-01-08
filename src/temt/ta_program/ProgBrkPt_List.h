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

#ifndef ProgBrkPt_List_h
#define ProgBrkPt_List_h 1

// parent includes:
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class ProgBrkPt; //
class ProgEl; //
class ProgLine; //

taTypeDef_Of(ProgBrkPt_List);

class TA_API ProgBrkPt_List : public taList<ProgBrkPt> {
  // #NO_DEFAULT_CHILD A list of breakpoints for an individual program to support viewing and enabling
INHERITED(taList<ProgBrkPt> )
public:
#ifdef __MAKETA__ // dummy to supress New
  taBase*         New_gui(int n_objs=1, TypeDef* typ = NULL); // #HIDDEN
#endif
  
  ProgBrkPt*      AddBrkPt(ProgEl* prog_el, ProgLine* codeline);
  void            DeleteBrkPt(ProgEl* prog_el);
  ProgBrkPt*      FindBrkPt(ProgEl* prog_el) const;

  bool            BrowserExpandAll() override;

  virtual void    EnableAll();
  // #MENU #MENU_CONTEXT #MENU_ON_BreakPoints #CAT_Code enable all breakpoints 
  virtual void    DisableAll();
  // #MENU #MENU_CONTEXT #MENU_ON_BreakPoints #CAT_Code disable all breakpoints 

  TA_SIMPLE_BASEFUNS(ProgBrkPt_List);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ProgBrkPt_List_h
