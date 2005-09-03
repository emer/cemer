/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/
// ieditgrid.h -- grid for EditHost class

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

