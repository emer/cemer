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

#ifndef Comment_h
#define Comment_h 1

// parent includes:
#include <ProgEl>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(Comment);

class TA_API Comment: public ProgEl { 
  // insert a highlighted (possibly) multi-line comment -- useful for describing an upcoming chunk of code
INHERITED(ProgEl)
public:
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String        GetColText(const KeyString& key, int itm_idx = -1) const override;
  bool          BrowserEditSet(const String& code, int move_after=0) override;

  String	GetDisplayName() const override;
  String	GetTypeDecoKey() const override { return "Comment"; }
  String	GetToolbarName() const override { return "comment"; }

  PROGEL_SIMPLE_BASEFUNS(Comment);
protected:
  bool		useDesc() const override {return false;} 
  void		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // Comment_h
