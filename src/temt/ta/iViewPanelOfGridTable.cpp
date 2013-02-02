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

#include "iViewPanelOfGridTable.h"
#include <GridTableView>
#include <taiWidgetField>
#include <iLineEdit>
#include <taiWidgetFieldIncr>
#include <iHColorScaleBar>
#include <BuiltinTypeDefs>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QToolBar>

String iViewPanelOfGridTable::panel_type() const {
  static String str("Grid Log");
  return str;
}

iViewPanelOfGridTable::iViewPanelOfGridTable(GridTableView* tlv)
:inherited(tlv)
{
  int font_spec = taiMisc::fonMedium;

  layTopCtrls = new QHBoxLayout; layWidg->addLayout(layTopCtrls);
//   layTopCtrls->setSpacing(2);        // plenty of room

  // note: check boxes require spacing after them apparently, at least on mac..

  chkDisplay = new QCheckBox("Disp", widg); chkDisplay->setObjectName("chkDisplay");
  chkDisplay->setToolTip("Whether to update the display when the underlying data changes");
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkDisplay);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkManip = new QCheckBox("Manip", widg); chkDisplay->setObjectName("chkManip");
  chkManip->setToolTip("Whether to enable manipulation of the view object via a transformation box that supports position, scale and rotation manipulations");
  connect(chkManip, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkManip);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkHeaders =  new QCheckBox("Hdrs", widg ); chkHeaders->setObjectName("chkHeaders");
  chkHeaders->setToolTip("Whether to display a top row of headers indicating the name of the columns");
  connect(chkHeaders, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkHeaders);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkRowNum =  new QCheckBox("Row\n#", widg); chkRowNum->setObjectName("chkRowNum");
  chkRowNum->setToolTip("Whether to display the row number as the first column");
  connect(chkRowNum, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkRowNum);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chk2dFont =  new QCheckBox("2d\nFont", widg); chk2dFont->setObjectName("chk2dFont");
  chk2dFont->setToolTip("Whether to use a two-dimensional font that is easier to read but does not obey 3d transformations of the display");
  connect(chk2dFont, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chk2dFont);
  layTopCtrls->addSpacing(taiM->hsep_c);

  lblFontScale = taiM->NewLabel("Font\nScale", widg, font_spec);
  lblFontScale->setToolTip("Scaling of the 2d font to make it roughly the same size as the 3d font -- adjust this to change the size of the 2d text (has no effect if 2d Font is not clicked");
  layTopCtrls->addWidget(lblFontScale);
  fldFontScale = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layTopCtrls->addWidget(fldFontScale->GetRep());
  ((iLineEdit*)fldFontScale->GetRep())->setCharWidth(8);

  layTopCtrls->addStretch();

  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  layVals = new QHBoxLayout; layWidg->addLayout(layVals);
  layVals->setSpacing(2);       // plenty of room

  lblRows = taiM->NewLabel("Rows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layVals->addWidget(fldRows->GetRep());

  lblCols = taiM->NewLabel("Cols", widg, font_spec);
  lblCols->setToolTip("Maximum number of columns to display (column widths are scaled to fit).");
  layVals->addWidget(lblCols);
  fldCols = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layVals->addWidget(fldCols->GetRep());

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of grid log display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldWidth->GetRep());
  ((iLineEdit*)fldWidth->GetRep())->setCharWidth(8);

  lblTxtMin = taiM->NewLabel("Min\nText", widg, font_spec);
  lblTxtMin->setToolTip("Minimum text size in 'view units' (size of entire display is 1.0) -- .02 is default -- increase to make small text more readable");
  layVals->addWidget(lblTxtMin);
  fldTxtMin = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldTxtMin->GetRep());
  ((iLineEdit*)fldTxtMin->GetRep())->setCharWidth(8);

  lblTxtMax = taiM->NewLabel("Max\nText", widg, font_spec);
  lblTxtMax->setToolTip("Maximum text size in 'view units' (size of entire display is 1.0) -- .05 is default");
  layVals->addWidget(lblTxtMax);
  fldTxtMax = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldTxtMax->GetRep());
  ((iLineEdit*)fldTxtMax->GetRep())->setCharWidth(8);

  layVals->addStretch();
  layMatrix = new QHBoxLayout; layWidg->addLayout(layMatrix);
  layMatrix->setSpacing(2);     // plenty of room

  lblMatrix = taiM->NewLabel("Matrix\nDisplay", widg, font_spec);
  lblMatrix->setToolTip("This row contains parameters that control the display of matrix values (shown in a grid of colored blocks)");
  layMatrix->addWidget(lblMatrix);

  chkValText =  new QCheckBox("Val\nTxt", widg); chkValText->setObjectName( "chkValText");
  chkValText->setToolTip("Whether to display text of the matrix block values.");
  connect(chkValText, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layMatrix->addWidget(chkValText);
  layMatrix->addSpacing(taiM->hsep_c);

  lblTrans = taiM->NewLabel("Trans-\nparency", widg, font_spec);
  lblTrans->setToolTip("Maximum transparency of the grid blocks (0 = fully opaque, 1 = fully transparent)\nBlocks with smaller magnitude values are more transparent.");
  layMatrix->addWidget(lblTrans);
  fldTrans = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldTrans->GetRep());
  ((iLineEdit*)fldTrans->GetRep())->setCharWidth(8);

  lblRot = taiM->NewLabel("Mat\nRot", widg, font_spec);
  lblRot->setToolTip("Rotation (in degrees) of the matrix in the Z axis, producing a denser stacking of patterns.");
  layMatrix->addWidget(lblRot);
  fldRot = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldRot->GetRep());
  ((iLineEdit*)fldRot->GetRep())->setCharWidth(8);

  lblBlockHeight = taiM->NewLabel("Blk\nHgt", widg, font_spec);
  lblBlockHeight->setToolTip("Maximum height of grid blocks (in Z dimension), as a proportion of their overall X-Y size.");
  layMatrix->addWidget(lblBlockHeight);
  fldBlockHeight = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layMatrix->addWidget(fldBlockHeight->GetRep());
  ((iLineEdit*)fldBlockHeight->GetRep())->setCharWidth(8);

  layMatrix->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  //    Colorscale etc
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);

  chkAutoScale = new QCheckBox("auto\nscale", widg);
  connect(chkAutoScale, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layColorScale->addWidget(chkAutoScale);
  layColorScale->addSpacing(taiM->hsep_c);

  cbar = new iHColorScaleBar(&tlv->colorscale, iColorScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(cbar_scaleValueChanged()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  butSetColor->setMinimumWidth(taiM->maxButtonWidth() / 2);
  layColorScale->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );

  ////////////////////////////////////////////////////////////////////////////
  layClickVals = new QHBoxLayout; layWidg->addLayout(layClickVals);
  layClickVals->setSpacing(2);  // plenty of room

  chkClickVals =  new QCheckBox("Click\nVals", widg); chkClickVals->setObjectName( "chkClickVals");
  chkClickVals->setToolTip("If on, then clicking on cell values in the grid view display in interact mode (red arrow) will change the values.");
  connect(chkClickVals, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layClickVals->addWidget(chkClickVals);
  layClickVals->addSpacing(taiM->hsep_c);

  lblLMBVal = taiM->NewLabel("Left Click\nValue", widg, font_spec);
  lblLMBVal->setToolTip("Value that will be set in the cell if you click with the left mouse button (if Click Vals is on).");
  layClickVals->addWidget(lblLMBVal);
  fldLMBVal = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layClickVals->addWidget(fldLMBVal->GetRep());
  ((iLineEdit*)fldLMBVal->GetRep())->setCharWidth(8);

  lblMMBVal = taiM->NewLabel("Middle Click\nValue", widg, font_spec);
  lblMMBVal->setToolTip("Value that will be set in the cell if you click with the middle mouse button (if Click Vals is on).");
  layClickVals->addWidget(lblMMBVal);
  fldMMBVal = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layClickVals->addWidget(fldMMBVal->GetRep());
  ((iLineEdit*)fldMMBVal->GetRep())->setCharWidth(8);

  layClickVals->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layRowNav = new QHBoxLayout; layWidg->addLayout(layRowNav);
  //  layRowNav->setSpacing(2); // plenty of room

  rowNavTB = new QToolBar(widg);
  layRowNav->addWidget(rowNavTB);

  lblRowGoto = taiM->NewLabel("Row: 00000 Goto", widg, font_spec);
  lblRowGoto->setToolTip("Row number to go to when the Go button is pressed -- rest of buttons provide one-click movment of the visible row in grid view");
  rowNavTB->addWidget(lblRowGoto);
  fldRowGoto = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  rowNavTB->addWidget(fldRowGoto->GetRep());
  ((iLineEdit*)fldRowGoto->GetRep())->setCharWidth(6);
  connect(fldRowGoto->rep(), SIGNAL(returnPressed()), this, SLOT(butRowGoto_pressed()) );

  actRowGoto = rowNavTB->addAction("Go");
  actRowGoto->setToolTip("Go to row number indicated in prior field");
  connect(actRowGoto, SIGNAL(triggered()), this, SLOT(butRowGoto_pressed()) );

  rowNavTB->addSeparator();

  actRowBackAll = rowNavTB->addAction("|<");
  actRowBackAll->setToolTip("Back all the way to first row");
  connect(actRowBackAll, SIGNAL(triggered()), this, SLOT(RowBackAll()) );

  actRowBackPg = rowNavTB->addAction("<<");
  actRowBackPg->setToolTip("Back one page of rows");
  connect(actRowBackPg, SIGNAL(triggered()), this, SLOT(RowBackPg()) );

  actRowBack1 = rowNavTB->addAction("<");
  actRowBack1->setToolTip("Back one row");
  connect(actRowBack1, SIGNAL(triggered()), this, SLOT(RowBack1()) );

  actRowFwd1 = rowNavTB->addAction(">" );
  actRowFwd1->setToolTip("Forward one row");
  connect(actRowFwd1, SIGNAL(triggered()), this, SLOT(RowFwd1()) );

  actRowFwdPg = rowNavTB->addAction(">>" );
  actRowFwdPg->setToolTip("Forward one page of rows");
  connect(actRowFwdPg, SIGNAL(triggered()), this, SLOT(RowFwdPg()) );

  actRowFwdAll = rowNavTB->addAction(">|" );
  actRowFwdAll->setToolTip("Forward all the way to the end");
  connect(actRowFwdAll, SIGNAL(triggered()), this, SLOT(RowFwdAll()) );

  layRowNav->addStretch();

  ////////////////////////////////////////////////////////////////////////////
  layColNav = new QHBoxLayout; layWidg->addLayout(layColNav);
  //  layColNav->setSpacing(2); // plenty of room

  colNavTB = new QToolBar(widg);
  layColNav->addWidget(colNavTB);

  lblColGoto = taiM->NewLabel("Col: 000 Goto", widg, font_spec);
  lblColGoto->setToolTip("Col number to go to when the Go button is pressed -- rest of buttons provide one-click movment of the visible col in grid view");
  colNavTB->addWidget(lblColGoto);
  fldColGoto = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  colNavTB->addWidget(fldColGoto->GetRep());
  ((iLineEdit*)fldColGoto->GetRep())->setCharWidth(6);
  connect(fldColGoto->rep(), SIGNAL(returnPressed()), this, SLOT(butColGoto_pressed()) );

  actColGoto = colNavTB->addAction("Go");
  actColGoto->setToolTip("Go to col number indicated in prior field");
  connect(actColGoto, SIGNAL(triggered()), this, SLOT(butColGoto_pressed()) );

  colNavTB->addSeparator();

  actColBackAll = colNavTB->addAction("|<");
  actColBackAll->setToolTip("Back all the way to first col");
  connect(actColBackAll, SIGNAL(triggered()), this, SLOT(ColBackAll()) );

  actColBackPg = colNavTB->addAction("<<");
  actColBackPg->setToolTip("Back one page of cols");
  connect(actColBackPg, SIGNAL(triggered()), this, SLOT(ColBackPg()) );

  actColBack1 = colNavTB->addAction("<");
  actColBack1->setToolTip("Back one col");
  connect(actColBack1, SIGNAL(triggered()), this, SLOT(ColBack1()) );

  actColFwd1 = colNavTB->addAction(">" );
  actColFwd1->setToolTip("Forward one col");
  connect(actColFwd1, SIGNAL(triggered()), this, SLOT(ColFwd1()) );

  actColFwdPg = colNavTB->addAction(">>" );
  actColFwdPg->setToolTip("Forward one page of cols");
  connect(actColFwdPg, SIGNAL(triggered()), this, SLOT(ColFwdPg()) );

  actColFwdAll = colNavTB->addAction(">|" );
  actColFwdAll->setToolTip("Forward all the way to the end");
  connect(actColFwdAll, SIGNAL(triggered()), this, SLOT(ColFwdAll()) );

  layColNav->addStretch();

  ////////////////////
  // all done..

  layWidg->addStretch();

  MakeButtons(layOuter);
}

iViewPanelOfGridTable::~iViewPanelOfGridTable() {
}

void iViewPanelOfGridTable::InitPanel_impl() {
  // nothing structural here (could split out cols, but not worth it)
}

void iViewPanelOfGridTable::GetValue_impl() {
  inherited::GetValue_impl(); // prob nothing

  GridTableView* glv = this->glv(); //cache
  if (!glv) return;

  glv->display_on = chkDisplay->isChecked();
  glv->manip_ctrl_on = chkManip->isChecked();
  glv->header_on = chkHeaders->isChecked();
  glv->row_num_on = chkRowNum->isChecked();
  glv->two_d_font = chk2dFont->isChecked();
  glv->two_d_font_scale = (float)fldFontScale->GetValue();
  glv->view_rows = (int)fldRows->GetValue();
  glv->col_n = (int)fldCols->GetValue();
  glv->setWidth((float)fldWidth->GetValue());
  glv->text_size_range.min = fldTxtMin->GetValue();
  glv->text_size_range.max = fldTxtMax->GetValue();
  glv->mat_val_text = chkValText->isChecked();
  glv->mat_trans = (float)fldTrans->GetValue();
  glv->mat_rot = (float)fldRot->GetValue();
  glv->mat_block_height = (float)fldBlockHeight->GetValue();
  glv->setScaleData(chkAutoScale->isChecked(), cbar->min(), cbar->max());
  glv->click_vals = chkClickVals->isChecked();
  glv->lmb_val = (float)fldLMBVal->GetValue();
  glv->mmb_val = (float)fldMMBVal->GetValue();

  glv->UpdateDisplay(false); // don't update us, because logic will do that anyway
}

void iViewPanelOfGridTable::CopyFrom_impl() {
  GridTableView* glv = this->glv(); //cache
  if (!glv) return;
  glv->CallFun("CopyFromView");
}

void iViewPanelOfGridTable::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  GridTableView* glv = this->glv(); //cache
  if (!glv) return;

  chkDisplay->setChecked(glv->display_on);
  chkManip->setChecked(glv->manip_ctrl_on);
  chkHeaders->setChecked(glv->header_on);
  chkRowNum->setChecked(glv->row_num_on);
  chk2dFont->setChecked(glv->two_d_font);
  fldFontScale->GetImage((String)glv->two_d_font_scale);

  fldRows->GetImage((String)glv->view_rows);
  fldCols->GetImage((String)glv->col_n);
  fldWidth->GetImage((String)glv->width);
  fldTxtMin->GetImage((String)glv->text_size_range.min);
  fldTxtMax->GetImage((String)glv->text_size_range.max);

  chkValText->setChecked(glv->mat_val_text);
  fldTrans->GetImage((String)glv->mat_trans);
  fldRot->GetImage((String)glv->mat_rot);
  fldBlockHeight->GetImage((String)glv->mat_block_height);

  cbar->UpdateScaleValues();
  chkAutoScale->setChecked(glv->colorscale.auto_scale);

  chkClickVals->setChecked(glv->click_vals);
  fldLMBVal->GetImage((String)glv->lmb_val);
  fldMMBVal->GetImage((String)glv->mmb_val);

  String rwtxt = "Row: " + taMisc::LeadingZeros(glv->view_range.min, 5) + " Goto";
  lblRowGoto->setText(rwtxt);
  String cltxt = "Col: " + taMisc::LeadingZeros(glv->col_range.min, 3) + " Goto";
  lblColGoto->setText(cltxt);
}

void iViewPanelOfGridTable::butRefresh_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->InitDisplay();
  glv->UpdateDisplay();
}

void iViewPanelOfGridTable::butRowGoto_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ViewRow_At(fldRowGoto->GetValue());
}

void iViewPanelOfGridTable::RowBackAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBackAll();
}
void iViewPanelOfGridTable::RowBackPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBackPg();
}
void iViewPanelOfGridTable::RowBack1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowBack1();
}
void iViewPanelOfGridTable::RowFwd1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwd1();
}
void iViewPanelOfGridTable::RowFwdPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwdPg();
}
void iViewPanelOfGridTable::RowFwdAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->RowFwdAll();
}

void iViewPanelOfGridTable::butColGoto_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->ViewCol_At(fldColGoto->GetValue());
}

void iViewPanelOfGridTable::ColBackAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBackAll();
}
void iViewPanelOfGridTable::ColBackPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBackPg();
}
void iViewPanelOfGridTable::ColBack1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColBack1();
}
void iViewPanelOfGridTable::ColFwd1() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwd1();
}
void iViewPanelOfGridTable::ColFwdPg() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwdPg();
}
void iViewPanelOfGridTable::ColFwdAll() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  glv->ColFwdAll();
}

void iViewPanelOfGridTable::butSetColor_pressed() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  glv->CallFun("SetColorSpec");
}

void iViewPanelOfGridTable::cbar_scaleValueChanged() {
  GridTableView* glv = this->glv(); //cache
  if (updating || !glv) return;

  //note: user changed value, so must no longer be autoscale
  ++updating;
  chkAutoScale->setChecked(false); //note: raises signal on widget! (grr...)
  --updating;
  Changed();
//  glv->setScaleData(false, cbar->min(), cbar->max());
}

