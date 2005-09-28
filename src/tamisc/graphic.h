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

/*
 * planar graphics
 */

#ifndef graphic_h
#define graphic_h

#include "ta_base.h"
#include "ta_misc_TA_type.h"

#include "qtdefs.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#endif

class ivBoxObjList;		// #IGNORE
class ivBrush;			// #IGNORE
class ivColor;			// #IGNORE
class ivFont;			// #IGNORE
class GraphicList;		// #IGNORE
class ivPolyGlyph;		// #IGNORE
class osString;			// #IGNORE
class ToolState;		// #IGNORE
class ivTransformer;		// #IGNORE
class ivCanvas;			// #IGNORE
class GlyphViewer;

#define DEFAULT_BRUSH_WIDTH 0
#define SELECTED_BRUSH_WIDTH 0
#define BRUSH_SOLID 0xffff
#define BRUSH_DOT 0xcccc
#define BRUSH_DASH 0xf0f0
#define BRUSH_DASH_DOT 0xe4e4

class Tool {			// ##NO_MEMBERS ##NO_TOKENS  ##NO_CSS
public:
    enum ToolTypes {
        nop, select, pick, move, scale, stretch, rotate, alter, create
    };

    Tool(unsigned int = Tool::nop);
    virtual ~Tool();

    virtual unsigned int tool();
    virtual void tool(unsigned int);

    virtual ToolState& toolstate();
    virtual void toolstate(ToolState*);
    virtual void reset();
protected:
    unsigned int _cur_tool;
    ToolState* _toolstate;
};

// put this on every graphic subclass with the name of the class

#define GRAPHIC_BASEFUNS(y) 	TypeDef* GetTypeDef() const { return &TA_##y; } \
			        static TypeDef* StatTypeDef(int) { return &TA_##y; }


class Graphic : public ivGlyph {	// ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS
public:
  static int		GetButton(const ivEvent& e);
  static void 		corners(ivCoord& left, ivCoord& bottom, ivCoord& right,
				ivCoord& top,ivTransformer& t);
  static  TypeDef*	GetTypeDef(int)		{ return &TA_Graphic; }

  virtual TypeDef*	GetTypeDef() const	{ return &TA_Graphic; }

  bool		HasOption(const char* op) const 	{ return GetTypeDef()->HasOption(op); }
  bool		CheckList(const String_PArray& lst) const { return GetTypeDef()->CheckList(lst); }

  bool 		InheritsFrom(const char* nm) const  	{ return GetTypeDef()->InheritsFrom(nm); }
  bool 		InheritsFrom(TypeDef* it) const	{ return GetTypeDef()->InheritsFrom(it); }
  bool		InheritsFrom(const TypeDef& it) const { return GetTypeDef()->InheritsFrom(it); }

  bool 		DerivesFrom(const char* nm) const	{ return GetTypeDef()->DerivesFrom(nm); }
  bool 		DerivesFrom(TypeDef* it) const 	{ return GetTypeDef()->DerivesFrom(it); }
  bool 		DerivesFrom(const TypeDef& it)  { return GetTypeDef()->DerivesFrom(it); }

  static int 		buf_size; // #IGNORE

  virtual void 		request(ivRequisition&) const;
  virtual void 		allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  virtual void 		draw(ivCanvas*, const ivAllocation&) const;
  virtual void 		drawit(ivCanvas*);
  virtual void 		drawclipped(ivCanvas*, ivCoord, ivCoord, ivCoord, ivCoord);
  virtual void		damage_me(ivCanvas* c)
  			{ ivCoord l,b,r,t; getbounds(l,b,r,t); c->damage(l,b,r,t); }
  virtual void		safe_damage_me(ivCanvas* c);
  virtual bool		update_from_state(ivCanvas*);

  virtual bool	selectable() { return true; } // whether to even consider this item for selecting
  virtual bool	pickable() { return true; } // whether to even consider this item for picking (viewing)
  virtual bool	graspable() { return true; } // whether to even consider this item for grasping

  virtual bool 	select(const ivEvent&, Tool&, bool unselect = false);
  virtual bool 	pick_me(const ivEvent&, Tool&, bool unpick = false);
  virtual bool 	grasp(const ivEvent&, Tool&);
  virtual bool 	manipulating(const ivEvent&, Tool&);
  virtual bool 	effect(const ivEvent&, Tool&);

  virtual bool	grasp_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  virtual bool	manip_stretch(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  virtual bool	effect_stretch(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);

  virtual bool	grasp_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy);
  virtual bool	manip_move(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy);
  virtual bool	effect_move(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy);

  virtual bool	grasp_scale(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord ctrx, ivCoord ctry,ivCoord w, ivCoord h);
  virtual bool	manip_scale(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy, ivCoord ctrx, ivCoord ctry);
  virtual bool	effect_scale(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy,
				     ivCoord ctrx, ivCoord ctry);

  virtual bool	grasp_alter(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord ctrx, ivCoord ctry,ivCoord w, ivCoord h);
  virtual bool	manip_alter(const ivEvent&, Tool&,ivCoord ix, ivCoord iy, ivCoord lx, ivCoord ly,
				   ivCoord cx, ivCoord cy, ivCoord ctrx, ivCoord ctry);
  virtual bool	effect_alter(const ivEvent&, Tool&,ivCoord ix,ivCoord iy, ivCoord fx, ivCoord fy,
				     ivCoord ctrx, ivCoord ctry);

  virtual bool	grasp_rotate(const ivEvent&, Tool&,ivCoord ideg, ivCoord ctrx, ivCoord ctry);
  virtual bool	manip_rotate(const ivEvent&, Tool&,ivCoord ideg, ivCoord ldeg, ivCoord cdeg,
				     ivCoord ctrx, ivCoord ctry);
  virtual bool	effect_rotate(const ivEvent&, Tool&,ivCoord ideg, ivCoord fdeg, ivCoord ctrx, ivCoord ctry);

  virtual bool 	effect_select(bool set_select);
  virtual bool	effect_pick(bool set_pick);
  virtual bool	effect_border();

  virtual ivGlyph* 	clone() const;
  virtual void 		flush();

  virtual ivTransformer* 	transformer();
  virtual void 		transformer(ivTransformer*);
  void 			eqv_transformer(ivTransformer&);

  virtual void 		brush(const ivBrush*);
  virtual const ivBrush* 	brush();
  virtual void 		stroke(const ivColor*);
  virtual const ivColor*	stroke();
  virtual void 		fill(const ivColor*);
  virtual const ivColor* 	fill();
  virtual void 		font(const ivFont*);
  virtual const ivFont* 	font();
  virtual void 		closed(bool);
  virtual bool 	closed();
  virtual void 		curved(bool);
  virtual bool 	curved();
  virtual int 		ctrlpts(ivCoord*&, ivCoord*&) const;
  virtual void 		ctrlpts(ivCoord*, ivCoord*, int);
  virtual Graphic* 	parent();
  virtual void 		parent(Graphic*);
  virtual Graphic* 	root();

  virtual void 		translate(ivCoord dx, ivCoord dy);
  virtual void 		scale(ivCoord sx, ivCoord sy, ivCoord ctrx = 0.0, ivCoord ctry = 0.0);
  virtual void 		rotate(ivCoord angle, ivCoord ctrx = 0.0, ivCoord ctry = 0.0);
  virtual void 		align(Alignment, Graphic*, Alignment);

  virtual void 		recompute_shape();
  virtual void 		getbounds(ivCoord&, ivCoord&, ivCoord&, ivCoord&);
  virtual void 		getcenter(ivCoord&, ivCoord&);
  virtual bool 		contains(PointObj&);
  virtual bool 		intersects(BoxObj&);
  virtual bool 		is_selected()		{ return _selected; }
  virtual bool 		is_picked()		{ return _picked; }
  virtual bool 		body_is_selected(); // if any sub part is selected
  virtual bool 		body_is_picked();  // if any sub part is picked
  virtual void 		undraw();
  virtual void 		append_(Graphic*);
  virtual void 		prepend_(Graphic*);
  virtual void 		insert_(ivGlyphIndex, Graphic*);
  virtual void 		remove_(ivGlyphIndex);
  virtual void 		remove_all();
  virtual void 		replace_(ivGlyphIndex, Graphic*);
  virtual void 		change_(ivGlyphIndex);

  virtual ivGlyphIndex 	count_() const;
  virtual Graphic* 	component_(ivGlyphIndex) const;
  virtual ivGlyphIndex	index_(Graphic*) const;
  virtual void 		modified_(ivGlyphIndex);

  virtual ivGlyphIndex 	first_containing(PointObj&);
  virtual ivGlyphIndex 	last_containing(PointObj&);
  virtual ivGlyphIndex 	next_containing(PointObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_containing(PointObj&, ivGlyphIndex start);

  virtual ivGlyphIndex 	first_intersecting(BoxObj&);
  virtual ivGlyphIndex 	last_intersecting(BoxObj&);
  virtual ivGlyphIndex 	next_intersecting(BoxObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_intersecting(BoxObj&, ivGlyphIndex start);

  virtual ivGlyphIndex 	first_within(BoxObj&);
  virtual ivGlyphIndex 	last_within(BoxObj&);
  virtual ivGlyphIndex 	next_within(BoxObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_within(BoxObj&, ivGlyphIndex start);

  virtual Graphic*	breadth_first_selected();
  virtual ivGlyphIndex 	first_selected();
  virtual ivGlyphIndex 	last_selected();
  virtual ivGlyphIndex 	next_selected(ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_selected(ivGlyphIndex start);

  virtual Graphic*	breadth_first_picked();
  virtual ivGlyphIndex 	first_picked();
  virtual ivGlyphIndex 	last_picked();
  virtual ivGlyphIndex 	next_picked(ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_picked(ivGlyphIndex start);

  virtual Graphic& operator = (Graphic&);

  void 		get_original(const ivCoord*&, const ivCoord*&);
  void 		add_point (ivCoord x, ivCoord y);
  void 		add_curve (ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2);
  void 		Bspline_move_to (
			ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2);
  void 		Bspline_curve_to (
			 ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2);
  Graphic(Graphic* gr = nil);
  virtual ~Graphic ();

protected:
  const ivBrush* 	_brush;
  const ivColor* 	_stroke;
  const ivColor* 	_orig_stroke; // save this for selects..
  const ivColor* 	_fill;
  const ivFont* 	_font;
  ivTransformer*  	_t;

  bool 			_closed;
  bool 			_curved;
  bool 			_selected;
  bool 			_picked;
  int 			_ctrlpts;
  int 			_buf_size;
  ivCoord* 		_x;
  ivCoord* 		_y;

  ivCoord 		_xmin;
  ivCoord 		_xmax;
  ivCoord 		_ymin;
  ivCoord 		_ymax;
  Graphic* 		_parent;

  virtual void 		draw_gs(ivCanvas*, Graphic*);
  virtual void 		drawclipped_gs(
			       ivCanvas*, ivCoord, ivCoord, ivCoord, ivCoord, Graphic*);
  virtual void 		getextent_gs(
			     ivCoord&, ivCoord&, ivCoord&, ivCoord& ,ivCoord& ,Graphic* gs);
  virtual bool 		contains_gs(PointObj&, Graphic* gs);
  virtual bool 		intersects_gs(BoxObj&, Graphic* gs);
  virtual void 		getbounds_gs(ivCoord&, ivCoord&, ivCoord&, ivCoord&, Graphic* gs);

  virtual void 		total_gs (Graphic& gs);
  void 			parentXform(ivTransformer& t);

  virtual void 		concat_gs(Graphic* a, Graphic* b, Graphic* dest);
  virtual void 		concatXform(
			    ivTransformer* a, ivTransformer* b, ivTransformer* dest);
  virtual void 		concat(Graphic* a, Graphic* b, Graphic* dest);

/*   Helpers   */

  virtual bool 		contains_(Graphic*, PointObj&, Graphic* gs);
  virtual bool 		intersects_(Graphic*, BoxObj&, Graphic* gs);
  virtual void 		getbounds_(
			   Graphic*, ivCoord&, ivCoord&, ivCoord&, ivCoord&, Graphic* gs);
  void 	total_gs_(Graphic*, Graphic& gs);
  void 	concatgs_(Graphic*, Graphic*, Graphic*, Graphic*);
  void 	concatXform_(Graphic*, ivTransformer*, ivTransformer*, ivTransformer*);
  void 	concat_(Graphic*, Graphic*, Graphic*, Graphic*);
  void 	getextent_(Graphic*, ivCoord&, ivCoord&, ivCoord&, ivCoord&, ivCoord&,Graphic*);

  void 	draw_(Graphic*, ivCanvas*, Graphic*);
  void 	drawclipped_(Graphic*, ivCanvas*, ivCoord, ivCoord, ivCoord, ivCoord, Graphic*);
  void 	transform_(ivCoord, ivCoord, ivCoord&, ivCoord&, Graphic*);

  Graphic (const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	   const ivFont* font, bool closed, bool curved, int ivCoords,
	   ivTransformer*);
};

class PolyGraphic : public Graphic {
public:
  PolyGraphic(Graphic* = nil) ;
  virtual ~PolyGraphic();

  GRAPHIC_BASEFUNS(PolyGraphic);

  virtual void 		request(ivRequisition&) const;
  virtual void 		allocate(ivCanvas*, const ivAllocation&, ivExtension&);

  virtual bool		update_from_state(ivCanvas*);

  virtual bool 		select(const ivEvent&, Tool&, bool unselect = false);
  virtual bool 		pick_me(const ivEvent&, Tool&, bool unpick = false);

  virtual void 		undraw()			{ _body->undraw(); }

  virtual void 		append_(Graphic*);
  virtual void 		prepend_(Graphic*);
  virtual void 		insert_(ivGlyphIndex, Graphic*);
  virtual void 		remove_(ivGlyphIndex);
  virtual void 		remove_all();
  virtual void 		replace_(ivGlyphIndex, Graphic*);
  virtual void 		change_(ivGlyphIndex i)		{ _body->change(i); }

  virtual ivGlyphIndex 	count_() const 			{ return _body->count(); }
  virtual Graphic* 	component_(ivGlyphIndex i) const 	{ return (Graphic*)_body->component(i); }
  virtual ivGlyphIndex	index_(Graphic*) const;

  virtual void 		modified_(ivGlyphIndex i)		{ _body->modified(i); }
  virtual void 		flush();
  virtual ivGlyph* 	clone() const;

  virtual ivGlyphIndex 	first_containing(PointObj&);
  virtual ivGlyphIndex 	last_containing(PointObj&);
  virtual ivGlyphIndex 	next_containing(PointObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_containing(PointObj&, ivGlyphIndex start);

  virtual ivGlyphIndex 	first_intersecting(BoxObj&);
  virtual ivGlyphIndex 	last_intersecting(BoxObj&);
  virtual ivGlyphIndex 	next_intersecting(BoxObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_intersecting(BoxObj&, ivGlyphIndex start);

  virtual ivGlyphIndex 	first_within(BoxObj&);
  virtual ivGlyphIndex 	last_within(BoxObj&);
  virtual ivGlyphIndex 	next_within(BoxObj&, ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_within(BoxObj&, ivGlyphIndex start);

  virtual Graphic*	breadth_first_selected();
  virtual ivGlyphIndex 	first_selected();
  virtual ivGlyphIndex 	last_selected();
  virtual ivGlyphIndex 	next_selected(ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_selected(ivGlyphIndex start);

  virtual Graphic*	breadth_first_picked();
  virtual ivGlyphIndex 	first_picked();
  virtual ivGlyphIndex 	last_picked();
  virtual ivGlyphIndex 	next_picked(ivGlyphIndex start);
  virtual ivGlyphIndex 	prev_picked(ivGlyphIndex start);

protected:
  virtual void 	draw_gs(ivCanvas*, Graphic*);
  virtual void 	drawclipped_gs(
			       ivCanvas*, ivCoord, ivCoord, ivCoord, ivCoord, Graphic*);
  virtual bool 	contains_gs(PointObj&, Graphic*);
  virtual bool 	intersects_gs(BoxObj&, Graphic*);
  virtual void 	getextent_gs(
			     ivCoord&, ivCoord&, ivCoord&, ivCoord&, ivCoord&, Graphic* gs
			     );

protected:
  ivPolyGlyph* 		_body;
};

class GraphicMaster : public PolyGraphic {
public:

  bool		no_text_extent; // so the no_scale_text_g does not cause overscaling
  taBase_List	selectgroup;	// The Currently selected group of objects
  taBase_List	pickgroup;	// The Currently picked  group of  objects

  virtual float		GetCurrentXformScale();
  virtual float		GetCurrentYformScale();

  GraphicMaster(Graphic* = nil, const ivColor* bg = nil);
  virtual ~GraphicMaster();

  GRAPHIC_BASEFUNS(GraphicMaster);

  void 			background(const ivColor*);
  const ivColor* 	background();

  virtual bool		update_from_state(ivCanvas*);

  virtual void 		request(ivRequisition&) const;
  virtual void 		allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  virtual bool 		select(const ivEvent&, Tool&, bool unselect = false);
  virtual bool 		pick_me(const ivEvent&, Tool&, bool unpick = false);
  virtual bool 		grasp(const ivEvent&, Tool&);
  virtual bool 		manipulating(const ivEvent&, Tool&);
  virtual bool		effect(const ivEvent&, Tool&);
  virtual ivGlyph* 	clone() const;

  virtual ivCanvas* 	canvas() const;
  virtual void 		canvas(ivCanvas* c);

  virtual GlyphViewer* 	viewer() const;
  virtual void 		viewer(GlyphViewer* v);

protected:
  virtual void drawclipped_gs(
			      ivCanvas*, ivCoord, ivCoord, ivCoord, ivCoord, Graphic*
			      );
protected:
  GraphicList* 		_gr_list;
  const ivColor* 	_bg;
  ivAllocation 		_a;
  ivCanvas*		_canvas;
  GlyphViewer*		_viewer;
};

inline const ivColor* GraphicMaster::background () { return _bg; }

class Line : public Graphic {
public:
  Line (
        const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
        ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2, ivTransformer*
	);
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Line);
protected:
  virtual ~Line ();
};

class Rectangle : public Graphic {
public:
  Rectangle (
	     const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	     ivCoord l, ivCoord b, ivCoord r, ivCoord t, ivTransformer*
	     );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Rectangle);
protected:
  virtual ~Rectangle ();
};

class Circle : public Graphic {
public:
  Circle (
	  const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	  ivCoord x, ivCoord y, ivCoord r, ivTransformer*
	  );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Circle);
protected:
  virtual ~Circle ();
};

class Ellipse : public Graphic {
public:
  Ellipse (
	   const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	   ivCoord x, ivCoord y, ivCoord rx, ivCoord ry, ivTransformer*
	   );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Ellipse);
protected:
  virtual ~Ellipse ();
};

class Open_BSpline : public Graphic {
public:
  Open_BSpline (
		const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
		ivCoord* x, ivCoord* y, int ctrlpts, ivTransformer*
		);
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Open_BSpline);
protected:
  Open_BSpline(Open_BSpline*);
  virtual ~Open_BSpline ();
};

class Closed_BSpline : public Graphic {
public:
  Closed_BSpline (
		  const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
		  ivCoord* x, ivCoord* y, int ctrlpts, ivTransformer*
		  );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Closed_BSpline);
protected:
  Closed_BSpline(Closed_BSpline*);
  virtual ~Closed_BSpline ();
};

class Polyline : public Graphic {
public:
  Polyline (
	    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	    ivCoord* x, ivCoord* y, int ctrlpts, ivTransformer*
	    );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Polyline);
protected:
  virtual ~Polyline ();
};

class Polygon : public Graphic {
public:
  Polygon (
	   const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
	   ivCoord* x, ivCoord* y, int ctrlpts, ivTransformer*
	   );
  virtual ivGlyph* clone() const;

  GRAPHIC_BASEFUNS(Polygon);
protected:
  virtual ~Polygon ();
};

class Text : public Graphic {
public:
  Text (
        const ivFont* font, const ivColor* stroke, const char*, ivTransformer*
	);
  virtual void 		text(const char*);
  virtual const char* 	text();
  virtual void 		draw(ivCanvas*, const ivAllocation&) const;
  virtual void 		request(ivRequisition&) const;
  virtual void 		allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  virtual ivGlyph* 	clone() const;

  GRAPHIC_BASEFUNS(Text);
protected:
  virtual ~Text();

  virtual void getextent_gs (ivCoord&, ivCoord&, ivCoord&, ivCoord&,ivCoord&,Graphic*);
  virtual void draw_gs(ivCanvas*, Graphic*);

  virtual void init();
protected:
  osString* _text;
  ivAllocation _a;
  ivPolyGlyph* _body;
};

class ToolState { 			// ##NO_MEMBERS ##NO_TOKENS ##NO_CSS
public:
  ivCoord	init_ptr_x;
  ivCoord	init_ptr_y;
  // where the mouse was when initially pressed for drag/manipulation: use instead of _init.pointer_x(), etc.
  ivEvent _init;
  ivEvent _last;
  ivCoord _l;
  ivCoord _b;
  ivCoord _r;
  ivCoord _t;
  Graphic _gs;
};

// used for storing transforms on ta objects

class Xform : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER interviews transformer class
public:
  float a00;
  float a01;
  float a10;
  float a11;
  float a20;
  float a21;

  void 		Set(float b00,float b01, float b10, float b11, float b20, float b21);
  void 		Set(ivTransformer*); // #IGNORE
  ivTransformer* transformer();

  void Initialize();
  void Destroy()	{ };
  void Copy_(const Xform& cp);
  COPY_FUNS(Xform, taNBase);
  Xform(ivTransformer* tr);
  TA_BASEFUNS(Xform);
};


class Xform_List : public taList<Xform> {
public:
  void Initialize()	{ SetBaseType(&TA_Xform);}
  void Destroy()	{ };
  TA_BASEFUNS(Xform_List);
};

#endif  // graphic_h
