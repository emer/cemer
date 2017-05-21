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
class Function; //
class taBase_PtrList; //

taTypeDef_Of(ProgEl_List);

class TA_API ProgEl_List: public taList<ProgEl> {
  // #TREEFILT_ProgGp ##CAT_Program ##NO_UNIQUE_NAME_WARNING list of program elements: a block of code
INHERITED(taList<ProgEl>)
public:
  String_Array          acceptable_types;     // #HIDDEN #NO_SAVE types that are ok for this list instance
  String_Array          unacceptable_types;   // #HIDDEN #NO_SAVE types that are NOT ok for this list instance
  bool                  check_with_parent;    // #HIDDEN #NO_SAVE if type is unacceptable should the parent be consulted?
  
  virtual void          PreGen(int& item_id); // iterates over all items
  virtual void          GenCss(Program* prog); // generate the Css code for this object
  virtual const String  GenListing(int indent_level = 0) const;
  // generate the listing of this program

  virtual ProgVar*      FindVarName(const String& var_nm) const;
  // find given variable within this progel list -- NULL if not found

  int          NumListCols() const override {return 2;}
  const        KeyString GetListColKey(int col) const override;
  String       GetColHeading(const KeyString& key) const override;

  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  virtual bool BrowserEditTest();
  // #EXPERT test the parsing functions of all program elements in list -- just compares output of BrowserEditString before and after running BrowserEditSet on BrowserEditString -- does the thing parse its own output string?

  bool         RemoveIdx(int idx) override;   // override to delete brk_pt before ProgEl is removed

  void         ReplaceLater(ProgEl* old_el, ProgEl* new_el, const String& fun_on_repl = "");
  // #IGNORE replace old_el with new_el into given location, and optionally do a delayed function call on new replacement guy
  void         DoReplaceLater();
  // actually do the later replacement that was setup earlier

  virtual void  AddAcceptableType(const String& type);
  // Add the name of a type that is allowed in this list - only used if unacceptable list is empty
  virtual void  AddUnacceptableType(const String& type);
  // Add the name of a type that is NOT allowed in this list

  bool  IsAcceptable(taBase* candidate) override;
 // is this candidate item acceptable for this list - Only used in drag and drop - must be at least type or subtype of el_base - Use the Add methods to include/exclude types

  virtual void UpdateProgElVars(const taBase* old_scope, taBase* new_scope);
  // #IGNORE update prog el vars after scope change (move, copy)
  
  virtual void GetProgramCallFuns(taBase_PtrList& callers, const Function* callee);
  // adds to the callers list any ProgramCallFuns that call the callee (must match name and program)
    
  SIMPLE_LINKS(ProgEl_List);
  TA_BASEFUNS(ProgEl_List);
protected:
  ProgEl_List*  el_to_repl;  // element(s) to replace with another -- in pairs: old, new
  String_Array* el_to_repl_fun; // function name(s) to call on new after replacing -- blank added to keep in register with els_to_repl list

  void         UpdateAfterEdit_impl() override;
private:
  void  Copy_(const ProgEl_List& cp);
  void  Initialize();
  void  Destroy();
};

#endif // ProgEl_List_h
