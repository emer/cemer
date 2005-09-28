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



#include "fontspec.h"
#include "xform.h"
#include "ta_qt.h"
#include "ta_css.h" // for label viewer and master lookup

#include "ifont.h"

#include <qfontdialog.h>

void FontSpec::Initialize() {
  text_g = NULL;
}

void FontSpec::InitLinks() {
  inherited::InitLinks();
  if(!taiMisc::gui_active) return;
/*obs
#ifndef WINDOWS
  pattern = "*" + (String) fnt->name + "-*";
//TODO???  if (fnt != (iFont*)iFont::lookup(pattern)) {
//    pattern = "*-" + (String) fnt->name() + "-medium-r*";
//  }
#else
  pattern = "*" + (String) fnt->name + "*medium*--10*";
#endif
  prv_pat = pattern; */
}

void FontSpec::CutLinks() {
  text_g = NULL;
/*obs  if(text_g != NULL) {
    ivResource::unref(text_g);
    text_g = NULL;
  } */
  inherited::CutLinks();
}

void FontSpec::SetFont(char* fn) {
  pattern = fn;
  UpdateAfterEdit();
}

void FontSpec::SetFontSize(int sz) {
  pointSize = sz;
/* obs
  String szstr = String(sz);
  int len = pattern.length();
  int szpos;
  for(szpos=0;szpos<len;szpos++) {
    if(isdigit(pattern[szpos])) break;
  }
  if(szpos < len) {
    pattern = pattern.before(szpos) + szstr + "*";
  }
  else {
    pattern += szstr + "*";
  }
  UpdateAfterEdit(); */
}

void FontSpec::SetTextG(NoScale_Text_G* txg) {
//obs  if(text_g != NULL)
//obs    ivResource::unref(text_g);
  text_g = txg;
//obs  ivResource::ref(text_g);
}

void FontSpec::UpdateAfterEdit(){
  if(!taiMisc::gui_active) return;
  if (!pattern.empty()) {
    //legacy value -- from load
  #if (defined(WINDOWS) && (!defined(CYGWIN)))
    if(pattern.contains("-Helvetica-medium-r"))
      pattern.gsub("Helvetica-medium-r", "Arial*medium*");
    if(pattern.contains("-helvetica-medium-r"))
      pattern.gsub("helvetica-medium-r", "Arial*medium*");
    if(pattern.contains("Arial*medium*")) {
      fontName = "Arial";
      String szs = pattern.after("Arial*medium*");
      String nwsz;
      int i;
      for(i=0;i<(int)szs.length();i++) {
        if(!isdigit(szs[i])) continue;
        nwsz += szs[i];
      }
      if(nwsz.empty()) pointSize = 12;
      else if((int)nwsz < 9) pointSize = 9;
      else pointSize = (int)nwsz;
    }
  #else
  /*code was:  const iFont* f = iFont::lookup(pattern);
    if((f==NULL) && pattern.contains("Arial*medium*")) {
      String szs = pattern.after("Arial*medium*");
      String nwsz;
      int i;
      for(i=0;i<(int)szs.length();i++) {
        if(!isdigit(szs[i])) continue;
        nwsz += szs[i];
      }
      if(nwsz.empty()) nwsz = "10";
      else if((int)nwsz <= 9) nwsz = "8";
      pattern = "*-Helvetica-medium-r*" + nwsz + "*";
      f = iFont::lookup(pattern);
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-medium-r*"));
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-r*"));
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-*"));
    }
    if(f==NULL){
      taMisc::Error("Cannot find font: ", pattern);
      return;
    }
    prv_pat = pattern;
    if(fnt == f) return;
    ivResource::unref(fnt);
    fnt = (iFont*)f;
    ivResource::ref(fnt);
  */
  //TODO: following is just a temporary hack:
    this->setRawName(pattern);
    pattern = _nilString;
#endif
  }
/*TODO:  if(text_g != NULL) {
    text_g->font(fnt);
  } */
  inherited::UpdateAfterEdit();
}

void FontSpec::SelectFont() {
  QFont fnt;
  copyTo(fnt);
  bool ok;
  QFontDialog::getFont(&ok, fnt, NULL, NULL);
  if (ok) {
    this->copyFrom(fnt);
    UpdateAfterEdit();
  }
}

void FontSpec::Copy_(const FontSpec& cp) {
  pattern = cp.pattern;
  iFont::copy((iFont)cp);
}


//////////////////////////
//	ViewLabel	//
//////////////////////////

void ViewLabel::Initialize() {
  label_xform = NULL;
  master = NULL;
  viewer = NULL;
  select_effect = NULL;
  get_color = NULL;
}

void ViewLabel::Destroy() {
  CutLinks();
  master = NULL;
  viewer = NULL;
}

void ViewLabel::Copy_(const ViewLabel& cp) {
  spec = cp.spec;
/*TODO  if(cp.label_xform != NULL) {
    SetLabelXform((Xform*)cp.label_xform->Clone());
  } */
}

void ViewLabel::UpdateAfterEdit(){
  taNBase::UpdateAfterEdit();
  if(!taMisc::gui_active) return;
/*TODO  // we need to find something with a viewer and a GraphicMaster
  GetMasterViewer();
  UpdateView(); */
}

void ViewLabel::InitLinks(){
  taNBase::InitLinks(); //TODO: huh??? spec says NOT to call inherited
  taBase::Own(spec, this);
  if (!taMisc::gui_active) return;
}

void ViewLabel::CutLinks() {
  spec.CutLinks();
//TODO  RemoveFromView();
  taBase::DelPointer((TAPtr*)&label_xform);
  taNBase::CutLinks();
}
/*TODO
void ViewLabel::SetLabelXform(Xform* xf){
  if(label_xform != NULL)
    taBase::DelPointer((TAPtr*)&label_xform);
  label_xform = xf;
  taBase::Own(label_xform,this);
}

void ViewLabel::XFontSel(){
  spec.XFontSel();
}

void ViewLabel::MakeText() {
  GetMasterViewer();
  if((master == NULL) || (viewer == NULL)) return;
  if(spec.text_g == NULL) {
    ivWidgetKit* wkit = ivWidgetKit::instance();
    spec.SetTextG(new NoScale_Text_G(this,master,spec.fnt,wkit->foreground(),
				     name,NULL));
    spec.text_g->editable = true;
    if(select_effect != NULL) spec.text_g->select_effect = select_effect;
    if(get_color != NULL) spec.text_g->get_color = get_color;
    if(label_xform != NULL) {
      ivTransformer* tx = label_xform->transformer();
      spec.text_g->transformer(tx);
      ivResource::unref(tx);	// unref the self-ref from transform
    }
  }
  AddToView();
}

void ViewLabel::AddToView() {
  if(UpdateView()) return;
  master->append_(spec.text_g);
}

bool ViewLabel::UpdateView() {
  if((viewer == NULL) || (master == NULL) || (spec.text_g == NULL))  return false;
  spec.text_g->select_effect = select_effect;
  spec.text_g->text(name);
  if(label_xform != NULL) {
    ivTransformer* tx = label_xform->transformer();
    spec.text_g->transformer(tx);
    ivResource::unref(tx);	// unref the self-ref from transform
  }
  Graphic* vlg;
  ivGlyphIndex j;
  for(j=master->count_()-1; j>=0; j--) {
    vlg = master->component_(j);
    if((vlg->InheritsFrom(&TA_NoScale_Text_G)) && (((NoScale_Text_G*)vlg)->obj == this)) {
      //	  master->modified_(j);
      if(viewer->canvas() != NULL)
	vlg->damage_me(viewer->canvas());
      return true;
    }
  }
  return false;
}

void ViewLabel::RemoveFromView() {
  if((viewer == NULL) || (master == NULL))  return;
  Graphic* vlg;
  ivGlyphIndex j;
  for(j=master->count_()-1; j>=0; j--) {
    vlg = master->component_(j);
    if((vlg->InheritsFrom(&TA_NoScale_Text_G)) && (((NoScale_Text_G*)vlg)->obj == this)) {
      if(viewer->canvas() != NULL)
	vlg->damage_me(viewer->canvas());
      master->remove_(j);
      return; // we've found and removed the label no need to keep searching
    }
  }
}
*/



