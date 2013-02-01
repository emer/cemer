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

#ifndef iGridTableView_Panel_h
#define iGridTableView_Panel_h 1

// parent includes:
#include <iDataTableView_Panel>

// member includes:

// declare all other types mentioned but not required to include:
class taiField; //
class taiIncrField; //
class ColorScaleBar; //
class GridTableView; //
class QCheckBox; //
class QPushButton; //
class QToolBar; //
class QHBoxLayout; // 
class QLabel; //
class QAction; //


TypeDef_Of(iGridTableView_Panel);

class TA_API iGridTableView_Panel: public iDataTableView_Panel {
  Q_OBJECT
INHERITED(iDataTableView_Panel)
public:
  QHBoxLayout*            layTopCtrls;
  QCheckBox*                chkDisplay;
  QCheckBox*                chkManip;
  QCheckBox*                chkHeaders;
  QCheckBox*                chkRowNum;
  QCheckBox*                chk2dFont;
  QLabel*                   lblFontScale;
  taiField*                 fldFontScale;
  QPushButton*              butRefresh;

  QHBoxLayout*            layVals;
  QLabel*                   lblRows;
  taiIncrField*             fldRows; // number of rows to display
  QLabel*                   lblCols;
  taiIncrField*             fldCols; // number of cols to display
  QLabel*                   lblWidth;
  taiField*                 fldWidth; // width of the display (height is always 1.0)
  QLabel*                   lblTxtMin;
  taiField*                 fldTxtMin;
  QLabel*                   lblTxtMax;
  taiField*                 fldTxtMax;

  QHBoxLayout*            layMatrix;
  QLabel*                   lblMatrix;
  QCheckBox*                chkValText;
  QLabel*                   lblTrans;
  taiField*                 fldTrans; // mat_trans parency
  QLabel*                   lblRot;
  taiField*                 fldRot; // mat_rot ation
  QLabel*                   lblBlockHeight;
  taiField*                 fldBlockHeight; // mat_block_height

  QHBoxLayout*            layColorScale;
  QCheckBox*                chkAutoScale;
  ColorScaleBar*                 cbar;             // colorbar
  QPushButton*              butSetColor;

  QHBoxLayout*            layClickVals;
  QCheckBox*                chkClickVals;
  QLabel*                   lblLMBVal;
  taiField*                 fldLMBVal;
  QLabel*                   lblMMBVal;
  taiField*                 fldMMBVal;

  QHBoxLayout*            layRowNav;
//   QLabel*               lblRowNav;
  QToolBar*                rowNavTB;
  QLabel*                   lblRowGoto;
  taiField*                 fldRowGoto;
  QAction*                  actRowGoto;
  QAction*                  actRowBackAll;
  QAction*                  actRowBackPg;
  QAction*                  actRowBack1;
  QAction*                  actRowFwd1;
  QAction*                  actRowFwdPg;
  QAction*                  actRowFwdAll;

  QHBoxLayout*            layColNav;
//   QLabel*               lblColNav;
  QToolBar*                colNavTB;
  QLabel*                   lblColGoto;
  taiField*                 fldColGoto;
  QAction*                  actColGoto;
  QAction*                  actColBackAll;
  QAction*                  actColBackPg;
  QAction*                  actColBack1;
  QAction*                  actColFwd1;
  QAction*                  actColFwdPg;
  QAction*                  actColFwdAll;

  override String       panel_type() const; // this string is on the subpanel button for this panel
  GridTableView*        glv() {return (GridTableView*)m_dv;}

  iGridTableView_Panel(GridTableView* glv);
  ~iGridTableView_Panel();

protected:
  override void         InitPanel_impl(); // called on structural changes
  override void         UpdatePanel_impl(); // called on structural changes
  override void         GetValue_impl();
  override void         CopyFrom_impl();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iGridTableView_Panel;}

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

#endif // iGridTableView_Panel_h
