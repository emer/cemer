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


