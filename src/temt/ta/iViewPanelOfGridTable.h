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

#ifndef iViewPanelOfGridTable_h
#define iViewPanelOfGridTable_h 1

// parent includes:
#include <iViewPanelOfDataTable>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetField; //
class taiWidgetFieldIncr; //
class iColorScaleBar; //
class GridTableView; //
class QCheckBox; //
class QPushButton; //
class QToolBar; //
class QHBoxLayout; // 
class QLabel; //
class QAction; //


taTypeDef_Of(iViewPanelOfGridTable);

class TA_API iViewPanelOfGridTable: public iViewPanelOfDataTable {
  Q_OBJECT
INHERITED(iViewPanelOfDataTable)
public:
  QHBoxLayout*            layTopCtrls;
  QCheckBox*                chkDisplay;
  QCheckBox*                chkManip;
  QCheckBox*                chkHeaders;
  QCheckBox*                chkRowNum;
  QCheckBox*                chk2dFont;
  QLabel*                   lblFontScale;
  taiWidgetField*           fldFontScale;
  QPushButton*              butRefresh;

  QHBoxLayout*            layVals;
  QLabel*                   lblRows;
  taiWidgetFieldIncr*       fldRows; // number of rows to display
  QLabel*                   lblCols;
  taiWidgetFieldIncr*       fldCols; // number of cols to display
  QLabel*                   lblWidth;
  taiWidgetField*           fldWidth; // width of the display (height is always 1.0)
  QLabel*                   lblTxtMin;
  taiWidgetField*           fldTxtMin;
  QLabel*                   lblTxtMax;
  taiWidgetField*           fldTxtMax;

  QHBoxLayout*            layMatrix;
  QCheckBox*                chkValText;
  QLabel*                   lblTrans;
  taiWidgetField*           fldTrans; // mat_trans parency
  QLabel*                   lblRot;
  taiWidgetField*           fldRot; // mat_rot ation
  QLabel*                   lblGridMarg;
  taiWidgetField*           fldGridMarg;
  QLabel*                   lblGridLine;
  taiWidgetField*           fldGridLine;
  QLabel*                   lblBlockHeight;
  taiWidgetField*           fldBlockHeight; // mat_block_height
  QLabel*                   lblBlockSpace;
  taiWidgetField*           fldBlockSpace;

  QHBoxLayout*            layColorScale;
  QCheckBox*                chkAutoScale;
  iColorScaleBar*           cbar;             // colorbar
  QPushButton*              butSetColor;

  QHBoxLayout*            layClickVals;
  QCheckBox*                chkClickVals;
  QLabel*                   lblLMBVal;
  taiWidgetField*           fldLMBVal;
  QLabel*                   lblMMBVal;
  taiWidgetField*           fldMMBVal;

  QHBoxLayout*            layRowNav;
  QToolBar*                rowNavTB;
  QLabel*                   lblRowGoto;
  taiWidgetField*           fldRowGoto;
  QAction*                  actRowGoto;
  QAction*                  actRowBackAll;
  QAction*                  actRowBackPg;
  QAction*                  actRowBack1;
  QAction*                  actRowFwd1;
  QAction*                  actRowFwdPg;
  QAction*                  actRowFwdAll;

  QHBoxLayout*            layColNav;
  QToolBar*                colNavTB;
  QLabel*                   lblColGoto;
  taiWidgetField*           fldColGoto;
  QAction*                  actColGoto;
  QAction*                  actColBackAll;
  QAction*                  actColBackPg;
  QAction*                  actColBack1;
  QAction*                  actColFwd1;
  QAction*                  actColFwdPg;
  QAction*                  actColFwdAll;

  override String       panel_type() const; // this string is on the subpanel button for this panel
  GridTableView*        glv() {return (GridTableView*)m_dv;}

  iViewPanelOfGridTable(GridTableView* glv);
  ~iViewPanelOfGridTable();

protected:
  override void         InitPanel_impl(); // called on structural changes
  override void         UpdatePanel_impl(); // called on structural changes
  override void         GetValue_impl();
  override void         CopyFrom_impl();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iViewPanelOfGridTable;}

protected slots:
  void          butRefresh_pressed();
  void          butRowGoto_pressed();
  void          butColGoto_pressed();

  virtual void          RowBackAll();
  virtual void          RowBackPg();
  virtual void          RowBack1();
  virtual void          RowFwd1();
  virtual void          RowFwdPg();
  virtual void          RowFwdAll();

  virtual void          ColBackAll();
  virtual void          ColBackPg();
  virtual void          ColBack1();
  virtual void          ColFwd1();
  virtual void          ColFwdPg();
  virtual void          ColFwdAll();

  void          butSetColor_pressed();

  void          cbar_scaleValueChanged();
};

#endif // iViewPanelOfGridTable_h
