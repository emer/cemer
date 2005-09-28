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
 *	glyphviewer
 */ 

#include <iv_graphic/graphic_viewer.h>

#include <ta/taiv_data.h> // for taivMisc::GetButton

#include <ta/enter_iv.h>
#include <Dispatch/dispatcher.h>
#include <Dispatch/iocallback.h>
#include <Dispatch/iohandler.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/event.h>
#include <InterViews/handler.h>
#include <InterViews/hit.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/target.h>
#include <IV-look/kit.h>

#include <InterViews/bitmap.h>
#include <InterViews/Bitmaps/enlargeMask.bm>
#include <InterViews/Bitmaps/enlargeHit.bm>
#include <InterViews/Bitmaps/reducerMask.bm>
#include <InterViews/Bitmaps/reducerHit.bm>
#include <InterViews/cursor.h>
#include <InterViews/window.h>

#include <ta/leave_iv.h>
#include <math.h>


class GlyphGrabber : public ivHandler {
public:
    GlyphGrabber(GlyphViewer*, ivAllocation*);
    virtual osboolean event(ivEvent&);
#ifdef CYGWIN
  int		grabber_handling;
#endif
protected:
  bool		manipulated;
  GlyphViewer*	_gviewer;
  ivAllocation*	_a;
};

GlyphGrabber::GlyphGrabber(GlyphViewer* gviewer, ivAllocation* a) {
    _gviewer = gviewer;
    _a = a;
    manipulated = false;
#ifdef CYGWIN
    grabber_handling = 0;
#endif
}

osboolean GlyphGrabber::event (ivEvent& e) {
#ifdef CYGWIN
  // need to make this reentrant because otherwise the event code
  // is totally unusable! 
  // handling = 0 = nothing going on, 1 = in bgd, 2 = quit bkd,
  // 3 = in obj, 4 = quit obj
  if(grabber_handling == 1) {
    Tool& tool = _gviewer->tool();
    ivCanvas* c = _gviewer->canvas(); // use the original canvas, not the one from event
    switch(tool.tool()) {
    case Tool::move: _gviewer->SetScroll(); break;
    case Tool::scale:_gviewer->SetZoom();  break;
    }
    if(e.type() == ivEvent::up) {
      _gviewer->release(e);
      grabber_handling = 2;	// exit condition
      c->pop_clipping();
    }
    else _gviewer->drag(e);
    return true;		// don't double-process events!
  }
  else if(grabber_handling == 2) {
    return true;
  }
  else if(grabber_handling == 3) {
    Graphic* root = _gviewer->root();
    Tool& tool = _gviewer->tool();
    ivCanvas* c = _gviewer->canvas(); // use the original canvas, not the one from event
    _gviewer->manip_recenter(e);
    bool ok = root->manipulating(e, tool);
    manipulated = manipulated || ok;
    if(!ok) {
      grabber_handling = 4;	// exit condition
      ok = root->effect(e, tool);
      if(!manipulated)		// if not manipulating, then must be selecting!
	root->select(e,tool); 
      else
	root->select(e,tool,true);	// unselect!
      c->pop_clipping();
    }
    return true;
  }
  else if(grabber_handling == 4) {
    return true;
  }
#endif
  _gviewer->last_button = taivM->GetButton(e);
  Graphic* root = _gviewer->root();
  Tool& tool = _gviewer->tool();
  ivCanvas* c = _gviewer->canvas(); // use the original canvas, not the one from event
  // because it strays into other windows!!
  //e.window()->canvas();
  c->push_clipping();
  c->clip_rect(_a->left(), _a->bottom(), _a->right(), _a->top());
  bool ok=true;

  if(tool.tool() == Tool::nop) {
    return ok;
  }

  if(tool.tool() == Tool::pick) {
    if(e.type() == ivEvent::down) {
      ok = root->pick_me(e,tool);
    }
    c->pop_clipping();
    return ok;
  }
    
  if(tool.tool() == Tool::select) {
    if(e.type() == ivEvent::down) {
      root->select(e,tool);
    }
    c->pop_clipping();
    return ok;
  }

  ok = root->grasp(e, tool);
  ivEvent ev;
  ivSession* s = ivSession::instance();

  // this prevents the while loops from going on forever, in case something goes wrong
  int max_manip_loops = 10000;

  if(ok == false) { // we didn't grasp anything so affect background
    root->effect(e, tool);// get rid of selection
    root->select(e,tool);	// deselect everything
    switch(tool.tool()) {
    case Tool::move:
    case Tool::scale:
      _gviewer->press(e); ok = true; break;
    }
#ifdef CYGWIN
    grabber_handling = 1;
    int manips = 0;
    while((grabber_handling == 1) && (manips++ < max_manip_loops)) {
      s->read(ev);
      ev.handle();		// all events *must* be handled immediately
    }
    grabber_handling = 0;
#else
    int manips = 0;
    while(ok && (manips++ < max_manip_loops)) {
      s->read(ev);
      switch(tool.tool()) {
      case Tool::move: _gviewer->SetScroll(); break;
      case Tool::scale:_gviewer->SetZoom();  break;
      }
      if(ev.type() == ivEvent::up) {
	_gviewer->release(ev);
	ok = false;
      }
      else _gviewer->drag(ev);
    }
    c->pop_clipping();
#endif
    return false;
  }

  manipulated = false;
#ifdef CYGWIN
  grabber_handling = 3;
  int manips = 0;
  while((grabber_handling == 3) && (manips++ < max_manip_loops)) {
    s->read(ev);
    ev.handle();		// all events *must* be handled immediately
  }
  grabber_handling = 0;
#else
  int manips = 0;
  root = _gviewer->root();
  while(ok && (manips++ < max_manip_loops)) {
    s->read(ev);
    _gviewer->manip_recenter(ev);
    ok = root->manipulating(ev, tool);
    manipulated = manipulated || ok;
  }
  ok = root->effect(ev, tool);
  if(!manipulated)		// if not manipulating, then must be selecting!
    root->select(e,tool); 
  else
    root->select(e,tool,true);	// unselect!
  c->pop_clipping();
#endif
  return ok;
}    


GlyphViewer::GlyphViewer(float w, float h, const ivColor* bg)
: ivInputHandler(new ivTarget(_root = new GraphicMaster, TargetAlwaysHit), 
	       ivSession::instance()->style())
{
  // added by CKD 4/24/93 since _canvas was not being set;
  _canvas = nil;
  _root->background(bg);
  _x = -1.0;
  _y = -1.0;
  _lx = -1.0;
  _ly = -1.0;
  _width = w;
  _height = h;
  _grabber = new GlyphGrabber(this, &_a);
  _grabber->ref();

  _zoom = _pan = _grab = false;
  initshape();
  initgraphic();
  zoom_in_cursor = nil;
  zoom_out_cursor= nil;
  zoom_factor = 400.0;
  initcursor();
  inittimer();
}

GlyphViewer::~GlyphViewer () {
    delete _timer;
    _grabber->unref();
    if (zoom_in_cursor) { delete zoom_in_cursor; zoom_in_cursor = NULL; }
    if (zoom_out_cursor) { delete zoom_out_cursor; zoom_out_cursor = NULL; }
}

void GlyphViewer::initcursor () {    
    if (zoom_in_cursor == nil) {
        ivBitmap* zoom_in = new ivBitmap(
            enlarger_hit_bits, enlarger_hit_width, enlarger_hit_height,
            enlarger_hit_x_hot, enlarger_hit_y_hot
        );
        ivBitmap* zoom_in_mask = new ivBitmap(
            enlarger_mask_bits, enlarger_mask_width, enlarger_mask_height,
            enlarger_mask_x_hot, enlarger_mask_y_hot
        );
        
        ivBitmap* zoom_out = new ivBitmap(
            reducer_hit_bits, reducer_hit_width, reducer_hit_height,
            reducer_hit_x_hot, reducer_hit_y_hot
        );
        ivBitmap* zoom_out_mask = new ivBitmap(
            reducer_mask_bits, reducer_mask_width, reducer_mask_height,
            reducer_mask_x_hot, reducer_mask_y_hot
        );
        
        zoom_in_cursor = new ivCursor(zoom_in, zoom_in_mask);
        zoom_out_cursor = new ivCursor(zoom_out, zoom_out_mask);
    } 
    _cursor = nil;
}

// #include <ta/enter_iv.h>
// declareActionCallback(GlyphViewer)
// implementActionCallback(GlyphViewer)

#include <ta/enter_iv.h>
declareIOCallback(GlyphViewer)
implementIOCallback(GlyphViewer)
#include <ta/leave_iv.h>


// #include <ta/leave_iv.h> 


void GlyphViewer::inittimer () {
    float seconds = 1.0;
    _delay = long(seconds * 1000000);
    _timer = new IOCallback(GlyphViewer)(this, &GlyphViewer::tick);
}

Tool& GlyphViewer::tool () {
    return _tool;
}

void GlyphViewer::cur_tool (unsigned int t) {
    _tool.tool(t);
}


GraphicMaster* GlyphViewer::root () { return _root; }

ivCanvas* GlyphViewer::canvas() const {
  if(_canvas == nil) {
    ((GlyphViewer*)this)->_canvas = ivInputHandler::canvas();
  }
  return _canvas;
}

void GlyphViewer::root (GraphicMaster* root) {
//    if (_root != root) {
  ivExtension ext;
  ivAllocation a;
  root->transformer(nil);
  root->background(_root->background());
  _root->allocate(nil, a, ext);
  _root = root;
  _root->viewer(this);
  body(new ivTarget(_root,TargetAlwaysHit));
  initgraphic();
  ivCanvas* c = canvas();
  if (c != nil) {
    _root->canvas(c);
    ivRequisition req;
    //_root->flush();
    _root->request(req);
    _root->allocate(c, _a, ext);
  }
}

void GlyphViewer::initshape () {
    if (_width < -0.5 && _height < -0.5) {
        ivCoord l, b, r, t;
        _root->getbounds(l, b, r, t);
        _width = r - l;
        _height = t - b;
    }
    ivAllotment& ax = _a.x_allotment();
    ivAllotment& ay = _a.y_allotment();
    ax.span(_width);
    ay.span(_height);
}

void GlyphViewer::initgraphic () {
    ivCoord l, b, r, t;
    _root->getbounds(l, b, r, t);
    _root->translate(-l-(r-l)/2.0, -b-(t-b)/2.0);
    ivCanvas* c = canvas();
    if (c != nil) {
        c->damage_all();
    }
}

void GlyphViewer::damage (ivCoord l, ivCoord b, ivCoord r, ivCoord t) {
    ivCanvas* c = canvas();
    if (c != nil) {
        c->damage(l, b, r, t);
    }
}

void GlyphViewer::repair () {
    ivCanvas* c = canvas();
    if (c != nil) {
        c->window()->repair();
    }
}

void GlyphViewer::update_from_state() {
  ivCanvas* c = canvas();
  if(c == nil)
    c = _canvas;
  if(c != nil)
    _root->update_from_state(c);
}

void GlyphViewer::Update_All() {
  ivCanvas* c = canvas();
  if(c != nil) {
    _root->damage_me(c);
  }
}
    
void GlyphViewer::press(const ivEvent& e) {
  last_button = taivM->GetButton(e);
  ivWindow* w = canvas()->window();
  _cursor = w->cursor();
  _zoom = _pan = false;
  _x = e.pointer_x();
  _y = e.pointer_y();
  Dispatcher::instance().startTimer(0, _delay/40, _timer);
}

void GlyphViewer::drag(const ivEvent& e) {
    _lx = e.pointer_x();
    _ly = e.pointer_y();
    if (_zoom == false && _pan == false && _grab == false) {
        if (e.left_is_down()) {
            _pan = true;
            rate_scroll();
        } else if (e.middle_is_down()) {
            _grab = true;
            grab_scroll();
        } else if (e.right_is_down()) {
            _zoom = true;
            rate_zoom();
        }
    } else if (_grab == true) {
        grab_scroll();
    } else if (_zoom == true) {
        rate_zoom();
    } else if (_pan == true) {
        rate_scroll();
    }
}

void GlyphViewer::release(const ivEvent&) {
    ivWindow* w = canvas()->window();
    w->cursor(_cursor);
    _zoom = _pan = _grab = false;
    //_root->flush();
    Dispatcher::instance().stopTimer(_timer);
}

bool GlyphViewer::manip_recenter(const ivEvent& e) {
  ivCoord curx = e.pointer_x();
  ivCoord cury = e.pointer_y();

  ivCoord l = _a.left();
  ivCoord b = _a.bottom();
  ivCoord r = _a.right();
  ivCoord t = _a.top();
  ivCoord redge = l + .975f * (r-l);
  ivCoord ledge = l + .025f * (r-l);
  ivCoord tedge = b + .975f * (t-b);
  ivCoord bedge = b + .025f * (t-b);
    
  ivCoord delta_x = 0.0f;
  ivCoord delta_y = 0.0f;

  ivCoord move_rate = .02f;

  if(curx > redge)
    delta_x = -move_rate * (r-l);
  else if(curx < ledge)
    delta_x = move_rate * (r-l);

  if(cury > tedge)
    delta_y = -move_rate * (t-b);
  else if(cury < bedge)
    delta_y = move_rate * (t-b);

  if((delta_x != 0.0f) || (delta_y != 0.0f)) {
    canvas()->damage(l, b, r, t);
    _root->translate(delta_x, delta_y);
    _root->getbounds(l, b, r, t);
    canvas()->damage(l, b, r, t);

    ToolState& ts = _tool.toolstate(); // reset offset!
    ts.init_ptr_x += delta_x;
    ts.init_ptr_y += delta_y;
    return true;
  }
  return false;
}

void GlyphViewer::tick(long, long) {
    if (_zoom == true) {
        rate_zoom();
    } else if (_grab == true) {
        grab_scroll();
    } else if (_pan == true) {
        rate_scroll();
    }
    Dispatcher::instance().startTimer(0, _delay/40, _timer);
}

void GlyphViewer::grab_scroll () {
    ivWindow* w = canvas()->window();
    ivWidgetKit* kit = ivWidgetKit::instance();
    
    ivCoord dx = _lx - _x;
    ivCoord dy = _ly - _y;
    
    if (dx != 0.0 || dy != 0.0) {
        ivCoord l, b, r, t;
        w->cursor(kit->hand_cursor());
	if(last_button == ivEvent::middle) dy = 0;
	else if(last_button == ivEvent::right) dx = 0;
        _root->getbounds(l, b, r, t);
        canvas()->damage(l, b, r, t);
        _root->translate(dx, dy);
        _root->getbounds(l, b, r, t);
        canvas()->damage(l, b, r, t);
        _x += dx;
        _y += dy;
    }
}

void GlyphViewer::rate_scroll () {        
    ivWindow* w = canvas()->window();
    ivWidgetKit* kit = ivWidgetKit::instance();
    
    ivCoord dx = _x - _lx;
    ivCoord dy = _y - _ly;
    
    if (dx != 0.0 && dy != 0.0) {
        double angle = atan2(dy, dx)*180/M_PI;
        
        if (angle < -157.5) {
            w->cursor(kit->rfast_cursor());
        } else if (angle < -112.5) {
            w->cursor(kit->rufast_cursor());
        } else if (angle < -67.5) {
            w->cursor(kit->ufast_cursor());
        } else if (angle < -22.5) {
            w->cursor(kit->lufast_cursor());
        } else if (angle < 22.5) {
            w->cursor(kit->lfast_cursor());
        } else if (angle < 67.5) {
            w->cursor(kit->ldfast_cursor());
        } else if (angle < 112.5) {
            w->cursor(kit->dfast_cursor());
        } else if (angle < 157.5) {
            w->cursor(kit->rdfast_cursor());
        } else {
            w->cursor(kit->rfast_cursor());
        }
    }
    
    ivCoord l, b, r, t;
    _root->getbounds(l, b, r, t);
    canvas()->damage(l, b, r, t);
    _root->translate(dx, dy);
    _root->getbounds(l, b, r, t);
    canvas()->damage(l, b, r, t);
}
    
void GlyphViewer::rate_zoom () {        
    ivWindow* w = canvas()->window();
    
    ivCoord dy = _y - _ly;
    
    if (dy != 0.0) {
        double factor;
        if (dy > 0.0) {
            w->cursor(zoom_out_cursor);
            factor = zoom_factor/(zoom_factor+dy);
            
        } else {
            w->cursor(zoom_in_cursor);
            factor = (zoom_factor-dy)/zoom_factor;
        }
        ivCoord l, b, r, t;
        _root->getbounds(l, b, r, t);
        canvas()->damage(l, b, r, t);
	double xfactor = factor;
	double yfactor = factor;
	if(last_button == ivEvent::middle) yfactor = 1.0;
	else if(last_button == ivEvent::right) xfactor = 1.0;
        _root->scale(xfactor, yfactor, _x, _y);
        _root->getbounds(l, b, r, t);
        canvas()->damage(l, b, r, t);
    }
}
    
void GlyphViewer::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    _canvas = c;
    _root->canvas(c);
    _a = a;
    ivInputHandler::allocate(c, a, ext);
    ext.merge(c, a);
}

void GlyphViewer::reallocate(){
  ivCanvas* c = canvas();
  if (c != nil) {
    _root->canvas(c);
    ivRequisition req;
    _root->request(req);
    ivExtension ext;
    _root->allocate(c, _a, ext);
  }
}

void GlyphViewer::request(ivRequisition& req) const {
    ivRequirement& rx = req.x_requirement();
    rx.natural(_width);
    rx.stretch(fil);
    rx.shrink(_width);
    rx.alignment(0.0);
    
    ivRequirement& ry = req.y_requirement();
    ry.natural(_height);
    ry.stretch(fil);
    ry.shrink(_height);
    ry.alignment(0.0);
}

void GlyphViewer::draw(ivCanvas* c, const ivAllocation& a) const {
    _root->draw(c, a);
}

void GlyphViewer::pick(ivCanvas* c, const ivAllocation& a, int depth, ivHit& h) {
    const ivEvent* e = h.event();
    if (e->control_is_down()) {
        ivInputHandler::pick(c, a, depth, h); }
    else if (e->type() == ivEvent::up || e->type() == ivEvent::down) {
        h.begin(depth, this, 0, _grabber);
        ivMonoGlyph::pick(c, a, depth, h);
        h.end();
    }
#ifdef CYGWIN
    else if((_grabber->grabber_handling > 0) && (e->type() == ivEvent::motion)) {
        h.begin(depth, this, 0, _grabber);
        ivMonoGlyph::pick(c, a, depth, h);
        h.end();
    }
#endif
}

