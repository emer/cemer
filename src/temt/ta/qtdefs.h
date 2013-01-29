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
  class QList;  // #IGNORE
  class QPointer; // #IGNORE
#else // not maketa
  template<class T> class       QList;  // #IGNORE
  template<class T> class       QPointer; // #IGNORE
#endif
class   QBitArray;              // #IGNORE
class   QDir;                   // #IGNORE
class   QEvent;                 // #IGNORE
class   QObject;                // #IGNORE
class   QMutex;                 // #IGNORE
class   QString;                // #IGNORE
class   QStringList;            // #IGNORE
class   QThread;                // #IGNORE
class   QTimer;                 // #IGNORE
class   QVariant;               // #IGNORE
class   QWaitCondition;         // #IGNORE

// Qt Gui defines -- only when TA_GUI

#ifdef TA_GUI
class   QAbstractButton;        // #IGNORE
class   QAbstractItemModel;     // #IGNORE
class   QAction;                // #IGNORE
class   QActionGroup;           // #IGNORE
class   QBitmap;                // #IGNORE
class   QBoxLayout;             // #IGNORE
class   QButtonGroup;           // #IGNORE
class   QCanvas;                // #IGNORE
class   QCheckBox;              // #IGNORE
class   QColor;                 // #IGNORE
//class QColorGroup;            // #IGNORE
class   QComboBox;              // #IGNORE
class   QCursor;                // #IGNORE
class   QDialog;                // #IGNORE
class   QDockWidget;            // #IGNORE
class   QDragObject;            // #IGNORE
class   QFileDialog;            // #IGNORE
class   QFont;                  // #IGNORE
#if (QT_VERSION >= 0x040400)
class   QFormLayout;            // #IGNORE
#endif
class   QFrame;                 // #IGNORE
class   QGrid;                  // #IGNORE
class   QGridLayout;            // #IGNORE
class   QGroupBox;              // #IGNORE
class   QHBox;                  // #IGNORE
class   QHBoxLayout;            // #IGNORE
class   QHostAddress;           // #IGNORE
class   QIcon;                  // #IGNORE
class   QImage;                 // #IGNORE
class   QItemDelegate;          // #IGNORE
class   QLabel;                 // #IGNORE
class   QLayout;                // #IGNORE
class   QLineEdit;              // #IGNORE
class   QListWidget;            // #IGNORE
class   QListWidgetItem;        // #IGNORE
class   QMainWindow;            // #IGNORE
class   QMenu;                  // #IGNORE
class   QMenuBar;               // #IGNORE
class   QMessageBox;            // #IGNORE
//class QMimeSource;            // #IGNORE
class   QMimeData;              // #IGNORE
class   QModelIndex;            // #IGNORE
#ifdef __MAKETA__
class   QModelIndexList;        // #IGNORE
#else
typedef QList<QModelIndex> QModelIndexList;     // #IGNORE
#endif
class   QPainter;               // #IGNORE
class   QPixmap;                // #IGNORE
class   QPoint;                 // #IGNORE
class   QProgressDialog;        // #IGNORE
class   QPushButton;            // #IGNORE
class   QRadioButton;           // #IGNORE
class   QRect;                  // #IGNORE
class   QScrollArea;            // #IGNORE
class   QScrollBar;             // #IGNORE
class   QSessionManager;        // #IGNORE
class   QSize;                  // #IGNORE
class   QSizePolicy;            // #IGNORE
class   QShortcut;              // #IGNORE
class   QSlider;                // #IGNORE
class   QSortFilterProxyModel;  // #IGNORE
class   QSpacerItem;            // #IGNORE
class   QSpinBox;               // #IGNORE
class   QSplitter;              // #IGNORE
class   QStackedLayout;         // #IGNORE
class   QStackedWidget;         // #IGNORE
class   QStandardItemModel;     // #IGNORE
class   QStatusBar;             // #IGNORE
class   QTableView;             // #IGNORE
class   QTableWidget;           // #IGNORE
class   QTableWidgetItem;       // #IGNORE
class   QTabBar;                // #IGNORE
class   QTabWidget;             // #IGNORE
class   QTcpServer;             // #IGNORE
class   QTcpSocket;             // #IGNORE
class   QTextBrowser;           // #IGNORE
class   QTextEdit;              // #IGNORE
class   QToolBar;               // #IGNORE
class   QToolBox;               // #IGNORE
class   QToolButton;            // #IGNORE
class   QToolTip;               // #IGNORE
class   QTreeWidget;            // #IGNORE
class   QTreeWidgetItem;        // #IGNORE
class   QValidator;             // #IGNORE
class   QVBoxLayout;            // #IGNORE
class   QWidget;                // #IGNORE

// Events
class   QCloseEvent;            // #IGNORE
class   QChildEvent;            // #IGNORE
class   QContextMenuEvent;      // #IGNORE
class   QDropEvent;             // #IGNORE
class   QFocusEvent;            // #IGNORE
class   QHideEvent;             // #IGNORE
class   QKeyEvent;              // #IGNORE
class   QMouseEvent;            // #IGNORE
class   QMoveEvent;             // #IGNORE
class   QPaintEvent;            // #IGNORE
class   QResizeEvent;           // #IGNORE
class   QShowEvent;             // #IGNORE

#ifdef TA_USE_INVENTOR
// Common So Classes
class SoBaseColor;              // #IGNORE
class SoCone;                   // #IGNORE
class SoCube;                   // #IGNORE
class SoCylinder;               // #IGNORE
class SoGroup;                  // #IGNORE
class SoNode;                   // #IGNORE
class SoPath;                   // #IGNORE
class SoSeparator;              // #IGNORE
class SoTransform;              // #IGNORE

#endif // TA_USE_INVENTOR
#endif // TA_GUI

#define COLOR_BLACK Qt::black

// some external systems, such as Coin3d, require this:
#ifdef __MAKETA__
# ifndef __cplusplus
#   define __cplusplus
# endif
#endif

#endif // TA_USE_QT

#endif // QTDEFS_H
