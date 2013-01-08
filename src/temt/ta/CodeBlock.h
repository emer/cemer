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

#ifndef CodeBlock_h
#define CodeBlock_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 
class TypeDef; // 


class TA_API CodeBlock: public ProgEl { 
  // a block of code (list of program elements), each executed in sequence
INHERITED(ProgEl)
public:
  ProgEl_List	    	prog_code; // list of Program elements: the block of code

  override int 		ProgElChildrenCount() const { return prog_code.size; }

 virtual ProgEl*	AddProgCode(TypeDef* el_type)	{ return (ProgEl*)prog_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new program code element

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);

//no  override taList_impl*	children_() {return &prog_code;}	
  override ProgVar*	FindVarName(const String& var_nm) const;
  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "block"; }

  PROGEL_SIMPLE_BASEFUNS(CodeBlock);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		PreGenChildren_impl(int& item_id);
  override void		GenCssPre_impl(Program* prog); 
  override void		GenCssBody_impl(Program* prog);
  override void		GenCssPost_impl(Program* prog); 
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
};

#endif // CodeBlock_h
