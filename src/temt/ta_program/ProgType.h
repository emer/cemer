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

#ifndef ProgType_h
#define ProgType_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>

// member includes:

// declare all other types mentioned but not required to include:
class Program; // 
taTypeDef_Of(Program);
class taBase; // 


taTypeDef_Of(ProgType);

class TA_API ProgType: public taNBase {
  // #STEM_BASE #NO_INSTANCE #VIRT_BASE ##EDIT_INLINE ##SCOPE_Program ##CAT_Program a program type -- base class for defining new types within a program
INHERITED(taNBase)
public:
  String        desc;   // #EDIT_DIALOG Description of this type

  virtual const String  GenCssType() const; // type name
  virtual void          GenCss(Program* prog); // generate css code to program
  virtual const String  GenListing(int indent_level); // generate listing of program

  virtual taBase*       FindTypeName(const String& nm) const;
  // find given type name (e.g., dynamic enum type or value) on variable

  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  bool                  BrowserEditSet(const String& code, int move_after = 0) override;
  virtual  String       CodeGetDesc(const String& code);
  // #IGNORE get description (comment) from the code string -- returns code without the comment

  String       GetDesc() const override { return desc; }
  String       GetTypeDecoKey() const override { return "ProgType"; }
  void         SetDefaultName() override {} // make it local to list, set by list
  TA_SIMPLE_BASEFUNS(ProgType);
protected:
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
  virtual void          GenCssPre_impl(Program* prog) {}; // #IGNORE generate the Css prefix code (if any) for this object
  virtual void          GenCssBody_impl(Program* prog) {}; // #IGNORE generate the Css body code for this object
  virtual void          GenCssPost_impl(Program* prog) {}; // #IGNORE generate the Css postfix code (if any) for this object
private:
  void  Initialize();
  void  Destroy();
};

SMARTREF_OF(TA_API, ProgType); // ProgTypeRef

#endif // ProgType_h
