// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#include "iDialogList.h"

#include <taMisc>
#include <taiMisc>
#include <iMainWindowViewer>
#include <iTextBrowser>
#include <MemberDef>
#include <String_Array>

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QDesktopServices>

iDialogList* iDialogList::New(int ft, iMainWindowViewer* par_window_)
{
  iDialogList* rval = new iDialogList(par_window_);
  Qt::WindowFlags wflg = rval->windowFlags();
  wflg &= ~Qt::WindowStaysOnTopHint;
  rval->setWindowFlags(wflg);
  rval->setFont(taiM->dialogFont(ft));
  rval->Constr();
  return rval;
}

iDialogList::iDialogList(iMainWindowViewer* par_window_)
:inherited(par_window_)
{
  init();
}

void iDialogList::init() {
  setSizeGripEnabled(true);
  resize(taiM->dialogSize(taiMisc::vdlg_m)); // don't hog too much screen size
}

void iDialogList::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
   
  results = new iTextBrowser(this);
  layOuter->addWidget(results, 1); // results is item to expand in host
  
  connect(results, SIGNAL(setSourceRequest(iTextBrowser*, const QUrl&, bool&)),
          this, SLOT(results_setSourceRequest(iTextBrowser*, const QUrl&, bool&)) );
}

void iDialogList::SetList(taBase_PtrList& base_list, String title,
                          const String_Array* info_strings, const String& info_title) {
  item_list.Duplicate(base_list);
  
  setWindowTitle(title);
  
  // display "item" and path - plus any additional members requested
  src = "<table border=1 cellpadding=10>";
  src += "<tr><th>";
  src += "Item";
  src += "</th><th>";
  src += "Path";
  src += "</th>";
  if (info_strings && info_strings->size > 0) {  // optional info column
    src += "</th><th>";
    src += info_title;
    src += "</th>";
  }
  for (int i=0; i<item_list.size; i++) {
    taBase* base = item_list.SafeEl(i);
    if (!base) continue;
    src += "<tr";
    if(base->GetEnabled() == 0)
      src += " bgcolor=\"#D0D0D0\"";
    src += "><td>";
    // item
    String item = base->GetColText(taBase::key_disp_name);
    String href = "ta:" + base->GetPath();
    src += "<a href=\"" + href + "\">" + item + "</a>";
    src += "</td><td>";
    // path
    src += base->GetPathNames();
    src += "</td>";
    if (info_strings && info_strings->size > 0) {
      src += "</td><td>";
      src += info_strings->SafeEl(i);
      src += "</td>";
    }
    src += "</tr>";
  }
  src += "</table>";
  results->setHtml(src);
}

void iDialogList::results_setSourceRequest(iTextBrowser* itb_src, const QUrl& url, bool& cancel)
{
  QDesktopServices::openUrl(url);
  cancel = true;
  //NOTE: we never let results call its own setSource because we don't want
  // link clicking to cause us to change our source page
}
