// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "iBrainViewEditDialog.h"
#include <BrainAtlasRegexpPopulator>
#include <taiObjChooser>
#include <ColorScaleSpec>
#include <ColorScaleColor_List>
#include <int_Array>

#include <QTableView>
#include <QStandardItem>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QPushButton>

iBrainViewEditDialog::iBrainViewEditDialog(taiRegexpField* regexp_field, const String& field_name, iRegexpDialogPopulator *re_populator, const void *fieldOwner, bool read_only, bool editor_mode)
  : inherited(regexp_field, field_name, re_populator, fieldOwner, read_only, editor_mode)
{
  AddButtons();
  SetColors();

  connect(m_table_view, SIGNAL(doubleClicked(const QModelIndex&)), this,
          SLOT(itemClicked(const QModelIndex&)));
}

void iBrainViewEditDialog::SetColors()
{
  BrainAtlasRegexpPopulator* bepop = (BrainAtlasRegexpPopulator*)m_populator;

  QList<QColor> clrs = bepop->getColors();
  int rows = clrs.size();

  int col = m_num_parts + NUM_EXTRA_COLS;
  for (int row = 0; row < rows; ++row) {
    QColor clr = clrs[row];
    QStandardItem* item = new QStandardItem(clr.name());
    item->setBackground(QBrush(clr));
    item->setEditable(false);
    m_table_model->setItem(row, col, item);
  }

  m_table_view->horizontalHeader()->setToolTip("Double-click on Color to edit with a color editor dialog");
}

void iBrainViewEditDialog::itemClicked(const QModelIndex & index)
{
  QModelIndex src_idx = m_proxy_model->mapToSource(index);
  int row = src_idx.row();
  int col = src_idx.column();
  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  if(col != clr_col) return;
  QStandardItem* item = m_table_model->item(row, clr_col);
  QColor clr(item->text());
  QColor nwclr = QColorDialog::getColor(clr, this);
  item->setText(nwclr.name());
  item->setBackground(QBrush(nwclr));
}

void iBrainViewEditDialog::btnApply_clicked()
{
  inherited::btnApply_clicked();
  if(!m_editor_mode) return;

  BrainAtlasRegexpPopulator* bepop = (BrainAtlasRegexpPopulator*)m_populator;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;

  QList<QColor> colors;
  int rows = m_table_model->rowCount();
  for(int row = 0; row < rows; ++row) {
    QStandardItem* item = m_table_model->item(row, clr_col);
    QColor clr(item->text());
    colors.append(clr);
  }
  bepop->setColors(colors);
}

void iBrainViewEditDialog::AddButtons()
{
  if(!m_editor_mode) return;

  QPushButton* btnRandomColors = m_button_box->addButton("RandomColors",
                                                         QDialogButtonBox::ActionRole);
  btnRandomColors->setToolTip("generates random colors for the currently-selected items (according to the current filter) from a selected color scale");

  QPushButton* btnColorsFromScale = m_button_box->addButton("ColorsFromScale",
                                                            QDialogButtonBox::ActionRole);
  btnColorsFromScale->setToolTip("generates colors for the currently-selected items (according to the current filter) from a selected color scale");

  // Connect the button-box buttons to our SLOTs.
  connect(btnRandomColors, SIGNAL(clicked()), this, SLOT(btnRandomColors_clicked()));
  connect(btnColorsFromScale, SIGNAL(clicked()), this, SLOT(btnColorsFromScale_clicked()));
}

void iBrainViewEditDialog::btnColorsFromScale_clicked()
{
  taiObjChooser* chs = taiObjChooser::createInstance(&TA_ColorScaleSpec,
                                                     "select a colorscale to apply to the currently-selected labels");
  bool rval = chs->Choose();
  if(!rval) return;
  ColorScaleSpec* cspec = (ColorScaleSpec*)chs->sel_obj();
  delete chs;
  chs = 0;
  if(!cspec) return;

  int rows = m_proxy_model->rowCount();
  if(rows == 0) return;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  ColorScaleColor_List cls;
  int extra = 0;
  do {                          // it doesn't always generate enough..
    cspec->GenRanges(&cls, rows+extra);
    extra += 2;
  } while (cls.size < rows);

  m_table_model->blockSignals(true);
  for(int row = 0; row < rows; row++) {
    QModelIndex pidx = m_proxy_model->index(row, clr_col);
    QModelIndex sidx = m_proxy_model->mapToSource(pidx);
    QStandardItem* itm = m_table_model->item(sidx.row(), clr_col);
    String rgb = String("#") + cls[row]->color().toString();
    itm->setText(rgb);
    itm->setBackground(QBrush(QColor(QString(rgb.chars()))));
  }
  m_table_model->blockSignals(false);
  m_table_view->viewport()->update();
}

void iBrainViewEditDialog::btnRandomColors_clicked()
{
  taiObjChooser* chs = taiObjChooser::createInstance(&TA_ColorScaleSpec,
                                                     "select a colorscale to select random colors from -- Rainbow is generally a good choice");
  bool rval = chs->Choose();
  if(!rval) return;
  ColorScaleSpec* cspec = (ColorScaleSpec*)chs->sel_obj();
  delete chs;
  chs = 0;
  if(!cspec) return;

  int rows = m_proxy_model->rowCount();
  if(rows == 0) return;

  int clr_col = m_num_parts + NUM_EXTRA_COLS;
  ColorScaleColor_List cls;
  int extra = 0;
  do {                          // it doesn't always generate enough..
    cspec->GenRanges(&cls, rows+extra);
    extra += 2;
  } while (cls.size < rows);
  int_Array prmt;
  prmt.SetSize(rows);
  prmt.FillSeq();
  prmt.Permute();

  m_table_model->blockSignals(true);
  for(int row = 0; row < rows; row++) {
    QModelIndex pidx = m_proxy_model->index(row, clr_col);
    QModelIndex sidx = m_proxy_model->mapToSource(pidx);
    QStandardItem* itm = m_table_model->item(sidx.row(), clr_col);
    ColorScaleColor* clr = cls[prmt[row]];
    String rgb = String("#") + clr->color().toString();
    itm->setText(rgb);
    itm->setBackground(QBrush(QColor(QString(rgb.chars()))));
  }
  m_table_model->blockSignals(false);
  m_table_view->viewport()->update();
}
