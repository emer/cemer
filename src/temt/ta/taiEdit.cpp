// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "taiEdit.h"
#include <taiEditorOfClass>

#include <taiMisc>
#include <taMisc>


void taiEdit::AddEdit(TypeDef* td) {
  InsertThisIntoBidList(td->ie);
}

taiEditorOfClass* taiEdit::CreateDataHost(void* base, bool read_only) {
  return new taiEditorOfClass(base, typ, read_only);
}

int taiEdit::Edit(void* base, bool readonly, const iColor& bgcol) {
  // get currently active win -- we will only look in any other window
  iMainWindowViewer* cur_win = taiMisc::active_wins.Peek_MainWindow();
  taiEditorOfClass* host = taiMisc::FindEdit(base, cur_win);
  if (!host) {
    host = CreateDataHost(base, readonly);

    if (typ->HasOption("NO_OK"))
      host->no_ok_but = true;
    if (typ->HasOption("NO_CANCEL"))
      host->read_only = true;
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      if (&bgcol == &def_color) {
        bool ok = false;
        iColor bg = GetBackgroundColor(base, ok);
        if (ok) host->setBgColor(bg);
      }
      else
        host->setBgColor(bgcol);
    }
    host->Constr("", "");
//TODO: no longer supported:    host->cancel_only = readonly;
    return host->Edit(false);
  }
  else if (!host->modal) {
    host->Raise();
  }
  return 2;
}

int taiEdit::EditDialog(void* base, bool read_only, bool modal,
                        const iColor& bgcol, int min_width, int min_height)
{
  taiEditorOfClass* host = NULL;
  if (!modal) {
    host = taiMisc::FindEditDialog(base, read_only);
    if (host) {
      host->Raise();
      return 2;
    }
  }
  host = CreateDataHost(base, read_only);
  if (taMisc::color_hints & taMisc::CH_EDITS) {
    if (&bgcol == &def_color) {
      bool ok = false;
      iColor bg = GetBackgroundColor(base, ok);
      if (ok) host->setBgColor(bg);
    }
    else
        host->setBgColor(bgcol);
  }
  else {
    //TODO: maybe we always null out, or should we allow caller to specify?
    //bgcol = NULL;
  }
  host->Constr("", "", taiEditorWidgetsMain::HT_DIALOG);
  return host->Edit(modal, min_width, min_height);
}

iPanelOfEditor* taiEdit::EditNewPanel(taiSigLink* link, void* base,
   bool read_only, const iColor& bgcol)
{
  taiEditorOfClass* host = CreateDataHost(base, read_only);
  if (taMisc::color_hints & taMisc::CH_EDITS) {
    if (&bgcol == &def_color) {
      bool ok = false;
      iColor bg = GetBackgroundColor(base, ok);
      if (ok) host->setBgColor(bg);
    }
    else
      host->setBgColor(bgcol);
  }
  else {
    //TODO: maybe we always null out, or should we allow caller to specify?
    //bgcol = NULL;
  }

  host->Constr("", "", taiEditorWidgetsMain::HT_PANEL, true);
  iPanelOfEditor* rval = host->EditPanelDeferred(link);
  return rval;
}

iPanelOfEditor* taiEdit::EditPanel(taiSigLink* link, void* base,
   bool read_only, iMainWindowViewer* not_in_win, const iColor& bgcol)
{
  taiEditorOfClass* host = NULL;
  host = taiMisc::FindEditPanel(base, read_only, not_in_win);
  if (host) {
    host->Raise();
    return host->dataPanel();
  }
  else {
    return EditNewPanel(link, base, read_only, bgcol);
  }
}

const iColor taiEdit::GetBackgroundColor(void* base, bool& ok) {
  if (typ->IsActualTaBase() && base) {
    return ((taBase*)base)->GetEditColorInherit(ok);
  }
  ok = false;
  return def_color;
}
