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

class Q3ListViewItem {
public:
  int dummy; // #IGNORE
};

class QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QAbstractTableModel: public QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};


class QAction : public QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QActionGroup : public QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPluginLoader: public QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QByteArray {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class Q3DragObject: public QAction {
//#IGNORE
public:
	int dummy; // #IGNORE
};


class QWidget : public QObject, public QPaintDevice {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class iRenderAreaWrapper: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QDialog: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QButton: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPushButton: public QButton  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QDockWindow: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QToolBar: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QMainWindow: public QWidget  {
//#IGNORE
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

class Q3Table: public Q3Scrollview {
public:
  int dummy; // #IGNORE
};

template<class T>
class QList {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QColor {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QIconSet {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QObjectList {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPaintDevice {
//#IGNORE
public:
	int dummy2; // #IGNORE
};

class QPalette {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPixmap {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPoint {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QRect {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QSize { //#IGNORE
public:
	int dummy; // #IGNORE
};

class QSizePolicy { //#IGNORE
public:
	int dummy; // #IGNORE
};


#endif // __MAKETA__
#endif // QTMAKETA_H
