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


#include "css_qtdialog.h"

#include "css_basic_types.h"
#include "css_c_ptr_types.h"

#include "ta_qttype.h"
#include "ta_type.h"
#include "ta_qt.h"

#include "ta_css.h"
#include "css_qt.h"
#include "css_qttype.h"
#include "css_qtdata.h"



#include "ta_TA_type.h"

//#include <stdlib.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <qscrollview.h>
#include <qtable.h>
#include <qtooltip.h>
#include <qwidget.h>

//////////////////////////////////////////////////
// 		cssiEditDialog			//
//////////////////////////////////////////////////

cssiEditDialog::cssiEditDialog(cssClassInst* ob, cssProgSpace* tp,
	bool read_only_, bool modal_, QObject* parent)
: taiEditDataHost(NULL, NULL, read_only_, modal_, parent)
{
  obj = ob;
  top = tp;
  if(ob->type_def->HasOption("NO_OK"))
    no_ok_but = true;
  if(ob->type_def->HasOption("NO_CANCEL"))
    read_only = true;
}

cssiEditDialog::~cssiEditDialog() {
  type_el.Reset();
}

void cssiEditDialog::ClearBody_impl() {
  type_el.Reset();
  taiEditDataHost::ClearBody_impl();
}

void cssiEditDialog::GetName(int idx, cssEl* md, String& name, String& desc) {
// TODO: get compound description, and if too long, then create a hyperlink-like label
// that pops up a little tooltip-style window -- see also taiEditDialog::GetNameRep
//   String nm = md->GetName();
//   ivGlyph* rval = layout->hfixed(taiM->lft_sep(wkit->label(nm)), hfix);
//   return rval;
  name = md->GetName();
  if ((obj->type_def != NULL) && (obj->type_def->member_desc.size > idx))
     desc = obj->type_def->member_desc[idx];
  else desc = "";
}

/* IV version:
void cssiEditDialog::Constr_Labels() {
  Constr_Labels_style();
  GetVFix(taiM->name_font);
  // GetHFix();
  ivFontBoundingBox fbb;
  int i;
  hfix = 0;
  for(i=0; i< obj->members->size; i++) {
    cssEl* md = obj->members->FastEl(i);
    String nm = md->GetName();
    taiM->name_font->string_bbox(nm, nm.length(), fbb);
    hfix = MAX(fbb.width(), hfix);
  }

  labels = layout->vbox();
  for(i=0; i< obj->members->size; i++) {
    cssEl* md = obj->members->FastEl(i);
    if((obj->type_def != NULL) && !obj->type_def->MbrHasOption(i, "SHOW") &&
       (obj->type_def->MbrHasOption(i, "HIDDEN") || obj->type_def->MbrHasOption(i, "READ_ONLY")))
      continue;
    bool read_only = false;
    if((obj->type_def != NULL) && obj->type_def->MbrHasOption(i, "READ_ONLY"))
      read_only = true;
    cssiType* tv = GetTypeFromEl(md, read_only); // get the actual object..
    if((tv == NULL) || (tv->cur_base == NULL))
      continue;
    type_el.Add(tv);
    // 32 is for the openlook extra v symbol...
    labels->append(layout->fixed(taiM->top_sep(GetNameRep(i, md)), hfix + 32, vfix));
  }
  wkit->pop_style();
}
*/
void cssiEditDialog::Constr_Labels() {
  int index = 0;
  String name;
  String desc;
  for (int i = 0; i < obj->members->size; ++i) {
    cssEl* md = obj->members->FastEl(i);
    if ((obj->type_def != NULL) && !obj->type_def->MbrHasOption(i, "SHOW") &&
       (obj->type_def->MbrHasOption(i, "HIDDEN") || obj->type_def->MbrHasOption(i, "READ_ONLY")))
      continue;
    bool read_only = false;
    if ((obj->type_def != NULL) && obj->type_def->MbrHasOption(i, "READ_ONLY"))
      read_only = true;
    cssiType* cit = GetTypeFromEl(md, read_only); // get the actual object..
    if ((cit == NULL) || (cit->cur_base == NULL))
      continue;

    // get the widget representation of the data
    taiData* mb_dat = data_el.SafeEl(index);
    // now get label
    GetName(i, md, name, desc);
    AddName(i, name, desc, mb_dat->GetRep());
    ++index;
  }
}

void cssiEditDialog::Constr_Data() {
  int index = 0;
  for (int i = 0; i < obj->members->size; ++i) {
    cssEl* md = obj->members->FastEl(i);
    if ((obj->type_def != NULL) && !obj->type_def->MbrHasOption(i, "SHOW") &&
       (obj->type_def->MbrHasOption(i, "HIDDEN") || obj->type_def->MbrHasOption(i, "READ_ONLY")))
      continue;
    bool read_only = false;
    if ((obj->type_def != NULL) && obj->type_def->MbrHasOption(i, "READ_ONLY"))
      read_only = true;
    cssiType* cit = GetTypeFromEl(md, read_only); // get the actual object..
    if ((cit == NULL) || (cit->cur_base == NULL))
      continue;

    type_el.Add(cit);
    // get the widget representation of the data
    taiData* mb_dat = cit->GetDataRep(this, NULL, body);
    data_el.Add(mb_dat);
    AddData(index, mb_dat->GetRep());
    ++index;
  }
}

/* NN:
void cssiEditDialog::Constr_Body() {
  if(menu != NULL) {
    body_box->prepend(taiM->vfsep);
    body_box->prepend(layout->hcenter(menu, 0));
  }
  if(meth_buts != NULL) {
    float ht = 1.4f * vfix;
    String rows;
    if(obj != NULL)
      rows = obj->type_def->OptionAfter("BUTROWS_");
    if(!rows.empty()) {
      ht *= (float)rows;
    }
    else {
      ivLRComposition* cm = (ivLRComposition*)meth_buts;
      if(cm->count() > 12)
	ht *= 3;
      else if(cm->count() > 6)
	ht *= 2;
    }
    meth_butg = layout->vflexible(layout->vnatural(layout->center(meth_buts, 0, 0),ht),fil,1.4f * vfix);
    body_box->append(meth_butg);
    body_box->append(taiM->vfspc);
  }
  body_box->append(but_patch);

  body = new ivPatch(layout->margin(body_box, taiM->hsep_c));
  ivResource::ref(body);
}
*/

void cssiEditDialog::Constr_Strings(const char*, const char* win_title) {
  prompt_str = obj->GetTypeName();
  prompt_str += String(" ") + obj->name + ": ";
  if (obj->type_def != NULL)
    prompt_str += obj->type_def->desc;
//TODO:  win_str = String(cssiSession::instance()->classname()) + ": " + win_title;
  win_str = String("(classname will go here): ") + win_title; //TEMP
  win_str += String(" ") + obj->name;
}

int cssiEditDialog::Edit(bool modal_) {
  if (!modal_)
    taiMisc::css_active_edits.Add(this);
  return taiEditDataHost::Edit(modal_);
}

void cssiEditDialog::GetValue() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    cit->GetValue(mb_dat);
    cit->orig_obj->UpdateAfterEdit();
  }
  obj->UpdateAfterEdit();
  Unchanged();
}

void cssiEditDialog::GetImage() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    cit->GetImage(mb_dat);
  }
  Unchanged();
}

cssiType* cssiEditDialog::GetTypeFromEl(cssEl* el, bool read_only) {
  cssEl* orig_obj = el;
  if(el->IsRef() || (el->GetType() == cssEl::T_Ptr))
    el = el->GetActualObj();

  switch(el->GetType()) {
  case cssEl::T_Bool:
    if(read_only)
      return new cssiROType(orig_obj, &TA_bool, (void*)&(((cssBool*)el)->val));
    return new cssiType(orig_obj, &TA_bool, (void*)&(((cssBool*)el)->val));
  case cssEl::T_Int:
    if(read_only)
      return new cssiROType(orig_obj, &TA_int, (void*)&(((cssInt*)el)->val));
    return new cssiType(orig_obj, &TA_int, (void*)&(((cssInt*)el)->val));
  case cssEl::T_Real:
    if(read_only)
      return new cssiROType(orig_obj, &TA_double, (void*)&(((cssReal*)el)->val));
    return new cssiType(orig_obj, &TA_double, (void*)&(((cssReal*)el)->val));
  case cssEl::T_Array:
    return new cssiArrayType(orig_obj, (void*)el);
  case cssEl::T_Class:
    return new cssiClassType(orig_obj, (void*)el);
  case cssEl::T_String:
    return new cssiType(orig_obj, &TA_taString, (void*)&(((cssString*)el)->val));
  case cssEl::T_Enum:
    return new cssiEnumType(orig_obj, ((cssEnum*)el)->type_def,
			     (void*)&(((cssEnum*)el)->val));
  case cssEl::T_C_Ptr: {
    String sb_typ = el->GetTypeName();
    if(sb_typ == "(c_int)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_int, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_int, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_bool)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_bool, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_bool, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_short)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_short, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_short, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_long)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_long, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_long, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_char)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_char, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_char, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_double)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_double, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_double, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_float)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_float, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_float, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_String)") {
      if(read_only)
	return new cssiROType(orig_obj, &TA_taString, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_taString, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == "(c_enum)") {
      cssCPtr_enum* enm = (cssCPtr_enum*)el;
      MemberDef* md = enm->GetEnumType();
      if(md != NULL)
	return new cssiType(orig_obj, md->type, (void*)(((cssCPtr*)el)->ptr));
      else
	return new cssiType(orig_obj, &TA_int, (void*)(((cssCPtr*)el)->ptr));
    }
    break;
  }
  case cssEl::T_TA: {
    TypeDef* td = ((cssTA*)el)->type_def;
    if(td == NULL) return NULL;
    // todo: put in support for fstreams, enums, etc.
    if(td->DerivesFrom(TA_taBase)) {
      if(td->ptr == 0)
	return new cssiType(orig_obj, td, (void*)&(((cssTA*)el)->ptr), true); // needs ptr type
      else
	return new cssiType(orig_obj, td, (void*)&(((cssTA*)el)->ptr)); // already has ptr type
    }
    if(td->DerivesFrom(TA_TypeDef) && (td->ptr == 1))
      return new cssiType(orig_obj, &TA_TypeDef_ptr, (void*)&(((cssTA*)el)->ptr));
    else if(td->DerivesFrom(TA_MemberDef) && (td->ptr == 1))
      return new cssiType(orig_obj, &TA_MemberDef_ptr, (void*)&(((cssTA*)el)->ptr));
    else if(td->DerivesFrom(TA_MethodDef) && (td->ptr == 1))
      return new cssiType(orig_obj, &TA_MethodDef_ptr, (void*)&(((cssTA*)el)->ptr));

    return NULL;
  }
  default:
    return NULL;
  }
  return NULL;
}

/*
void cssiEditDialog::Constr_Methods() {
  if((obj == NULL) || (obj->type_def == NULL))
    return;
  cssClassType* td = obj->type_def;
  int i;
  for(i=0; i<td->methods->size; i++) {
    cssMbrScriptFun* md = (cssMbrScriptFun*)td->methods->FastEl(i);
    if((md->GetType() == cssEl::T_MbrCFun) ||
       md->is_tor || md->HasOption("HIDDEN"))
      continue;
    cssiMethMenu* mth_rep = new cssiMethMenu(obj, top, md, this);
    meth_el.Add(mth_rep);
    if(mth_rep->is_menu_item) {
      GetMenuRep(md);
      mth_rep->AddToMenu(cur_menu);
    }
    else {
      if(meth_buts == NULL)
	GetButtonRep();
//        if(meth_buts->count() > 0)
//  	meth_buts->append(taiM->hsep);
      meth_buts->append(mth_rep->GetLook());
    }
  }
}

void cssiEditDialog::GetMenuRep(cssMbrScriptFun* md) {
  if(menu == NULL)
    menu = wkit->menubar();
  String men_nm = md->OptionAfter("MENU_ON_");
  if(men_nm != "") {
    cur_menu = ta_menus.FindName(men_nm);
    if(cur_menu != NULL)
      return;
  }
  if(cur_menu != NULL)
    return;

  if(men_nm == "")
    men_nm = "Actions";
  cur_menu = new
    taiMenu(taiMenu::menuitem, taiMenu::normal, taiMenu::small,
	      men_nm);
  menu->append_item(cur_menu->GetMenuItem());
  ta_menus.Add(cur_menu);
}
*/


///////////////////////////
//    cssiArgDialog       //
///////////////////////////

cssiArgDialog::cssiArgDialog(MethodDef* md_, TypeDef* typ_, void* base_, int use_argc_, int hide_args_,
  bool read_only_, bool modal_, QObject* parent)
: cssiEditDialog(new cssClassInst(md_->name), NULL, read_only_, modal_, parent)
{
  md = md_;
  typ = typ_;
  base = base_;
  use_argc = use_argc_;
  hide_args = hide_args_;
  err_flag = false;
  use_show = false;
}


cssiArgDialog::~cssiArgDialog() {
  delete obj;
  obj = NULL;
}

void cssiArgDialog::Ok() {
  GetValue();			// always get the value of an arg dialog..
  state = ACCEPTED;
  dialog->dismiss(1);
}

void cssiArgDialog::Constr_Strings(const char*, const char* win_title) {
  if (md->HasOption("CONFIRM")) {
    prompt_str = "Ok To: ";
    prompt_str += md->name + ": " + md->desc;
    win_str = String(taiM->classname()) + ": " + win_title;
    win_str += String(" ") + md->name + " () Confirm";
  }
  else {
    prompt_str = md->name + ": " + md->desc;
    win_str = String(taiM->classname()) + ": " + win_title;
    win_str += String(" ") + md->name + " (Args)";
  }
}

void cssiArgDialog::Constr_impl()
{
  Constr_ArgTypes();
  cssiEditDialog::Constr_impl();
}

void cssiArgDialog::Constr_ArgTypes() {
  obj->name = String("(") + md->type->name + ") " + md->name +
    ": " + md->desc;
  obj->members->Push(&cssMisc::Void); // this is just a place-holder for arg[0]
  // note: constr all args, even if any leading hidden
  for (int i = 0; i < use_argc; ++i) {
    taiArgType* art = GetBestArgType(i, md->arg_types.FastEl(i),
				       md, typ);
    if (art == NULL)
      break;			// don't add new args after bad one..
    cssEl* el = art->GetElFromArg(md->arg_names.FastEl(i), base);
    if (el == NULL) {
      delete art;
      break;
    }
    // set to default value if not empty
    if (!md->arg_vals.FastEl(i).empty()) {
      // not for type def pointers (cuz you lose ability to select other types)
      if (!art->arg_typ->DerivesFrom(&TA_TypeDef) && !art->arg_typ->DerivesFrom(&TA_ios)) {
	String val = md->arg_vals.FastEl(i);
	while (val.firstchar() == ' ')
          val = val.after(' ');
	if (art->arg_typ->DerivesFormal(TA_enum)) {
	  art->arg_typ->SetValStr(val, art->arg_base);
	}
	else {
	  *el = val;
	}
      }
    }
    obj->members->Push(el);
    type_el.Add(art);
  }
}

taiArgType* cssiArgDialog::GetBestArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  taiArgType* hi_arg = NULL;
  int hi_bid = 0;
  for (int i = 0; i < taiMisc::arg_types.size; ++i) {
    taiArgType* art = (taiArgType*)taiMisc::arg_types.FastEl(i)->GetInstance();
    int bid = art->BidForArgType(aidx, argt, md, td);
    if (bid >= hi_bid) {		// preference for the last one..
      hi_bid = bid;
      hi_arg = art;
    }
  }
  if (hi_arg == NULL)
    return NULL;
  return hi_arg->ArgTypeInst(argt, md, td);
}

void cssiArgDialog::Constr_Data() {
  QWidget* rep;
  taiData* mb_dat;

  // create the data fields
  for (int i = hide_args; i < type_el.size; ++i) {
    int j = i - hide_args;
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    mb_dat = art->GetDataRep(this, NULL, (QWidget*)body);

    data_el.Add(mb_dat);
    rep = mb_dat->GetRep();
    AddData(j, rep);
  }
}

void cssiArgDialog::Constr_Labels() {
  QWidget* rep;
  String name;
  String desc;
  taiData* mb_dat;

  // create the labels

  for (int i = 1 + hide_args; i < obj->members->size; ++i) { //note: we start from 1 + #ignored
    int j = i - hide_args;
    cssEl* md = obj->members->FastEl(i);

    rep = NULL;
    mb_dat = data_el.SafeEl(j);
    if (mb_dat != NULL)
      rep = mb_dat->GetRep();

    GetName(j, md, name, desc);
    AddName(j - 1, name, desc, rep);
  }
}

void cssiArgDialog::GetValue() {
  err_flag = false;
  for (int i = hide_args; i < type_el.size; ++i) {
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    art->GetValue(mb_dat, base);
    if (art->err_flag)
      err_flag = true;
    else {
      if(art->arg_typ->DerivesFormal(TA_enum)) {
	art->meth->arg_vals.FastEl(i) = art->arg_typ->GetValStr(art->arg_base);
      }
      else if(!art->arg_typ->DerivesFrom(TA_ios)) {
	art->meth->arg_vals.FastEl(i) = art->arg_val->GetStr();
      }
    }
  }
  Unchanged();
}

void cssiArgDialog::GetImage() {
  for (int i = hide_args; i < type_el.size; ++i) {
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiData* mb_dat = data_el.FastEl(i);
    art->GetImage(mb_dat, base);
  }
  Unchanged();
}

int cssiArgDialog::Edit(bool modal_) {
  // special case for a single stream arg
  if ((use_argc == 1) && md->arg_types[0]->InheritsFrom(TA_ios)
     && !md->HasOption("FILE_ARG_EDIT"))
  {
    taiStreamArgType* sa = (taiStreamArgType*)type_el.FastEl(0);
    if (sa->gf == NULL)
      return cssiEditDialog::Edit(modal_);
    if (sa->arg_typ->InheritsFrom(TA_ostream)) {
      if (md->HasOption("QUICK_SAVE"))
	sa->gf->Save();
      else if (md->HasOption("APPEND_FILE"))
	sa->gf->Append();
      else
	sa->gf->SaveAs();
    }
    else {
      sa->gf->Open();
    }
    state = ACCEPTED;
    sa->GetValueFromGF();
    if (sa->err_flag)
      return false;
    return true;
  } else
    return cssiEditDialog::Edit(modal_);
}


