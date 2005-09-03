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

#include <iv_graphic/graphic_viewer.h>

#include <ta/enter_iv.h>
#include <iv_misc/grabber.bm>
#include <iv_misc/grabberMask.bm>
#include <InterViews/bitmap.h>
#include <InterViews/brush.h>
#include <InterViews/canvas.h>
#include <InterViews/character.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/display.h>
#include <InterViews/font.h>
#include <InterViews/handler.h>
#include <InterViews/layout.h>
#include <InterViews/tformsetter.h>
#include <InterViews/transformer.h>
#include <InterViews/window.h>
#ifndef CYGWIN
#include <IV-X11/xcanvas.h>
#endif
#include <OS/list.h>
#include <OS/math.h>
#include <OS/memory.h>
#include <OS/string.h>
#include <ta/leave_iv.h>

#include <stdlib.h>
#include <ctype.h>
#include <math.h>


// from taivM, but we don't want to include that here

int Graphic::GetButton(const ivEvent& e){
  if(e.pointer_button() == ivEvent::left){
    if(e.shift_is_down() == true) return ivEvent::middle;
    if(e.meta_is_down() == true) return ivEvent::right;
    return ivEvent::left;
  }
  else return(e.pointer_button());
}


void Graphic::corners(
    ivCoord& left, ivCoord& bottom, ivCoord& right, ivCoord& top,ivTransformer& t
) {
    ivCoord x1, y1, x2, y2, x3, y3, x4, y4;
    
    t.transform(left, bottom, x1, y1);
    t.transform(left, top, x2, y2);
    t.transform(right, top, x3, y3);
    t.transform(right, bottom, x4, y4);
    left = osMath::min(x1, x2, x3, x4);
    bottom = osMath::min(y1, y2, y3, y4);
    right = osMath::max(x1, x2, x3, x4);
    top = osMath::max(y1, y2, y3, y4);
}    

int Graphic::buf_size = 256;

Graphic::Graphic (Graphic* gr) {
    if (gr != nil) {
        _brush = gr->brush();
        ivResource::ref(_brush);
        _stroke = gr->stroke();
        ivResource::ref(_stroke);
        _fill = gr->fill();
        ivResource::ref(_fill);
        _font = gr->font();
        ivResource::ref(_font);
        _closed = gr->closed();
        _curved = gr->curved();
        ivCoord* foo;
        
        _ctrlpts = gr->ctrlpts(foo, foo);
        _x = new ivCoord[buf_size];
        _y = new ivCoord[buf_size];
        _buf_size = buf_size;
        if (gr->transformer() != nil) {
            ivTransformer* tx = new ivTransformer;
            transformer(tx);
            ivResource::unref(tx);
            *_t = *gr->transformer();
        } else {
            _t = nil;
        }
    } else {
        _brush = nil;
        _stroke = nil;
        _fill = nil;
        _font = nil;

	_selected = false;
	_picked = false;

        _x = nil;
        _y = nil;
        _closed = false;
        _curved = false;
        _ctrlpts = 0;
        _x = new ivCoord[buf_size];
        _y = new ivCoord[buf_size];
        _buf_size = buf_size;
        _t = nil;
    } 
    _orig_stroke = nil;
    _xmin = _xmax = _ymin = _ymax = 0.0;
    _parent = nil;
}

Graphic::Graphic (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    const ivFont* font, bool closed, bool curved, int crds, 
    ivTransformer* t
) {
    _brush = brush;
    ivResource::ref(_brush);
    _stroke = stroke;
    ivResource::ref(_stroke);
    _orig_stroke = nil;
    _fill = fill;
    ivResource::ref(_fill);
    _font = font;
    ivResource::ref(_font);
    _closed = closed;
    _curved = curved;
    _selected = false;
    _picked = false;
    _ctrlpts = 0;
    if (crds > 0) {
        _x = new ivCoord[crds];
        _y = new ivCoord[crds];
    } else {
        _x = nil;
        _y = nil;
    }
    _buf_size = crds;
    _t = nil;
    if (t != nil) {
        ivTransformer* tx = new ivTransformer;
        transformer(tx);
        ivResource::unref(tx);
        *_t = *t;
    }
    _parent = nil;
}

Graphic::~Graphic () {
    ivResource::unref(_brush);
    ivResource::unref(_stroke);
    ivResource::unref(_orig_stroke);
    ivResource::unref(_fill);
    ivResource::unref(_font);
    ivResource::unref(_t);
    delete _x;
    delete _y;
}

int Graphic::ctrlpts (ivCoord*& x, ivCoord*& y) const{
    x = _x;
    y = _y;
    return _ctrlpts;
}

void Graphic::ctrlpts (ivCoord* x, ivCoord* y, int count) {
  delete _x;
  delete _y;
  _ctrlpts = max(count+1, buf_size);
  _x = new ivCoord[_ctrlpts];
  _y = new ivCoord[_ctrlpts];
  for (int i = 0; i < count; i++) {
    _x[i] = x[i];
    _y[i] = y[i];
  }
}

bool Graphic::update_from_state(ivCanvas*) {
  return false;
}

bool Graphic::select(const ivEvent& e, Tool&, bool unselect) {
  bool damage_done = false;

  if(unselect) {
    if(_selected)
      damage_done = effect_select(false);
  }
  else {
    if(_selected)
      damage_done = effect_select(false);
    else {
      damage_done = effect_select(true);
    }
  }
  if(damage_done) {
    ivWindow* w = e.window();
    ivCanvas* c = w->canvas();
    
    damage_me(c);
  }
  return damage_done;
}

bool Graphic::pick_me(const ivEvent& e, Tool&, bool unpick) {
  bool damage_done = false;

  if(unpick) {
    if(_picked)
      damage_done = effect_pick(false);
  }
  else {
    if(_picked)
      damage_done = effect_pick(false);
    else
      damage_done = effect_pick(true);
  }
  if(damage_done) {
    ivWindow* w = e.window();
    ivCanvas* c = w->canvas();
    
    damage_me(c);
  }
  return damage_done;
}

bool Graphic::grasp (const ivEvent& e, Tool& tool) {
  unsigned int tool_type = tool.tool();
  ToolState& ts = tool.toolstate();
    
  if (tool_type == Tool::nop)
    return false;

  ts._init = e;
  ts._last = e;
  ts.init_ptr_x = e.pointer_x();
  ts.init_ptr_y = e.pointer_y();

  getbounds(ts._l, ts._b, ts._r, ts._t);

  Graphic gs;
  ivTransformer* tx = ts._gs.transformer();
  
  ivCoord ix, iy;
  ivCoord ctrx, ctry;
  
  ix = ts.init_ptr_x; 
  iy = ts.init_ptr_y;
  ctrx = (ts._l + ts._r)/2.0;
  ctry = (ts._b + ts._t)/2.0;
  
  //  cerr << "grasp ixy: " << ix << ", " << iy << "\n";
  
  switch(tool_type) {
  case Tool::stretch:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
    }
    return grasp_stretch(e,tool,ix, iy);
  case Tool::move:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
    }
    return grasp_move(e,tool,ix, iy);
  case Tool::scale:
    return grasp_scale(e,tool,ix,iy,ctrx,ctry,ts._r-ts._l, ts._t-ts._b);
  case Tool::alter:
    return grasp_alter(e,tool,ix,iy,ctrx,ctry,ts._r-ts._l, ts._t-ts._b);
  case Tool::rotate:
    {
      ivCoord pi = 3.14159;
      ivCoord idy = iy-ctry; 	ivCoord idx = ix-ctrx;
      
      ivCoord ideg = atan(idy/idx)/pi*180.0;
      
      if (idx < 0.0)	ideg += 180.0;
      
      return grasp_rotate(e,tool,ideg, ctrx, ctry);
    }
  }
  return true;
}

bool Graphic::manipulating (const ivEvent& e, Tool& tool) {
  if (e.type() == ivEvent::up)
    return false;
  if (e.type() != ivEvent::motion)
    return true;		// don't bail, just don't do anything!

  unsigned int tool_type = tool.tool();
  ToolState& ts = tool.toolstate();
  if (tool_type == Tool::nop)
    return false;

  Graphic gs;
  ivTransformer* tx = ts._gs.transformer();
  
  ivCoord cx, cy, lx, ly, ix, iy;
  ivCoord ctrx, ctry;
  
  ix = ts.init_ptr_x;
  iy = ts.init_ptr_y;
  lx = ts._last.pointer_x();
  ly = ts._last.pointer_y();
  cx = e.pointer_x(); 
  cy = e.pointer_y();
  //  cerr << "ixy: " << ix << ", " << iy << "\t lxy: " << lx << ", " << ly << "\t cxy: " << cx << ", " << cy << "\n";
  ctrx = (ts._l + ts._r)/2.0;
  ctry = (ts._b + ts._t)/2.0;
  
  ts._last = e;

  switch(tool_type) {
  case Tool::stretch:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
      tx->inverse_transform(lx, ly);
      tx->inverse_transform(cx, cy);
    }
    return manip_stretch(e,tool,ix, iy, lx, ly, cx, cy);
  case Tool::move:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
      tx->inverse_transform(lx, ly);
      tx->inverse_transform(cx, cy);
    }
    return manip_move(e,tool,ix, iy, lx, ly, cx, cy);
  case Tool::scale:
    return manip_scale(e,tool,ix, iy, lx, ly, cx, cy, ctrx, ctry);
  case Tool::alter:
    return manip_alter(e,tool,ix, iy, lx, ly, cx, cy, ctrx, ctry);
  case Tool::rotate:
    {
      ivCoord pi = 3.14159;
      ivCoord idy = iy-ctry; 	ivCoord idx = ix-ctrx;
      ivCoord cdy = cy-ctry; 	ivCoord cdx = cx-ctrx;
      ivCoord ldy = ly-ctry; 	ivCoord ldx = lx-ctrx;
      
      ivCoord ideg = atan(idy/idx)/pi*180.0;
      ivCoord cdeg = atan(cdy/cdx)/pi*180.0;
      ivCoord ldeg = atan(ldy/ldx)/pi*180.0;
      
      if (idx < 0.0)	ideg += 180.0;
      if (cdx < 0.0)	cdeg += 180.0;
      if (ldx < 0.0)	ldeg += 180.0;
      
      return manip_rotate(e,tool,ideg, ldeg, cdeg, ctrx, ctry);
    }
  }
  return true;
}

bool Graphic::effect (const ivEvent& e, Tool& tool) {
  unsigned int tool_type = tool.tool();
  ToolState& ts = tool.toolstate();
    
  if (tool_type == Tool::nop)
    return false;

  Graphic gs;
  ivTransformer* tx = ts._gs.transformer();
  
  ivCoord cx, cy, ix, iy;
  ivCoord ctrx, ctry;
  
  ix = ts.init_ptr_x;
  iy = ts.init_ptr_y;
  cx = e.pointer_x(); 
  cy = e.pointer_y();
  ctrx = (ts._l + ts._r)/2.0;
  ctry = (ts._b + ts._t)/2.0;
  

  switch(tool_type) {
  case Tool::stretch:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
      tx->inverse_transform(cx, cy);
    }
    return effect_stretch(e,tool,ix, iy, cx, cy);
  case Tool::move:
    if (tx != nil) {
      tx->inverse_transform(ix, iy);
      tx->inverse_transform(cx, cy);
    }
    return effect_move(e,tool,ix, iy, cx, cy);
  case Tool::scale:
    return effect_scale(e,tool,ix, iy, cx, cy, ctrx, ctry);

  case Tool::alter:
    return effect_alter(e,tool,ix, iy, cx, cy, ctrx, ctry);

  case Tool::rotate:
    {
      ivCoord pi = 3.14159;
      ivCoord idy = iy-ctry; 	ivCoord idx = ix-ctrx;
      ivCoord cdy = cy-ctry; 	ivCoord cdx = cx-ctrx;
      
      ivCoord ideg = atan(idy/idx)/pi*180.0;
      ivCoord cdeg = atan(cdy/cdx)/pi*180.0;
      
      if (idx < 0.0)	ideg += 180.0;
      if (cdx < 0.0)	cdeg += 180.0;
      
      return effect_rotate(e,tool,ideg, cdeg, ctrx, ctry);
    }
  }
  return true;
}



bool	Graphic::grasp_stretch(const ivEvent& , Tool& ,ivCoord, ivCoord) {
  return true;
}

bool	Graphic::manip_stretch(const ivEvent& , Tool&, ivCoord, ivCoord, ivCoord , ivCoord ,
			    ivCoord , ivCoord ) {
//  translate(cx-lx, cy-ly);
  return true;
}

bool	Graphic::effect_stretch(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord, ivCoord) {
  return true;
}

bool	Graphic::grasp_move(const ivEvent&, Tool&,ivCoord, ivCoord) {
  return true;
}

bool	Graphic::manip_move(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord lx, ivCoord ly,
			    ivCoord cx, ivCoord cy) {
  translate(cx-lx, cy-ly);
  return true;
}

bool	Graphic::effect_move(const ivEvent&, Tool& ,ivCoord, ivCoord, ivCoord, ivCoord) {
  return true;
}


bool	Graphic::grasp_scale(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,ivCoord,ivCoord){
//bool	Graphic::grasp_scale(ivCoord ix, ivCoord iy,
//			     ivCoord ctrx, ivCoord ctry,ivCoord w, ivCoord h){
//  scale(osMath::abs((ix-ctrx)*2.0/w),osMath::abs((iy-ctry)*2.0/h));
  return true;
}


bool	Graphic::manip_scale(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord lx, ivCoord ly,
			     ivCoord cx, ivCoord cy, ivCoord ctrx, ivCoord ctry) {

  scale((cx-ctrx)/(lx-ctrx), (cy-ctry)/(ly-ctry), ctrx, ctry);
  return true;
}

bool	Graphic::effect_scale(const ivEvent& , Tool& ,ivCoord, ivCoord, ivCoord, ivCoord, ivCoord, ivCoord) {
  return true;
}

bool	Graphic::grasp_alter(const ivEvent&, Tool&,ivCoord,ivCoord,ivCoord,ivCoord,ivCoord,ivCoord){
  return true;
}

bool	Graphic::manip_alter(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord , ivCoord ,
			     ivCoord , ivCoord , ivCoord , ivCoord ) {
  return true;
}

bool	Graphic::effect_alter(const ivEvent& , Tool& ,ivCoord, ivCoord, ivCoord, ivCoord, ivCoord, ivCoord) {
  return true;
}



bool	Graphic::grasp_rotate(const ivEvent&, Tool&,ivCoord, ivCoord, ivCoord) {
  return true;
}

bool	Graphic::manip_rotate(const ivEvent&, Tool&,ivCoord, ivCoord ldeg, ivCoord cdeg,
			      ivCoord ctrx, ivCoord ctry) {
  rotate(cdeg - ldeg, ctrx, ctry);
  return true;
}

bool	Graphic::effect_rotate(const ivEvent& , Tool& ,ivCoord, ivCoord, ivCoord, ivCoord) {
  return true;
}

bool Graphic::effect_border() {
  if(_selected && _picked)
    brush(new ivBrush(BRUSH_DASH_DOT,SELECTED_BRUSH_WIDTH));
  else if(_selected)
    brush(new ivBrush(BRUSH_DASH,SELECTED_BRUSH_WIDTH));
  else if(_picked)
    brush(new ivBrush(BRUSH_DOT,SELECTED_BRUSH_WIDTH));
  else
    brush(new ivBrush(BRUSH_SOLID,DEFAULT_BRUSH_WIDTH));

  if(_selected || _picked) {
    if(_orig_stroke == nil) {
      _orig_stroke = _stroke;
      ivResource::ref(_orig_stroke);
      stroke(_stroke->brightness(.90));
    }
  }
  else {
    if(_orig_stroke != nil) {
      stroke(_orig_stroke);
      ivResource::unref(_orig_stroke);
      _orig_stroke = nil;
    }
  }	 
  return true;
}

bool Graphic::effect_select(bool set_select) {
  if(_selected == set_select)
    return false;
  _selected = set_select;
  effect_border();
  return true;
}
bool Graphic::effect_pick(bool set_pick) { 
  if(_picked == set_pick)
    return false;
  _picked = set_pick;
  effect_border();
  return true;
}
  


void Graphic::flush () {}

ivGlyph* Graphic::clone () const { return nil; }

ivTransformer* Graphic::transformer() { return _t; }

/*   To be replaced by templates  */

void Graphic::transformer(ivTransformer* t) {
    if (t != nil) {
        ivResource::ref(t);
    }
    if (_t != nil) {
        ivResource::unref(_t);
    }
    _t = t;
}

const ivBrush* Graphic::brush() { return _brush; }

void Graphic::brush(const ivBrush* b) { 
    if (b != nil) {
        ivResource::ref(b);
    }
    if (_brush != nil) {
        ivResource::unref(_brush);
    }
    _brush = b;
}

const ivColor* Graphic::stroke() { return _stroke; }

void Graphic::stroke(const ivColor* s) {
    if (s != nil) {
        ivResource::ref(s);
    }
    if (_stroke != nil) {
        ivResource::unref(_stroke);
    }
    _stroke = s;
}

const ivColor* Graphic::fill() { return _fill; }

void Graphic::fill(const ivColor* f) {
    if (f != nil) {
        ivResource::ref(f);
    }
    if (_fill != nil) {
        ivResource::unref(_fill);
    }
    _fill = f;
}

const ivFont* Graphic::font() { return _font; }

void Graphic::font(const ivFont* f) {
    if (f != nil) {
        ivResource::ref(f);
    }
    if (_font != nil) {
        ivResource::unref(_font);
    }
    _font = f;
}

void Graphic::closed (bool c) { _closed = c; }

bool Graphic::closed () { return _closed; }

void Graphic::curved (bool c) { _curved = c; }

bool Graphic::curved () { return _curved; }

void Graphic::parent (Graphic* p) { _parent = p; }

Graphic* Graphic::parent () { return _parent; }

Graphic* Graphic::root () {
    Graphic* cur, *parent = this;

    do {
        cur = parent;
        parent = cur->parent();
    } while (parent != nil);

    return cur;
}

void Graphic::total_gs (Graphic& gs) {
    Graphic* p = parent();

    if (p == nil) {
        concat(nil, this, &gs);

    } else {
        p->total_gs(gs);
        concat(this, &gs, &gs);
    }
}

void Graphic::translate (ivCoord dx, ivCoord dy) {
    if (dx != 0 || dy != 0) {
	if (_t == nil) {
            ivTransformer* tx = new ivTransformer;
            transformer(tx);
            ivResource::unref(tx);
	}
	_t->translate(dx, dy);
    }
}

void Graphic::scale (ivCoord sx, ivCoord sy, ivCoord cx, ivCoord cy) {
    ivCoord ncx, ncy;

    if (sx != 1.0 || sy != 1.0) {
	if (_t == nil) {
            ivTransformer* tx = new ivTransformer;
            transformer(tx);
            ivResource::unref(tx);
	}
	ivTransformer parents;
	parentXform(parents);
	parents.inverse_transform(cx, cy, ncx, ncy);
	
	if (ncx != 0 || ncy != 0) {
	    _t->translate(-ncx, -ncy);
	    _t->scale(sx, sy);
	    _t->translate(ncx, ncy);
	} else {
	    _t->scale(sx, sy);
	}
    }
}

void Graphic::rotate (ivCoord angle, ivCoord cx, ivCoord cy) {
    ivCoord mag = (angle < 0) ? -angle : angle;
    ivCoord ncx, ncy;

    if ((mag - int(mag)) != 0 || int(mag)%360 != 0) {
	if (_t == nil) {
            ivTransformer* tx = new ivTransformer;
            transformer(tx);
            ivResource::unref(tx);
	}
	ivTransformer parents;
	parentXform(parents);
	parents.inverse_transform(cx, cy, ncx, ncy);
	
	if (ncx != 0 || ncy != 0) {
	    _t->translate(-ncx, -ncy);
	    _t->rotate(angle);
	    _t->translate(ncx, ncy);
	} else {
	    _t->rotate(angle);
	}
    }
}

void Graphic::align(Alignment falign, Graphic* moved, Alignment malign) {
    ivCoord fx0, fy0, fx1, fy1, mx0, my0, mx1, my1, dx = 0, dy = 0;

    getbounds(fx0, fy0, fx1, fy1);
    moved->getbounds(mx0, my0, mx1, my1);
    
    switch (falign) {
	case BottomLeft:
	case CenterLeft:
	case TopLeft:
	case Left:
	    dx = fx0;
	    break;
	case BottomCenter:
	case Center:
	case TopCenter:
	case HorizCenter:
	    dx = (fx0 + fx1 + 1)/2;
	    break;
	case BottomRight:
	case CenterRight:
	case TopRight:
	case Right:
	    dx = fx1 + 1;
	    break;
    default:
      break;
    }
    switch (falign) {
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	case Bottom:
	    dy = fy0;
	    break;
	case CenterLeft:
	case Center:
	case CenterRight:
	case VertCenter:
	    dy = (fy0 + fy1 + 1)/2;
	    break;
	case TopLeft:
	case TopCenter:
	case TopRight:
	case Top:
	    dy = fy1 + 1;
	    break;
    default:
      break;
    }
    
    switch (malign) {
	case BottomLeft:
	case CenterLeft:
	case TopLeft:
	case Left:
	    dx -= mx0;
	    break;	
	case BottomCenter:
	case Center:
	case TopCenter:
	case HorizCenter:
	    dx -= (mx0 + mx1 + 1)/2;
	    break;
	case BottomRight:
	case CenterRight:
	case TopRight:
	case Right:
	    dx -= (mx1 + 1);
	    break;
    default:
      break;
    }
    switch (malign) {
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	case Bottom:
	    dy -= my0;
	    break;
	case CenterLeft:
	case Center:
	case CenterRight:
	case VertCenter:
	    dy -= (my0 + my1 + 1)/2;
	    break;
	case TopLeft:
	case TopCenter:
	case TopRight:
	case Top:
	    dy -= (my1 + 1);
	    break;
    default:
      break;
    }
    if (dx != 0 || dy != 0) {
        ivTransformer parents;
	moved->parentXform(parents);

        parents.invert();
	parents.transform(0.0, 0.0, fx0, fy0);
	parents.transform(dx, dy, mx0, my0);

	moved->translate(mx0-fx0, my0-fy0);
    }
}

void Graphic::recompute_shape () {
    if (_ctrlpts == 0) {
        return;
    } else {
        _xmin = _xmax = _x[0];
        _ymin = _ymax = _y[0];
    }
    for (int i = 1; i < _ctrlpts; i++) {
        _xmin = osMath::min(_xmin, _x[i]);
        _xmax = osMath::max(_xmax, _x[i]);
        _ymin = osMath::min(_ymin, _y[i]);
        _ymax = osMath::max(_ymax, _y[i]);
    }
}

void Graphic::getbounds (ivCoord& x0, ivCoord& y0, ivCoord& x1, ivCoord& y1) {
    Graphic gs;
    
    total_gs(gs);
    getbounds_gs(x0, y0, x1, y1, &gs);
}

void Graphic::getcenter (ivCoord& x, ivCoord& y) {
    Graphic gs;
    ivCoord l, b, tol;
    
    total_gs(gs);
    getextent_gs(l, b, x, y, tol, &gs);
}    

bool Graphic::contains (PointObj& p) {
    if (parent() == nil) {
        return contains_gs(p, this);

    } else {
	Graphic gs;
	total_gs(gs);
	return contains_gs(p, &gs);
    }
}

bool Graphic::intersects (BoxObj& b) {
    if (parent() == nil) {
        return intersects_gs(b, this);

    } else {
	Graphic gs;
	total_gs(gs);
	return intersects_gs(b, &gs);
    }
}

void Graphic::safe_damage_me(ivCanvas* c){
  if(c!=NULL){
    ivWindow* win = c->window();
#ifndef CYGWIN
    if((c->status() == ivCanvas::mapped) &&
       win && win->is_mapped()&& win->bound()){
      damage_me(c);
    }
#else
    damage_me(c);
#endif
  }
}

void Graphic::undraw () {}
void Graphic::append_(Graphic*) {}
void Graphic::prepend_(Graphic*) {}
void Graphic::insert_(ivGlyphIndex, Graphic*) {}
void Graphic::remove_(ivGlyphIndex) {}
void Graphic::remove_all() {}
void Graphic::replace_(ivGlyphIndex, Graphic*) {}
void Graphic::change_(ivGlyphIndex) {}
ivGlyphIndex Graphic::count_ () const { return 0; }
Graphic* Graphic::component_ (ivGlyphIndex) const { return nil; }
ivGlyphIndex Graphic::index_(Graphic*) const {return -1;}
void Graphic::modified_ (ivGlyphIndex) {}

ivGlyphIndex Graphic::first_containing (PointObj&) { return -1; }
ivGlyphIndex Graphic::last_containing (PointObj&) { return -1; }
ivGlyphIndex Graphic::next_containing (PointObj&, ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::prev_containing (PointObj&, ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::first_intersecting (BoxObj&) { return -1; }
ivGlyphIndex Graphic::last_intersecting (BoxObj&) { return -1; }
ivGlyphIndex Graphic::next_intersecting (BoxObj&, ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::prev_intersecting (BoxObj&, ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::first_within (BoxObj&) { return -1; }
ivGlyphIndex Graphic::last_within (BoxObj&) { return -1; }
ivGlyphIndex Graphic::next_within (BoxObj&, ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::prev_within (BoxObj&, ivGlyphIndex) { return -1; }
Graphic*   Graphic::breadth_first_selected() { return nil; }
ivGlyphIndex Graphic::first_selected() { return -1; }
ivGlyphIndex Graphic::last_selected () { return -1; }
ivGlyphIndex Graphic::next_selected (ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::prev_selected (ivGlyphIndex) { return -1; }
Graphic*   Graphic::breadth_first_picked() { return nil; }
ivGlyphIndex Graphic::first_picked() { return -1; }
ivGlyphIndex Graphic::last_picked () { return -1; }
ivGlyphIndex Graphic::next_picked (ivGlyphIndex) { return -1; }
ivGlyphIndex Graphic::prev_picked (ivGlyphIndex) { return -1; }

Graphic& Graphic::operator = (Graphic& g) {
    brush(g.brush());
    stroke(g.stroke());
    fill(g.fill());
    font(g.font());

    if (g._t == nil) {
        ivResource::unref(_t);
        _t = nil;

    } else {
	if (_t == nil) {
            ivTransformer* tx = new ivTransformer(*g._t);
            transformer(tx);
            ivResource::unref(tx);
	} else {
	    *_t = *g._t;
	}
    }
    return *this;
}

void Graphic::get_original (const ivCoord*& x, const ivCoord*& y) {
    x = _x;
    y = _y;
}

void Graphic::add_point(ivCoord x, ivCoord y) {
    if (_ctrlpts == 0) {
        _xmin = x - 1;
        _xmax = x + 1;
        _ymin = y - 1;
        _ymax = y + 1;
    } else {
        _xmin = osMath::min(_xmin, x);
        _xmax = osMath::max(_xmax, x);
        _ymin = osMath::min(_ymin, y);
        _ymax = osMath::max(_ymax, y);
    }
    _x[_ctrlpts] = x;
    _y[_ctrlpts] = y;
    _ctrlpts += 1;
    if (_ctrlpts >= _buf_size) {
        _buf_size = buf_size + _buf_size;
        ivCoord* x = new ivCoord[_buf_size];
        ivCoord* y = new ivCoord[_buf_size];
        osMemory::copy(_x, x, _ctrlpts*sizeof(int));
        osMemory::copy(_y, y, _ctrlpts*sizeof(int));
        delete _x;
        delete _y;
        _x = x;
        _y = y;
    }
}

void Graphic::add_curve(
    ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2
) {
    add_point(x1, y1);
    add_point(x2, y2);
    add_point(x, y);
}

void Graphic::Bspline_move_to (
    ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2
) {
    ivCoord p1x = (x + x + x1) / 3;
    ivCoord p1y = (y + y + y1) / 3;
    ivCoord p2x = (x + x + x2) / 3;
    ivCoord p2y = (y + y + y2) / 3;
    add_point((p1x + p2x) / 2, (p1y + p2y) / 2);
}

void Graphic::Bspline_curve_to (
    ivCoord x, ivCoord y, ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2
) {
    ivCoord p1x = (x + x + x1) / 3;
    ivCoord p1y = (y + y + y1) / 3;
    ivCoord p2x = (x + x + x2) / 3;
    ivCoord p2y = (y + y + y2) / 3;
    ivCoord p3x = (x1 + x1 + x) / 3;
    ivCoord p3y = (y1 + y1 + y) / 3;
    add_curve((p1x + p2x) / 2, (p1y + p2y) / 2, p3x, p3y, p1x, p1y);
}

void Graphic::request(ivRequisition& req) const {
    if (_ctrlpts > 0) {
        ivCoord left = _xmin, bottom = _ymin;
        ivCoord right = _xmax, top = _ymax;
        ivRequirement& rx = req.x_requirement();
        ivRequirement& ry = req.y_requirement();

        if (_t != nil) {
            corners(left, bottom, right, top, *_t);
        } 
        
        rx.natural(right - left);
        rx.stretch(0.0);
        rx.shrink(0.0);
        rx.alignment(-left / ((rx.natural() == 0 ? 1 : rx.natural())));
        
        ry.natural(top - bottom);
        ry.stretch(0.0);
        ry.shrink(0.0);
        ry.alignment(-bottom / (ry.natural() == 0 ? 1 : ry.natural()));
    }
}

void Graphic::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    if (_ctrlpts > 0) {
        ivCoord w = _brush == nil ? 0 : _brush->width();
        ivCoord x = a.x();
        ivCoord y = a.y();
        ext.merge_xy(
            c, x + _xmin - w, x + _xmax + w,
            y + _ymin - w, y + _ymax + w
        );
    }
}

void Graphic::draw(ivCanvas* c, const ivAllocation&) const {
  if (c != nil) {
    Graphic* gr = (Graphic*) this;
#ifndef CYGWIN
    ivCanvasDamage& cd = c->rep()->damage_;
    gr->drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
#else
    ivExtension cd;
    c->damage_area(cd);
    gr->drawclipped(c, cd.left(), cd.bottom(), cd.right(), cd.top());
#endif
  }
}

void Graphic::drawit (ivCanvas* c) {
  if (parent() == nil) {
    draw_gs(c, this);
    
  } else {
    Graphic gs;
    total_gs(gs);
    draw_gs(c, &gs);
  }
}    

void Graphic::drawclipped (ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t) {
    if (parent() == nil) {
        drawclipped_gs(c, l, b, r, t, this);
    } else {
        Graphic gs;
        total_gs(gs);
        drawclipped_gs(c, l, b, r, t, &gs);
    }
}

void Graphic::draw_gs (ivCanvas* c, Graphic* gs) {
    if(_ctrlpts <= 0)
      return;

    const ivBrush* brush = gs->brush();
    const ivColor* stroke = gs->stroke();
    const ivColor* fill = gs->fill();
    ivTransformer* tx = gs->transformer();
    if (tx != nil) {
        c->push_transform();
        c->transform(*tx);
    }
      
    c->new_path();
    c->move_to(_x[0], _y[0]);
    if (_curved) {
        for (int i = 1; i < _ctrlpts; i += 3) {
            c->curve_to(
                _x[i + 2], _y[i + 2],
                _x[i], _y[i],
                _x[i + 1], _y[i + 1]
            );
        }
    } else {
        for (int i = 1; i < _ctrlpts; ++i) {
            c->line_to(_x[i], _y[i]);
        }
    }
    if (_closed) {
        c->close_path();
    }
    if (fill != nil) {
        c->fill(fill);
    }
    if (brush != nil && stroke != nil) {
        c->stroke(stroke, brush);
    }
    if (tx != nil) {
        c->pop_transform();
    }
}

void Graphic::drawclipped_gs (
    ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t, Graphic* gs
) {
    ivCoord ll, bb, rr, tt;
    getbounds_gs(ll, bb, rr, tt, gs);

    BoxObj thisBox(ll, bb, rr, tt);
    BoxObj clipBox(l, b, r, t);
    if (clipBox.Intersects(thisBox)) {
	draw_gs(c, gs);
    }
}

void Graphic::eqv_transformer (ivTransformer& total) {
    Graphic* p = parent();
    
    if (p == nil) {
        concatXform(nil, transformer(), &total);

    } else {
        p->eqv_transformer(total);
        concatXform(transformer(), &total, &total);
    }
}

void Graphic::getextent_gs (
    ivCoord& l, ivCoord& b, ivCoord& cx, ivCoord& cy, ivCoord& tol, Graphic* gs
) {
    tol = 1.0;
    const ivBrush* br = gs->brush();
    if (br != nil) {
        ivCoord width = ivCoord(br->width());
        tol = (width > 1) ? width : tol;
    }
    if (_ctrlpts > 0) {
        ivCoord left = _xmin, bottom = _ymin;
        ivCoord right = _xmax, top = _ymax;
        ivTransformer* t = gs->transformer();

        if (t != nil) {
            corners(left, bottom, right, top, *t);
        } 
        l = left;
        b = bottom;
        cx = (left + right)/2.0;
        cy = (top + bottom)/2.0;
    }
}

void Graphic::getbounds_gs (
    ivCoord& l, ivCoord& b, ivCoord& r, ivCoord& t, Graphic* gs
) {
    ivCoord tol;
    
    getextent_gs(l, b, r, t, tol, gs);
    r += r - l;
    t += t - b;
    l -= tol;
    b -= tol;
    r += tol;
    t += tol;
}

static void invXform_gs (ivCoord& tx, ivCoord& ty, Graphic* g) {
    ivTransformer* t = g->transformer();
    if (t != nil) {
        t->inverse_transform(tx, ty);
    }
}

static void Xform_gs(ivCoord x[], ivCoord y[], int n,
		     ivCoord tx[], ivCoord ty[], Graphic* g) {
  ivTransformer* t = g->transformer();
  if (t != nil) {
    register ivCoord* ox, * oy, *nx, *ny;
    ivCoord* lim;
    
    lim = &x[n];
    for (
	 ox = x, oy = y, nx = tx, ny = ty; ox < lim; ox++, oy++, nx++, ny++
	 ) {
      t->transform(*ox, *oy, *nx, *ny);
    }
  } else {
    osMemory::copy(x, tx, n*sizeof(ivCoord));
    osMemory::copy(y, ty, n*sizeof(ivCoord));
  }
}

bool Graphic::contains_gs (PointObj& po, Graphic* gs) { 
    ivCoord ll, bb, rr, tt;
    PointObj pt (&po);
    getbounds_gs(ll, bb, rr, tt, gs);
    BoxObj b(ll, bb, rr, tt);;

    if (!_curved && !_fill) {
        if (b.Contains(po)) {
            MultiLineObj ml (_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            if (_closed) {
                LineObj l (_x[_ctrlpts - 1], _y[_ctrlpts - 1], *_x, *_y);
                return ml.Contains(pt) || l.Contains(pt);
            } else {
                return ml.Contains(pt);
            }
        }
        return false;

    } else if (!_curved && _fill) {
        if (b.Contains(pt)) {
            FillPolygonObj fp (_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            return fp.Contains(pt);
        }
        return false;

    } else if (_curved && !_fill) {
        if (b.Contains(pt)) {
            MultiLineObj ml;
            if (_closed) {
                ml.ClosedSplineToPolygon(_x, _y, _ctrlpts);
            } else {
                ml.SplineToMultiLine(_x, _y, _ctrlpts);
            }
            invXform_gs(pt._x, pt._y, gs);
            return ml.Contains(pt);
        }
        return false;

    } else {
        if (b.Contains(pt)) {
            FillPolygonObj fp;
            fp.ClosedSplineToPolygon(_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            return fp.Contains(pt);
        }
        return false;
    }
}

bool Graphic::intersects_gs (BoxObj& userb, Graphic* gs) { 
    ivCoord* convx, *convy;
    ivCoord ll, bb, rr, tt;
    getbounds_gs(ll, bb, rr, tt, gs);
    BoxObj b(ll, bb, rr, tt);;
    bool result = false;

    if (!_curved && !_fill) {
        if (b.Intersects(userb)) {
            convx = new ivCoord[_ctrlpts+1];
            convy = new ivCoord[_ctrlpts+1];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            if (_closed) {
                convx[_ctrlpts] = *convx;
                convy[_ctrlpts] = *convy;
                MultiLineObj ml(convx, convy, _ctrlpts+1);
                result = ml.Intersects(userb);
            } else {
                MultiLineObj ml(convx, convy, _ctrlpts);
                result = ml.Intersects(userb);
            }
            delete convx;
            delete convy;
        }
        return result;

    } else if (!_curved && _fill) {
        if (b.Intersects(userb)) {
            convx = new ivCoord[_ctrlpts];
            convy = new ivCoord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            FillPolygonObj fp (convx, convy, _ctrlpts);
            result = fp.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;    

    } else if (_curved && !_fill) {
        if (b.Intersects(userb)) {
            convx = new ivCoord[_ctrlpts];
            convy = new ivCoord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            MultiLineObj ml;
            if (_closed) {
                ml.ClosedSplineToPolygon(convx, convy, _ctrlpts);
            } else {
                ml.SplineToMultiLine(convx, convy, _ctrlpts);
            }
            result = ml.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;

    } else {
        if (b.Intersects(userb)) {
            convx = new ivCoord[_ctrlpts];
            convy = new ivCoord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            FillPolygonObj fp;
            fp.ClosedSplineToPolygon(convx, convy, _ctrlpts);
            result = fp.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;
    }
}

void Graphic::parentXform (ivTransformer& t) {
    ivTransformer identity;
    Graphic* p = parent();

    if (p == nil) {
        *(&t) = *(&identity);
    } else {
        p->eqv_transformer(t);
    }
}

void Graphic::concat_gs (Graphic* a, Graphic* b, Graphic* dest) {
    const ivColor* stroke, *fill;
    const ivFont* font;
    const ivBrush* br;
    
    if (a == nil) {
        *dest = *b;
        return;
    } else if (b == nil) {
        *dest = *a;
        return;
    }

    if ((fill = b->fill()) == nil) {
	fill = a->fill();
    }
    dest->fill(fill);

    if ((stroke = b->stroke()) == nil) {
	stroke = a->stroke();
    }
    dest->stroke(stroke);

    if ((font = b->font()) == nil) {
	font = a->font();
    }
    dest->font(font);

    if ((br = b->brush()) == nil) {
	br = a->brush();
    }
    dest->brush(br);
}

void Graphic::concatXform (
    ivTransformer* a, ivTransformer* b, ivTransformer* dest
) {
    ivTransformer identity;
    if (a == nil) {
        *dest = (b == nil) ? *(&identity) : *b;

    } else if (b == nil) {
        *dest = *a;
        
    } else {
        ivTransformer tmp(*a);
        tmp.Postmultiply(b);
        *dest = tmp;
    }
}

void Graphic::concat (Graphic* a, Graphic* b, Graphic* dest) {
    ivTransformer* ta = (a == nil) ? nil : a->transformer();
    ivTransformer* tb = (b == nil) ? nil : b->transformer();
    ivTransformer* td = dest->transformer();
    if (td == nil) {
        td = new ivTransformer;
    } else {
        ivResource::ref(td);
    }
    concatXform(ta, tb, td);
    dest->transformer(td);
    concat_gs(a, b, dest);
    ivResource::unref(td);
}

bool Graphic::contains_ (Graphic* g, PointObj& p, Graphic* gs) {
    return g->contains_gs(p, gs);
}
bool Graphic::intersects_ (Graphic* g, BoxObj& b, Graphic* gs) {
    return g->intersects_gs(b, gs);
}
void Graphic::getbounds_ (
    Graphic* g, ivCoord& l, ivCoord& b, ivCoord& r, ivCoord& t, Graphic* gs
) {
    g->getbounds_gs(l, b, r, t, gs);
}
void Graphic::total_gs_(Graphic* gr, Graphic& gs) {
    gr->total_gs(gs);
}
void Graphic::concatgs_ (Graphic* gr, Graphic* a, Graphic* b, Graphic* dest) {
    gr->concat_gs(a, b, dest);
}
void Graphic::concatXform_ (
    Graphic* gr, ivTransformer* a, ivTransformer* b, ivTransformer* dest
) {
    gr->concatXform(a, b, dest);
}
void Graphic::concat_ (Graphic* g, Graphic* a, Graphic* b, Graphic* d) {
    g->concat(a, b, d);
}
void Graphic::getextent_ (
    Graphic* gr,ivCoord& l, ivCoord& b, ivCoord& r, ivCoord& t, ivCoord& tol, Graphic* gs
) {
    gr->getextent_gs(l, b, r, t, tol, gs);
}
void Graphic::draw_ (Graphic* gr, ivCanvas* c, Graphic* gs) {
    gr->draw_gs(c, gs);
}
void Graphic::drawclipped_ (
    Graphic* gr, ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t, Graphic* gs
) {
    gr->drawclipped_gs(c, l, b, r, t, gs);
}
void Graphic::transform_ (ivCoord x, ivCoord y, ivCoord& tx, ivCoord& ty, Graphic* g) {
    ivTransformer* t = (g == nil) ? transformer() : g->transformer();

    if (t != nil) {
        t->transform(x, y, tx, ty);
    } else {
        tx = x;
        ty = y;
    }
}




bool Graphic::body_is_selected(){
  int i;
  for(i=0;i<count_();i++){
    if(component_(i)->is_selected() || component_(i)->body_is_selected()){
      return true;
    }
  }
  return false;
}

bool Graphic::body_is_picked(){
  int i;
  for(i=0;i<count_();i++){
    if(component_(i)->is_picked() || component_(i)->body_is_picked()){
      return true;
    }
  }
  return false;
}

/************************************************************************/

PolyGraphic::PolyGraphic (Graphic* gr) : Graphic (gr) {
    ivLayoutKit* layout = ivLayoutKit::instance();
    _body = layout->overlay();
}

PolyGraphic::~PolyGraphic () {
    delete _body;
}

void PolyGraphic::request (ivRequisition& req) const { 
   _body->request(req);
    ivRequirement& rx = req.x_requirement();
    ivRequirement& ry = req.y_requirement();

    ivCoord left, bottom, right, top;
    left = -rx.natural()*rx.alignment();
    right = left + rx.natural();
    bottom = -ry.natural()*ry.alignment();
    top = bottom + ry.natural();

   PolyGraphic* p = (PolyGraphic *) this;
   ivTransformer* t = p->transformer();
    if (_t != nil) {
      corners(left, bottom, right, top,  *t);
    }
    rx.natural(right - left);
    rx.stretch(0.0);
    rx.shrink(0.0);
    rx.alignment(-left / (rx.natural() == 0 ? 1 : rx.natural()));

    ry.natural(top - bottom);
    ry.stretch(0.0);
    ry.shrink(0.0);
    ry.alignment(-bottom / (ry.natural() == 0 ? 1 : ry.natural()));
}

void PolyGraphic::allocate (ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    if (transformer() != nil) {
        c->push_transform();
        c->transform(*transformer());
    } 
    _body->allocate(c, a, ext);
    if (_t != nil) {
        c->pop_transform();
    }
}

bool PolyGraphic::update_from_state(ivCanvas* c) {
  long i;
  bool rval = false;
  for(i=0; i < count_(); i++) {
    if(component_(i)->update_from_state(c))
      rval = true;
  }
  return rval;
}


bool PolyGraphic::select (const ivEvent& e, Tool& tool, bool unselect) {
  long i;
  for(i=0; i < count_(); i++)
    component_(i)->select(e, tool, unselect);

  if(unselect)
    _selected = false;
  else
    _selected = !_selected;

  return true;
}

bool PolyGraphic::pick_me (const ivEvent& e, Tool& tool, bool unpick) {
  long i;
  for(i=0; i < count_(); i++)
    component_(i)->pick_me(e, tool, unpick);

  if(unpick)
    _picked = false;
  else
    _picked = !_picked;

  return true;
}


void PolyGraphic::append_ (Graphic* g) { 
    _body->append(g); 
    g->parent(this);
}

void PolyGraphic::prepend_ (Graphic* g) { 
    _body->prepend(g); 
    g->parent(this);
}

void PolyGraphic::insert_ (ivGlyphIndex i, Graphic* g) { 
    _body->insert(i, g); 
    g->parent(this);
}

void PolyGraphic::remove_ (ivGlyphIndex i) { 
    Graphic* gr = (Graphic*)_body->component(i);
    if (gr != nil) {
        gr->parent(nil);
    }
    _body->remove(i); 
}

void PolyGraphic::remove_all() {
  ivGlyphIndex i;
  for(i=count_()-1; i>=0; i--) {
    component_(i)->remove_all();
    remove_(i);
  }
}

void PolyGraphic::replace_ (ivGlyphIndex i, Graphic* g) { 
    Graphic* gr = (Graphic*) _body->component(i);
    gr->parent(nil);
    _body->replace(i, g); 
    g->parent(this);
}

void PolyGraphic::flush () {
    ivGlyphIndex count = count_();
    
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = component_(i);
        concat_(gr, gr, this, gr);
        gr->flush();
    }
    Graphic n;
    *((Graphic*) this) = *(&n);
}

ivGlyph* PolyGraphic::clone () const {
    Graphic* pg = new PolyGraphic((Graphic*)this);

    ivGlyphIndex count = _body->count();
    for (ivGlyphIndex i = 0; i < count; i++) {
        ivGlyph* gr = _body->component(i);
        pg->append(gr->clone());
    }
    return pg;
}
    
ivGlyphIndex PolyGraphic::first_containing (PointObj& pt) {
  for (ivGlyphIndex i = 0; i < count_(); i++) {
    if (component_(i)->contains(pt)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::last_containing (PointObj& pt) {
  for (ivGlyphIndex i = count_()-1; i >= 0; i--) {
    if (component_(i)->contains(pt)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::next_containing (PointObj& pt, ivGlyphIndex i) {
  for(; i < count_(); i++) {
    if (component_(i)->contains(pt)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::prev_containing (PointObj& pt, ivGlyphIndex i) {
  for (; i >= 0; i--) { 
    if (component_(i)->contains(pt)) return i;
  }
  return -1;
}


ivGlyphIndex PolyGraphic::first_intersecting (BoxObj& b) {
  for (ivGlyphIndex i = 0; i < count_(); i++) {
    if (component_(i)->intersects(b)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::last_intersecting (BoxObj& b) {
  for (ivGlyphIndex i = count_()-1; i >= 0; i--) {
    if (component_(i)->intersects(b)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::next_intersecting (BoxObj& b, ivGlyphIndex i) {
  for (; i < count_(); i++) {
    if (component_(i)->intersects(b)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::prev_intersecting (BoxObj& b, ivGlyphIndex i) {
  for (; i >= 0; i--) {
    if (component_(i)->intersects(b)) return i;
  }
  return -1;
}

ivGlyphIndex PolyGraphic::first_within (BoxObj& gb) {
  ivCoord l, b, r, t;
  for (ivGlyphIndex i = 0; i < count_(); i++) {
    Graphic* gr = component_(i);
    gr->getbounds(l, b, r, t);
    BoxObj box(l, b, r, t);
    if (box.Within(gb))	return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::last_within (BoxObj& gb) {
  ivCoord l, b, r, t;
  for (ivGlyphIndex i = count_()-1; i >= 0; i--) {
    Graphic* gr = component_(i);
    gr->getbounds(l, b, r, t);
    BoxObj box(l, b, r, t);
    if (box.Within(gb))	return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::next_within (BoxObj& gb, ivGlyphIndex i) {
  ivCoord l, b, r, t;
  for (; i < count_(); i++) {
    Graphic* gr = component_(i);
    gr->getbounds(l, b, r, t);
    BoxObj box(l, b, r, t);
    if (box.Within(gb)) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::prev_within (BoxObj& gb, ivGlyphIndex i) {
  ivCoord l, b, r, t;
  for (; i >= 0; i--) {
    Graphic* gr = component_(i);
    gr->getbounds(l, b, r, t);
    BoxObj box(l, b, r, t);
    if (box.Within(gb)) return i;
  }
  return -1;
}

Graphic* PolyGraphic::breadth_first_selected() {
  ivGlyphIndex i;
  for (i = 0; i < count_(); i++) {
    if (component_(i)->is_selected()) return component_(i);
  }
  ivGlyphIndex nxt;
  for (i = 0; i < count_(); i++) {
    if((nxt = component_(i)->first_selected()) >= 0)
      return component_(i)->component_(nxt);
  }
  return nil;
}
ivGlyphIndex PolyGraphic::first_selected () {
  for (ivGlyphIndex i = 0; i < count_(); i++) {
    if (component_(i)->is_selected()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::last_selected () {
  for (ivGlyphIndex i = count_()-1; i >= 0; i--) {
    if (component_(i)->is_selected()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::next_selected (ivGlyphIndex i) {
  for (; i < count_(); i++) {
    if (component_(i)->is_selected()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::prev_selected (ivGlyphIndex i) {
  for (; i >= 0; i--) {
    if (component_(i)->is_selected()) return i;
  }
  return -1;
}


Graphic* PolyGraphic::breadth_first_picked() {
  ivGlyphIndex i;
  for (i = 0; i < count_(); i++) {
    if (component_(i)->is_picked()) return component_(i);
  }
  ivGlyphIndex nxt;
  for (i = 0; i < count_(); i++) {
    if((nxt = component_(i)->first_picked()) >= 0)
      return component_(i)->component_(nxt);
  }
  return nil;
}
ivGlyphIndex PolyGraphic::first_picked () {
  for (ivGlyphIndex i = 0; i < count_(); i++) {
    if (component_(i)->is_picked()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::last_picked () {
  for (ivGlyphIndex i = count_()-1; i >= 0; i--) {
    if (component_(i)->is_picked()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::next_picked (ivGlyphIndex i) {
  for (; i < count_(); i++) {
    if (component_(i)->is_picked()) return i;
  }
  return -1;
}
ivGlyphIndex PolyGraphic::prev_picked (ivGlyphIndex i) {
  for (; i >= 0; i--) {
    if (component_(i)->is_picked()) return i;
  }
  return -1;
}



void PolyGraphic::draw_gs (ivCanvas* c, Graphic* gs) {
    Graphic gstemp;

    ivGlyphIndex count = _body->count();
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
	concat_(gr, gr, gs, &gstemp);
	draw_(gr, c, &gstemp);
    }
}

void PolyGraphic::drawclipped_gs (
    ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t, Graphic* gs
) {
    Graphic gstemp;

    ivGlyphIndex count = _body->count();
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        drawclipped_(gr, c, l, b, r, t, &gstemp);
    }
}

void PolyGraphic::getextent_gs (
    ivCoord& l, ivCoord& b, ivCoord& cx, ivCoord& cy, ivCoord& tol, Graphic* gs
) {
    Extent e;
    l = b = cx = cy = tol = 0.0;
        
    Graphic gstemp;
    ivTransformer ttemp;
    Extent te;
    gstemp.transformer(&ttemp);

    ivGlyphIndex count = _body->count();
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        
        concatgs_(gr, gr, gs, &gstemp);
        concatXform_(gr, nil, gr->transformer(), &ttemp);
        getextent_(gr, te._left, te._bottom, te._cx, te._cy, te._tol, &gstemp);
        e.Merge(te);
    }
    gstemp.transformer(nil); // to avoid deleting ttemp explicitly
    l = e._left; b = e._bottom; cx = l+(e._cx-l)*2.0; cy = b+(e._cy-b)*2.0;
    tol = e._tol;

    ivTransformer* tx = gs->transformer();
    if (tx != nil) {
        corners(l, b, cx, cy, *tx);
    }
    cx = (cx + l)/2.0;
    cy = (cy + b)/2.0;
}

ivGlyphIndex PolyGraphic::index_(Graphic* g) const {
  ivGlyphIndex count = _body->count();
  for (ivGlyphIndex i = 0; i < count; i++) {
    if((Graphic*) _body->component(i)== g) return i;
  }
  return -1;
}

bool PolyGraphic::contains_gs (PointObj& po, Graphic* gs) {
    ivGlyphIndex count = _body->count();
    Graphic gstemp;
    ivTransformer ttemp;

    gstemp.transformer(&ttemp);
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        
        if (contains_(gr, po, &gstemp)) {
            gstemp.transformer(nil);
            return true;
        }
    }
    gstemp.transformer(nil); /* to avoid deleting ttemp explicitly*/
    return false;
}

bool PolyGraphic::intersects_gs (BoxObj& box, Graphic* gs) {
    ivGlyphIndex count = _body->count();
    Graphic gstemp;
    ivTransformer ttemp;

    gstemp.transformer(&ttemp);
    for (ivGlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        
        if (intersects_(gr, box, &gstemp)) {
            gstemp.transformer(nil);
            return true;
        }
    }
    gstemp.transformer(nil); /* to avoid deleting ttemp explicitly*/
    return false;
}

/**********************************************************************/
#include <ta/enter_iv.h>
declarePtrList(GraphicList, Graphic);
implementPtrList(GraphicList, Graphic);
#include <ta/leave_iv.h>

static ivCursor* grabber_cursor = nil;
static ivCursor* window_cursor = nil;

GraphicMaster::GraphicMaster (Graphic* gr, const ivColor* bg) : PolyGraphic(gr) {
  _canvas = NULL;
  _viewer = NULL;
  _gr_list = new GraphicList;
  if (grabber_cursor == nil) {
    ivBitmap* grabber = new ivBitmap(
				     grabber_bits, grabber_width, grabber_height,
				     grabber_x_hot, grabber_y_hot
				     );
    ivBitmap* grabber_mask = new ivBitmap(
					  grabberMask_bits, grabberMask_width, grabberMask_height,
					  grabberMask_x_hot, grabberMask_y_hot
					  );
    grabber_cursor = new ivCursor(grabber, grabber_mask);
  }
  _bg = bg;
  ivResource::ref(_bg);
  no_text_extent = false;
}

GraphicMaster::~GraphicMaster () {
    delete _gr_list;
    ivResource::unref(_bg);
}

ivCanvas* GraphicMaster::canvas() const {
  if(_canvas != NULL)
    return _canvas;
  ((GraphicMaster*)this)->_canvas = _viewer->canvas();
  return _canvas;
}
void GraphicMaster::canvas(ivCanvas* c) {
  _canvas = c;
}

GlyphViewer* GraphicMaster::viewer() const {
  return _viewer;
}
void GraphicMaster::viewer(GlyphViewer* v) {
  _viewer = v;
}

void GraphicMaster::background(const ivColor* bg) {
    if (bg != nil) {
        ivResource::ref(bg);
    }
    if (_bg != nil) {
        ivResource::unref(_bg);
    }
    _bg = bg;
}

ivGlyph* GraphicMaster::clone () const {
    Graphic* pg = new GraphicMaster((Graphic*)this);

    ivGlyphIndex count = _body->count();
    for (ivGlyphIndex i = 0; i < count; i++) {
        ivGlyph* gr = _body->component(i);
        pg->append(gr->clone());
    }
    return pg;
}

void GraphicMaster::request(ivRequisition& req) const {
    PolyGraphic::request(req);
    ivRequirement& rx = req.x_requirement();
    rx.alignment(0.0);
    
    ivRequirement& ry = req.y_requirement();
    ry.alignment(0.0);
}

void GraphicMaster::allocate (ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    if (!_a.equals(a, 0.001)) {
        ivAllocation b(a);
        ivAllotment& ax = _a.x_allotment();
        ivAllotment& ay = _a.y_allotment();
        ivAllotment& bx = b.x_allotment();
        ivAllotment& by = b.y_allotment();
        translate((bx.span()-ax.span())/2.0, (by.span()-ay.span())/2.0);
        translate(bx.begin()-ax.begin(), by.begin()-ay.begin());
        _a = a;
    }
    if (c != nil) {
        PolyGraphic::allocate(c, a, ext);
    }
}

void GraphicMaster::drawclipped_gs(
    ivCanvas* c, ivCoord l, ivCoord b, ivCoord r, ivCoord t, Graphic* gs
) {
    c->push_clipping();
    c->clip_rect(_a.left(), _a.bottom(), _a.right(), _a.top());
    if (_bg != nil) {
        c->fill_rect(l, b, r, t, _bg);
    }
    PolyGraphic::drawclipped_gs(c, l, b, r, t, gs);
    c->pop_clipping();
}


bool GraphicMaster::update_from_state(ivCanvas* c) {
  return PolyGraphic::update_from_state(c);
}

bool GraphicMaster::grasp (const ivEvent& e, Tool& tool) {
  if (window_cursor == nil) {
    window_cursor = e.window()->cursor();
  }
  bool flag = false;
  unsigned int tool_type = tool.tool();
  switch (tool_type) {
  case Tool::move:
  case Tool::scale:
  case Tool::stretch:
  case Tool::rotate:
  case Tool::alter: {
    ivCoord tol = 2.0;
    BoxObj box(e.pointer_x()-tol, e.pointer_y()-tol, 
	       e.pointer_x()+tol, e.pointer_y()+tol);
    ivGlyphIndex tgdx = last_intersecting(box);
    if (tgdx < 0)	return false;
    Graphic* target = component_(tgdx);
    while(!target->graspable()) { // find something graspable!
      tgdx = target->last_intersecting(box);
      if(tgdx >= 0) {
	target = target->component_(tgdx);
      }
      else break;
    }
    if(tgdx < 0) return false;
    while(!target->is_selected() && target->body_is_selected()) {
      tgdx = target->last_intersecting(box);
      if(tgdx >= 0)
	target = target->component_(tgdx);
    }
    if(!target->is_selected()){
      // unselect everything else first
      long i;
      for(i=0; i < count_(); i++) {
	if(component_(i) != target)
	  component_(i)->select(e, tool, true);
      }
      // select target
      target->select(e,tool,false);
    }

    ivWindow* w = e.window();
    ivCanvas* c = canvas();
    w->cursor(grabber_cursor);
      
    ToolState& ts = tool.toolstate();
    total_gs(ts._gs);
      
    _gr_list->append(target);
    target->damage_me(c);
    flag = target->grasp(e, tool);
    if (flag)
      target->damage_me(c);
#ifndef CYGWIN
    ivCanvasRep& rep = *c->rep();
    ivCanvasDamage& cd = rep.damage_;
    rep.start_repair();
    drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
    rep.finish_repair();
#else
    c->window()->repair();
#endif
  }
  break;
  } 
  return flag;
}


bool GraphicMaster::select (const ivEvent& e, Tool& tool, bool) {
  bool flag = false;
  ivCoord tol = 2.0;
  BoxObj box(e.pointer_x()-tol, e.pointer_y()-tol, 
	     e.pointer_x()+tol, e.pointer_y()+tol);
  ivGlyphIndex tgdx = last_intersecting(box);

  ivCanvas* c = canvas();

  if(tgdx < 0) {		// no selection, unselect everybody
    if(Graphic::GetButton(e) == ivEvent::middle) // except when middle
      return false;
    long i;
    for(i=0; i < count_(); i++) {
      component_(i)->select(e, tool, true);
    }
#ifndef CYGWIN
    ivCanvasRep& rep = *c->rep();
    ivCanvasDamage& cd = rep.damage_;
    rep.start_repair();
    drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
    rep.finish_repair();
#else
    c->window()->repair();
#endif
    return true;
  }  
  Graphic* target = component_(tgdx);
  while(tgdx >=0) {
    if(!target->selectable()) {
      tgdx = target->last_intersecting(box);
      if(tgdx >= 0) {
	target = target->component_(tgdx);  // then just go around the loop again
	if(!target->selectable()) tgdx = 0; // go around again
      }
    }
    else if(target->is_selected()) {
      tgdx = target->last_intersecting(box);
      if(tgdx >= 0) {
	if(Graphic::GetButton(e) != ivEvent::middle) {
	  target->select(e,tool,true); // turn container off
	}
	else {
	  target->select(e,tool); // flip target
	}
	target = target->component_(tgdx);
	tgdx = -1;
	if(!target->selectable()) tgdx = 0; // go around again
      }
    }
    else {
      if(target->body_is_selected()) {
	tgdx = target->last_intersecting(box);
	if(tgdx >= 0) {
	  target = target->component_(tgdx);
	} 
	else {
	  target = NULL;
	}
      }
      else {
	tgdx = -1;
      }
    }
  }
  if(target == NULL) return false;
  
  ToolState& ts = tool.toolstate();
  total_gs(ts._gs);
  flag = target->select(e, tool);	// always select
  
  if (flag) {
    switch(Graphic::GetButton(e)) {
    case ivEvent::middle:
      break;
    case ivEvent::left:
    case ivEvent::right:		// unselect all the others!!
      {
	if(target->is_selected()) { // only if the target was selected
	  long i;
	  for(i=0; i < count_(); i++) {
	    if((component_(i) != target))
	      component_(i)->select(e, tool, true);
	  }
	  if(!target->is_selected())
	    target->select(e, tool);	// reselect in case it was turned off
	}
      }
      break;
    }
  }
  
#ifndef CYGWIN
  ivCanvasRep& rep = *c->rep();
  ivCanvasDamage& cd = rep.damage_;
  rep.start_repair();
  drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
  rep.finish_repair();
#else
  c->window()->repair();
#endif
  return flag;
}


bool GraphicMaster::pick_me (const ivEvent& e, Tool& tool, bool) {
  bool flag = false;
  ivCoord tol = 2.0;
  BoxObj box(e.pointer_x()-tol, e.pointer_y()-tol, 
	     e.pointer_x()+tol, e.pointer_y()+tol);
  ivGlyphIndex tgdx = last_intersecting(box);

  ivCanvas* c = canvas();

  if(tgdx < 0) {		// no selection, unselect everybody
    long i;
    for(i=0; i < count_(); i++) {
      component_(i)->pick_me(e, tool, true);
    }
#ifndef CYGWIN
    ivCanvasRep& rep = *c->rep();
    ivCanvasDamage& cd = rep.damage_;
    rep.start_repair();
    drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
    rep.finish_repair();
#else
    c->window()->repair();
#endif
    return true;
  }

  // default behavior for pick is to get a terminal, _not_ a group.
  Graphic* target = component_(tgdx);
  while((tgdx >= 0) && (target->count_() > 0)) {
    tgdx = target->last_intersecting(box);
    if(tgdx >= 0) {
      target = target->component_(tgdx);
    }
  }
  if(!target->pickable()) return false;	// if what you got wasn't pickable, bail
  
  ToolState& ts = tool.toolstate();
  total_gs(ts._gs);
  flag = target->pick_me(e, tool);	// always pick
  
  if (flag) {
    switch(Graphic::GetButton(e)) {
    case ivEvent::middle:
      break;
    case ivEvent::left:
    case ivEvent::right:		// unpick all the others!!
      {
	if(target->is_picked()) { // only if the target was picked
	  long i;
	  for(i=0; i < count_(); i++) {
	    if(component_(i) != target)
	      component_(i)->pick_me(e, tool, true);
	  }
	  if(!target->is_picked())
	    target->pick_me(e, tool);	// repick in case it was turned off
	}
      }
      break;
    }
  }
  
#ifndef CYGWIN
  ivCanvasRep& rep = *c->rep();
  ivCanvasDamage& cd = rep.damage_;
  rep.start_repair();
  drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
  rep.finish_repair();
#else
  c->window()->repair();
#endif
  return flag;
}


bool GraphicMaster::manipulating (const ivEvent& e, Tool& tool) {
  bool flag = true;
  long count = _gr_list->count();
  if(count == 0) return false;
  ivWindow* w = e.window();
  ivCanvas* c = canvas();
  for (long i = 0; i < count && flag; i++) {
    Graphic* target = _gr_list->item(i);
    target->damage_me(c);
    flag = target->manipulating(e, tool);
    if (flag)
      target->damage_me(c);
#ifndef CYGWIN
    ivCanvasRep& rep = *c->rep();
    ivCanvasDamage& cd = rep.damage_;
    rep.start_repair();
    drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
    rep.finish_repair();
    w->display()->flush();
#else
    c->window()->repair();
#endif
  }
  return flag;
}

bool GraphicMaster::effect (const ivEvent& e, Tool& tool) {
  bool flag = true;
  ivWindow* w = e.window();
  if(w == nil) return false;
  w->cursor(window_cursor);
  ivCanvas* c = canvas();
  long count = _gr_list->count();
  if(count == 0) return false;
  for (long i = 0; i < count && flag; i++) {
    Graphic* target = _gr_list->item(i);
    target->damage_me(c);
    flag = target->effect(e, tool);
    if (flag)
      target->damage_me(c);
    //      target->flush();  // this is bad, makes text dissappear!
#ifndef CYGWIN
    ivCanvasRep& rep = *c->rep();
    ivCanvasDamage& cd = rep.damage_;
    rep.start_repair();
    drawclipped(c, cd.left, cd.bottom, cd.right, cd.top);
    rep.finish_repair();
#else
    c->window()->repair();
#endif
  }
  _gr_list->remove_all();
  return flag;
}

float GraphicMaster::GetCurrentXformScale(){
  if (transformer() == NULL) return 1;
  float a00,a01,a10,a11,a20,a21;
  transformer()->matrix(a00,a01,a10,a11,a20,a21);
  return a00; // x scale
}

float GraphicMaster::GetCurrentYformScale(){
  if (transformer() == NULL) return 1;
  float a00,a01,a10,a11,a20,a21;
  transformer()->matrix(a00,a01,a10,a11,a20,a21);
  return a11; // y scale
}



/**********************************************************************/

Line::Line(
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    ivCoord x1, ivCoord y1, ivCoord x2, ivCoord y2, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, false, false, 2, t) {
    add_point(x1, y1);
    add_point(x2, y2);
}

Line::~Line () { }

ivGlyph* Line::clone () const {
    return new Line(_brush, _stroke, _fill, _x[0], _y[0], _x[1], _y[1], _t);
}

/**********************************************************************/
Rectangle::Rectangle (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    ivCoord l, ivCoord b, ivCoord r, ivCoord t, ivTransformer* tx
) : Graphic(brush, stroke, fill, nil, true, false, 4, tx) {
    add_point(r, b);
    add_point(r, t);
    add_point(l, t);
    add_point(l, b);
}

Rectangle::~Rectangle () { }

ivGlyph* Rectangle::clone () const {
    return new Rectangle(
        _brush, _stroke, _fill, _x[0], _y[0], _x[1], _y[1], _t
    );
}

/**********************************************************************/
static ivCoord p0 = 1.00000000;
static ivCoord p1 = 0.89657547;   // cos 30 * sqrt(1 + tan 15 * tan 15)
static ivCoord p2 = 0.70710678;   // cos 45 
static ivCoord p3 = 0.51763809;   // cos 60 * sqrt(1 + tan 15 * tan 15)
static ivCoord p4 = 0.26794919;   // tan 15

Circle::Circle (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    ivCoord x, ivCoord y, ivCoord r, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, true, true, 25, t) {
    ivCoord px0 = p0 * r, py0 = p0 * r;
    ivCoord px1 = p1 * r, py1 = p1 * r;
    ivCoord px2 = p2 * r, py2 = p2 * r;
    ivCoord px3 = p3 * r, py3 = p3 * r;
    ivCoord px4 = p4 * r, py4 = p4 * r;
    
    add_point(x + r, y);
    add_curve(x + px2, y + py2, x + px0, y + py4, x + px1, y + py3);
    add_curve(x, y + r, x + px3, y + py1, x + px4, y + py0);
    add_curve(x - px2, y + py2, x - px4, y + py0, x - px3, y + py1);
    add_curve(x - r, y, x - px1, y + py3, x - px0, y + py4);
    add_curve(x - px2, y - py2, x - px0, y - py4, x - px1, y - py3);
    add_curve(x, y - r, x - px3, y - py1, x - px4, y - py0);
    add_curve(x + px2, y - py2, x + px4, y - py0, x + px3, y - py1);
    add_curve(x + r, y, x + px1, y - py3, x + px0, y - py4);
}

Circle::~Circle () { }

ivGlyph* Circle::clone () const {
    ivCoord r = _x[0] - _x[4];
    ivCoord x = _x[4];
    ivCoord y = _y[0];
    return new Circle(_brush, _stroke, _fill, x, y, r, _t);
}

/**********************************************************************/
Ellipse::Ellipse (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    ivCoord x, ivCoord y, ivCoord rx, ivCoord ry, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, true, true, 25, t) {
    ivCoord px0 = p0 * rx, py0 = p0 * ry;
    ivCoord px1 = p1 * rx, py1 = p1 * ry;
    ivCoord px2 = p2 * rx, py2 = p2 * ry;
    ivCoord px3 = p3 * rx, py3 = p3 * ry;
    ivCoord px4 = p4 * rx, py4 = p4 * ry;
    
    add_point(x + rx, y);
    add_curve(x + px2, y + py2, x + px0, y + py4, x + px1, y + py3);
    add_curve(x, y + ry, x + px3, y + py1, x + px4, y + py0);
    add_curve(x - px2, y + py2, x - px4, y + py0, x - px3, y + py1);
    add_curve(x - rx, y, x - px1, y + py3, x - px0, y + py4);
    add_curve(x - px2, y - py2, x - px0, y - py4, x - px1, y - py3);
    add_curve(x, y - ry, x - px3, y - py1, x - px4, y - py0);
    add_curve(x + px2, y - py2, x + px4, y - py0, x + px3, y - py1);
    add_curve(x + rx, y, x + px1, y - py3, x + px0, y - py4);
}

Ellipse::~Ellipse () { }

ivGlyph* Ellipse::clone () const {
    ivCoord rx = _x[0] - _x[4];
    ivCoord ry = _y[4] - _x[0];
    ivCoord x = _x[4];
    ivCoord y = _y[0];
    return new Ellipse(_brush, _stroke, _fill, x, y, rx, ry, _t);
}

/**********************************************************************/
Open_BSpline::Open_BSpline (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill, 
    ivCoord* x, ivCoord* y, int n, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, false, true, (n + 2) * 3 + 1, t) {
    Bspline_move_to(x[0], y[0], x[0], y[0], x[0], y[0]);
    Bspline_curve_to(x[0], y[0], x[0], y[0], x[1], y[1]);
    for (int i = 1; i < n - 1; ++i) {
        Bspline_curve_to(x[i], y[i], x[i-1], y[i-1], x[i+1], y[i+1]);
    }
    Bspline_curve_to(x[n-1], y[n-1], x[n-2], y[n-2], x[n-1], y[n-1]);
    Bspline_curve_to(x[n-1], y[n-1], x[n-1], y[n-1], x[n-1], y[n-1]);
}

Open_BSpline::Open_BSpline (Open_BSpline* gr) : Graphic(gr) {}


Open_BSpline::~Open_BSpline () { }

ivGlyph* Open_BSpline::clone () const {
    return new Open_BSpline((Open_BSpline*) this);
}

/**********************************************************************/
Closed_BSpline::Closed_BSpline (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill, 
    ivCoord* x, ivCoord* y, int n, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, true, true, n * 3 + 1, t) {
    Bspline_move_to(x[0], y[0], x[n-1], y[n-1], x[1], y[1]);
    for (int i = 1; i < n - 1; ++i) {
        Bspline_curve_to(x[i], y[i], x[i-1], y[i-1], x[i+1], y[i+1]);
    }
    Bspline_curve_to(x[n-1], y[n-1], x[n-2], y[n-2], x[0], y[0]);
    Bspline_curve_to(x[0], y[0], x[n-1], y[n-1], x[1], y[1]);
}

Closed_BSpline::Closed_BSpline (Closed_BSpline* gr) : Graphic(gr) {}

Closed_BSpline::~Closed_BSpline () { }

ivGlyph* Closed_BSpline::clone () const {
    return new Closed_BSpline((Closed_BSpline*) this);
}
/**********************************************************************/
Polyline::Polyline (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill,
    ivCoord* x, ivCoord* y, int n, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, false, false, n, t) {
    add_point(x[0], y[0]);
    for (int i = 1; i < n; ++i) {
        add_point(x[i], y[i]);
    }
}

Polyline::~Polyline () { }

ivGlyph* Polyline::clone () const {
    return new Polyline(_brush, _stroke, _fill, _x, _y, _ctrlpts, _t);
}

/**********************************************************************/
Polygon::Polygon (
    const ivBrush* brush, const ivColor* stroke, const ivColor* fill, 
    ivCoord* x, ivCoord* y, int n, ivTransformer* t
) : Graphic(brush, stroke, fill, nil, true, false, n, t) {
    add_point(x[0], y[0]);
    for (int i = 1; i < n; ++i) {
        add_point(x[i], y[i]);
    }
}

Polygon::~Polygon () { }

ivGlyph* Polygon::clone () const {
    return new Polygon(_brush, _stroke, _fill, _x, _y, _ctrlpts, _t);
}

/**********************************************************************/
Text::Text (
    const ivFont* f, const ivColor* fg, const char* text, ivTransformer* tx
) {
    _stroke = fg;
    ivResource::ref(_stroke);
    _font = f;
    ivResource::ref(_font);
    ivTransformer* t = new ivTransformer;
    transformer(t);
    ivResource::unref(t);
    if (tx != nil) {
        *_t = *tx;
    }
    _fill = fg; /* a hack */

    _text = nil;
    if (text != nil) {
        _text = new osString(text);
    }
    ivLayoutKit* layout = ivLayoutKit::instance();
    _body = layout->overlay();
    init();
}

Text::~Text () {
    delete _text;
    delete _body;
}

ivGlyph* Text::clone () const {
    return new Text(_font, _stroke, _text->string(), _t);
}

void Text::text (const char* text) {
    delete _text;
    _text = nil;
    if (text != nil) {
        _text = new osString(text);
    }
    init();
}

const char* Text::text () {
    return _text->string();
}

void Text::init () {
    ivLayoutKit* layout = ivLayoutKit::instance();
    ivPolyGlyph* col = layout->vbox();
    ivPolyGlyph* line = layout->hbox();
    ivFontBoundingBox bbox;
    _font->font_bbox(bbox);
    ivCoord lineheight = bbox.ascent() + bbox.descent();
    char ch = '\0';
    
    for (int i = 0; (*_text)[i] != '\0'; i++) {
        ch = (*_text)[i];

        if (ch == '\n') {
            line->append(layout->strut(_font));
            col->append(layout->fixed_dimension(line, Dimension_Y,lineheight));
            line = layout->hbox();
        } else if (ch == ' ') {
            line->append(new ivCharacter(' ', _font, _stroke));
        } else if (ch != ')' && ch != '(') {
            if (ch == '\\') {
                ch = (*_text)[++i];
                if (isdigit(ch)) {
                    ch -= '0';
                    ch *= 8;
                    char digit;
                    digit = (*_text)[i++];
                    ch = (ch * 8) + digit - '0';
                    digit = (*_text)[i++];
                    ch = (ch * 8) + digit - '0';
                }
            }
            line->append(new ivCharacter(ch, _font, _stroke));
        }
    }

    // *** added by CKD 07/20/93 so that text strings that are not terminated
    //     by a /n are still displayed
    if (ch != '\n') {
      line->append(layout->strut(_font));
      col->append(layout->fixed_dimension(line, Dimension_Y,lineheight));
    }

    ivTransformer fixtext;
    fixtext.translate(0, bbox.descent());
    _t->premultiply(fixtext);
    remove_all();
    _body->append(col);
}

void Text::draw (ivCanvas* c, const ivAllocation& a) const {
    if (_t != nil) {
        c->push_transform();
        c->transform(*_t);
    } 
    _body->draw(c, a);
    if (_t != nil) {
        c->pop_transform();
    }
}

void Text::draw_gs (ivCanvas* c, Graphic* gs) {
    ivTransformer* tx = gs->transformer();
    if (tx != nil) {
        c->push_transform();
        c->transform(*tx);
    }
    if (_ctrlpts == 0) {
        ivRequisition req;
        request(req);
    }
    ivAllocation b(_a);
    ivAllotment& bx = b.x_allotment();
    ivAllotment& by = b.y_allotment();
    bx.origin(0.0);
    by.origin(0.0);
    bx.span(_x[2]-_x[0]);
    by.span(_y[2]-_y[0]);

    _body->draw(c, b);
    if (tx != nil) {
        c->pop_transform();
    }
}

void Text::allocate(ivCanvas* c, const ivAllocation& a, ivExtension& ext) {
    if (_t != nil) {
        c->push_transform();
        c->transform(*_t);
    }
    if (_ctrlpts == 0) {
        ivRequisition req;
        request(req);
    }
    _a = a;
    ivAllocation b(_a);
    ivAllotment& bx = b.x_allotment();
    ivAllotment& by = b.y_allotment();
    bx.origin(0.0);
    by.origin(0.0);
    bx.span(_x[2]-_x[0]);
    by.span(_y[2]-_y[0]);
    _body->allocate(c, b, ext);
    if (_t != nil) {
        c->pop_transform();
    }
}

void Text::getextent_gs (
    ivCoord& l, ivCoord& b, ivCoord& cx, ivCoord& cy, ivCoord& tol, Graphic* gr
) {
    if (_ctrlpts == 0) {
        ivRequisition req;
        request(req);
    }
    ivCoord left, bottom, right, top;

    right = _x[2];
    top = _y[2];
    left = _x[0];
    bottom = _y[0];

    ivTransformer* t = gr->transformer();
    if (t != nil) {
        corners(left, bottom, right, top, *t);
    }
    tol = 3.0;
    l = left;
    b = bottom;
    cx = (left + right)/2.0;
    cy = (top + bottom)/2.0;
}

void Text::request (ivRequisition& req) const { 
    _body->request(req);
    ivRequirement& rx = req.x_requirement();
    ivRequirement& ry = req.y_requirement();

    ivCoord left, bottom, right, top;
    left = -rx.natural()*rx.alignment();
    right = left + rx.natural();
    bottom = -ry.natural()*ry.alignment();
    top = bottom + ry.natural();

    Text* text = (Text*) this;
    text->_ctrlpts = 4;
    text->_x[0] = left;
    text->_y[0] = bottom;
    text->_x[1] = left;
    text->_y[1] = top;
    text->_x[2] = right;
    text->_y[2] = top;
    text->_x[3] = right;
    text->_y[3] = bottom;

    if (_t != nil) {
        corners(left, bottom, right, top, *_t);
    }
    rx.natural(right - left);
    rx.stretch(0.0);
    rx.shrink(0.0);
    rx.alignment(-left / (rx.natural()== 0 ? 1 : rx.natural()));

    ry.natural(top - bottom);
    ry.stretch(0.0);
    ry.shrink(0.0);
    ry.alignment(-bottom / (ry.natural() == 0 ? 1 : ry.natural()));
}

Tool::Tool (unsigned int cur_tool) {
    _cur_tool = cur_tool;
    _toolstate = new ToolState;
}

Tool::~Tool () {
    delete _toolstate;
}

unsigned int Tool::tool () { return _cur_tool; }

void Tool::tool (unsigned int cur_tool) { _cur_tool = cur_tool; }

void Tool::reset () {
    delete _toolstate;
    _toolstate = new ToolState;
}


ToolState& Tool::toolstate () { return *_toolstate; }

void Tool::toolstate(ToolState* toolstate) {
    delete _toolstate;
    _toolstate = toolstate;
}

/////////////////
//   Xform     //
/////////////////

Xform::Xform(ivTransformer * tr){
  Initialize();
  if(tr == NULL) return;
  Set(tr);
}

void Xform::Initialize() {
  a00 = a01 = a10 = a11 = a20 = a21 = 0;
}

void Xform::Copy_(const Xform& cp) {
  a00 = cp.a00; a01 = cp.a01; a10 = cp.a10; a11 = cp.a11; 
  a20 = cp.a20; a21 = cp.a21;
}

void Xform::Set(float b00,float b01, float b10, float b11, float b20, float b21) {
  a00 = b00; a01 = b01; a10 = b10; a11 = b11; a20 = b20; a21 = b21;
}

void Xform::Set(ivTransformer* tr){
  tr->matrix(a00,a01,a10,a11,a20,a21);
}

ivTransformer* Xform::transformer() {
  return new ivTransformer(a00,a01,a10,a11,a20,a21);
}

