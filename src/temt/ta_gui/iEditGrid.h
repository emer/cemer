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

#ifndef iEditGrid_h
#define iEditGrid_h 1

#include "ta_def.h"

#include <qsize.h>
#include <QWidget>

class QHBoxLayout; // 
class iStripeWidget; //
class QGridLayout; //
class QScrollArea; //


class TA_API iEditGrid: public QWidget {
  Q_OBJECT
public:
  QHBoxLayout*		layOuter;
  iStripeWidget*	  bodyNames;	// parent for the name items
  QGridLayout*		  layNames;
  QScrollArea*		  scrBody;	// scrollbars for the body items
  iStripeWidget*	    body;	// parent for the data items
  QGridLayout*		    layBody;	// layout for the body

  void		setDimensions(int rows_, int cols_);
  void		setColNameWidget(int col, QWidget* name);
  void		setDataLayout(int row, int col, QLayout* data);
  void		setDataWidget(int row, int col, QWidget* data);
  int		cols() {return mcols;}
  QWidget*	dataGridWidget() {return (QWidget*)body;} //returns data grid widget, for parentage of its children
  inline bool	hasHeader() {return (mhead > 0);} 
  inline bool	hasNames() {return mnames;} 
  void 		setPaletteBackgroundColor3 (const QColor& color); //override
  void 		setColors(const QColor& hilight, const QColor& bg); // convenient way to set both
  void 		setHiLightColor (const QColor& color);
  int		minVisibleRows() const {return mmin_vis_rows;}
  void		setMinVisibleRows(int value);
  int		rows() {return mrows;}
  void		setRowNameWidget(int row, QWidget* name);
 // void		setSpacing(int hor, int ver); // set spacing between items
  void		setRowHeight(int value, bool force = false); // set height of items (default is 25)
//  int 		visibleCols() {return mvisibleCols;}
//  void		setVisibleCols(int num); // min 1, max??

  void		clearLater(); // clears all contained items, but does it via deleteLater, not delete
  void		resizeNames() {/*resizeNames_impl();*/} // TODO: this should get called automatically inside this widget


  iEditGrid (bool header_, QWidget* parent = 0);
  iEditGrid (bool header_, int hmargin_, int vmargin_, QWidget* parent = 0);
  iEditGrid (bool header_, int hmargin_, int vmargin_, 
    int rows_, int cols_, QWidget* parent = 0);
  iEditGrid (bool names_, bool header_, int hmargin_, int vmargin_, 
    int rows_, int cols_, QWidget* parent = 0); // only constructor that lets you not use names
  ~iEditGrid() {}

protected:
  QColor  mhilightColor;
  void		init(bool header_, int hmargin_, int vmargin_, int rows_, 
    int cols_, bool names_ = true);
  void		createContent(); // we call in init, and after clearLater
  int mhmargin; // h margin inside cells
  int mvmargin; // v margin inside cells
  int mcols;
  int mrows;
  int mhead; // 1 if using header row, 0 if not
//  int mvisibleCols;  // num of cols to make visible in the data area
  int mrow_height; // row heights
  bool mnames; // if uses names on left
  int mmin_vis_rows;
  void		adjustMinHeight(); // rejigs min size for hor scroll bar
  void		resizeRows_impl(); // resize the name heights, after the data heights have been established
//  virtual void		setVisibleCols_impl(int num);
  void			checkSetParent(QWidget* widget, QWidget* parent); // sets parent, if necessary
  void		updateDimensions(int row, int col); // if row or col is > current, update us, alo
};

#endif // iEditGrid_h
