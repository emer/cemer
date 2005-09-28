// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#ifndef font_spec_h
#define font_spec_h

#include "ta_group.h"
#include "ifont.h"
#include "tamisc_TA_type.h"

//TEMP
class  Xform;	// #IGNORE xform for name label
class GraphicMaster;	// #IGNORE associated graphic master
class GlyphViewer;	// #IGNORE associate viewer

class NoScale_Text_G; //#IGNORE

class FontSpec : public taOBase, public iFont {
 // #INLINE a specification of a font (for Unix - X logical font description)
#ifndef __MAKETA__
typedef taOBase inherited;
#endif
public:
  String		pattern; // #READ_ONLY #NO_SAVE DEPRECATED the name of the font
//  String		prv_pat; // #READ_ONLY #NO_SAVE the previous font that was successfully obtained
  NoScale_Text_G*	text_g;  // #IGNORE an associated text glyph that can be updated if font changes

  virtual void	SelectFont();
  // #BUTTON opens font selection dialog
  virtual void 	SetFont(char* fn);
  virtual void	SetFontSize(int point_size = 10);
  // #BUTTON set font to given point size
  virtual void	SetTextG(NoScale_Text_G* txg); // set the text_g to a new one

  void	UpdateAfterEdit();
  void	CutLinks();
  void	InitLinks();
  void	Initialize();
  void	Copy_(const FontSpec& cp);
  COPY_FUNS(FontSpec,taOBase);
  TA_BASEFUNS(FontSpec);
};


class ViewLabel : public taNBase {
  // contains a label in a view display
public:
  FontSpec	 spec;		// #EDIT_INLINE specification for the font
  Xform*	 label_xform;	// #HIDDEN xform for name label
  GraphicMaster* master;	// #IGNORE associated graphic master
  GlyphViewer*   viewer;	// #IGNORE associate viewer
  void		(*select_effect)(void*);
  // #IGNORE function to call when text has been selected/unselected
  iColor*	(*get_color)(void *);
  // #IGNORE function to call to get background contrast color

/*TODO:  virtual void	SetLabelXform(Xform* xf); // #HIDDEN
  virtual void	XFontSel();
  // #BUTTON launch xfontsel program to find desired font pattern */
  virtual void	GetMasterViewer()	{ };
  // have to create an overloaded one of these for each type of view label..
/*  virtual void	MakeText();	// make text object and insert into view
  virtual void	AddToView();	// add text object to view
  virtual bool	UpdateView();	// update the view with any changes
  virtual void  RemoveFromView(); // remove from view */
//TEMP:
virtual void	SetLabelXform(Xform* xf) {} // #HIDDEN
virtual void	XFontSel() {}
// #BUTTON launch xfontsel program to find desired font pattern
virtual void	MakeText() {}	// make text object and insert into view
virtual void	AddToView() {}	// add text object to view
virtual bool	UpdateView() {return false;}	// update the view with any changes
virtual void  RemoveFromView() {} // remove from view

  void	Initialize();
  void	Destroy();
  void	InitLinks();
  void	CutLinks();
  void	UpdateAfterEdit();
  void	Copy_(const ViewLabel& cp);
  COPY_FUNS(ViewLabel, taNBase);
  TA_BASEFUNS(ViewLabel);
};

class ViewLabel_List : public taList<ViewLabel> {
  // ##NO_TOKENS #NO_UPDATE_AFTER list of ViewLabel objects
public:
  void	Initialize() 		{ };
  void 	Destroy()		{ };
  TA_BASEFUNS(ViewLabel_List);
};

#endif // font_spec_h
