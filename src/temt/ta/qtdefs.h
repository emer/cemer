// Copyright, 1995-2007, Regents of the University of Colorado,
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

// qtdefs.h -- Qt definitions, for inclusion in any header file

//TODO: some of the defs in TA_GUI list may belong in USE_QT section

#ifndef QTDEFS_H
#define QTDEFS_H

// Qt Core defines -- always included
#ifdef TA_USE_QT
#ifdef __MAKETA__
  class	QList;	// #IGNORE #HIDDEN
  class	QPointer; // #IGNORE #HIDDEN 
#else // not maketa
  template<class T> class	QList;	// #IGNORE #HIDDEN
  template<class T> class	QPointer; // #IGNORE #HIDDEN 
#endif
class	QBitArray;		// #IGNORE #HIDDEN 
class	QDir;			// #IGNORE #HIDDEN
class	QEvent;			// #IGNORE #HIDDEN 
class	QObject;		// #IGNORE #HIDDEN 
class	QMutex;			// #IGNORE #HIDDEN
class	QString;		// #IGNORE #HIDDEN 
class	QStringList;		// #IGNORE #HIDDEN 
class	QThread;		// #IGNORE #HIDDEN
class	QTimer;			// #IGNORE #HIDDEN 
class	QVariant;		// #IGNORE #HIDDEN 
class	QWaitCondition;		// #IGNORE #HIDDEN

// Qt Gui defines -- only when TA_GUI

#ifdef TA_GUI
class	QAbstractButton;	// #IGNORE #HIDDEN  note: abstract base class for buttons
class	QAbstractItemModel;	// #IGNORE #HIDDEN 
class	QAction;		// #IGNORE #HIDDEN 
class	QActionGroup;		// #IGNORE #HIDDEN 
class	QBitmap;		// #IGNORE #HIDDEN 
class	QBoxLayout;		// #IGNORE #HIDDEN 
class	QButtonGroup;		// #IGNORE #HIDDEN note: not a widget
class	QCanvas;		// #IGNORE #HIDDEN 
class	QCheckBox;		// #IGNORE #HIDDEN 
class	QColor;			// #IGNORE #HIDDEN 
//class	QColorGroup;		// #IGNORE #HIDDEN 
class	QComboBox;		// #IGNORE #HIDDEN 
class	QCursor;		// #IGNORE #HIDDEN 
class	QDialog;		// #IGNORE #HIDDEN 
class	QDockWidget;		// #IGNORE #HIDDEN 
class	QDragObject;		// #IGNORE #HIDDEN 
class	QFileDialog;		// #IGNORE #HIDDEN 
class	QFont;			// #IGNORE #HIDDEN 
#if (QT_VERSION >= 0x040400)
class	QFormLayout;		// #IGNORE #HIDDEN
#endif
class	QFrame;			// #IGNORE #HIDDEN 
class	QGrid;			// #IGNORE #HIDDEN 
class	QGridLayout;		// #IGNORE #HIDDEN 
class	QGroupBox;		// #IGNORE #HIDDEN 
class	QHBox;			// #IGNORE #HIDDEN 
class	QHBoxLayout;		// #IGNORE #HIDDEN 
class	QHostAddress;		// #IGNORE #HIDDEN
class	QIcon;			// #IGNORE #HIDDEN
class	QImage;			// #IGNORE #HIDDEN
class	QItemDelegate;		// #IGNORE #HIDDEN
class	QLabel;			// #IGNORE #HIDDEN 
class	QLayout;		// #IGNORE #HIDDEN 
class	QLineEdit;		// #IGNORE #HIDDEN 
class	QListWidget;		// #IGNORE #HIDDEN 
class	QListWidgetItem;	// #IGNORE #HIDDEN 
class	QMainWindow;		// #IGNORE #HIDDEN 
class	QMenu;			// #IGNORE #HIDDEN  Qt4
class	QMenuBar;		// #IGNORE #HIDDEN 
class	QMessageBox;		// #IGNORE #HIDDEN 
//class	QMimeSource;		// #IGNORE #HIDDEN 
class	QMimeData;		// #IGNORE #HIDDEN 
class	QModelIndex;		// #IGNORE #HIDDEN
#ifdef __MAKETA__
class	QModelIndexList;	// #IGNORE #HIDDEN
#else
typedef QList<QModelIndex> QModelIndexList;	// #IGNORE #HIDDEN
#endif
class	QPainter;		// #IGNORE #HIDDEN 
class	QPixmap;		// #IGNORE #HIDDEN 
class	QPoint;			// #IGNORE #HIDDEN 
class	QProgressDialog;	// #IGNORE #HIDDEN 
class	QPushButton;		// #IGNORE #HIDDEN 
class	QRadioButton;		// #IGNORE #HIDDEN 
class	QRect;			// #IGNORE #HIDDEN 
class	QScrollArea;		// #IGNORE #HIDDEN  Qt4
class	QScrollBar;		// #IGNORE #HIDDEN 
class	QSessionManager;	// #IGNORE #HIDDEN
class	QSize;			// #IGNORE #HIDDEN 
class	QSizePolicy;		// #IGNORE #HIDDEN 
class 	QShortcut;		// #IGNORE #HIDDEN
class	QSpacerItem;		// #IGNORE #HIDDEN 
class	QSpinBox;		// #IGNORE #HIDDEN 
class	QSplitter;		// #IGNORE #HIDDEN 
class	QStackedLayout;		// #IGNORE #HIDDEN 
class	QStackedWidget;		// #IGNORE #HIDDEN 
class	QStatusBar;		// #IGNORE #HIDDEN
class	QTableView;		// #IGNORE #HIDDEN 
class	QTableWidget;		// #IGNORE #HIDDEN 
class	QTableWidgetItem;	// #IGNORE #HIDDEN 
class	QTabBar;		// #IGNORE #HIDDEN 
class	QTabWidget;		// #IGNORE #HIDDEN 
class	QTcpServer;		// #IGNORE #HIDDEN 
class	QTcpSocket;		// #IGNORE #HIDDEN
class	QTextBrowser;		// #IGNORE #HIDDEN 
class	QTextEdit;		// #IGNORE #HIDDEN 
class	QToolBar;		// #IGNORE #HIDDEN 
class	QToolBox;		// #IGNORE #HIDDEN
class	QToolButton;		// #IGNORE #HIDDEN 
class	QToolTip;		// #IGNORE #HIDDEN 
class	QTreeWidget;		// #IGNORE #HIDDEN
class	QTreeWidgetItem;	// #IGNORE #HIDDEN
class	QValidator;		// #IGNORE #HIDDEN 
class	QVBoxLayout;		// #IGNORE #HIDDEN 
class	QWidget;		// #IGNORE #HIDDEN 

// Events
class	QCloseEvent;		// #IGNORE #HIDDEN 
class	QChildEvent;		// #IGNORE #HIDDEN 
class	QContextMenuEvent;	// #IGNORE #HIDDEN 
class	QDropEvent;		// #IGNORE #HIDDEN 
class	QFocusEvent;		// #IGNORE #HIDDEN 
class	QHideEvent;		// #IGNORE #HIDDEN 
class	QKeyEvent;		// #IGNORE #HIDDEN 
class	QMouseEvent;		// #IGNORE #HIDDEN 
class	QMoveEvent;		// #IGNORE #HIDDEN 
class	QPaintEvent;		// #IGNORE #HIDDEN 
class	QResizeEvent;		// #IGNORE #HIDDEN 
class	QShowEvent;		// #IGNORE #HIDDEN 

# ifdef QT3_SUPPORT
# else
#define setPaletteBackgroundColor(c)\
{QPalette palette;\
 palette.setColor(backgroundRole(), c);\
 setPalette(palette); }
# endif

#ifdef TA_USE_INVENTOR
// Common So Classes
class SoBaseColor;		// #IGNORE #HIDDEN 
class SoCone; 			// #IGNORE #HIDDEN 
class SoCube; 			// #IGNORE #HIDDEN 
class SoCylinder; 		// #IGNORE #HIDDEN 
class SoGroup; 			//
class SoNode; 			//
class SoPath;			// #IGNORE #HIDDEN 
class SoSeparator; 		//
class SoTransform; 		// #IGNORE #HIDDEN 

// T3 Gadgets
class	T3ViewspaceWidget;	// #IGNORE #HIDDEN 
#endif // TA_USE_INVENTOR
#endif // TA_GUI

// Custom Controls

class	iBrush;			// #IGNORE #HIDDEN  -- TODO
class	iColor;			// #IGNORE #HIDDEN 
class	iColorButton;		// #IGNORE #HIDDEN 
class	iCheckBox;		// #IGNORE #HIDDEN  -- support for Hilight and ReadOnly
class	iComboBox;		// #IGNORE #HIDDEN  -- support for Hilight
class	iClipToolWidget;	// #IGNORE #HIDDEN
class	iClipWidgetAction;	// #IGNORE #HIDDEN
class	iDimEdit;		// #IGNORE #HIDDEN  for editing dimensions of a Matrix
class	iEditGrid;		// #IGNORE #HIDDEN 
class	iFlowLayout;		// #IGNORE #HIDDEN 
class	iFont;			//
#if (QT_VERSION >= 0x040400)
class	iFormLayout;		// #IGNORE #HIDDEN
#endif
class	HiLightButton;		//
class	iLabel;			// #IGNORE #HIDDEN  -- label w/ highlighting
class	iLineEdit;		// #IGNORE #HIDDEN 
class	iMenuButton;		// #IGNORE #HIDDEN -- some AbstractButton derivitive used for menus
class	iMethodButtonFrame;
class	iDataViewer;		// #IGNORE #HIDDEN  -- in ta_qtviewer.h
#define iPixmap		QPixmap
class	iPoint;			//
class	iRect;			//
class	iSize;			//
class	iSpinBox;		// #IGNORE #HIDDEN 
class	iStripeWidget;		// #IGNORE #HIDDEN  -- in ieditgrid.h/.cc
class	iTransformer;		// #IGNORE #HIDDEN 
class	iTextBrowser;		// #IGNORE #HIDDEN
class	iTreeWidget;		// #IGNORE #HIDDEN 

// clipboard types
class	taiClipData;		// #IGNORE #HIDDEN

#define COLOR_BLACK Qt::black

// aliases

//TODO: get rid of these...
#define iWindow 	QWidget  //DO NOT CHANGE (needed for WindowList)

// some external systems, such as Coin3d, require this:
#ifdef __MAKETA__
# ifndef __cplusplus
#   define __cplusplus
# endif
#endif

#endif // TA_USE_QT

#endif // QTDEFS_H
