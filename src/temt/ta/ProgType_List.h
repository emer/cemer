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

#ifndef ProgType_List_h
#define ProgType_List_h 1

// parent includes:
#include <ProgType>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Program; // 
class DynEnumType; // 
class taBase; // 


TypeDef_Of(ProgType_List);

class TA_API ProgType_List : public taList<ProgType> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of script variables
INHERITED(taList<ProgType>)
public:
  virtual void          GenCss(Program* prog) const; // generate css script code for the context
  virtual const String  GenListing(int indent_level) const; // generate the listing of program

  virtual DynEnumType* NewDynEnum();
  // #BUTTON #MENU_CONTEXT create a new DynEnumType (shortcut)

  virtual taBase* FindTypeName(const String& nm) const;
  // find given type name (e.g., dynamic enum type or value) on list

  override String GetTypeDecoKey() const { return "ProgType"; }

  override bool         BrowserSelectMe();
  override bool         BrowserExpandAll();
  override bool         BrowserCollapseAll();

  void  setStale();
  TA_BASEFUNS_NOCOPY(ProgType_List);
protected:
  override void El_SetIndex_(void*, int);

private:
  void  Initialize();
  void  Destroy() {Reset();}
};

#endif // ProgType_List_h
