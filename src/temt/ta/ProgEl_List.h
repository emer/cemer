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

#ifndef ProgEl_List_h
#define ProgEl_List_h 1

// parent includes:
#include <ProgEl>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Program; // 
class ProgVar; // 


TypeDef_Of(ProgEl_List);

class TA_API ProgEl_List: public taList<ProgEl> {
  // #TREEFILT_ProgGp ##CAT_Program list of program elements: a block of code
INHERITED(taList<ProgEl>)
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

  SIMPLE_LINKS(ProgEl_List);
  TA_BASEFUNS(ProgEl_List);
protected:
  override void UpdateAfterEdit_impl();
private:
  void  Copy_(const ProgEl_List& cp);
  void  Initialize();
  void  Destroy();
};

#endif // ProgEl_List_h
