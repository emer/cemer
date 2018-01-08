// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "iTextBrowser.h"

iTextBrowser::iTextBrowser(QWidget* parent)
:inherited(parent)
{
  init();
}

iTextBrowser::~iTextBrowser()
{
}

void iTextBrowser::init() {
}

void iTextBrowser::setSource(const QUrl& name) {
  bool cancel = false;
  emit setSourceRequest(this, name, cancel);
  if (cancel) return;
  inherited::setSource(name);
}
