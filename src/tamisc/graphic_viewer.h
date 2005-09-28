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



/*
 * GlyphViewer - glyph that handles input for glypheditor
 */

#ifndef glyphviewer_h
#define glyphviewer_h

#include <iv_graphic/graphic.h>

#include <ta/enter_iv.h>
#include <InterViews/event.h>
#include <InterViews/input.h>
#include <InterViews/action.h>
#include <ta/leave_iv.h>

class ivCursor;			// #IGNORE
class dpIOHandler;		// #IGNORE
class GlyphGrabber;		// #IGNORE


class GlyphViewer : public ivInputHandler {
  // ##NO_INSTANCE ##NO_MEMBERS ##NO_TOKENS ##NO_CSS
public:
  GlyphViewer(float w, float h, const ivColor* bg);
  virtual ~GlyphViewer();
  
  ivCursor*	zoom_in_cursor;
  ivCursor* 	zoom_out_cursor;
  double	zoom_factor;
  int		last_button;

  virtual void	press(const ivEvent&);
  virtual void	drag(const ivEvent&);
  virtual void	release(const ivEvent&);
  virtual bool	manip_recenter(const ivEvent&);
  // recenter viewer while manipulating if mouse goes to edge of root object..
  
  void		SetZoom(int toggle=true){_zoom = toggle;}
  void		SetScroll(int toggle=true){_grab= toggle;}

  virtual void	request(ivRequisition&) const;
  virtual void	allocate(ivCanvas*, const ivAllocation&, ivExtension&);
  virtual void	draw(ivCanvas*, const ivAllocation&) const;
  virtual void	pick(ivCanvas*, const ivAllocation&, int depth, ivHit&);
  
  virtual GraphicMaster*	root();
  virtual void			root(GraphicMaster*);
  
  virtual Tool&			tool();
  virtual void			cur_tool(unsigned int);

  ivCanvas* canvas() const;
  
  void damage(ivCoord, ivCoord, ivCoord, ivCoord);
  void repair();
  
  void reallocate();
  void update_from_state();	// causes damage where state was updated, redraws
  void Update_All();

  ivAllocation& viewallocation()	{ return _a;}

protected:
  virtual void rate_scroll();
  virtual void grab_scroll();
  virtual void rate_zoom();
  virtual void initshape();
  virtual void initgraphic();
  virtual void initcursor();
  virtual void inittimer();
protected:
  ivCoord 	_x;
  ivCoord	_y;
  ivCoord 	_lx;
  ivCoord	_ly;
  
  float 	_width;
  float		_height;
  ivAllocation 	_a;
  
  osboolean 	_zoom;
  osboolean 	_pan;
  osboolean	_grab;
  ivCursor* 	_cursor;
  long 		_delay;
  dpIOHandler*	 _timer;
  GraphicMaster* _root;
  Tool 		_tool;
  GlyphGrabber* _grabber;
  ivCanvas*	_canvas;
private:	
  void tick(long, long);
};

#endif


