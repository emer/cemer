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

#include "taiqtso_def.h"

#include "ieditgrid.h"

#include <qlayout.h>
#include <qobject.h>
//obs #include <qobjectlist.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
#include <Q3ScrollView>
#include <qwidget.h>

//////////////////////////////////
// 	iStripeWidget		//
//////////////////////////////////

iStripeWidget::iStripeWidget(QWidget* parent)
:QWidget(parent), mhiLightColor(0x80, 0x80, 0x80)
{
  mstripeHeight = 25;
  mtopMargin = 0;
}

iStripeWidget::~iStripeWidget()
{
}

QSize iStripeWidget::minimumSizeHint() const {
  // get superclass's hint, and override the height
  QSize rval = QWidget::minimumSizeHint();
  return rval.expandedTo(QSize(rval.width(), mtopMargin + 2 * mstripeHeight));
}

void iStripeWidget::setHiLightColor(const QColor& val) {
  mhiLightColor = val;
  update();

}

void iStripeWidget::setStripeHeight(int val) {
  if (val == mstripeHeight) return;
  if (val < 1) return; // must be +ve
  mstripeHeight = val;
  update();
}

void iStripeWidget::setTopMargin(int val) {
  if (val == mtopMargin) return;
  if (val < 0) return; // must be +ve
  mtopMargin = val;
  update();
}

void iStripeWidget::paintEvent(QPaintEvent* pev)
{
  QWidget::paintEvent(pev);
  if (height() <= mtopMargin) return;
  QPainter p(this);
  int num_stripes = (height() - mtopMargin) / mstripeHeight;
  if ((mstripeHeight * num_stripes) > (height() - mtopMargin)) num_stripes++; // one fraction of a stripe -- should be clipped by painting

  p.setPen(mhiLightColor);
  p.setBrush(mhiLightColor);

  // hilight every second stripe
  for (int i = 1; i < num_stripes; i+= 2 ) {
    p.drawRect(0, (i * mstripeHeight) + mtopMargin, width(), mstripeHeight); // draw hilighted rect
  }
}


//////////////////////////////////
// 	iEditGrid		//
//////////////////////////////////


/*NOTE: to insure proper visual appearance when an hscrollbar appears in the data area, we always
  have one extra empty dummy name slot at the end -- this pushes the entire structure down, and
  therefore the last actual data line doesn't get clipped */

iEditGrid::iEditGrid (QWidget* parent)
: QWidget(parent)
{
  init(3, 2, 2, 1, 1);
}

iEditGrid::iEditGrid (int margin_, int hspace_, int vspace_, QWidget* parent)
: QWidget(parent)
{
  init(margin_, hspace_, vspace_, 1, 1);
}

iEditGrid::iEditGrid (int margin_, int hspace_, int vspace_, int rows_, int cols_, QWidget* parent)
: QWidget(parent)
{
  init(margin_, hspace_, vspace_, rows_, cols_);
}

void iEditGrid::init(int margin_, int hspace_, int vspace_, int rows_, int cols_) {
  mmargin = margin_;
  mhspace = hspace_;
  mvspace = vspace_;
  if (rows_ < 1) rows_ = 1;
  if (cols_ < 1) cols_ = 1;
  mrows = rows_;
  mcols = cols_;
  mrow_height = 1; //must set later
  layOuter = new QHBoxLayout(this);
  layNamesOuter = new QVBoxLayout();
  layOuter->addLayout(layNamesOuter);
  bodyNames = new iStripeWidget(this);
  QVBoxLayout* vbl = new QVBoxLayout(bodyNames);
  layNames = new QGridLayout(); //extra dummy row
//TODO: Qt4 -- maybe no way to set rows and cols???  
  layNames->setSpacing(mvspace);
  layNames->setMargin(mmargin);
  vbl->addLayout(layNames);
  vbl->addStretch();
  layNamesOuter->addWidget(bodyNames);
  layNamesOuter->addStretch();
//nn  layNames->setRowMinimumHeight(0, mrow_height); //TEMP
  layNamesOuter->addStretch();
  layOuter->addSpacing(mhspace);
  scrBody = new Q3ScrollView(this);
  scrBody->setVScrollBarMode(Q3ScrollView::AlwaysOff); // use outer container for scrolling, to keep names in sync
  scrBody->setResizePolicy(Q3ScrollView::AutoOneFit);
  body = new iStripeWidget();		// parent for the data items
  body->resize(1, 1); // let it expand
  scrBody->addChild(body);

  vbl = new QVBoxLayout(body);
//  layBody = new QGridLayout(mrows + 1, mcols, mvspace); //note: vspace passed for "spacing", applies to both dims
  layBody = new QGridLayout(); //note: vspace passed for "spacing", applies to both dims
//TODO: Qt4 -- maybe no way to set rows and cols???  
  layBody->setSpacing(mvspace);
  layBody->setMargin(mmargin);
  vbl->addLayout(layBody);
  vbl->addStretch();
  layOuter->addWidget(scrBody, 2); // anonymous outer layout in scroll view, so we can add stretch at bottom
  resizeRows_impl();
}

void iEditGrid::checkSetParent(QWidget* widget, QWidget* parent) {
  if (widget->parent() != parent) {
//Qt3    widget->reparent(parent, QPoint(0,0)); //get's moved anyway, based on layout
    widget->setParent(parent); //get's moved anyway, based on layout
  }
}

void iEditGrid::clearLater() { // clears all contained items, but does it via deleteLater, not delete
  {const QObjectList& ol = bodyNames->children(); //unconstify it
  for (int i = ol.count() - 1; i > 0; --i) { //Note: we presume item 0 is layNames
    QObject* chobj = ol.at(i);
    chobj->deleteLater(); // deleted in event loop
  }}
  {const QObjectList& ol = body->children(); //unconstify it
  for (int i = ol.count() - 1; i > 0; --i) { //Note: we presume item 0 is layN
    QObject* chobj = ol.at(i);
    chobj->deleteLater(); // deleted in event loop
  }}
}

void iEditGrid::resizeRows_impl() {
  for (int i = 0; i < (mrows + 1); ++i) {
    layNames->setRowMinimumHeight(i, mrow_height + (2 * mmargin));
    layBody->setRowMinimumHeight(i,  mrow_height + (2 * mmargin));
  }
  layNames->setRowMinimumHeight(mrows + 1, mrow_height + (2 * mmargin)); //dummy row
}

void iEditGrid::setDimensions(int rows_, int cols_) {
  if ((rows_ == mrows) && (cols_ == mcols)) return;
  //note: layouts won't shrink, only expand
//nn4  layNames->expand(rows_ + 2, 1);
//nn4  layBody->expand(rows_ + 1, cols_);
  mrows = MAX(mrows, rows_);
  mcols = MAX(mcols, cols_);
  resizeRows_impl();
}

void iEditGrid::setColNameWidget(int col, QWidget* name) {
  checkSetParent(name, (QWidget*)body);
  layBody->addWidget(name, 0, col, (Qt::AlignCenter | Qt::AlignVCenter));
}

void iEditGrid::setDataWidget(int row, int col, QWidget* data) {
  checkSetParent(data, (QWidget*)body);
  layBody->setRowMinimumHeight(row + 1,  mrow_height + (2 * mmargin));
  layBody->addWidget(data, row + 1, col, (Qt::AlignLeft | Qt::AlignVCenter));
//  resizeRows_impl();
}

void iEditGrid::setDataLayout(int row, int col, QLayout* data) {
  layBody->setRowMinimumHeight(row + 1,  mrow_height + (2 * mmargin));
  layBody->addLayout(data, row + 1, col);
//  resizeRows_impl();
}

void iEditGrid::setHiLightColor(const QColor& val) {
  bodyNames->setHiLightColor(val);
  body->setHiLightColor(val);
}

void iEditGrid::setRowHeight(int value, bool force) {
  if ((mrow_height == value) && (!force)) return;
  mrow_height = value;
  bodyNames->setStripeHeight(value + (2 * mmargin) + mvspace);
  bodyNames->setTopMargin(value + (2 * mmargin) + mvspace + 1); //+1 for frame border of group items
  body->setStripeHeight(value + (2 * mmargin) + mvspace);
  body->setTopMargin(value + (2 * mmargin) + mvspace);
  resizeRows_impl();
}

void iEditGrid::setPaletteBackgroundColor3 (const QColor& c) {
  QPalette pal(palette());
  pal.setColor(QPalette::Background,c); 
  setPalette(pal);
  pal = body->palette();
  pal.setColor(QPalette::Background,c); 
  body->setPalette(pal); //note: may not be necessary
  pal = scrBody->viewport()->palette();
  pal.setColor(QPalette::Background,c); 
  scrBody->viewport()->setPalette(pal);
}

void iEditGrid::setRowNameWidget(int row, QWidget* name) {
  checkSetParent(name, (QWidget*)bodyNames);
  layNames->setRowMinimumHeight(row + 1,  mrow_height + (2 * mmargin));
  layNames->addWidget(name, row + 1, 0, (Qt::AlignLeft | Qt::AlignVCenter));
//  resizeRows_impl();
}

void iEditGrid::setSpacing(int hor, int ver) {
  if ((mhspace == hor) && (mvspace == ver)) return;
  mhspace = hor;
  mvspace = ver;
  setRowHeight(mrow_height, true);
}

void iEditGrid::setVisibleCols(int num) {
  if (mvisibleCols == num) return;
  setVisibleCols_impl(num);
}

void iEditGrid::setVisibleCols_impl(int num) {
  // TODO: sets widths of columns
  mvisibleCols = num;
}

