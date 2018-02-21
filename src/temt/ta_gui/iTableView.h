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

#ifndef iTableView_h
#define iTableView_h 1

// parent includes:
#include "ta_def.h"

#include <QTableView>
#include <QStyledItemDelegate>

// member includes:
#include <QPointer>

// declare all other types mentioned but not required to include:
class QWidget; //
class QPoint; //
class iMainWindowViewer; //
class taiWidgetMenu; //
class CellRange; //
class QWheelEvent; //


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

  static const int row_margin;  // #NO_SAVE #HIDDEN #CAT_GUI each row should have this margin top and bottom and between lines
  static const int column_margin;  // #NO_SAVE #HIDDEN #CAT_GUI each column should have this margin on left and right for readability
  static const int max_lines_per_row; // #NO_SAVE #HIDDEN #CAT_GUI maximum lines of text within one table row
  static const int max_chars_per_line; // #NO_SAVE #HIDDEN #CAT_GUI maximum characters per line of text in column (don't let column get to wide)
  static const int default_chars_per_line; // #NO_SAVE #HIDDEN #CAT_GUI default number of  characters per line of text in column
  static const int resize_precision_rows; // #NO_SAVE #HIDDEN #CAT_GUI number of rows to include in auto-resizing columns
  
  int           edit_start_pos;  // position to start when editing
  bool          edit_start_kill; // start editing with kill
#ifndef __MAKETA__
  QPointer<iMainWindowViewer> m_window; // used for tab processing etc
#endif

  virtual void          SetItemDelegates();        // this is the place to associate column types with delegates
  virtual void          clearExtSelection();       // clear extended selection mode and also clear any existing selection
  virtual void          selectCurCell();           // call clearExtSelection and then select current index
  virtual bool          SelectColumns(int st_col, int st_row);
  
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
  virtual bool          SelectedRows(int& st_row, int& end_row);
  // return start and end range of selected rows in the view
  virtual bool          SelectRows(int st_row, int end_row);
  // select given range of rows - inclusive
  virtual void          SetCurrentAndSelect(int row, int col);
  // for selecting single cell only

  iTableView(QWidget* parent = NULL);

#ifndef __MAKETA__
 signals:
  void                  hasFocus(iTableView* sender); // we emit anytime something happens which implies we are focused
#endif

 public slots: // cliphandler i/f
  virtual void          EditAction(int ea) {}
  virtual void          GetEditActionsEnabled(int& ea) {}

  virtual void          ViewAction(int va) {}
  virtual void          ResetColorScaleAction(int va) {}
  virtual void          SetColorSpecAction(int va) {}
#ifndef __MAKETA__
 signals:
  void                  UpdateUi();
#endif

 protected:
  enum RowColOpCode {
    OP_APPEND                     = 0x00001,
    OP_INSERT                     = 0x00002,
    OP_DUPLICATE                  = 0x00004,
    OP_DELETE                     = 0x00008,
    OP_INSERT_AFTER               = 0x00010,
    OP_DELETE_UNSELECTED          = 0x00020,
    OP_ROW                        = 0x00040,
    OP_COL                        = 0x00080,
    OP_RESIZE_TO_CONTENT          = 0x00100,
    OP_RESIZE_TO_CONTENT_ALL      = 0x00200,
    OP_COMPARE                    = 0x00400,
    OP_CLEAR_COMPARE              = 0x00800,
    OP_SHOW_ALL                   = 0x01000,
    OP_SET_WIDTH                  = 0x02000,
    OP_HILITE                     = 0X04000,
    OP_CLEAR_HILITE               = 0X08000,
    OP_SET_HEIGHT                 = 0x10000, // row height
    OP_RESIZE_HEIGHT_TO_CONTENT   = 0X20000,
    OP_RESTORE_HEIGHT             = 0X40000,
  };
  
  int                   last_x;
  int                   last_y;

  bool                  ext_select_on;     // toggled by Ctrl+space -- extends selection with keyboard movement
  int                   m_saved_scroll_pos;

  bool                  event(QEvent* ev) override;
  void                  keyPressEvent(QKeyEvent* e) override;
  void                  wheelEvent(QWheelEvent *) override;
  bool                  eventFilter(QObject* obj, QEvent* event) override;
  virtual void          FillContextMenu_impl(ContextArea ca, taiWidgetMenu* menu, const CellRange& sel);
  virtual void          RowColOp_impl(int op_code, const CellRange& sel) {};
  virtual bool          isFixedRowCount() const { return false; } // true, ex. for tab mat cells with fixed rows
  virtual bool          isFixedColCount() const { return false; } // true, ex. for tab mat cells with fixed geom

 public slots:
  virtual void          this_customContextMenuRequested(const QPoint& pos);
  virtual void          hor_customContextMenuRequested(const QPoint& pos);
  virtual void          ver_customContextMenuRequested(const QPoint& pos);
  virtual void          SetRowHeight(int n_lines);
  // set row height to given number of lines
  virtual void          SetRowHeightToContents();
  // set row height per row to size of contents up to max_lines_per_row
  virtual void          SetColumnWidth(int column, int n_chars);
  // set column width to given number of characters - uses font metrics to calculate width in pixels
  virtual int           ConvertCharsToPixels(int chars);
  // uses max char width for conversion
  virtual int           ConvertPixelsToChars(int pixels);
  // uses max char width for conversion

protected slots:
  void                  RowColOp(int op_code); // based on selection
};

class TA_API iTableViewDefaultDelegate: public QStyledItemDelegate {
  // this delegate is used to provide editing feedback info to the standard tree widget
INHERITED(QStyledItemDelegate)
Q_OBJECT
public:
  iTableView*   own_table_widg;
 
  iTableViewDefaultDelegate(iTableView* own_tw);

  QWidget*     createEditor(QWidget *parent,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

  void         setModelData(QWidget* editor, QAbstractItemModel* model,
                            const QModelIndex& index) const override;
};

#endif // iTableView_h
