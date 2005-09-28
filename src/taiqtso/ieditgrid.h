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


#ifndef IEDITGRID_H
#define IEDITGRID_H

#include "taiqtso_def.h"

#include <qsize.h>
#include <qwidget.h>
//#include <qframe.h>

//////////////////////////////////
// 	iStripeWidget		//
//////////////////////////////////

class TAIQTSO_API iStripeWidget: public QWidget { // #IGNORE provides a horizontal striped background, to highlight items
public:
  QSize 		minimumSizeHint () const; // override
  void			setHiLightColor(const QColor& val);
  void			setStripeHeight(int val);
  void			setTopMargin(int val);

  iStripeWidget(QWidget* parent = NULL, const char* name = 0);
  ~iStripeWidget();
protected:
  QColor		mhiLightColor;
  int			mstripeHeight; // default is 25;
  int			mtopMargin; // default is 0;

  void		paintEvent(QPaintEvent* pev); // override
};


//////////////////////////////////
// 	iEditGrid		//
//////////////////////////////////

class TAIQTSO_API iEditGrid: public QWidget {
  Q_OBJECT
public:
  QHBoxLayout*		layOuter;
    QVBoxLayout*	layNamesOuter;
      iStripeWidget*		bodyNames;	// parent for the name items
        QGridLayout*	layNames;
    QScrollView*	scrBody;	// scrollbars for the body items
      iStripeWidget*		body;		// parent for the data items
      QGridLayout* 	layBody;	// layout for the body

  void		setDimensions(int rows_, int cols_);
  void		setColNameWidget(int col, QWidget* name);
  void		setDataLayout(int row, int col, QLayout* data);
  void		setDataWidget(int row, int col, QWidget* data);
  int		cols() {return mcols;}
  QWidget*	dataGridWidget() {return (QWidget*)body;} //returns data grid widget, for parentage of its children
  void 		setPaletteBackgroundColor (const QColor& color); //override
  void 		setHiLightColor (const QColor& color);
  int		rows() {return mrows;}
  void		setRowNameWidget(int row, QWidget* name);
  void		setSpacing(int hor, int ver); // set spacing between items
  void		setRowHeight(int value, bool force = false); // set height of items (default is 25)
  int 		visibleCols() {return mvisibleCols;}
  void		setVisibleCols(int num); // min 1, max??

  void		clearLater(); // clears all contained items, but does it via deleteLater, not delete
  void		resizeNames() {/*resizeNames_impl();*/} // TODO: this should get called automatically inside this widget


  iEditGrid (QWidget* parent = 0, const char* name = 0);
  iEditGrid (int margin_, int hspace_, int vspace_, QWidget* parent = 0, const char* name = 0);
  iEditGrid (int margin_, int hspace_, int vspace_, int rows_, int cols_, QWidget* parent = 0, const char* name = 0);
  ~iEditGrid() {}

protected:
  void		init(int margin_, int hspace_, int vspace_, int rows_, int cols_);
  int mmargin; // inner margin
  int mhspace; // h spacing between items/cols
  int mvspace; // h spacing between items/cols
  int mcols;
  int mrows;
  int mvisibleCols;  // num of cols to make visible in the data area
  int mrow_height; // row heights
  void		resizeRows_impl(); // resize the name heights, after the data heights have been established
  virtual void		setVisibleCols_impl(int num);
  void			checkSetParent(QWidget* widget, QWidget* parent); // sets parent, if necessary
};


#endif

