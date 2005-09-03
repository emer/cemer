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
