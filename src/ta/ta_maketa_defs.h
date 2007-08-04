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

// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// Hack file for maketa because it chokes on Qt header files

#ifndef TA_MAKETA_DEFS_H
#define TA_MAKETA_DEFS_H

// a couple of these are the actual guys when we have NO_GUI

// dummy classes for the view system, when no TA_GUI
#if defined(__MAKETA__) || !defined(TA_GUI)
class QObject {
public:
  QObject() {} // #IGNORE
};

class QPaintDevice {
public:
  QPaintDevice() {} // #IGNORE
};

class QWidget : public QObject, public QPaintDevice {
public:
  QWidget() {} // #IGNORE
};
#endif


#ifdef __MAKETA__

// NOTE: we need the "dummy" member items because maketa chokes on empty class defs

typedef uint WFlags; // #IGNORE
typedef uint GFlags; // #IGNORE

//define any WFlags used in header files here -- (from qnamespace.h)

class QEvent {
public:
  enum Type {
    None = 0
  };
  int dummy; // #IGNORE
};

class QKeySequence {
public:
  int dummy; // #IGNORE
};

class QStringList {
//  
public:
	int dummy; // #IGNORE
};

class QAbstractItemDelegate: public QObject {
//  
public:
	int dummy; // #IGNORE
};

class QItemDelegate: public QAbstractItemDelegate {
//  
public:
	int dummy; // #IGNORE
};

class QAbstractTableModel: public QObject {
//  
public:
	int dummy; // #IGNORE
};


class QAction : public QObject {
//  
public:
	int dummy; // #IGNORE
};

class QActionGroup : public QObject {
//  
public:
	int dummy; // #IGNORE
};

class QPluginLoader: public QObject {
//  
public:
	int dummy; // #IGNORE
};

class QByteArray {
//  
public:
	int dummy; // #IGNORE
};

class Q3DragObject: public QAction {
//  
public:
	int dummy; // #IGNORE
};


class iRenderAreaWrapper: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class QDialog: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class QButton: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class QPushButton: public QButton  {
//  
public:
	int dummy; // #IGNORE
};

class QDockWidget: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class QLabel: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class QMainWindow: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class Q3Scrollview: public QWidget {
public:
  int dummy; // #IGNORE
};

class QSplitter: public QWidget {
public:
  int dummy; // #IGNORE
};

class QTabBar: public QWidget {
public:
  int dummy; // #IGNORE
};

class QToolBar: public QWidget  {
//  
public:
	int dummy; // #IGNORE
};

class Q3Table: public Q3Scrollview {
public:
  int dummy; // #IGNORE
};

class iClipToolWidget: public QLabel  {
//  
public:
	int dummy; // #IGNORE
};


template<class T>
class QList {
//  
public:
	int dummy; // #IGNORE
};

class QColor {
//  
public:
	int dummy; // #IGNORE
};

class QIconSet {
//  
public:
	int dummy; // #IGNORE
};

class QObjectList {
//  
public:
	int dummy; // #IGNORE
};

class QPalette {
//  
public:
	int dummy; // #IGNORE
};

class QPixmap {
//  
public:
	int dummy; // #IGNORE
};

class QPoint {
//  
public:
	int dummy; // #IGNORE
};

class QRect {
//  
public:
	int dummy; // #IGNORE
};

class QSize { //  
public:
	int dummy; // #IGNORE
};

class QSizePolicy { //  
public:
	int dummy; // #IGNORE
};

#ifdef TA_USE_INVENTOR
class SoBase {// ##NO_INSTANCE ##NO_TOKENS
public:
	int dummy; // #IGNORE
	virtual ~SoBase() {}
};

class SoFieldContainer: public SoBase {
public:
	int dummy; // #IGNORE
};

class SoNode: public SoFieldContainer {
public:
	int dummy; // #IGNORE
};

class SoGroup: public SoNode {
public:
	int dummy; // #IGNORE
};

class SoShape: public SoNode {
public:
	int dummy; // #IGNORE
};

class SoSeparator: public SoGroup {
public:
	int dummy; // #IGNORE
};

class SoImage: public SoShape {
public:
	int dummy; // #IGNORE
};

class SoTriangleStripSet: public SoBase {
public:
  int dummy; // #IGNORE
};
#endif

#endif // __MAKETA__
#endif // QTMAKETA_H
