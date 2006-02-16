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



// Qt definitions, for inclusion in any header file

//NOTE: this file should *not* be included in build dependencies, if posssible
// (it is always fully backwards compatible, so shouldn't affect other files
//  that include it.)

#ifndef QTDEFS_H
#define QTDEFS_H

// Qt Core defines -- always included

class	QBitArray;		// #IGNORE
class	QEvent;			// #IGNORE
template<class T> class	QList;		// #IGNORE
class	QObject;		// #IGNORE
class	QString;		// #IGNORE
class	QStringList;		// #IGNORE
class	QVariant;		// #IGNORE

class	QCustomEvent;		// #IGNORE Qt3: is deprecated in Qt4, 

// Qt Gui defines -- only when TA_GUI

#ifdef TA_GUI
class	QAction;		// #IGNORE
class	QActionGroup;		// #IGNORE
class	QBitmap;		// #IGNORE
class	QBoxLayout;		// #IGNORE
class	QButton;		// #IGNORE note: abstract parent class for buttons
class	Q3Button;		// #IGNORE note: abstract parent class for buttons
//class	QButtonGroup;		// #IGNORE
class	Q3ButtonGroup;		// #IGNORE
class	QCanvas;		// #IGNORE
class	QCheckBox;		// #IGNORE
class	QColor;			// #IGNORE
class	QColorGroup;		// #IGNORE
class	QComboBox;		// #IGNORE
class	QCursor;		// #IGNORE
class	QDialog;		// #IGNORE
class	QDockWindow;		// #IGNORE
class	QDragObject;		// #IGNORE
class	QFileDialog;		// #IGNORE
class	QFont;			// #IGNORE
class	QFrame;			// #IGNORE
class	QGrid;			// #IGNORE
class	QGridLayout;		// #IGNORE
class	QHBox;			// #IGNORE
class	QHBoxLayout;		// #IGNORE
//class	QHButtonGroup;		// #IGNORE
class	Q3HButtonGroup;		// #IGNORE
class	Q3Header;		// #IGNORE
//class	QIconSet;		// #IGNORE
class	QLabel;			// #IGNORE
class	QLayout;		// #IGNORE
class	QLineEdit;		// #IGNORE
class	Q3ListBox;		// #IGNORE
class	Q3ListBoxItem;		// #IGNORE
class	Q3ListView;		// #IGNORE
class	Q3ListViewItem;		// #IGNORE
class	QMainWindow;		// #IGNORE
//no class	QMap;			// #IGNORE
class	QMenu;			// #IGNORE Qt4
class	QMenuBar;		// #IGNORE
//obs class	QMenuData;		// #IGNORE
class	QMenuItem;		// #IGNORE
class	QMessageBox;		// #IGNORE
class	QMimeSource;		// #IGNORE
//class	QObjectList;		// #IGNORE
class	QPainter;		// #IGNORE
class	QPixmap;		// #IGNORE
class	QPoint;			// #IGNORE
//class	Q3PopupMenu;		// #IGNORE
class	QProgressDialog;	// #IGNORE
class	QPushButton;		// #IGNORE
class	QRadioButton;		// #IGNORE
class	QRect;			// #IGNORE
class	QScrollArea;		// #IGNORE Qt4
class	QScrollBar;		// #IGNORE
//class	Q3ScrollView;		// #IGNORE
class	QSize;			// #IGNORE
class	QSizePolicy;		// #IGNORE
class	QSpacerItem;		// #IGNORE
class	QSpinBox;		// #IGNORE
class	QSplitter;		// #IGNORE
class	Q3Table;			// #IGNORE
class	Q3TableItem;			// #IGNORE
class	QTabBar;		// #IGNORE
class	QTabWidget;		// #IGNORE
class	QTextBrowser;		// #IGNORE
class	QTextEdit;		// #IGNORE
class	QTimer;			// #IGNORE
class	QToolBar;		// #IGNORE
class	QToolTip;		// #IGNORE
class	QValidator;		// #IGNORE
class	Q3VBox;			// #IGNORE
class	QVBoxLayout;		// #IGNORE
//class	QVButtonGroup;		// #IGNORE
class	Q3VButtonGroup;		// #IGNORE
class	QWidget;		// #IGNORE
class	Q3WidgetStack;		// #IGNORE
//class	QWMatrix;		// #IGNORE


// Events
class	QCloseEvent;		// #IGNORE
class	QChildEvent;		// #IGNORE
class	QContextMenuEvent;	// #IGNORE
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

#ifdef TA_USE_INVENTOR
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
#endif

// Custom Controls

//class	iAction;		// #IGNORE -- TODO, used cf pdplog.h
class	iBrush;			// #IGNORE -- TODO
//class	iButton;		// #IGNORE
class	iColor;			// #IGNORE
//class	iCoord;			// support Iv/float-Qt/int compatability
class	iCheckBox;		// #IGNORE -- support for Hilight and ReadOnly
class	iComboBox;		// #IGNORE -- support for Hilight
class	iEditGrid;		// #IGNORE
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
//TODO: get rid of these...
#define	iButton 	Q3Button
#define iWidget		QWidget
#define iWindow 	QWidget  //DO NOT CHANGE (needed for WindowList)
#define iTopLevelWindow	QMainWindow  // tentative

// 	IV Class	QT Class
//	ivDeck		Q3WidgetStack

// some external systems, such as Coin3d, require this:
#ifdef __MAKETA__
  #ifndef __cplusplus
    #define __cplusplus
  #endif
#endif

#endif // TA_GUI

#endif // QTDEFS_H

