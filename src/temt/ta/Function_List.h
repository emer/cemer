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

#ifndef Function_List_h
#define Function_List_h 1

// parent includes:
#include <Function>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Program; // 
class ProgVar; // 


class TA_API Function_List: public taList<Function> {
  // #TREEFILT_ProgGp list of functions defined within a program
INHERITED(taList<Function>)
public:
  virtual void          PreGen(int& item_id); // iterates over all items
  virtual void          GenCss(Program* prog); // generate the Css code for this object
  virtual const String  GenListing(int indent_level = 0); // generate the listing of this program

  virtual ProgVar*      FindVarName(const String& var_nm) const;
  // find given variable within this progel list -- NULL if not found

  override int          NumListCols() const {return 2;}
  override const        KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const;

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  SIMPLE_LINKS(Function_List);
  TA_BASEFUNS(Function_List);
private:
  void  Copy_(const Function_List& cp);
  void  Initialize();
  void  Destroy();
};

#endif // Function_List_h
