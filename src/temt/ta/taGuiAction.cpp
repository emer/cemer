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

#include "taGuiAction.h"

void taGuiAction::Initialize() {
  m_helper = new taGuiActionHelper(this);
}

void taGuiAction::Destroy() {
  delete m_helper;
}

void taGuiAction::Connect_UrlAction(QObject* src_obj, const char* src_signal) {
  static const char* slot_nm = SLOT(UrlAction());
  QObject::connect(src_obj, src_signal, m_helper, slot_nm);
}

void taGuiAction::UrlAction() {
  if(action_url.empty()) return;
  if(action_url.startsWith("ta:")) {
    if(taiMisc::main_window)
      taiMisc::main_window->taUrlHandler(QUrl(action_url));
    // skip over middleman -- was not triggering in C++ dialogs for some reason..
  }
  else {
    QDesktopServices::openUrl(QUrl(action_url));
  }
}

void taGuiAction::FixUrl(const String& url_tag, const String& path) {
  if(action_url.startsWith(url_tag)) {
    action_url = "ta:" + path + "." + action_url.after(url_tag);
  }
}

