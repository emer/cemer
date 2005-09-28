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


#include <ta_misc/ta_uifiler_iv.h> 
//#include <ta/ta_base.h>
//#include <ta/tdefaults.h>
//#include <ta/ta_dump.h>

// TODO: prune the following, since these were blindly copied from ta_ivdialog.cc and contain many unnecessary includes
#include <ta/taiv_data.h>
#include <ta/taiv_dialog.h>
#include <ta/taiv_type.h>
#include <css/css_iv.h>
#include <css/basic_types.h>
#include <css/ta_css.h>
#include <ta/wait_cursor.h>
#include <ta/wait_mask.h>
#include <ta/record_cursor.h>
#include <ta/record_mask.h>
#include <ta/enter_iv.h>
#include <InterViews/action.h>
#include <InterViews/event.h>
#include <InterViews/window.h>
#include <InterViews/font.h>
#include <InterViews/handler.h>
#include <InterViews/color.h>
#include <InterViews/background.h>
#include <InterViews/bitmap.h>
#include <InterViews/cursor.h>
#include <InterViews/deck.h>
#include <InterViews/patch.h>
#include <InterViews/label.h>
#include <InterViews/target.h>
#include <IV-look/kit.h>
#include <IV-look/dialogs.h>
#include <IV-look/choice.h>
#include <InterViews/layout.h>
#include <InterViews/style.h>
#include <InterViews/printer.h>	// for highlightbg
#include <IV-look/menu.h>
#include <IV-look/field.h> // for scroll field editor
#include <IV-look/fbrowser.h>
#include <OS/string.h> //  for scroll field editor
#ifndef CYGWIN
#include <IV-X11/xwindow.h>	// this is for window dumps
#include <IV-X11/xdisplay.h>	// this is for window dumps
#endif
#include <ta/leave_iv.h>

 
taUiFiler* taUiFiler_impl_CreateInstance() {
  return new taUiFiler_impl();
}

taUiFiler_impl::taUiFiler_impl() {
  win = NULL;
  dlg = NULL;
}

taUiFiler_impl::taUiFiler_impl(ivWindow* win_) {
  win = win_;
  dlg = NULL;
}

bool taUiFiler_impl::GetFileName(String& fname, taFiler* filer, FilerOperation filerOperation) {
 ivFileChooser* chooser = taivM->dkit->file_chooser(filer->dir, taivM->wkit->style());
  ivResource::ref(chooser);
  bool result = false;
  
  chooser->style()->attribute("filter", "on");
  chooser->style()->attribute("defaultSelection", fname);
  chooser->style()->attribute("filterPattern", filer->filter);
  ivCoord x=0; ivCoord y=0;
  String win_title;
  switch (filerOperation) {
  case foOpen:
    chooser->style()->attribute("open", "Open");
    chooser->style()->attribute("caption", "Select File to Open for Reading");
    win_title = String("Open: ") + filer->filter;
    break;
  case foSave:
    // TODO: will this ever be called???
    return false;
    break;
  case foSaveAs:
    chooser->style()->attribute("open", "Save");
    chooser->style()->attribute("caption", "Select File to Save for Writing");
    win_title = String("Save: ") + filer->filter;
    break;
  case foAppend:
    chooser->style()->attribute("open", "Append");
    chooser->style()->attribute("caption", "Select File to Append for Writing");
    win_title = String("Append: ") + filer->filter;
    break;
  }
  
  chooser->style()->attribute("name", win_title);
  chooser->style()->attribute("title", win_title);
  chooser->style()->attribute("iconName", win_title);
  taivDialog::GetPointerPos(x,y);
  result = (chooser->post_at_aligned(x, y, POINTER_ALIGN_X, POINTER_ALIGN_Y) != 0);
  
  if (result) {
    fname = chooser->selected()->string();
  }
  
  if(chooser != NULL) {
    ivResource::unref(chooser);
    ivResource::flush();
    chooser = NULL;
  }
  return result;
}
