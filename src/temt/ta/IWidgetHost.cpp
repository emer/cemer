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

#include "IWidgetHost.h"
#include <iMainWindowViewer>
#include <taiData>


void IWidgetHost::SetItemAsHandler(taiData* item, bool set_it) {
  iMainWindowViewer* dv = viewerWindow();
  if (!dv) return;
  //TODO: we really should check to make sure our class expresses these,
  // and not include the last two if not expressed
  if (set_it) {
    dv->SetClipboardHandler(item,
      SLOT(this_GetEditActionsEnabled(int&)),
      SLOT(this_EditAction(int)),
      SLOT(this_SetActionsEnabled()),
      SIGNAL(UpdateUi()) );
  } else {
    dv->SetClipboardHandler(NULL);
  }
}
