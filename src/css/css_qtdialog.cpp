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



#include "css_qtdialog.h"

#include "css_basic_types.h"
#include "css_c_ptr_types.h"

#include "ta_qttype.h"
#include "ta_type.h"
#include "ta_qt.h"

#include "css_ta.h"
#include "css_qt.h"
#include "css_qttype.h"
#include "css_qtdata.h"



#include "ta_TA_type.h"

//#include <stdlib.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <Q3ScrollView>
#include <Q3Table>
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
  membs.def_size = 0; // we handle all
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
    taiData* mb_dat = data_el(0).SafeEl(index);
    // now get label
    GetName(i, md, name, desc);
    AddName(i, name, desc, mb_dat);
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
    data_el(0).Add(mb_dat);
    AddData(index, mb_dat->GetRep());
    ++index;
  }
}

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

void cssiEditDialog::GetValue_Membs() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el(0).SafeEl(i);
    if (mb_dat == NULL) break; // shouldn't happen
    cit->GetValue(mb_dat);
    cit->orig_obj->UpdateAfterEdit();
  }
  obj->UpdateAfterEdit();
}

void cssiEditDialog::GetImage_Membs() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiData* mb_dat = data_el(0).SafeEl(i);
    if (mb_dat == NULL) break; // shouldn't happen
    cit->GetImage(mb_dat);
  }
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
  case cssEl::T_Variant:
    return new cssiType(orig_obj, &TA_Variant, (void*)&(((cssVariant*)el)->val));
  case cssEl::T_Enum:
    return new cssiEnumType(orig_obj, ((cssEnum*)el)->type_def,
			     (void*)&(((cssEnum*)el)->val));
  case cssEl::T_C_Ptr: {
    cssEl::cssTypes sb_typ = el->GetPtrType();
    if(sb_typ == cssEl::T_Int) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_int, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_int, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Bool) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_bool, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_bool, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Short) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_short, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_short, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Long) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_long, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_long, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_LongLong) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_int64_t, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_int64_t, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Char) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_char, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_char, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Real) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_double, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_double, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Float) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_float, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_float, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_String) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_taString, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_taString, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Variant) {
      if(read_only)
	return new cssiROType(orig_obj, &TA_Variant, (void*)(((cssCPtr*)el)->ptr));
      return new cssiType(orig_obj, &TA_Variant, (void*)(((cssCPtr*)el)->ptr));
    }
    if(sb_typ == cssEl::T_Enum) {
      cssCPtr_enum* enm = (cssCPtr_enum*)el;
      TypeDef* et = enm->GetEnumType();
      if(et != NULL)
	return new cssiType(orig_obj, et, (void*)(((cssCPtr*)el)->ptr));
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
    String arg_name = md->arg_names.FastEl(i);
    TypeDef* argtd = md->arg_types.FastEl(i);
    taiArgType* art = GetBestArgType(i, argtd,
				       md, typ);
    if (art == NULL) {
      taMisc::Warning("could not get a taiArgType for parameter of type: ", argtd->name,
        "for arg_name: ", arg_name, " -- no more parameters will be shown for this function"); 
      break;			// don't add new args after bad one..
    }
    cssEl* el = art->GetElFromArg(arg_name, base);
    if (el == NULL) {
      taMisc::Warning("could not get a cssEl for taiArgType: ", art->GetTypeDef()->name,
        "for arg_name: ", arg_name, " -- no more parameters will be shown for this function"); 
      delete art;
      break;
    }
    // set to default value if not empty
    String val = md->arg_vals.FastEl(i);
    if (!val.empty()) {
      // not for type def pointers (cuz you lose ability to select other types)
      if (!art->arg_typ->DerivesFrom(&TA_TypeDef) && !art->arg_typ->DerivesFrom(&TA_ios)) {
	while (val.firstchar() == ' ')
          val = val.after(' ');
	if (art->arg_typ->DerivesFormal(TA_enum)) {
	  art->arg_typ->SetValStr(val, art->arg_base);
	} else {
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

    data_el(0).Add(mb_dat);
    rep = mb_dat->GetRep();
    AddData(j, rep);
  }
}

void cssiArgDialog::Constr_Labels() {
  String name;
  String desc;
  taiData* mb_dat;

  // create the labels

  for (int i = 1 + hide_args; i < obj->members->size; ++i) { //note: we start from 1 + #ignored
    int j = i - hide_args;
    cssEl* md = obj->members->FastEl(i);

    mb_dat = data_el(0).SafeEl(j);

    GetName(j, md, name, desc);
    AddName(j - 1, name, desc, mb_dat);
  }
}

void cssiArgDialog::GetValue() {
  err_flag = false;
  for (int i = hide_args; i < type_el.size; ++i) {
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiData* mb_dat = data_el(0).SafeEl(i);
    if (mb_dat == NULL) break; // shouldn't happen
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
    taiData* mb_dat = data_el(0).SafeEl(i);
    if (mb_dat == NULL) break; // shouldn't happen
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


