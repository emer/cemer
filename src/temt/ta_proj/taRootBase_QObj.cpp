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

#include "taRootBase_QObj.h"
#include <taMisc>
#include <QApplication>
#include <QWidget>
#include <taRootBase>
#include <iMainWindowViewer>

#include <taMisc>
#include <taiMisc>

void taRootBase_QObj::Startup_ProcessArgs() {
  taRootBase::Startup_ProcessArgs();
}

void taRootBase_QObj::Startup_RunStartupScript() {
  taRootBase::Startup_RunStartupScript();
}

void taRootBase_QObj::ConsoleNewStdin(int n_lines) {
  taRootBase::instance()->ConsoleNewStdin(n_lines);
}

void taRootBase_QObj::FocusRootWinAtStartup() {
  if (taiMisc::main_window) {
    taiMisc::main_window->activateWindow();
    taiMisc::main_window->raise();

    // If there are any modal dialogs (e.g., recompile plugins), the main
    // window may have just been raised above them.  Find and raise them.
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
      if (widget->isModal()) {
        widget->activateWindow();
        widget->raise();
      }
    }
  }
}


#ifdef DMEM_COMPILE
void taRootBase_QObj::DMem_SubEventLoop() {
  taRootBase::DMem_SubEventLoop();
}
#endif // DMEM_COMPILE
