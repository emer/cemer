/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
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


// Qt definitions, for inclusion in any header file

//NOTE: this file should *not* be included in build dependencies, if posssible
// (it is always fully backwards compatible, so shouldn't affect other files
//  that include it.)

#ifndef QTDEFS_H
#define QTDEFS_H

class	QAction;		// #IGNORE
class	QActionGroup;		// #IGNORE
class	QBitmap;		// #IGNORE
class	QBitArray;		// #IGNORE
class	QBoxLayout;		// #IGNORE
class	QButton;		// #IGNORE note: abstract parent class for buttons
class	QButtonGroup;		// #IGNORE
class	QCanvas;		// #IGNORE
class	QCheckBox;		// #IGNORE
class	QColor;			// #IGNORE
class	QColorGroup;		// #IGNORE
class	QComboBox;		// #IGNORE
class	QCursor;		// #IGNORE
class	QDialog;		// #IGNORE
class	QDockWindow;		// #IGNORE
class	QDragObject;		// #IGNORE
class	QEvent;			// #IGNORE
class	QFileDialog;		// #IGNORE
class	QFont;			// #IGNORE
class	QFrame;			// #IGNORE
class	QGrid;			// #IGNORE
class	QGridLayout;		// #IGNORE
class	QHBox;			// #IGNORE
class	QHBoxLayout;		// #IGNORE
class	QHButtonGroup;		// #IGNORE
class	QHeader;		// #IGNORE
class	QIconSet;		// #IGNORE
class	QLabel;			// #IGNORE
class	QLayout;		// #IGNORE
class	QLineEdit;		// #IGNORE
class	QListBox;		// #IGNORE
class	QListBoxItem;		// #IGNORE
class	QListView;		// #IGNORE
class	QListViewItem;		// #IGNORE
class	QMainWindow;		// #IGNORE
//no class	QMap;			// #IGNORE
class	QMenuBar;		// #IGNORE
class	QMenuData;		// #IGNORE
class	QMenuItem;		// #IGNORE
class	QMessageBox;		// #IGNORE
class	QMimeSource;		// #IGNORE
class	QObject;		// #IGNORE
class	QObjectList;		// #IGNORE
class	QPainter;		// #IGNORE
class	QPixmap;		// #IGNORE
class	QPoint;			// #IGNORE
class	QPopupMenu;		// #IGNORE
class	QProgressDialog;	// #IGNORE
class	QPushButton;		// #IGNORE
class	QRadioButton;		// #IGNORE
class	QRect;			// #IGNORE
class	QScrollBar;		// #IGNORE
class	QScrollView;		// #IGNORE
class	QSize;			// #IGNORE
class	QSizePolicy;		// #IGNORE
class	QSpacerItem;		// #IGNORE
class	QSpinBox;		// #IGNORE
class	QSplitter;		// #IGNORE
class	QString;		// #IGNORE
class	QStringList;		// #IGNORE
class	QTable;			// #IGNORE
class	QTableItem;			// #IGNORE
class	QTabBar;		// #IGNORE
class	QTabWidget;		// #IGNORE
class	QTextBrowser;		// #IGNORE
class	QTextEdit;		// #IGNORE
class	QTimer;			// #IGNORE
class	QToolBar;		// #IGNORE
class	QToolTip;		// #IGNORE
class	QValidator;		// #IGNORE
class	QVariant;		// #IGNORE
class	QVBox;			// #IGNORE
class	QVBoxLayout;		// #IGNORE
class	QVButtonGroup;		// #IGNORE
class	QWidget;		// #IGNORE
class	QWidgetStack;		// #IGNORE
class	QWMatrix;		// #IGNORE


// Events
class	QCloseEvent;		// #IGNORE
class	QChildEvent;		// #IGNORE
class	QContextMenuEvent;	// #IGNORE
class	QCustomEvent;		// #IGNORE
class	QDropEvent;		// #IGNORE
class	QFocusEvent;		// #IGNORE
class	QHideEvent;		// #IGNORE
class	QKeyEvent;		// #IGNORE
class	QMouseEvent;		// #IGNORE
class	QPaintEvent;		// #IGNORE
class	QResizeEvent;		// #IGNORE
class	QShowEvent;		// #IGNORE

// T3 Gadgets
class	T3ViewspaceWidget;	// #IGNORE


// SoQt Classes (Coin3D/Inventor)
class SoQtRenderArea; //#IGNORE
class SoQtViewer; //#IGNORE

// Common So Classes
class SoBaseColor;		// #IGNORE
class SoCone; 			// #IGNORE
class SoCube; 			// #IGNORE
class SoCylinder; 		// #IGNORE
class SoGroup; 			//
class SoNode; 			//
class SoPath;			// #IGNORE
class SoSeparator; 		//
class SoTransform; 		// #IGNORE

// Custom Controls

class	iAction;		// #IGNORE -- TODO, used cf pdplog.h
class	iBrush;			// #IGNORE -- TODO
//class	iButton;		// #IGNORE
class	iColor;			// #IGNORE
//class	iCoord;			// support Iv/float-Qt/int compatability
class	iCheckBox;		// #IGNORE -- support for Hilight and ReadOnly
class	iComboBox;		// #IGNORE -- support for Hilight
class	iEditGrid;		// #IGNORE
class	iFlowBox;		// #IGNORE
class	iFlowLayout;		// #IGNORE
class	iFont;			//
class	iLineEdit;		// #IGNORE
class	iDataViewer;		// #IGNORE -- in ta_qtviewer.h
#define iPixmap		QPixmap
class	iPoint;			//
class	iRect;			//
class	iRenderAreaWrapper;	// #IGNORE -- wrapper for SoQtRenderArea
class	iSize;			//
class	iSpinBox;		// #IGNORE
class	iStripeWidget;		// #IGNORE -- in ieditgrid.h/.cc
class	iTransformer;		// #IGNORE
class	iTreeView;		// #IGNORE
class	iTreeViewItem;		// #IGNORE

// All Event IDs in the PDP system should be allocated here:
/* if needed
enum CustomEventType {
  FirstEvent		= 1000 //note: QT's custom events start at 1000
}; */

#define COLOR_BLACK Qt::black
//#define COLOR_HILIGHT qtsoMisc::color_hilight
// NOTE: these are instantiated in tai_qtso_ti.cc (for want of a better place)
// COLOR_BRIGHT_HILIGHT is for checkboxes, for better contrast
//#define COLOR_BRIGHT_HILIGHT qtsoMisc::color_bright_hilight
extern QColor& COLOR_HILIGHT;
extern QColor& COLOR_BRIGHT_HILIGHT;

// aliases

// IV CLASS MAPPINGS:

//TEMP:
#define	iButton 	QButton

// 	IV Class	QT Class
//	ivDeck		QWidgetStack

// some external systems, such as Coin3d, require this:
#ifdef __MAKETA__
  #ifndef __cplusplus
    #define __cplusplus
  #endif
#endif


#endif // QTDEFS_H

