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

#include "taiEditorWidgetsMulti.h"
#include <iEditGrid>
#include <iSplitter>
#include <iScrollArea>

#include <QVBoxLayout>


taiEditorWidgetsMulti::taiEditorWidgetsMulti(void* base, TypeDef* typ_, bool read_only_,
      bool modal_, QObject* parent)
: inherited(base, typ_, read_only_, modal_, parent)
{
  multi = NULL;
  scrMulti = NULL;
  multi_body = NULL;
  lay_multi = NULL;
  multi_rows = 1;
  multi_cols = 1;
  header_row = true; // most compatible choice
}

void taiEditorWidgetsMulti::SetMultiSize(int rows, int cols) {
  if (rows < multi_rows) rows = multi_rows;
  if (cols < multi_cols) cols = multi_cols;
  if ((rows == multi_rows) && (cols == multi_cols)) return;
  multi_body->setDimensions(rows, cols);
  multi_rows = rows;
  multi_cols = cols;
}

void taiEditorWidgetsMulti::ClearBody_impl() {
  ClearMultiBody_impl();
  taiEditorOfClass::ClearBody_impl();
}

void taiEditorWidgetsMulti::ClearMultiBody_impl() {
  multi_body->clearLater(); // clears items in event loop
}

void taiEditorWidgetsMulti::Constr_Body() {
  inherited::Constr_Body(); // reuse entire implementation for list members
  Constr_MultiBody();
}

void taiEditorWidgetsMulti::Constr_Box() {
  // create the splitter before calling base, so scrbody gets put into the splitter
  splBody = new iSplitter(widget());
  splBody->setOrientation(Qt::Vertical);
  vblDialog->addWidget(splBody, 1); // gets all the space

  taiEditorOfClass::Constr_Box();

  scrMulti = new iScrollArea(splBody);
  QPalette pal = scrMulti->viewport()->palette();
  pal.setColor(QPalette::Background, bg_color);
  scrMulti->viewport()->setPalette(pal); 
  scrMulti->setWidgetResizable(true);
  multi = new QWidget();
  scrMulti->setWidget(multi);

  lay_multi = new QHBoxLayout(multi);
  lay_multi->setMargin(0);
  multi_body = new iEditGrid(header_row, 2, 1, 1, 1, multi);  // , hmargins, vmargins, rows, cols, par
  pal = multi_body->palette();
  pal.setColor(QPalette::Background, bg_color);
  multi_body->setPalette(pal); 
  multi_body->setHiLightColor(bg_color_dark);
  multi_body->setRowHeight(row_height);
  lay_multi->addWidget(multi_body);
}

void taiEditorWidgetsMulti::Constr_MultiBody() {
  // nothing
}

void taiEditorWidgetsMulti::RebuildMultiBody() {
//note: don't disable updates before clear, because it really doesn't help,
// and just requires recursive descent into everything to be nuked
  ClearMultiBody_impl();
  multi->setUpdatesEnabled(false);
    Constr_MultiBody();
  multi->setUpdatesEnabled(true);
}

