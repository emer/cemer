// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef graphic_text_h
#define graphic_text_h

#include <iv_graphic/graphic.h>
#include <iv_misc/dynalabel.h>
#include <iv_graphic/graphic_viewer.h>

class DynamicLabel;		// #IGNORE

class NoScale_Text_G : public Graphic {
public:
  GraphicMaster* 	master; 	// add lower level xforming objects after this
  TAPtr			obj;		// corresponding object to edit
  String 		name;		// text of label
  bool			editable; 	// if true, right-mouse-button will edit this object
  bool			editb_used; 	// if true, right-mouse-button was used on select

  void			(*select_effect)(void*); // #IGNORE 
  ivColor*		(*get_color)(void *);  // #IGNORE

  ivTransformer		scaled_tform;
  ivAllocation 		a;
  DynamicLabel* 	label;
  ivPatch*  		patch;
  
  void 		draw(ivCanvas*, const ivAllocation&) const;
  void 		request(ivRequisition&) const;
  void 		allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  ivGlyph* clone() const;
  
  void 		getextent_gs (ivCoord&, ivCoord&, ivCoord&, ivCoord&,
  		     ivCoord&,Graphic*);
  void 		draw_gs(ivCanvas*, Graphic*);
  
  bool  	select(const ivEvent& e, Tool& tool, bool unselect);
  bool		effect_select(bool set_select);
  bool		effect_stretch(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);
  
  bool 		manip_move(const ivEvent&,Tool&,ivCoord,ivCoord,
				   ivCoord,ivCoord,ivCoord,ivCoord);
  bool		effect_move(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);
  bool 		effect_border();
  
  void		transformer(ivTransformer* t);
  ivTransformer* transformer();
  
  void 		font(const ivFont* f);
  const 	ivFont*	font(){ return  Graphic::font();};
  
  void		text(const char* text);
  const char* 	text();

  void 		stroke(const ivColor*);
  const ivColor* stroke();
  void 		fill(const ivColor*);
  const ivColor* fill();
  
  virtual void 	init();
  
  ~NoScale_Text_G();
  NoScale_Text_G(TAPtr ob, GraphicMaster* m,const ivFont* f, const ivColor* c,
		 const char* nm, ivTransformer* t);
  GRAPHIC_BASEFUNS(NoScale_Text_G);  
};


#endif /* graphic_text_h */

