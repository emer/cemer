// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiMethod.h"
#include <MethodDef>
#include <taiWidgetMethod>

#include <taMisc>

void taiMethod::AddMethod(MethodDef* md) {
  InsertThisIntoBidList(md->im);
  //TEST:
  meth = md;
}

taiWidgetMethod* taiMethod::GetButtonMethodRep(void* base, IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_)
{
  taiWidgetMethod* rval = GetButtonMethodRep_impl(base, host_, par, gui_parent_, 0);
  rval->SetBase((taBase*)base); // pray!
  return rval;
}

taiWidgetMethod* taiMethod::GetGenericMethodRep(void* base, taiWidget* par) {
  // this case is ONLY called by the CallFun() function, on methods, typically via
  // a taBase->CallFun call
//   taiWidgetMethod* rval = new taiWidgetMethod(base, meth, meth->type, NULL, par, NULL, 0);
  // the above generic guy doesn't have all the right stuff -- use menu as default..
  if(!meth->GetOwnerType()->IsTaBase()) {
    taMisc::Error("Method is trying to establish a GUI interface via #MENU or #BUTTON -- only available for taBase objects -- please remove those directives!  method:", meth->name, "on type:", meth->GetOwnerType()->name);
    return NULL;
  }
  taiWidgetMethod* rval = GetMenuMethodRep_impl(base, NULL, par, NULL, 0);
  rval->SetBase((taBase*)base); // pray!
  return rval;
}

taiWidgetMethod* taiMethod::GetMenuMethodRep(void* base, IWidgetHost* host_, taiWidget* par,
  QWidget* gui_parent_)
{
  taiWidgetMethod* rval = GetMenuMethodRep_impl(base, host_, par, gui_parent_, 0);
  rval->SetBase((taBase*)base); // pray!
  return rval;
}
