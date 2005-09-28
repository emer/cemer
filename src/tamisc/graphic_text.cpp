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
 * planar graphics
 */

#include <iv_graphic/graphic_text.h>

#include <ta/enter_iv.h>
#include <InterViews/font.h>
#include <InterViews/color.h>
#include <InterViews/patch.h>
#include <ta/leave_iv.h>

#include <ta/taiv_data.h>


NoScale_Text_G::NoScale_Text_G(TAPtr o,GraphicMaster* ng, const ivFont* f,
			       const ivColor* fg,
			       const char* c, ivTransformer* tx) {
  master = ng;
  obj  = o;
  name = c;
  editable = false;
  editb_used = false;

  label = NULL;
  patch = NULL;
  select_effect = NULL;
  get_color = NULL;

  stroke(fg);
  _t = new ivTransformer;
  if (tx != nil) {
    *_t = *tx;
  }
  fill(fg);
  font(f);
}

NoScale_Text_G::~NoScale_Text_G(){
  if(label != NULL) ivResource::unref(label); label = NULL;
  if(patch != NULL) ivResource::unref(patch); patch = NULL;
}

void NoScale_Text_G::font(const ivFont* f){
  Graphic::font(f);
  init();
}

ivGlyph* NoScale_Text_G::clone() const {
  return new NoScale_Text_G(obj,master,_font, _stroke, name.chars(), _t);
}

void NoScale_Text_G::text(const char* t){
  if(name == t) return;
  name = t;
  init();
}

const char* NoScale_Text_G::text(){
  return name;
}

void NoScale_Text_G::stroke(const ivColor* c){
  Graphic::stroke(c);
  if(label != NULL) label->SetColor(c);
}

const ivColor*	NoScale_Text_G::stroke() {return Graphic::stroke();}

void NoScale_Text_G::fill(const ivColor* c){
  Graphic::fill(c);
  if(label != NULL) label->SetColor(c);
}

const ivColor* 	NoScale_Text_G::fill() { return Graphic::fill();}

bool NoScale_Text_G::effect_stretch(const ivEvent& , Tool& ,ivCoord ,ivCoord ,
				    ivCoord , ivCoord ) {
  return true;
}

bool NoScale_Text_G::select(const ivEvent& e, Tool& tool, bool unselect) {
  if(editable) {
    int but = Graphic::GetButton(e);
    if(but == ivEvent::right)
      editb_used = true;
    else
      editb_used = false;
  }
  return Graphic::select(e, tool, unselect);
}

bool NoScale_Text_G::effect_select(bool set_select) {
  bool tmp = Graphic::effect_select(set_select);
  if(!editable) return tmp;
  if(set_select && editb_used && (obj != NULL)) {
    obj->Edit();
  }
  editb_used = false;
  return tmp;
}

class AlwaysDrawPatch : public ivPatch {
public:
    virtual void draw(ivCanvas*, const ivAllocation&) const;
    AlwaysDrawPatch(ivGlyph* g) : ivPatch(g) {};
};

void AlwaysDrawPatch::draw(ivCanvas* c, const ivAllocation& a) const {
  ivMonoGlyph::draw(c, a);
}

void NoScale_Text_G::init(){
  if(patch == NULL) {
    patch = new AlwaysDrawPatch(NULL);
    ivResource::ref(patch);
  }
  patch->redraw(); // if label changes shape, this cleans off the old label mess
  if(label != NULL) ivResource::unref(label);
  label = new DynamicLabel((char*) name,_font,_stroke,NULL); ivResource::ref(label);
  patch->body(label);
  patch->reallocate();
}

void NoScale_Text_G::draw (ivCanvas* c, const ivAllocation& al) const {
  if(patch == NULL) return;
  if (_t != nil) {
    c->push_transform();
    c->transform(*_t);
  } 
  patch->draw(c, al);
  if (_t != nil) {
    c->pop_transform();
  }
}

void NoScale_Text_G::draw_gs (ivCanvas* c, Graphic* gs) {
  if(patch == NULL) return;
  ivTransformer* tx = gs->transformer();
  if (tx != nil) {
    c->push_transform();
    c->transform(*tx);
  }
  if (_ctrlpts == 0) {
    ivRequisition req;
    request(req);
  }
  ivAllocation b(a);
  ivAllotment& bx = b.x_allotment();
  ivAllotment& by = b.y_allotment();
  bx.origin(0.0);
  by.origin(0.0);
  bx.span(_x[2]-_x[0]);
  by.span(_y[2]-_y[0]);

  if(get_color != NULL) {
    ivColor* col = (*get_color)(master);
    if(_stroke != col) stroke(col);
  }
  patch->draw(c, b);
  // draw a box around it
  if(_selected || _picked){
    c->new_path();
    c->move_to(_x[0],_y[0]);
    c->line_to(_x[1],_y[1]);
    c->line_to(_x[2],_y[2]);
    c->line_to(_x[3],_y[3]);
    c->close_path();
    c->stroke(_stroke,_brush);
  }
  if (tx != nil) {
    c->pop_transform();
  }
}

void NoScale_Text_G::allocate(ivCanvas* c, const ivAllocation& al,
			     ivExtension& ext) {
  if ((transformer()) != nil) {
    c->push_transform();
    c->transform(*(transformer()));
  }
  if (_ctrlpts == 0) {
    ivRequisition req;
    request(req);
  }
  a = al;
  ivAllocation b(a);
  ivAllotment& bx = b.x_allotment();
  ivAllotment& by = b.y_allotment();
  bx.origin(0.0);
  by.origin(0.0);
  bx.span(_x[2]-_x[0]);
  by.span(_y[2]-_y[0]);
  patch->allocate(c, b, ext);
  if ((transformer()) != nil) {
    c->pop_transform();
  }
}

void NoScale_Text_G::getextent_gs(ivCoord& l,ivCoord& b,ivCoord& cx,
				 ivCoord& cy,ivCoord& tol,Graphic* gr){
  if(master->no_text_extent == true){
    l = b = cx = cy = 0;
  }
  else {
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
}
void NoScale_Text_G::request (ivRequisition& req) const { 
  patch->request(req);
  ivRequirement& rx = req.x_requirement();
  ivRequirement& ry = req.y_requirement();

  ivCoord left, bottom, right, top;
  left = -rx.natural()*rx.alignment();
  right = left + rx.natural();
  bottom = -ry.natural()*ry.alignment();
  top = bottom + ry.natural();

  NoScale_Text_G* gr = (NoScale_Text_G*) this;
  gr->_ctrlpts = 4;
  gr->_x[0] = left;
  gr->_y[0] = bottom;
  gr->_x[1] = left;
  gr->_y[1] = top;
  gr->_x[2] = right;
  gr->_y[2] = top;
  gr->_x[3] = right;
  gr->_y[3] = bottom;

  if (_t != nil) {
    corners(left, bottom, right, top, *_t);
  }
  rx.natural(right - left);
  rx.stretch(0.0);
  rx.shrink(0.0);
  rx.alignment(-left / rx.natural());

  ry.natural(top - bottom);
  ry.stretch(0.0);
  ry.shrink(0.0);
  ry.alignment(-bottom / ry.natural());
}

bool NoScale_Text_G::manip_move(const ivEvent& ,Tool& ,
				     ivCoord , ivCoord ,
				     ivCoord lx, ivCoord ly,
				     ivCoord cx, ivCoord cy) {
  float nx = master->GetCurrentXformScale();
  float ny = master->GetCurrentYformScale();
  cx *= nx; cy*= ny;
  lx *= nx; ly*=ny;
  translate(cx-lx, cy-ly);
  return true;
}


void NoScale_Text_G::transformer(ivTransformer * t){
 Graphic::transformer(t);
}

ivTransformer* NoScale_Text_G::transformer(){
  float sx = 1.0; float sy = 1.0;
  float nx = master->GetCurrentXformScale();
  float ny = master->GetCurrentYformScale();
  if(nx != 0) sx /= nx;
  if(ny != 0) sy /= ny;
  scaled_tform = *(Graphic::transformer());
  scaled_tform.scale(sx,sy);
  return &scaled_tform;
}

bool NoScale_Text_G::effect_border(){
//  bool result = Graphic::effect_border();
//  fill(stroke());
//  init();
  if(_selected)
    master->selectgroup.LinkUnique(obj);
  else {
    master->selectgroup.Remove(obj);
  }
  if(select_effect != NULL) select_effect(master);
  return true;
}

bool NoScale_Text_G::effect_move(const ivEvent& ev, Tool& tl, ivCoord ix,
				  ivCoord iy, ivCoord fx, ivCoord fy) {
  Graphic::effect_move(ev,tl,ix,iy,fx,fy);
  ivTransformer* tr = _t; // get the real thing
  if(tr == NULL) return false;
  // find the xform on the obj
  MemberDef* xform_mbr = obj->FindMember(&TA_Xform); // non ptr types
  bool ptr = false;
  if(xform_mbr == NULL) { // couldn't find non ptr version
    ptr = true;
    int idx;
    xform_mbr = obj->GetTypeDef()->members.FindTypeDerives(&TA_Xform,idx);
  }
  if(xform_mbr == NULL) return false;
  Xform* xfp;
  if(ptr == true){
    Xform** xfpp = (Xform **) xform_mbr->GetOff(obj);
    // if no xform create one
    if(*xfpp != NULL)  taBase::DelPointer((TAPtr*)xfpp);
    *xfpp = new Xform(tr);
    taBase::Own(*xfpp,obj);
    xfp = *xfpp;
  }
  else {
    xfp= (Xform *) xform_mbr->GetOff(obj);
  }
  xfp->Set(tr);
  taivMisc::RecordScript((xfp)->GetPath() + "->Set(" + 
 			  String((xfp)->a00) +  "," + String((xfp)->a11)+","+
 			  String((xfp)->a10) +  "," + String((xfp)->a10)+","+
 			  String((xfp)->a20) +  "," + String((xfp)->a20) +  ");");
  taivMisc::RecordScript(obj->GetPath() + ".UpdateAfterEdit();");
  return true;
}


