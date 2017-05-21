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

#include "iViewPanelOfGraphTable.h"
#include <DataTable>
#include <GraphTableView>
#include <GraphColView>
#include <iHColorScaleBar>
#include <taiWidgetComboBox>
#include <taiWidgetFieldIncr>
#include <taiWidgetField>
#include <taiWidgetListElChooser>
#include <taiWidgetMenu>
#include <taiWidgetPoly>
#include <iCheckBox>
#include <iLabel>
#include <BuiltinTypeDefs>
#include <iLineEdit>
#include <iStripeWidget>
#include <iFormLayout>
#include <taGuiDialog>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSignalMapper>
#include <QContextMenuEvent>


const int iViewPanelOfGraphTable::axis_chooser_width = 160;
const int iViewPanelOfGraphTable::axis_label_width = 25;
const int iViewPanelOfGraphTable::margin_l_r = 3;
const int iViewPanelOfGraphTable::margin_t_b = 0;

String iViewPanelOfGraphTable::panel_type() const {
  static String str("Graph Log");
  return str;
}

iViewPanelOfGraphTable::iViewPanelOfGraphTable(GraphTableView* tlv)
:inherited(tlv)
{
  int font_spec = taiMisc::fonMedium;
  
  layPlots = NULL;
  row_height = 10;
  for(int i=0; i<max_plots; i++) { // coverity food
    layYAxis[i] = NULL;
    oncYAxis[i] = NULL;
    lblYAxis[i] = NULL;
    lelYAxis[i] = NULL;
    pdtYAxis[i] = NULL;
    chkYAltY[i] = NULL;
    lelErr[i] = NULL;
    oncErr[i] = NULL;
    lblcellYAxis[i] = NULL;
    cellYAxis[i] = NULL;
    butLineProps[i] = NULL;
  }
  
  layTopCtrls = new QHBoxLayout; layWidg->addLayout(layTopCtrls);
  layTopCtrls->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  chkDisplay = new QCheckBox("Disp", widg); chkDisplay->setObjectName("chkDisplay");
  chkDisplay->setToolTip(taiMisc::ToolTipPreProcess("Whether to update the display when the underlying data changes"));
  connect(chkDisplay, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkDisplay);
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  chkManip = new QCheckBox("Manip", widg); chkDisplay->setObjectName("chkManip");
  chkManip->setToolTip(taiMisc::ToolTipPreProcess("Whether to enable manipulation of the view object via a transformation box that supports position, scale and rotation manipulations"));
  connect(chkManip, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkManip);
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  // todo: fix tool tips on all of these..
  
  lblGraphType = taiM->NewLabel("Graph", widg, font_spec);
  lblGraphType->setToolTip(taiMisc::ToolTipPreProcess("How to display the graph"));
  layTopCtrls->addWidget(lblGraphType);
  cmbGraphType = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("GraphType"),
                                              this, NULL, widg, taiWidget::flgAutoApply));
  layTopCtrls->addWidget(cmbGraphType->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  lblPlotStyle = taiM->NewLabel("Style", widg, font_spec);
  lblPlotStyle->setToolTip(taiMisc::ToolTipPreProcess("How to plot the lines"));
  layTopCtrls->addWidget(lblPlotStyle);
  cmbPlotStyle = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("PlotStyle"),
                                              this, NULL, widg, taiWidget::flgAutoApply));
  layTopCtrls->addWidget(cmbPlotStyle->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  chkNegDraw =  new QCheckBox("Neg\nDraw", widg); chkNegDraw->setObjectName("chkNegDraw");
  chkNegDraw->setToolTip(taiMisc::ToolTipPreProcess("Whether to draw a line when going in a negative direction (to the left), which may indicate a wrap-around to a new iteration of data"));
  connect(chkNegDraw, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDraw);
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  chkNegDrawZ =  new QCheckBox("Neg\nDraw Z", widg); chkNegDrawZ->setObjectName("chkNegDrawZ");
  chkNegDrawZ->setToolTip(taiMisc::ToolTipPreProcess("Whether to draw a line when going in a negative direction of Z (to the front), which may indicate a wrap-around to a new channel of data"));
  connect(chkNegDrawZ, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDrawZ);
  layTopCtrls->addSpacing(taiM->hsep_c);
  
  layTopCtrls->addStretch();
  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );
  
  layVals = new QHBoxLayout; layWidg->addLayout(layVals);
  layVals->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  lblRows = taiM->NewLabel("View\nRows", widg, font_spec);
  lblRows->setToolTip(taiMisc::ToolTipPreProcess("Maximum number of rows to display (row height is scaled to fit)."));
  layVals->addWidget(lblRows);
  fldRows = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layVals->addWidget(fldRows->GetRep());
  layVals->addSpacing(taiM->hsep_c);
  
  lblLineWidth = taiM->NewLabel("Line\nWidth", widg, font_spec);
  lblLineWidth->setToolTip(taiMisc::ToolTipPreProcess("Width to draw lines with."));
  layVals->addWidget(lblLineWidth);
  fldLineWidth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLineWidth->GetRep());
  ((iLineEdit*)fldLineWidth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);
  
  lblPointSpacing = taiM->NewLabel("Pt\nSpc", widg, font_spec);
  lblPointSpacing->setToolTip(taiMisc::ToolTipPreProcess("Spacing of points drawn relative to underlying data points."));
  layVals->addWidget(lblPointSpacing);
  fldPointSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldPointSpacing->GetRep());
  ((iLineEdit*)fldPointSpacing->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);
  
  lblLabelSpacing = taiM->NewLabel("Lbl\nSpc", widg, font_spec);
  lblLabelSpacing->setToolTip(taiMisc::ToolTipPreProcess("Spacing of text labels of data point values. -1 means no text labels."));
  layVals->addWidget(lblLabelSpacing);
  fldLabelSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLabelSpacing->GetRep());
  ((iLineEdit*)fldLabelSpacing->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);
  
  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip(taiMisc::ToolTipPreProcess("Width of graph display, in normalized units (default is 1.0 = same as height)."));
  layVals->addWidget(lblWidth);
  fldWidth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldWidth->GetRep());
  ((iLineEdit*)fldWidth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);
  
  lblDepth = taiM->NewLabel("Depth", widg, font_spec);
  lblDepth->setToolTip(taiMisc::ToolTipPreProcess("Depth of graph display, in normalized units (default is 1.0 = same as height)."));
  layVals->addWidget(lblDepth);
  fldDepth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldDepth->GetRep());
  ((iLineEdit*)fldDepth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);
  
  lblNPlots = taiM->NewLabel("N Plots", widg, font_spec);
  lblNPlots->setToolTip(taiMisc::ToolTipPreProcess("Number of different plots available to display in this graph -- can increase to up to 64 -- just takes more room in the control panel"));
  layVals->addWidget(lblNPlots);
  fldNPlots = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  layVals->addWidget(fldNPlots->GetRep());
  ((iLineEdit*)fldNPlots->GetRep())->setCharWidth(5);
  layVals->addSpacing(taiM->hsep_c);
  
  layVals->addStretch();
  
  //    A series of checkboxes for show/hide axes labels
  layAxisLabelChks = new QHBoxLayout;
  layAxisLabelChks->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  layWidg->addLayout(layAxisLabelChks);
  
  lblAxisLabelChks =  taiM->NewLabel("Axes Labels:  ", widg, font_spec);
  layAxisLabelChks->addWidget(lblAxisLabelChks);
  lblAxisLabelChks->setToolTip(taiMisc::ToolTipPreProcess("Check to have axis label appear"));
  
  chkXAxisLabel = new QCheckBox("X    ", widg);
  chkXAxisLabel->setObjectName("chkXAxisLabel");
  layAxisLabelChks->addWidget(chkXAxisLabel);
  connect(chkXAxisLabel, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  chkYAxisLabel = new QCheckBox("Y    ", widg);
  chkYAxisLabel->setObjectName("chkYAxisLabel");
  layAxisLabelChks->addWidget(chkYAxisLabel);
  connect(chkYAxisLabel, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  chkAltYAxisLabel = new QCheckBox("Alt_Y    ", widg);
  chkAltYAxisLabel->setObjectName("chkAltYAxisLabel");
  layAxisLabelChks->addWidget(chkAltYAxisLabel);
  connect(chkAltYAxisLabel, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  chkZAxisLabel = new QCheckBox("Z              ", widg);
  chkZAxisLabel->setObjectName("chkZAxisLabel");
  layAxisLabelChks->addWidget(chkZAxisLabel);
  connect(chkZAxisLabel, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  chkLinesSolid = new QCheckBox("Solid Lines    ", widg);
  chkLinesSolid->setObjectName("chkLinesSolid");
  layAxisLabelChks->addWidget(chkLinesSolid);
  chkLinesSolid->setToolTip(taiMisc::ToolTipPreProcess("make all lines solid instead of dashed, dotted -- useful for cluster run results or other cases using grouped plot colors"));
  connect(chkLinesSolid, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  
  layAxisLabelChks->addSpacing(taiM->hsep_c);
  layAxisLabelChks->addStretch();
  //    Axes
  
  // -------- X Axis --------
  row_height = taiM->max_control_height(taiM->ctrl_size);
  layXAxis = new QHBoxLayout; layWidg->addLayout(layXAxis);
  layXAxis->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  int list_flags = taiWidget::flgNullOk | taiWidget::flgAutoApply | taiWidget::flgNoHelp;
  
  lblXAxis = taiM->NewLabel("X:", widg, font_spec);
  lblXAxis->setFixedWidth(axis_label_width);
  lblXAxis->setToolTip(taiMisc::ToolTipPreProcess("Column of data to plot for the X Axis"));
  layXAxis->addWidget(lblXAxis);
  String start_text = "";
  lelXAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags, start_text, axis_chooser_width));
  layXAxis->addWidget(lelXAxis->GetRep());
  
  rncXAxis = new QCheckBox("Row\nNum", widg); rncXAxis->setObjectName("rncXAxis");
  rncXAxis->setToolTip(taiMisc::ToolTipPreProcess("Use row number instead of column value for axis value"));
  connect(rncXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(rncXAxis);
  layXAxis->addSpacing(taiM->hsep_c);
  
  pdtXAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layXAxis->addWidget(pdtXAxis->GetRep());
  layXAxis->addSpacing(taiM->hsep_c);
  
  lblcellXAxis = taiM->NewLabel("Mtx\nCel", widg, font_spec);
  lblcellXAxis->setToolTip(taiMisc::ToolTipPreProcess("Matrix cell -- only for matrix columns -- choose which cell from matrix to display."));
  layXAxis->addWidget(lblcellXAxis);
  cellXAxis = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layXAxis->addWidget(cellXAxis->GetRep());

  labonXAxis = new iCheckBox("Lbls On", widg);
  labonXAxis->setToolTip(taiMisc::ToolTipPreProcess("Use a different column for the axis labels?"));
  labonXAxis->setFixedHeight(row_height);
  connect(labonXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(labonXAxis);
  layXAxis->addSpacing(taiM->hsep_c);
    
  labXAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  QWidget* ew = labXAxis->GetRep();
  ew->setFixedHeight(row_height);
  layXAxis->addWidget(ew);
  layXAxis->addSpacing(taiM->hsep_c);

  flipXAxis = new iCheckBox("Flip", widg);
  flipXAxis->setToolTip(taiMisc::ToolTipPreProcess("Flip (invert) the order of values plotted (high to low)"));
  flipXAxis->setFixedHeight(row_height);
  connect(flipXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(flipXAxis);

  butLinePropsXAxis = new QPushButton("", widg);
  butLinePropsXAxis->setIcon(QIcon(QPixmap(":/images/editedit.png")));
  butLinePropsXAxis->setToolTip(taiMisc::ToolTipPreProcess("Set axis color, tick marks, etc"));
  butLinePropsXAxis->setFixedHeight(row_height);
  butLinePropsXAxis->setFixedWidth(row_height);
  butLinePropsXAxis->setFlat(true);  // hide the border or the row height will be wrong and the alternating color background won't be right
  connect(butLinePropsXAxis, SIGNAL(pressed()), this, SLOT(butSetLineStyleXAxis()));
  layXAxis->addWidget(butLinePropsXAxis);
  
  layXAxis->addStretch();
  
  // -------- Z Axis --------
  layZAxis = new QHBoxLayout; layWidg->addLayout(layZAxis);
  layZAxis->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  lblZAxis = taiM->NewLabel("Z:", widg, font_spec);
  lblZAxis->setFixedWidth(axis_label_width);
  lblZAxis->setToolTip(taiMisc::ToolTipPreProcess("Column of data to plot for the Z Axis"));
  layZAxis->addWidget(lblZAxis);
  lelZAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags, start_text, axis_chooser_width));
  layZAxis->addWidget(lelZAxis->GetRep());
  layZAxis->addSpacing(taiM->hsep_c);
  
  oncZAxis = new iCheckBox("On", widg);
  oncZAxis->setToolTip(taiMisc::ToolTipPreProcess("Display a Z axis?"));
  connect(oncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(oncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);
  
  rncZAxis = new QCheckBox("Row\nNum", widg); rncZAxis->setObjectName("rncZAxis");
  rncZAxis->setToolTip(taiMisc::ToolTipPreProcess("Use row number instead of column value for axis value"));
  connect(rncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(rncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);
  
  pdtZAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layZAxis->addWidget(pdtZAxis->GetRep());
  layZAxis->addSpacing(taiM->hsep_c);
  
  lblcellZAxis = taiM->NewLabel("Mtx\nCel", widg, font_spec);
  lblcellZAxis->setToolTip(taiMisc::ToolTipPreProcess("Matrix cell -- only for matrix columns -- choose which cell from matrix to display."));
  layZAxis->addWidget(lblcellZAxis);
  cellZAxis = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layZAxis->addWidget(cellZAxis->GetRep());
  
  labonZAxis = new iCheckBox("Lbls On", widg);
  labonZAxis->setToolTip(taiMisc::ToolTipPreProcess("Use a different column for the axis labels?"));
  labonZAxis->setFixedHeight(row_height);
  connect(labonZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(labonZAxis);
  layZAxis->addSpacing(taiM->hsep_c);
    
  labZAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  ew = labZAxis->GetRep();
  ew->setFixedHeight(row_height);
  layZAxis->addWidget(ew);
  layZAxis->addSpacing(taiM->hsep_c);

  flipZAxis = new iCheckBox("Flip", widg);
  flipZAxis->setToolTip(taiMisc::ToolTipPreProcess("Flip (invert) the order of values plotted (high to low)"));
  flipZAxis->setFixedHeight(row_height);
  connect(flipZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(flipZAxis);
  // layZAxis->addSpacing(taiM->hsep_c);
    
  butLinePropsZAxis = new QPushButton("", widg);
  butLinePropsZAxis->setIcon(QIcon(QPixmap(":/images/editedit.png")));
  butLinePropsZAxis->setToolTip(taiMisc::ToolTipPreProcess("Set axis color, tick marks, etc"));
  butLinePropsZAxis->setFixedHeight(row_height);
  butLinePropsZAxis->setFixedWidth(row_height);
  butLinePropsZAxis->setFlat(true);  // hide the border or the row height will be wrong and the alternating color background won't be right
  connect(butLinePropsZAxis, SIGNAL(pressed()), this, SLOT(butSetLineStyleZAxis()));
  layZAxis->addWidget(butLinePropsZAxis);
  
  layZAxis->addStretch();
  
  plotsWidg = new iStripeWidget(widg);
  layWidg->addWidget(plotsWidg);
  cur_built_plots = 0;
  
  //    Colors
  
  layCAxis = new QHBoxLayout; layWidg->addLayout(layCAxis);
  layCAxis->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  lblColorMode = taiM->NewLabel("Color\nMode", widg, font_spec);
  lblColorMode->setToolTip(taiMisc::ToolTipPreProcess("How to determine line color:\n BY_VALUE makes the color change as a function of the\n Y axis value, according to the colorscale pallete\n FIXED uses fixed colors associated with each Y axis line\n (click on line/legend/axis and do View Properties in context menu to change)\n BY_VARIABLE uses a separate column of data to determine color value"));
  layCAxis->addWidget(lblColorMode);
  cmbColorMode = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("ColorMode"),
                                              this, NULL, widg, taiWidget::flgAutoApply));
  layCAxis->addWidget(cmbColorMode->GetRep());
  //  layColorScale->addSpacing(taiM->hsep_c);
  
  lblCAxis = taiM->NewLabel("Color\nAxis:", widg, font_spec);
  lblCAxis->setToolTip(taiMisc::ToolTipPreProcess("Column of data for BY_VARIABLE or BY_GROUP color mode"));
  layCAxis->addWidget(lblCAxis);
  lelCAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layCAxis->addWidget(lelCAxis->GetRep());
  
  lblThresh = taiM->NewLabel("Thresh", widg, font_spec);
  lblThresh->setToolTip(taiMisc::ToolTipPreProcess("Threshold for THRESH_LINE and THRESH_POINT styles -- only draw a line when value is over this threshold."));
  layCAxis->addWidget(lblThresh);
  fldThresh = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldThresh->GetRep());
  ((iLineEdit*)fldThresh->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);
  
  // Err Spacing
  lblErrSpacing = taiM->NewLabel("Err\nSpc", widg, font_spec);
  lblErrSpacing->setToolTip(taiMisc::ToolTipPreProcess("Spacing of error bars relative to data points."));
  layCAxis->addWidget(lblErrSpacing);
  fldErrSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldErrSpacing->GetRep());
  ((iLineEdit*)fldErrSpacing->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);
  
  lblLabelSz = taiM->NewLabel("Lbl\nSz", widg, font_spec);
  lblLabelSz->setToolTip(taiMisc::ToolTipPreProcess("Font size for the labels (strings) in the graph -- in normalized units."));
  layCAxis->addWidget(lblLabelSz);
  fldLabelSz = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldLabelSz->GetRep());
  ((iLineEdit*)fldLabelSz->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);
  
  lblPointSz = taiM->NewLabel("Pt\nSz", widg, font_spec);
  lblPointSz->setToolTip(taiMisc::ToolTipPreProcess("Size of point symbol markers, in normalized units."));
  layCAxis->addWidget(lblPointSz);
  fldPointSz = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldPointSz->GetRep());
  ((iLineEdit*)fldPointSz->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);
  
  lblAxisSz = taiM->NewLabel("Axis\nSz", widg, font_spec);
  lblAxisSz->setToolTip(taiMisc::ToolTipPreProcess("Font size for the axis labels in the graph -- in normalized units."));
  layCAxis->addWidget(lblAxisSz);
  fldAxisSz = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldAxisSz->GetRep());
  ((iLineEdit*)fldAxisSz->GetRep())->setCharWidth(6);
  
  lblXlblRot = taiM->NewLabel("X Lbl\nRot", widg, font_spec);
  lblXlblRot->setToolTip(taiMisc::ToolTipPreProcess("Rotation in degrees for the tick labels on the X axis -- rotating can allow longer string-based tick labels to be visible."));
  layCAxis->addWidget(lblXlblRot);
  fldXlblRot = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldXlblRot->GetRep());
  ((iLineEdit*)fldXlblRot->GetRep())->setCharWidth(6);
  //  layCAxis->addSpacing(taiM->hsep_c);
  
  layCAxis->addStretch();
  
  // Raster Axis
  layRAxis = new QHBoxLayout; layWidg->addLayout(layRAxis);
  layRAxis->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  lblRAxis = taiM->NewLabel("Raster:", widg, font_spec);
  lblRAxis->setToolTip(taiMisc::ToolTipPreProcess("Column of data for the Y axis in RASTER graphs"));
  layRAxis->addWidget(lblRAxis);
  lelRAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layRAxis->addWidget(lelRAxis->GetRep());
  layRAxis->addSpacing(taiM->hsep_c);
  
  pdtRAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layRAxis->addWidget(pdtRAxis->GetRep());
  layRAxis->addSpacing(taiM->hsep_c);
  
  lblBarSpace = taiM->NewLabel("Bar\nSpc", widg, font_spec);
  lblBarSpace->setToolTip(taiMisc::ToolTipPreProcess("Spacing between bars in the BAR plot."));
  layRAxis->addWidget(lblBarSpace);
  fldBarSpace = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layRAxis->addWidget(fldBarSpace->GetRep());
  ((iLineEdit*)fldBarSpace->GetRep())->setCharWidth(6);
  layRAxis->addSpacing(taiM->hsep_c);
  
  lblBarDepth = taiM->NewLabel("Bar\nDpth", widg, font_spec);
  lblBarDepth->setToolTip(taiMisc::ToolTipPreProcess("Depth in Z plane of bars in the BAR plot."));
  layRAxis->addWidget(lblBarDepth);
  fldBarDepth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layRAxis->addWidget(fldBarDepth->GetRep());
  ((iLineEdit*)fldBarDepth->GetRep())->setCharWidth(6);
  
  layRAxis->addStretch();
  
  // second row: color bar + button
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);
  layColorScale->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  
  cbar = new iHColorScaleBar(&tlv->colorscale, iColorScaleBar::RANGE, true, true, widg);
  //  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space
  
  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setToolTip(taiMisc::ToolTipPreProcess("Select color pallette for color value plotting (also determines background color)."));
  butSetColor->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layColorScale->addWidget(butSetColor);
  connect(butSetColor, SIGNAL(pressed()), this, SLOT(butSetColor_pressed()) );
  
  layWidg->addStretch();
  
  MakeButtons(layOuter);
}

iViewPanelOfGraphTable::~iViewPanelOfGraphTable() {
}

void iViewPanelOfGraphTable::InitPanel_impl() {
  // nothing structural here (could split out cols, but not worth it)
}

bool iViewPanelOfGraphTable::BuildPlots() {
  const int LAYBODY_MARGIN = 1;
  const int LAYBODY_SPACING = 0;
  int font_spec = taiMisc::fonMedium;
  int list_flags = taiWidget::flgNullOk | taiWidget::flgAutoApply | taiWidget::flgNoHelp;
  
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return false; // probably destructing
  
  int pltsz = MIN(max_plots, glv->plots.size);
  
  if(cur_built_plots == pltsz)
    return false;
  
  row_height = taiM->max_control_height(taiM->ctrl_size);
  
  plotsWidg->setHiLightColor(QColor(220, 220, 220));
  plotsWidg->setStripeHeight(row_height + (9 * LAYBODY_MARGIN));
  
  if(plotsWidg->layout()) {
    delete plotsWidg->layout();
    taiMisc::DeleteChildrenLater(plotsWidg);
  }
  
  layPlots = new iFormLayout(plotsWidg);
  layPlots->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
  layPlots->setLabelAlignment(Qt::AlignLeft);
  layPlots->setRowWrapPolicy(iFormLayout::DontWrapRows);
  layPlots->setHorizontalSpacing(2 * LAYBODY_MARGIN);
  layPlots->setVerticalSpacing(2 * LAYBODY_MARGIN);
  layPlots->setContentsMargins(margin_l_r, margin_t_b, margin_l_r, margin_t_b);
  layPlots->setFieldGrowthPolicy(iFormLayout::AllNonFixedFieldsGrow); // TBD
  
  // Y AXes
  // create a signal mapper to pass the plot number as a parameter to butSetLineStyle()
  QSignalMapper* sig_map_for_prop_buttons = new QSignalMapper(this);
  connect(sig_map_for_prop_buttons, SIGNAL(mapped(int)), this, SLOT(butSetLineStyle(int)));
  QSignalMapper* sig_map_for_chooser_buttons = new QSignalMapper(this);
  connect(sig_map_for_chooser_buttons, SIGNAL(mapped(int)), this, SLOT(ChooseVarPressed(int)));
  
  for(int i=0;i<pltsz; i++) {
    layYAxis[i] = new QHBoxLayout;
    layYAxis[i]->setMargin(0);
    layYAxis[i]->addStrut(row_height); // make it full height, so controls center
    
    String lbl = "Y" + String(i+1) + ":";
    String desc;
    const int ctrl_size = taiM->ctrl_size;
    lblYAxis[i] = taiEditorWidgetsMain::MakeInitEditLabel(lbl, this, ctrl_size,  desc, NULL,
                                                          this, SLOT(label_contextMenuInvoked(iLabel*, QContextMenuEvent*)), i);
    lblYAxis[i]->setFixedWidth(axis_label_width);
    lblYAxis[i]->setToolTip(taiMisc::ToolTipPreProcess("Column of data to plot (optional)"));
    lblYAxis[i]->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lblYAxis[i]);
    
    // fix the button width so all of the checkboxes and other control align
    String start_text = "";
    lelYAxis[i] = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags, start_text, axis_chooser_width));
    connect(lelYAxis[i]->rep(), SIGNAL(pressed()), sig_map_for_chooser_buttons, SLOT(map()));
    sig_map_for_chooser_buttons->setMapping(lelYAxis[i]->rep(), i);
    QWidget* lw = lelYAxis[i]->GetRep();
    lw->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lw);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    oncYAxis[i] = new iCheckBox("On", widg);
    oncYAxis[i]->setToolTip(taiMisc::ToolTipPreProcess("Display this column's data or not?"));
    oncYAxis[i]->setFixedHeight(row_height);
    connect(oncYAxis[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(oncYAxis[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    chkYAltY[i] =  new QCheckBox("Alt\nY", widg);
    chkYAltY[i]->setToolTip(taiMisc::ToolTipPreProcess("Whether to display values on an alternate Y axis for this column of data (otherwise it uses the main Y axis)"));
    chkYAltY[i]->setFixedHeight(row_height);
    connect(chkYAltY[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(chkYAltY[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    pdtYAxis[i] = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
    QWidget* pw = pdtYAxis[i]->GetRep();
    pw->setFixedHeight(row_height);
    layYAxis[i]->addWidget(pw);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    lblcellYAxis[i] = taiM->NewLabel("Mtx\nCel", widg, font_spec);
    lblcellYAxis[i]->setToolTip(taiMisc::ToolTipPreProcess("Matrix cell -- only for matrix columns -- choose which cell from matrix to display -- enter -1 to display all lines at once."));
    lblcellYAxis[i]->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lblcellYAxis[i]);
    cellYAxis[i] = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
    cellYAxis[i]->setMinimum(-1);
    QWidget* cw = cellYAxis[i]->GetRep();
    cw->setFixedHeight(row_height);
    layYAxis[i]->addWidget(cw);
    
    oncErr[i] = new iCheckBox("Err On", widg);
    oncErr[i]->setToolTip(taiMisc::ToolTipPreProcess("Display error bars for this column's data?"));
    oncErr[i]->setFixedHeight(row_height);
    connect(oncErr[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(oncErr[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    lelErr[i] = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
    QWidget* ew = lelErr[i]->GetRep();
    ew->setFixedHeight(row_height);
    layYAxis[i]->addWidget(ew);
    layYAxis[i]->addSpacing(taiM->hsep_c);
    
    flipYAxis[i] = new iCheckBox("Flip", widg);
    flipYAxis[i]->setToolTip(taiMisc::ToolTipPreProcess("Flip (invert) the order of values plotted (high to low)"));
    flipYAxis[i]->setFixedHeight(row_height);
    connect(flipYAxis[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(flipYAxis[i]);

    butLineProps[i] = new QPushButton("", widg);
    butLineProps[i]->setIcon( QIcon( QPixmap(":/images/editedit.png") ) );
    butLineProps[i]->setToolTip(taiMisc::ToolTipPreProcess("Set color, line style, etc"));
    butLineProps[i]->setFixedHeight(row_height);
    butLineProps[i]->setFixedWidth(row_height);
    butLineProps[i]->setFlat(true);  // hide the border or the row height will be wrong and the alternating color background won't be right
    connect(butLineProps[i], SIGNAL(pressed()), sig_map_for_prop_buttons, SLOT(map()));
    sig_map_for_prop_buttons->setMapping(butLineProps[i], i);
    layYAxis[i]->addWidget(butLineProps[i]);
    
    layPlots->addRow(layYAxis[i]);
  }
  
  cur_built_plots = pltsz;
  return true;
}

void iViewPanelOfGraphTable::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();
  
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return; // probably destructing
  
  DataTable* dt = glv->dataTable();
  if(!dt) return;

  int dt_cells = dt->Cells();
  dt_cells_last_updt = dt_cells; // used in get values to determine if it is current
  
  BuildPlots();
  
  chkDisplay->setChecked(glv->display_on);
  chkManip->setChecked(glv->manip_ctrl_on);
  cmbGraphType->GetImage(glv->graph_type);
  cmbPlotStyle->GetImage(glv->plot_style);
  fldRows->GetImage((String)glv->view_rows);
  
  fldLineWidth->GetImage((String)glv->line_width);
  fldPointSpacing->GetImage((String)glv->point_spacing);
  fldLabelSpacing->GetImage((String)glv->label_spacing);
  chkNegDraw->setChecked(glv->negative_draw);
  chkNegDrawZ->setChecked(glv->negative_draw_z);
  fldWidth->GetImage((String)glv->width);
  fldDepth->GetImage((String)glv->depth);
  fldNPlots->GetImage((String)glv->tot_plots);
  chkLinesSolid->setChecked(glv->solid_lines);
  
  lelXAxis->GetImage(&(glv->children), glv->x_axis.GetColPtr());
  rncXAxis->setChecked(glv->x_axis.row_num);
  pdtXAxis->GetImage_(&(glv->x_axis.fixed_range));
  cellXAxis->GetImage((String)glv->x_axis.matrix_cell);
  chkXAxisLabel->setChecked(glv->x_axis.show_axis_label);
  labXAxis->GetImage(&(glv->children), glv->x_axis.GetLabelsColPtr());
  labonXAxis->setReadOnly(glv->x_axis.GetLabelsColPtr() == NULL);
  labonXAxis->setChecked(glv->x_axis.labels_on);
  flipXAxis->setChecked(glv->x_axis.flip);
  
  lelZAxis->GetImage(&(glv->children), glv->z_axis.GetColPtr());
  oncZAxis->setReadOnly(glv->z_axis.GetColPtr() == NULL);
  oncZAxis->setChecked(glv->z_axis.on);
  rncZAxis->setChecked(glv->z_axis.row_num);
  pdtZAxis->GetImage_(&(glv->z_axis.fixed_range));
  cellZAxis->GetImage((String)glv->z_axis.matrix_cell);
  chkZAxisLabel->setChecked(glv->z_axis.show_axis_label);
  labZAxis->GetImage(&(glv->children), glv->z_axis.GetLabelsColPtr());
  labonZAxis->setReadOnly(glv->z_axis.GetLabelsColPtr() == NULL);
  labonZAxis->setChecked(glv->z_axis.labels_on);
  flipZAxis->setChecked(glv->z_axis.flip);
  
  lelZAxis->SetFlag(taiWidget::flgReadOnly, !glv->z_axis.on);
  rncZAxis->setAttribute(Qt::WA_Disabled, !glv->z_axis.on);
  pdtZAxis->SetFlag(taiWidget::flgReadOnly, !glv->z_axis.on);
  
  int pltsz = MIN(max_plots, glv->plots.size);
  
  for(int i=0;i<pltsz; i++) {
    lelYAxis[i]->GetImage(&(glv->children), glv->plots[i]->GetColPtr());
    oncYAxis[i]->setReadOnly(glv->plots[i]->GetColPtr() == NULL);
    oncYAxis[i]->setChecked(glv->plots[i]->on);
    pdtYAxis[i]->GetImage_(&(glv->plots[i]->fixed_range));
    lelYAxis[i]->SetFlag(taiWidget::flgReadOnly, !glv->plots[i]->on);
    pdtYAxis[i]->SetFlag(taiWidget::flgReadOnly, !glv->plots[i]->on);
    chkYAltY[i]->setChecked(glv->plots[i]->alt_y);
    cellYAxis[i]->GetImage((String)glv->plots[i]->matrix_cell);
    
    lelErr[i]->GetImage(&(glv->children), glv->errbars[i]->GetColPtr());
    oncErr[i]->setReadOnly(glv->errbars[i]->GetColPtr() == NULL);
    oncErr[i]->setChecked(glv->errbars[i]->on);
    
    flipYAxis[i]->setChecked(glv->plots[i]->flip);
    
    // set each but all set by single checkbox
    chkYAxisLabel->setChecked(glv->plots[i]->show_axis_label);
    chkAltYAxisLabel->setChecked(glv->plots[i]->show_alt_axis_label);
  }
  
  fldErrSpacing->GetImage((String)glv->err_spacing);
  fldLabelSz->GetImage((String)glv->label_font_size);
  fldPointSz->GetImage((String)glv->point_size);
  fldAxisSz->GetImage((String)glv->axis_font_size);
  fldXlblRot->GetImage((String)glv->x_axis_label_rot);
  fldBarSpace->GetImage((String)glv->bar_space);
  fldBarDepth->GetImage((String)glv->bar_depth);
  
  cmbColorMode->GetImage(glv->color_mode);
  lelCAxis->GetImage(&(glv->children), glv->color_axis.GetColPtr());
  lelCAxis->SetFlag(taiWidget::flgReadOnly, glv->color_mode != GraphTableView::BY_VARIABLE);
  
  fldThresh->GetImage((String)glv->thresh);
  
  cbar->UpdateScaleValues();
  
  lelRAxis->GetImage(&(glv->children), glv->raster_axis.GetColPtr());
  pdtRAxis->GetImage_(&(glv->raster_axis.fixed_range));
  
  lelRAxis->SetFlag(taiWidget::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
  pdtRAxis->SetFlag(taiWidget::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
}

static void GetFixedMinMaxVal(taiWidgetPoly* widg, FixedMinMax& fmm, bool panel_current,
                              FixedMinMax& cur_mm) {
  // implements the auto-check function for fixed min max
  FixedMinMax fmm_orig; fmm_orig = fmm;
  widg->GetValue_(&fmm);
  if(!panel_current) return;
  
  if(!fmm_orig.fix_min) {     // not originally fixed
    if((String)fmm.min != (String)fmm_orig.min) {
      fmm.fix_min = true;
      if(!cur_mm.fix_min) {
        cur_mm.fix_min = true;
        cur_mm.min = fmm.min;
      }
      // else {
      //   cur_mm.min = fminf(cur_mm.min, mm.min); // get the effective min
      //   // actually, graph doesn't do this.. so we don't
      // }
    }
    else if(fmm.fix_min) {    // we just clicked on fix
      if(cur_mm.fix_min) {    // there is an already-established primary min/max in effect
        fmm.min = cur_mm.min; // use it
      }
    }
  }
  else {
    if(!cur_mm.fix_min) {     // we're it
      cur_mm.fix_min = true;
      cur_mm.min = fmm.min;
    }
  }

  if(!fmm_orig.fix_max) {     // not originally fixed
    if((String)fmm.max != (String)fmm_orig.max) {
      fmm.fix_max = true;
      if(!cur_mm.fix_max) {
        cur_mm.fix_max = true;
        cur_mm.max = fmm.max;
      }
      // else {
      //   cur_mm.max = fmaxf(cur_mm.max, mm.max); // get the effective max
      //   // actually, graph doesn't do this.. so we don't
      // }
    }
    else if(fmm.fix_max) {    // we just clicked on fix
      if(cur_mm.fix_max) {    // there is an already-established primary max/max in effect
        fmm.max = cur_mm.max; // use it
      }
    }
  }
  else {
    if(!cur_mm.fix_max) {     // we're it
      cur_mm.fix_max = true;
      cur_mm.max = fmm.max;
    }
  }
}


void iViewPanelOfGraphTable::GetValue_impl() {
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return;

  DataTable* dt = glv->dataTable();
  if(!dt) return;

  int dt_cells = dt->Cells();
  bool panel_current = (dt_cells == dt_cells_last_updt);
  
  glv->display_on = chkDisplay->isChecked();
  glv->manip_ctrl_on = chkManip->isChecked();
  int i = 0;
  cmbGraphType->GetEnumValue(i); glv->graph_type = (GraphTableView::GraphType)i;
  cmbPlotStyle->GetEnumValue(i); glv->plot_style = (GraphTableView::PlotStyle)i;
  glv->view_rows = (int)fldRows->GetValue();
  glv->line_width = (float)fldLineWidth->GetValue();
  glv->point_spacing = (int)fldPointSpacing->GetValue();
  glv->point_spacing = MAX(1, glv->point_spacing);
  glv->label_spacing = (int)fldLabelSpacing->GetValue();
  glv->negative_draw = chkNegDraw->isChecked();
  glv->negative_draw_z = chkNegDrawZ->isChecked();
  glv->width = (float)fldWidth->GetValue();
  glv->depth = (float)fldDepth->GetValue();
  glv->tot_plots = (float)fldNPlots->GetValue();
  glv->solid_lines = chkLinesSolid->isChecked();
  
  glv->setScaleData(false, cbar->min(), cbar->max());

  FixedMinMax cur_x_mm;
  
  glv->x_axis.row_num = rncXAxis->isChecked();
  GetFixedMinMaxVal(pdtXAxis, glv->x_axis.fixed_range, panel_current, cur_x_mm);
  glv->x_axis.SetColPtr((GraphColView*)lelXAxis->GetValue());
  glv->x_axis.matrix_cell = (int)cellXAxis->GetValue();
  glv->x_axis.show_axis_label = chkXAxisLabel->isChecked();
  glv->x_axis.SetLabelsColPtr((GraphColView*)labXAxis->GetValue());
  glv->x_axis.labels_on = labonXAxis->isChecked();
  glv->x_axis.flip = flipXAxis->isChecked();

  FixedMinMax cur_z_mm;
  // if setting a col for 1st time, we automatically turn on (since it would be ro)
  GraphColView* tcol = (GraphColView*)lelZAxis->GetValue();
  if (tcol && !glv->z_axis.GetColPtr())
    oncZAxis->setChecked(true);
  glv->z_axis.on = oncZAxis->isChecked();
  GetFixedMinMaxVal(pdtZAxis, glv->z_axis.fixed_range, panel_current, cur_z_mm);
  glv->z_axis.row_num = rncZAxis->isChecked();
  glv->z_axis.SetColPtr(tcol);
  glv->z_axis.matrix_cell = (int)cellZAxis->GetValue();
  glv->z_axis.show_axis_label = chkZAxisLabel->isChecked();
  glv->z_axis.SetLabelsColPtr((GraphColView*)labZAxis->GetValue());
  glv->z_axis.labels_on = labonZAxis->isChecked();
  glv->z_axis.flip = flipZAxis->isChecked();
  
  int pltsz = MIN(max_plots, glv->plots.size);

  FixedMinMax cur_reg_mm;       // track first fixed min / max and use for new clicks
  FixedMinMax cur_alt_mm;
  
  for(int pi=0; pi<pltsz; pi++) {
    tcol = (GraphColView*)lelYAxis[pi]->GetValue();
    if (tcol && !glv->plots[pi]->GetColPtr())
      oncYAxis[pi]->setChecked(true);
    glv->plots[pi]->on = oncYAxis[pi]->isChecked();
    glv->plots[pi]->alt_y = chkYAltY[pi]->isChecked();

    if(glv->plots[pi]->alt_y) {
      GetFixedMinMaxVal(pdtYAxis[pi], glv->plots[pi]->fixed_range, panel_current,
                        cur_alt_mm);
    }
    else {
      GetFixedMinMaxVal(pdtYAxis[pi], glv->plots[pi]->fixed_range, panel_current,
                        cur_reg_mm);
    }
    
    glv->plots[pi]->SetColPtr(tcol);
    glv->plots[pi]->matrix_cell = (int)cellYAxis[pi]->GetValue();
    
    tcol = (GraphColView*)lelErr[pi]->GetValue();
    if (tcol && !glv->errbars[pi]->GetColPtr())
      oncErr[pi]->setChecked(true);
    glv->errbars[pi]->on = oncErr[pi]->isChecked();
    glv->errbars[pi]->SetColPtr(tcol);
    glv->plots[pi]->flip = flipYAxis[pi]->isChecked();
    
    // set each but all set by single checkbox
    glv->plots[pi]->show_axis_label = chkYAxisLabel->isChecked();
    glv->plots[pi]->show_alt_axis_label = chkAltYAxisLabel->isChecked();
  }
  
  glv->err_spacing = (int)fldErrSpacing->GetValue();
  glv->label_font_size = (float)fldLabelSz->GetValue();
  glv->point_size = (float)fldPointSz->GetValue();
  glv->axis_font_size = (float)fldAxisSz->GetValue();
  glv->x_axis_label_rot = (float)fldXlblRot->GetValue();
  glv->bar_space = (float)fldBarSpace->GetValue();
  glv->bar_depth = (float)fldBarDepth->GetValue();
  
  cmbColorMode->GetEnumValue(i); glv->color_mode = (GraphTableView::ColorMode)i;
  glv->color_axis.SetColPtr((GraphColView*)lelCAxis->GetValue());
  
  glv->thresh = (float)fldThresh->GetValue();
  
  glv->setScaleData(false, cbar->min(), cbar->max());
  
  glv->raster_axis.SetColPtr((GraphColView*)lelRAxis->GetValue());
  pdtRAxis->GetValue_(&(glv->raster_axis.fixed_range));
  
  glv->UpdateAfterEdit(); // so many guys require this, we just always do it
  glv->UpdateDisplay(false); // don't update us, because logic will do that anyway
}

void iViewPanelOfGraphTable::CopyFrom_impl() {
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return;
  glv->CallFun("CopyFromView");
}


void iViewPanelOfGraphTable::butRefresh_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  
  glv->InitDisplay();
  glv->UpdateDisplay();
}

void iViewPanelOfGraphTable::butClear_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  
  glv->ClearData();
}

void iViewPanelOfGraphTable::butSetColor_pressed() {
  GraphTableView* glv = this->glv(); //cache
  if (updating || !glv) return;
  
  glv->CallFun("SetColorSpec");
}

void iViewPanelOfGraphTable::butSetLineStyle(int plot_num) {
  if (glv()) {
    glv()->plots[plot_num]->OpenInWindow();
  }
}

void iViewPanelOfGraphTable::butSetLineStyleXAxis() {
  if (glv()) {
    glv()->x_axis.OpenInWindow();
  }
}

void iViewPanelOfGraphTable::butSetLineStyleZAxis() {
  if (glv()) {
    glv()->z_axis.OpenInWindow();
  }
}

void iViewPanelOfGraphTable::label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  Q_CHECK_PTR(menu);
  
  iAction* act = NULL;
  act = menu->AddItem("Insert Before", taiWidgetMenu::normal, iAction::int_act, this, SLOT(InsertPlotBefore(int)), sender->index());
  act = menu->AddItem("Insert After", taiWidgetMenu::normal, iAction::int_act, this, SLOT(InsertPlotAfter(int)), sender->index());
  act = menu->AddItem("Delete", taiWidgetMenu::normal, iAction::int_act, this, SLOT(DeletePlot(int)), sender->index());
  act = menu->AddItem("Move Before", taiWidgetMenu::normal, iAction::int_act, this, SLOT(MovePlotBefore(int)), sender->index());
  menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

// NOTE - the Y plot labels start with 1 but the list index starts at 0 of course
void iViewPanelOfGraphTable::InsertPlotBefore(int plot_index) {
  GraphTableView* glv = this->glv();
  glv->AddPlot(1);
  glv->plots.MoveBeforeIdx(glv->tot_plots-1, plot_index);
  glv->UpdateAfterEdit();
  glv->UpdateDisplay();
}

void iViewPanelOfGraphTable::InsertPlotAfter(int plot_index) {
  GraphTableView* glv = this->glv();
  glv->AddPlot(1);
  glv->plots.MoveBeforeIdx(glv->tot_plots-1, plot_index+1);
  glv->UpdateAfterEdit();
  glv->UpdateDisplay();
}

void iViewPanelOfGraphTable::DeletePlot(int plot_index) {
  GraphTableView* glv = this->glv();
  glv->DeletePlot(plot_index);
  glv->UpdateAfterEdit();
  glv->UpdateDisplay();
}

void iViewPanelOfGraphTable::MovePlotBefore(int old_index) {
  int new_index = 1;
  
  taGuiDialog dlg;
  dlg.win_title = "Move Plot";
  String prompt = "Move Y" + String(old_index+1) +  " before Y?";
  dlg.prompt = prompt;
  dlg.width = taiMisc::resizeByMainFont(200);
  dlg.height = taiMisc::resizeByMainFont(150);
  
  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);
  
  String tt;
  String row = "one_row";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddIntField(&new_index, "", widget, row, tt);
  
  int drval = dlg.PostDialog(true);
  if(drval != 0) {
    GraphTableView* glv = this->glv();
    glv->plots.MoveBeforeIdx(old_index, new_index-1);  // user visible numbers start at 1 not zero
    glv->UpdateAfterEdit();
    glv->UpdateDisplay();
  }
}

void* iViewPanelOfGraphTable::GetAlternateSelection() {
  if (!glv()){
    return NULL;
  }
  // has the list of plots been created?
  if (last_plot_index < 0 || last_plot_index > glv()->plots.size) {
    return NULL;
  }
  
  // if the plot var was NULL when pressed check the plot choice above and possibly the one below
  //  to use as alternate starting variable for initializing the chooser
  int good_index = last_plot_index;
  if (glv()->plots[last_plot_index]->GetColPtr() == NULL) {
    if (last_plot_index > 0) {
      if (glv()->plots[last_plot_index - 1]->GetColPtr() != NULL) {
        good_index -= 1;
      }
    }
    if ((good_index == last_plot_index) && (last_plot_index < glv()->plots.size - 1)) {
      if (glv()->plots[last_plot_index + 1]->GetColPtr() != NULL) {
        good_index = last_plot_index + 1;
      }
    }
  }
  return glv()->plots[good_index]->GetColPtr();
}

void iViewPanelOfGraphTable::ChooseVarPressed(int index) {
  last_plot_index = index;
}

