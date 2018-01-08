// Co2018ght 2006-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "css_ta.h"
#include "css_qt.h"
#include "css_qttype.h"
#include "css_qtdata.h"

#include <taMisc>
#include <tabMisc>
#include <taBase>
#include <int_Matrix>
#include <taiMisc>
#include <iDialogEditor>
#include <taiArgTypeOfStream>
#include <taiWidgetTokenChooser>
#include <iHiLightButton>
#include <taFiler>
#include <DataTable>
#include <iCodeCompleter>

#include <QLabel>
#include <QLayout>
#include <QObject>
#include <QToolTip>
#include <QWidget>


//////////////////////////////////////////////////
// 		cssiEditDialog			//
//////////////////////////////////////////////////

cssiEditDialog::cssiEditDialog(cssClassInst* ob, cssProgSpace* tp,
	bool read_only_, bool modal_, QObject* parent)
: taiEditorOfClass(NULL, NULL, read_only_, modal_, parent)
{
  obj = ob;
  top = tp;
  if(ob->type_def->HasOption("NO_OK"))
    no_ok_but = true;
  if(ob->type_def->HasOption("NO_CANCEL"))
    read_only = true;
  membs.def_size = 0; // we handle all
  reshow_on_apply = false; // breaks us, and not necessary
}

cssiEditDialog::~cssiEditDialog() {
  type_el.Reset();
}

void cssiEditDialog::ClearBody_impl() {
  type_el.Reset();
  taiEditorOfClass::ClearBody_impl();
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

void cssiEditDialog::Constr_Widget_Labels() {
  String name;
  String desc;
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
    taiWidget* mb_dat = cit->GetWidgetRep(this, NULL, body);
    widget_el(0).Add(mb_dat);
    //AddWidget(index, mb_dat->GetRep());
    
    // now get label
    GetName(i, md, name, desc);
    AddNameWidget(-1, name, desc, mb_dat->GetRep(), mb_dat);
    
    ++index;
  }
}

void cssiEditDialog::Constr_Strings() {
  prompt_str = obj->GetTypeName();
  prompt_str += String(" ") + obj->name + ": ";
  if (obj->type_def != NULL)
    prompt_str += obj->type_def->desc;
//TODO:  win_str = String(cssiSession::instance()->classname()) + ": " + win_title;
  win_str = String("(classname will go here): ") + def_title(); //TEMP
  win_str += String(" ") + obj->name;
}

int cssiEditDialog::Edit(bool modal_, int min_width, int min_height) {
  if (!modal_)
    taiMisc::css_active_edits.Add(this);
  return taiEditorOfClass::Edit(modal_, min_width, min_height);
}

void cssiEditDialog::GetValue() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiWidget* mb_dat = widget_el(0).SafeEl(i);
    if (mb_dat == NULL) break; // shouldn't happen
    cit->GetValue(mb_dat);
    cit->orig_obj->UpdateAfterEdit();
  }
  Unchanged();
  obj->UpdateAfterEdit();
}

void cssiEditDialog::GetImage_Membs() {
  for (int i = 0; i < type_el.size; ++i) {
    cssiType* cit = (cssiType*)type_el.FastEl(i);
    taiWidget* mb_dat = widget_el(0).SafeEl(i);
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
    if(td->IsTaBase()) {
      if(!td->IsPointer())
	return new cssiType(orig_obj, td, (void*)&(((cssTA*)el)->ptr), true); // needs ptr type
      else
	return new cssiType(orig_obj, td, (void*)&(((cssTA*)el)->ptr)); // already has ptr type
    }
    if(td->DerivesFrom(TA_TypeDef) && (td->IsPointer()))
      return new cssiType(orig_obj, TA_TypeDef.GetPtrType(),
                          (void*)&(((cssTA*)el)->ptr));
    else if(td->DerivesFrom(TA_MemberDef) && (td->IsPointer()))
      return new cssiType(orig_obj, TA_MemberDef.GetPtrType(),
                          (void*)&(((cssTA*)el)->ptr));
    else if(td->DerivesFrom(TA_MethodDef) && (td->IsPointer()))
      return new cssiType(orig_obj, TA_MethodDef.GetPtrType(),
                          (void*)&(((cssTA*)el)->ptr));

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
    taiWidgetMenu(taiWidgetMenu::menuitem, taiWidgetMenu::normal, taiWidgetMenu::small,
	      men_nm);
  menu->append_item(cur_menu->GetMenuItem());
  ta_menus.Add(cur_menu);
}
*/


///////////////////////////
//    cssiArgDialog       //
///////////////////////////

// this is the offset of arg indexes into css-passed arg strings relative to the actual
// type scanned arg values, which start at 0
// 0 = instruction element??
// 1 = 'this' pointer for methods
// 2+ = actual args
static const int stub_arg_off = 2;

cssiArgDialog::cssiArgDialog(MethodDef* md_, TypeDef* typ_, void* base_, int use_argc_, int hide_args_,
  bool read_only_, bool modal_, QObject* parent)
: cssiEditDialog(new cssClassInst(md_->name), NULL, read_only_, modal_, parent)
{
  md = md_;
  typ = typ_;
  root = base_;
  use_argc = use_argc_;
  hide_args = hide_args_;
  err_flag = false;
  code_completer = NULL;
}


cssiArgDialog::~cssiArgDialog() {
  if (code_completer) {
    delete code_completer;
    code_completer = NULL;
  }
  delete obj;
  obj = NULL;
}

void cssiArgDialog::Ok() {
  GetValue();			// always get the value of an arg dialog..
  state = ACCEPTED;
  dialog->dismiss(1);
}

void cssiArgDialog::Constr_Strings() {
  if (md->HasOption("CONFIRM")) {
    prompt_str = "Ok To: ";
    prompt_str += md->name + ": " + md->desc;
    win_str = String(taiM->classname()) + ": " + def_title();
    win_str += String(" ") + md->name + " () Confirm";
  }
  else {
    prompt_str = md->name + ": " + md->desc;
    win_str = String(taiM->classname()) + ": " + def_title();
    win_str += String(" ") + md->name + " (Args)";
  }
}

void cssiArgDialog::Constr_impl()
{
  cssTA::BuildCssObjFromArgTypes(obj, md, root, use_argc, type_el);
  cssiEditDialog::Constr_impl();
}

void cssiArgDialog::Constr_Widget_Labels() {
  String name;
  String desc;
  // create the data fields
  for (int i = hide_args; i < type_el.size; ++i) {
    int j = i - hide_args;
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiWidget* mb_dat = art->GetWidgetRep(this, NULL, (QWidget*)body);

    widget_el(0).Add(mb_dat);
    QWidget* rep = mb_dat->GetRep();
    //int row = AddWidget(-1, rep);
    
    cssEl* tmd = obj->members->FastEl(i+stub_arg_off); // need to skip over arg[0] arg[1]
    GetName(j, tmd, name, desc);
    AddNameWidget(-1, name, desc, rep, mb_dat);
  }
}

void cssiArgDialog::GetValue() {
  err_flag = false;
  for (int i = hide_args; i < type_el.size; ++i) {
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiWidget* mb_dat = widget_el(0).SafeEl(i-hide_args);
    if (mb_dat == NULL) break; // shouldn't happen
    art->GetValue(mb_dat, root);
    if (art->err_flag)
      err_flag = true;
    else if(!art->meth->HasOption("NO_SAVE_ARG_VAL")) {
      if(art->arg_typ->IsEnum()) {
	art->meth->arg_vals.FastEl(i) = art->arg_typ->GetValStr(art->arg_base);
      }
      else if(!art->arg_typ->DerivesFrom(TA_ios)) {
	art->meth->arg_vals.FastEl(i) = art->arg_val->GetStr();
#ifdef DEBUG
// 	cerr << i << " " << art->arg_typ->name << " arg val: " << art->meth->arg_vals.FastEl(i) << endl;
#endif
      }
    }
  }
  Unchanged();
}

String cssiArgDialog::GetArgValue(int index) {
  String arg_value;
  err_flag = false;
  int arg_index = hide_args + index;
  taiArgType* art = (taiArgType*)type_el.FastEl(hide_args + arg_index);
  taiWidget* mb_dat = widget_el(0).SafeEl(index - hide_args);
  if (mb_dat == NULL) return _nilString; // shouldn't happen
  art->GetValue(mb_dat, root);
  return art->arg_val->GetStr();
}

taBase* cssiArgDialog::GetBaseForArg(const String& arg_name) {
  taiArgType* art = NULL;
  taiWidget* mb_dat = NULL;
  for (int i = hide_args; i < type_el.size; ++i) {
    art = (taiArgType*)type_el.FastEl(i);
    if (art->arg_val->name == arg_name) {
      mb_dat = widget_el(0).SafeEl(i-hide_args);
      break;
    }
  }
//  taiArgType* art = (taiArgType*)type_el.FastEl(arg_pos);
//  taiWidget* mb_dat = widget_el(0).SafeEl(arg_pos-hide_args);
  if (mb_dat == NULL) return NULL; // shouldn't happen
  
  art->GetValue(mb_dat, root);
//  taMisc::DebugInfo(art->arg_val->GetName());  // could pass in name instead of position!! then loop with this code to find the arg
//  taMisc::DebugInfo((String)art->arg_val);
  
  taBase* obj = (taBase*)art->arg_val->GetVoidPtrOfType(&TA_taBase);
  return obj;
}

void cssiArgDialog::GetImage(bool) {
  for (int i = hide_args; i < type_el.size; ++i) {
    taiArgType* art = (taiArgType*)type_el.FastEl(i);
    taiWidget* mb_dat = widget_el(0).SafeEl(i-hide_args);
    if (mb_dat == NULL) break; // shouldn't happen
    art->GetImage(mb_dat, root);
  }
  GetImage_PromptTitle();
  Unchanged();
}

int cssiArgDialog::Edit(bool modal_, int min_width, int min_height) {
  if(use_argc == 1) {
    // special cases for single args -- can pop up choosers directly instead
    TypeDef* argt = md->arg_types[0];
    if(argt->InheritsFrom(TA_ios) && !md->HasOption("FILE_ARG_EDIT")) {
      taiArgTypeOfStream* sa = (taiArgTypeOfStream*)type_el.FastEl(0);
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
    }
    else if(argt->IsBasePointerType()) {
      taiArgType* art = (taiArgType*)type_el.FastEl(hide_args);
      taiWidget* mb_dat = widget_el(0).SafeEl(hide_args);
      if (mb_dat == NULL) return false; // shouldn't happen
      art->GetImage(mb_dat, root);
      taiWidgetTokenChooser* tokbut = (taiWidgetTokenChooser*)mb_dat;
      bool ok = tokbut->OpenChooser(); // call chooser now
      if(ok) {
	art->GetValue(mb_dat, root);
      }
      state = ACCEPTED;
      return ok;
    }
  }
  return cssiEditDialog::Edit(modal_, min_width, min_height);
}


