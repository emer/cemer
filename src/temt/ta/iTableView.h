// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef iTableView_h
#define iTableView_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QTableView>
#endif

// member includes:
#include <QPointer>

// declare all other types mentioned but not required to include:
class QWidget; //
class QPoint; //
class iMainWindowViewer; //
class taiWidgetMenu; //
class CellRange; //


class TA_API iTableView: public QTableView {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBER table editor; model flattens >2d into 2d by frames
  INHERITED(QTableView)
  Q_OBJECT
public:
  enum ContextArea { // where ctxt menu was requested
    CA_EMPTY    = 0x01,
    CA_GRID     = 0x02,
    CA_COL_HDR  = 0x04,
    CA_ROW_HDR  = 0x08,
    CA_HDR_MASK = 0x0C
  };

#ifndef __MAKETA__
  QPointer<iMainWindowViewer> m_window; // used for tab processing etc
#endif

  virtual bool          isFixedRowCount() const { return false; } // true, ex. for tab mat cells with fixed rows
  virtual bool          isFixedColCount() const { return false; } // true, ex. for tab mat cells with fixed geom
  virtual void          clearExtSelection();       // clear extended selection mode and also clear any existing selection
  virtual void          selectCurCell();           // call clearExtSelection and then select current index

  /////////////////////////////////////////////////////////////////
  //            ScrollArea Management

  virtual void          SaveScrollPos();
  // save the current vertical scroll position for later restore
  virtual void          RestoreScrollPos();
  // restore the current vertical scroll position
  virtual void          ScrollTo(int scr_pos);
  // scroll vertically to given position -- directly controls vertical scroll bar
  virtual void          CenterOn(QWidget* widg);
  // center the scrollbar on center of given widget
  virtual void          KeepInView(QWidget* widg);
  // ensure that the given widget is fully in view -- just move up or down as needed to keep fully in view
  virtual bool          PosInView(int scr_pos);
  // is given position within the main scroll area (in coordinates relative to central widget) within view?
  virtual QPoint        MapToTree(QWidget* widg, const QPoint& pt);
  // map coordinate point within given child widget on panel to the coordinates of the panel scroll area
  virtual int           MapToTreeV(QWidget* widg, int pt_y);
  // map vertical coordinate value within given child widget on panel to the coordinates of the panel scroll area
  virtual bool		      SelectedRows(int& st_row, int& end_row);
  // return start and end range of selected rows in the view
  virtual bool		      SelectRows(int st_row, int end_row);
  // select given range of rows

  iTableView(QWidget* parent = NULL);

#ifndef __MAKETA__
 signals:
  void                  hasFocus(iTableView* sender); // we emit anytime something happens which implies we are focused
#endif

 public slots: // cliphandler i/f
  virtual void          EditAction(int ea) {}
  virtual void          GetEditActionsEnabled(int& ea) {}

  virtual void          ViewAction(int va) {}
  virtual void          ResetColorsAction(int va) {}
#ifndef __MAKETA__
 signals:
  void                  UpdateUi();
#endif

 protected:
  enum RowColOpCode {
    OP_APPEND                 = 0x001,
    OP_INSERT                 = 0x002,
    OP_DUPLICATE              = 0x004,
    OP_DELETE                 = 0x008,
    OP_INSERT_AFTER           = 0X010,
    OP_DELETE_UNSELECTED      = 0X020,
    OP_ROW                    = 0x040,
    OP_COL                    = 0x080,
    OP_RESIZE_TO_CONTENT      = 0x100,
    OP_RESIZE_TO_CONTENT_ALL  = 0x200,
    OP_COMPARE                = 0x400,
    OP_CLEAR_COMPARE          = 0x800
  };

  bool                  ext_select_on;     // toggled by Ctrl+space -- extends selection with keyboard movement
  int                   m_saved_scroll_pos;

  bool                  event(QEvent* ev) override;
  void                  keyPressEvent(QKeyEvent* e) override;
  bool                  eventFilter(QObject* obj, QEvent* event) override;
  virtual void          FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu,
                                             const CellRange& sel);
  virtual void          RowColOp_impl(int op_code, const CellRange& sel) {}

 public slots:
  virtual void          this_customContextMenuRequested(const QPoint& pos);
  virtual void          hor_customContextMenuRequested(const QPoint& pos);
  virtual void          ver_customContextMenuRequested(const QPoint& pos);

 protected slots:
  void                  RowColOp(int op_code); // based on selection
};

#endif // iTableView_h
