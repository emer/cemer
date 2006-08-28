// #IGNORE Copyright, 1995-2005, Regents of the University of Colorado,
// #IGNORE Carnegie Mellon University, Princeton University.
//
// #IGNORE This file is part of TA/CSS
//
// #IGNORE   This library is free software; you can redistribute it and/or
// #IGNORE   modify it under the terms of the GNU Lesser General Public
// #IGNORE   License as published by the Free Software Foundation; either
// #IGNORE   version 2.1 of the License, or (at your option) any later version.
// #IGNORE   
// #IGNORE   This library is distributed in the hope that it will be useful,
// #IGNORE   but WITHOUT ANY WARRANTY; without even the implied warranty of
// #IGNORE   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// #IGNORE   Lesser General Public License for more details.

// #IGNORE qtdefs.h -- Qt definitions, for inclusion in any header file
//TODO: some of the defs in TA_GUI list may belong in USE_QT section


#ifndef QTDEFS_H
#define QTDEFS_H

// #IGNORE Qt Core defines -- always included
#ifdef TA_USE_QT
class	QBitArray;		// #IGNORE 
class	QDir;			// #IGNORE
class	QEvent;			// #IGNORE 
template<class T> class	QList;	// #IGNORE 
class	QObject;		// #IGNORE 
class	QString;		// #IGNORE 
class	QStringList;		// #IGNORE 
class	QTimer;			// #IGNORE 
class	QVariant;		// #IGNORE 

// #IGNORE Qt Gui defines -- only when TA_GUI

#ifdef TA_GUI
class	QAbstractItemModel;	// #IGNORE 
class	QAction;		// #IGNORE 
class	QActionGroup;		// #IGNORE 
class	QBitmap;		// #IGNORE 
class	QBoxLayout;		// #IGNORE 
class	QButton;		// #IGNORE  note: abstract parent class for buttons
class	Q3Button;		// #IGNORE  note: abstract parent class for buttons
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
class	Q3HButtonGroup;		// #IGNORE 
class	Q3Header;		// #IGNORE 
class	QHostAddress;		// #IGNORE
class	QIcon;			// #IGNORE
class	QItemDelegate;		// #IGNORE
class	QLabel;			// #IGNORE 
class	QLayout;		// #IGNORE 
class	QLineEdit;		// #IGNORE 
class	Q3ListBox;		// #IGNORE 
class	Q3ListBoxItem;		// #IGNORE 
//class	Q3ListView;		// #IGNORE 
//class	Q3ListViewItem;		// #IGNORE 
class	QMainWindow;		// #IGNORE 
class	QMenu;			// #IGNORE  Qt4
class	QMenuBar;		// #IGNORE 
class	QMenuItem;		// #IGNORE 
class	QMessageBox;		// #IGNORE 
//class	QMimeSource;		// #IGNORE 
class	QMimeData;		// #IGNORE 
class	QModelIndex;		// #IGNORE
class	QPainter;		// #IGNORE 
class	QPixmap;		// #IGNORE 
class	QPoint;			// #IGNORE 
class	QProgressDialog;	// #IGNORE 
class	QPushButton;		// #IGNORE 
class	QRadioButton;		// #IGNORE 
class	QRect;			// #IGNORE 
class	QScrollArea;		// #IGNORE  Qt4
class	QScrollBar;		// #IGNORE 
class	QSize;			// #IGNORE 
class	QSizePolicy;		// #IGNORE 
class	QSpacerItem;		// #IGNORE 
class	QSpinBox;		// #IGNORE 
class	QSplitter;		// #IGNORE 
class	QStackedWidget;		// #IGNORE 
class	Q3Table;		// #IGNORE 
class	QTableView;		// #IGNORE 
class	Q3TableItem;			// #IGNORE 
class	QTabBar;		// #IGNORE 
class	QTabWidget;		// #IGNORE 
class	QTcpServer;		// #IGNORE 
class	QTcpSocket;		// #IGNORE
class	QTextBrowser;		// #IGNORE 
class	QTextEdit;		// #IGNORE 
class	QToolBar;		// #IGNORE 
class	QToolButton;		// #IGNORE 
class	QToolTip;		// #IGNORE 
class	QTreeWidget;		// #IGNORE
class	QTreeWidgetItem;	// #IGNORE
class	QValidator;		// #IGNORE 
class	Q3VBox;			// #IGNORE 
class	QVBoxLayout;		// #IGNORE 
class	Q3VButtonGroup;		// #IGNORE 
class	QWidget;		// #IGNORE 
class	Q3WidgetStack;		// #IGNORE 

// #IGNORE Events
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


#ifdef TA_USE_INVENTOR
// #IGNORE Common So Classes
class SoBaseColor;		// #IGNORE 
class SoCone; 			// #IGNORE 
class SoCube; 			// #IGNORE 
class SoCylinder; 		// #IGNORE 
class SoGroup; 			//
class SoNode; 			//
class SoPath;			// #IGNORE 
class SoSeparator; 		//
class SoTransform; 		// #IGNORE 

// #IGNORE SoQt Classes (Coin3D/Inventor)
class SoQtRenderArea; //
class SoQtViewer; //

// #IGNORE T3 Gadgets
class	T3ViewspaceWidget;	// #IGNORE 
#endif // #IGNORE TA_GUI
#endif // #IGNORE TA_USE_QT

// #IGNORE Custom Controls

class	iBrush;			// #IGNORE  -- TODO
class	iColor;			// #IGNORE 
class	iCheckBox;		// #IGNORE  -- support for Hilight and ReadOnly
class	iComboBox;		// #IGNORE  -- support for Hilight
class	iDimEdit;		// #IGNORE  for editing dimensions of a Matrix
class	iEditGrid;		// #IGNORE 
class	iFlowLayout;		// #IGNORE 
class	iFont;			//
class	iLabel;			// #IGNORE  -- label w/ highlighting
class	iLineEdit;		// #IGNORE 
class	iDataViewer;		// #IGNORE  -- in ta_qtviewer.h
#define iPixmap		QPixmap
class	iPoint;			//
class	iRect;			//
class	iRenderAreaWrapper;	// #IGNORE  -- wrapper for SoQtRenderArea
class	iSize;			//
class	iSpinBox;		// #IGNORE 
class	iStripeWidget;		// #IGNORE  -- in ieditgrid.h/.cc
class	iTransformer;		// #IGNORE 
class	iTreeWidget;		// #IGNORE 

#define COLOR_BLACK Qt::black

// #IGNORE aliases

//TODO: get rid of these...
#define	iButton 	Q3Button
#define iWidget		QWidget
#define iWindow 	QWidget  //DO NOT CHANGE (needed for WindowList)

// #IGNORE some external systems, such as Coin3d, require this:
#ifdef __MAKETA__
# ifndef __cplusplus
#   define __cplusplus
# endif
#endif

#endif // #IGNORE TA_GUI

#endif // #IGNORE QTDEFS_H

