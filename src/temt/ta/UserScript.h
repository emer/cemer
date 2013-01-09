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

#ifndef UserScript_h
#define UserScript_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:

class TA_API UserScript: public ProgEl { 
  // a user-defined css script (can access all program variables, etc)
INHERITED(ProgEl)
public:
  ProgExpr		script;	// the css (C++ syntax) code to be executed

  virtual void	    	ImportFromFile(istream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css import script from file
  virtual void	    	ImportFromFileName(const String& fnm); // import script from file
  virtual void	    	ExportToFile(ostream& strm); // #MENU_ON_Object #MENU_CONTEXT #BUTTON #EXT_css export script to file
  virtual void	    	ExportToFileName(const String& fnm); // export script to file
  
  override void		SetProgExprFlags();
  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "script"; }
  override String 	GetTypeDecoKey() const { return "Comment"; }

  PROGEL_SIMPLE_BASEFUNS(UserScript);
protected:
  override void UpdateAfterEdit_impl();
  override void		GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{}
};

#endif // UserScript_h
