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
#include <GraphTableView>
#include <GraphColView>
#include <iHColorScaleBar>
#include <taiWidgetComboBox>
#include <taiWidgetFieldIncr>
#include <taiWidgetField>
#include <taiWidgetListElChooser>
#include <taiWidgetPoly>
#include <iCheckBox>
#include <BuiltinTypeDefs>
#include <iLineEdit>
#include <iStripeWidget>
#include <iFormLayout>

#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>

String iViewPanelOfGraphTable::panel_type() const {
  static String str("Graph Log");
  return str;
}

iViewPanelOfGraphTable::iViewPanelOfGraphTable(GraphTableView* tlv)
:inherited(tlv)
{
  int font_spec = taiMisc::fonMedium;

  layTopCtrls = new QHBoxLayout; layWidg->addLayout(layTopCtrls);

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

  // todo: fix tool tips on all of these..

  lblGraphType = taiM->NewLabel("Graph", widg, font_spec);
  lblGraphType->setToolTip("How to display the graph");
  layTopCtrls->addWidget(lblGraphType);
  cmbGraphType = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("GraphType"),
                                 this, NULL, widg, taiWidget::flgAutoApply));
  layTopCtrls->addWidget(cmbGraphType->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);

  lblPlotStyle = taiM->NewLabel("Style", widg, font_spec);
  lblPlotStyle->setToolTip("How to plot the lines");
  layTopCtrls->addWidget(lblPlotStyle);
  cmbPlotStyle = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("PlotStyle"),
                                 this, NULL, widg, taiWidget::flgAutoApply));
  layTopCtrls->addWidget(cmbPlotStyle->GetRep());
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkNegDraw =  new QCheckBox("Neg\nDraw", widg); chkNegDraw->setObjectName("chkNegDraw");
  chkNegDraw->setToolTip("Whether to draw a line when going in a negative direction (to the left), which may indicate a wrap-around to a new iteration of data");
  connect(chkNegDraw, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDraw);
  layTopCtrls->addSpacing(taiM->hsep_c);

  chkNegDrawZ =  new QCheckBox("Neg\nDraw Z", widg); chkNegDrawZ->setObjectName("chkNegDrawZ");
  chkNegDrawZ->setToolTip("Whether to draw a line when going in a negative direction of Z (to the front), which may indicate a wrap-around to a new channel of data");
  connect(chkNegDrawZ, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layTopCtrls->addWidget(chkNegDrawZ);
  layTopCtrls->addSpacing(taiM->hsep_c);

  layTopCtrls->addStretch();
  butRefresh = new QPushButton("Refresh", widg);
  butRefresh->setFixedHeight(taiM->button_height(taiMisc::sizSmall));
  layTopCtrls->addWidget(butRefresh);
  connect(butRefresh, SIGNAL(pressed()), this, SLOT(butRefresh_pressed()) );

  layVals = new QHBoxLayout; layWidg->addLayout(layVals);

  lblRows = taiM->NewLabel("View\nRows", widg, font_spec);
  lblRows->setToolTip("Maximum number of rows to display (row height is scaled to fit).");
  layVals->addWidget(lblRows);
  fldRows = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layVals->addWidget(fldRows->GetRep());
  layVals->addSpacing(taiM->hsep_c);

  lblLineWidth = taiM->NewLabel("Line\nWidth", widg, font_spec);
  lblLineWidth->setToolTip("Width to draw lines with.");
  layVals->addWidget(lblLineWidth);
  fldLineWidth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLineWidth->GetRep());
  ((iLineEdit*)fldLineWidth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);

  lblPointSpacing = taiM->NewLabel("Pt\nSpc", widg, font_spec);
  lblPointSpacing->setToolTip("Spacing of points drawn relative to underlying data points.");
  layVals->addWidget(lblPointSpacing);
  fldPointSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldPointSpacing->GetRep());
  ((iLineEdit*)fldPointSpacing->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);

  lblLabelSpacing = taiM->NewLabel("Lbl\nSpc", widg, font_spec);
  lblLabelSpacing->setToolTip("Spacing of text labels of data point values. -1 means no text labels.");
  layVals->addWidget(lblLabelSpacing);
  fldLabelSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldLabelSpacing->GetRep());
  ((iLineEdit*)fldLabelSpacing->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);

  lblWidth = taiM->NewLabel("Width", widg, font_spec);
  lblWidth->setToolTip("Width of graph display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblWidth);
  fldWidth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldWidth->GetRep());
  ((iLineEdit*)fldWidth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);

  lblDepth = taiM->NewLabel("Depth", widg, font_spec);
  lblDepth->setToolTip("Depth of graph display, in normalized units (default is 1.0 = same as height).");
  layVals->addWidget(lblDepth);
  fldDepth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layVals->addWidget(fldDepth->GetRep());
  ((iLineEdit*)fldDepth->GetRep())->setCharWidth(6);
  layVals->addSpacing(taiM->hsep_c);

  lblNPlots = taiM->NewLabel("N Plots", widg, font_spec);
  lblNPlots->setToolTip("Number of different plots available to display in this graph -- can increase to up to 64 -- just takes more room in the control panel");
  layVals->addWidget(lblNPlots);
  fldNPlots = dl.Add(new taiWidgetField(&TA_int, this, NULL, widg));
  layVals->addWidget(fldNPlots->GetRep());
  ((iLineEdit*)fldNPlots->GetRep())->setCharWidth(5);
  layVals->addSpacing(taiM->hsep_c);

  layVals->addStretch();

  //    Axes

  // X AXis
  layXAxis = new QHBoxLayout; layWidg->addLayout(layXAxis);

  int list_flags = taiWidget::flgNullOk | taiWidget::flgAutoApply | taiWidget::flgNoHelp;

  lblXAxis = taiM->NewLabel("X:", widg, font_spec);
  lblXAxis->setToolTip("Column of data to plot for the X Axis");
  layXAxis->addWidget(lblXAxis);
  lelXAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layXAxis->addWidget(lelXAxis->GetRep());

  rncXAxis = new QCheckBox("Row\nNum", widg); rncXAxis->setObjectName("rncXAxis");
  rncXAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncXAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layXAxis->addWidget(rncXAxis);
  layXAxis->addSpacing(taiM->hsep_c);

  pdtXAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layXAxis->addWidget(pdtXAxis->GetRep());
  layXAxis->addSpacing(taiM->hsep_c);

  lblcellXAxis = taiM->NewLabel("Mtx\nCel", widg, font_spec);
  lblcellXAxis->setToolTip("Matrix cell -- only for matrix columns -- choose which cell from matrix to display.");
  layXAxis->addWidget(lblcellXAxis);
  cellXAxis = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layXAxis->addWidget(cellXAxis->GetRep());

  layXAxis->addStretch();

  // Z AXis
  layZAxis = new QHBoxLayout; layWidg->addLayout(layZAxis);

  lblZAxis = taiM->NewLabel("Z:", widg, font_spec);
  lblZAxis->setToolTip("Column of data to plot for the Z Axis");
  layZAxis->addWidget(lblZAxis);
  lelZAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layZAxis->addWidget(lelZAxis->GetRep());
  layZAxis->addSpacing(taiM->hsep_c);

  oncZAxis = new iCheckBox("On", widg);
  oncZAxis->setToolTip("Display a Z axis?");
  connect(oncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(oncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);

  rncZAxis = new QCheckBox("Row\nNum", widg); rncZAxis->setObjectName("rncZAxis");
  rncZAxis->setToolTip("Use row number instead of column value for axis value");
  connect(rncZAxis, SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
  layZAxis->addWidget(rncZAxis);
  layZAxis->addSpacing(taiM->hsep_c);

  pdtZAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layZAxis->addWidget(pdtZAxis->GetRep());
  layZAxis->addSpacing(taiM->hsep_c);

  lblcellZAxis = taiM->NewLabel("Mtx\nCel", widg, font_spec);
  lblcellZAxis->setToolTip("Matrix cell -- only for matrix columns -- choose which cell from matrix to display.");
  layZAxis->addWidget(lblcellZAxis);
  cellZAxis = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
  layZAxis->addWidget(cellZAxis->GetRep());

  layZAxis->addStretch();

  plotsWidg = new iStripeWidget(widg);
  layWidg->addWidget(plotsWidg);
  cur_built_plots = 0;

  //    Colors

  layCAxis = new QHBoxLayout; layWidg->addLayout(layCAxis);
  lblColorMode = taiM->NewLabel("Color\nMode", widg, font_spec);
  lblColorMode->setToolTip("How to determine line color:\n VALUE_COLOR makes the color change as a function of the\n Y axis value, according to the colorscale pallete\n FIXED_COLOR uses fixed colors associated with each Y axis line\n (click on line/legend/axis and do View Properties in context menu to change)\n COLOR_AXIS uses a separate column of data to determine color value");
  layCAxis->addWidget(lblColorMode);
  cmbColorMode = dl.Add(new taiWidgetComboBox(true, TA_GraphTableView.sub_types.FindName("ColorMode"),
        this, NULL, widg, taiWidget::flgAutoApply));
  layCAxis->addWidget(cmbColorMode->GetRep());
  //  layColorScale->addSpacing(taiM->hsep_c);

  lblCAxis = taiM->NewLabel("Color\nAxis:", widg, font_spec);
  lblCAxis->setToolTip("Column of data for COLOR_AXIS color mode");
  layCAxis->addWidget(lblCAxis);
  lelCAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layCAxis->addWidget(lelCAxis->GetRep());

  lblThresh = taiM->NewLabel("Thresh", widg, font_spec);
  lblThresh->setToolTip("Threshold for THRESH_LINE and THRESH_POINT styles -- only draw a line when value is over this threshold.");
  layCAxis->addWidget(lblThresh);
  fldThresh = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldThresh->GetRep());
  ((iLineEdit*)fldThresh->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);

  // Err Spacing
  lblErrSpacing = taiM->NewLabel("Err\nSpc", widg, font_spec);
  lblErrSpacing->setToolTip("Spacing of error bars relative to data points.");
  layCAxis->addWidget(lblErrSpacing);
  fldErrSpacing = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldErrSpacing->GetRep());
  ((iLineEdit*)fldErrSpacing->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);

  lblLabelSz = taiM->NewLabel("Lbl\nSz", widg, font_spec);
  lblLabelSz->setToolTip("Font size for the labels (strings) in the graph -- in normalized units.");
  layCAxis->addWidget(lblLabelSz);
  fldLabelSz = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldLabelSz->GetRep());
  ((iLineEdit*)fldLabelSz->GetRep())->setCharWidth(6);
  layCAxis->addSpacing(taiM->hsep_c);

  lblAxisSz = taiM->NewLabel("Axis\nSz", widg, font_spec);
  lblAxisSz->setToolTip("Font size for the axis labels in the graph -- in normalized units.");
  layCAxis->addWidget(lblAxisSz);
  fldAxisSz = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layCAxis->addWidget(fldAxisSz->GetRep());
  ((iLineEdit*)fldAxisSz->GetRep())->setCharWidth(6);
  //  layCAxis->addSpacing(taiM->hsep_c);

  layCAxis->addStretch();

  // Raster Axis
  layRAxis = new QHBoxLayout; layWidg->addLayout(layRAxis);

  lblRAxis = taiM->NewLabel("Raster:", widg, font_spec);
  lblRAxis->setToolTip("Column of data for the Y axis in RASTER graphs");
  layRAxis->addWidget(lblRAxis);
  lelRAxis = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg, list_flags));
  layRAxis->addWidget(lelRAxis->GetRep());
  layRAxis->addSpacing(taiM->hsep_c);

  pdtRAxis = dl.Add(taiWidgetPoly::New(true, &TA_FixedMinMax, this, NULL, widg));
  layRAxis->addWidget(pdtRAxis->GetRep());
  layRAxis->addSpacing(taiM->hsep_c);

  lblBarSpace = taiM->NewLabel("Bar\nSpc", widg, font_spec);
  lblBarSpace->setToolTip("Spacing between bars in the BAR plot.");
  layRAxis->addWidget(lblBarSpace);
  fldBarSpace = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layRAxis->addWidget(fldBarSpace->GetRep());
  ((iLineEdit*)fldBarSpace->GetRep())->setCharWidth(6);
  layRAxis->addSpacing(taiM->hsep_c);

  lblBarDepth = taiM->NewLabel("Bar\nDpth", widg, font_spec);
  lblBarDepth->setToolTip("Depth in Z plane of bars in the BAR plot.");
  layRAxis->addWidget(lblBarDepth);
  fldBarDepth = dl.Add(new taiWidgetField(&TA_float, this, NULL, widg));
  layRAxis->addWidget(fldBarDepth->GetRep());
  ((iLineEdit*)fldBarDepth->GetRep())->setCharWidth(6);

  layRAxis->addStretch();

  // second row: color bar + button
  layColorScale = new QHBoxLayout; layWidg->addLayout(layColorScale);
  cbar = new iHColorScaleBar(&tlv->colorscale, iColorScaleBar::RANGE, true, true, widg);
//  cbar->setMaximumWidth(30);
  connect(cbar, SIGNAL(scaleValueChanged()), this, SLOT(Changed()) );
  layColorScale->addWidget(cbar); // stretchfact=1 so it stretches to fill the space

  butSetColor = new QPushButton("Colors", widg);
  butSetColor->setToolTip("Select color pallette for color value plotting (also determines background color).");
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
  layPlots->setContentsMargins(LAYBODY_MARGIN, 0, LAYBODY_MARGIN, 0);
  layPlots->setFieldGrowthPolicy(iFormLayout::AllNonFixedFieldsGrow); // TBD

  // Y AXes
  for(int i=0;i<pltsz; i++) {
    layYAxis[i] = new QHBoxLayout;
    layYAxis[i]->setMargin(0);
    layYAxis[i]->addStrut(row_height); // make it full height, so controls center

    String lbl = "Y" + String(i+1) + ":";
    lblYAxis[i] = taiM->NewLabel(lbl, widg, font_spec);
    lblYAxis[i]->setToolTip("Column of data to plot (optional)");
    lblYAxis[i]->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lblYAxis[i]);
    lelYAxis[i] = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg,
                                                    list_flags));
    QWidget* lw = lelYAxis[i]->GetRep();
    lw->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lw);
    layYAxis[i]->addSpacing(taiM->hsep_c);

    oncYAxis[i] = new iCheckBox("On", widg);
    oncYAxis[i]->setToolTip("Display this column's data or not?");
    oncYAxis[i]->setFixedHeight(row_height);
    connect(oncYAxis[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(oncYAxis[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);

    chkYAltY[i] =  new QCheckBox("Alt\nY", widg); 
    chkYAltY[i]->setToolTip("Whether to display values on an alternate Y axis for this column of data (otherwise it uses the main Y axis)");
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
    lblcellYAxis[i]->setToolTip("Matrix cell -- only for matrix columns -- choose which cell from matrix to display -- enter -1 to display all lines at once.");
    lblcellYAxis[i]->setFixedHeight(row_height);
    layYAxis[i]->addWidget(lblcellYAxis[i]);
    cellYAxis[i] = dl.Add(new taiWidgetFieldIncr(&TA_int, this, NULL, widg));
    cellYAxis[i]->setMinimum(-1);
    QWidget* cw = cellYAxis[i]->GetRep();
    cw->setFixedHeight(row_height);
    layYAxis[i]->addWidget(cw);

    oncErr[i] = new iCheckBox("Err On", widg);
    oncErr[i]->setToolTip("Display error bars for this column's data?");
    oncErr[i]->setFixedHeight(row_height);
    connect(oncErr[i], SIGNAL(clicked(bool)), this, SLOT(Apply_Async()) );
    layYAxis[i]->addWidget(oncErr[i]);
    layYAxis[i]->addSpacing(taiM->hsep_c);

    lelErr[i] = dl.Add(new taiWidgetListElChooser(&TA_T3DataView_List, this, NULL, widg,
                                                  list_flags));
    QWidget* ew = lelErr[i]->GetRep();
    ew->setFixedHeight(row_height);
    layYAxis[i]->addWidget(ew);

    layYAxis[i]->addStretch();

    layPlots->addRow(layYAxis[i]);
  }

  cur_built_plots = pltsz;
  return true;
}

void iViewPanelOfGraphTable::UpdatePanel_impl() {
  inherited::UpdatePanel_impl();

  GraphTableView* glv = this->glv(); //cache
  if (!glv) return; // probably destructing

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

  lelXAxis->GetImage(&(glv->children), glv->x_axis.GetColPtr());
  rncXAxis->setChecked(glv->x_axis.row_num);
  pdtXAxis->GetImage_(&(glv->x_axis.fixed_range));
  cellXAxis->GetImage((String)glv->x_axis.matrix_cell);

  lelZAxis->GetImage(&(glv->children), glv->z_axis.GetColPtr());
  oncZAxis->setReadOnly(glv->z_axis.GetColPtr() == NULL);
  oncZAxis->setChecked(glv->z_axis.on);
  rncZAxis->setChecked(glv->z_axis.row_num);
  pdtZAxis->GetImage_(&(glv->z_axis.fixed_range));
  cellZAxis->GetImage((String)glv->z_axis.matrix_cell);

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
  }

  fldErrSpacing->GetImage((String)glv->err_spacing);
  fldLabelSz->GetImage((String)glv->label_font_size);
  fldAxisSz->GetImage((String)glv->axis_font_size);
  fldBarSpace->GetImage((String)glv->bar_space);
  fldBarDepth->GetImage((String)glv->bar_depth);

  cmbColorMode->GetImage(glv->color_mode);
  lelCAxis->GetImage(&(glv->children), glv->color_axis.GetColPtr());
  lelCAxis->SetFlag(taiWidget::flgReadOnly, glv->color_mode != GraphTableView::COLOR_AXIS);

  fldThresh->GetImage((String)glv->thresh);

  cbar->UpdateScaleValues();

  lelRAxis->GetImage(&(glv->children), glv->raster_axis.GetColPtr());
  pdtRAxis->GetImage_(&(glv->raster_axis.fixed_range));

  lelRAxis->SetFlag(taiWidget::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
  pdtRAxis->SetFlag(taiWidget::flgReadOnly, glv->graph_type != GraphTableView::RASTER);
}

void iViewPanelOfGraphTable::GetValue_impl() {
  GraphTableView* glv = this->glv(); //cache
  if (!glv) return;

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

  glv->setScaleData(false, cbar->min(), cbar->max());

  glv->x_axis.row_num = rncXAxis->isChecked();
  pdtXAxis->GetValue_(&(glv->x_axis.fixed_range));
  glv->x_axis.SetColPtr((GraphColView*)lelXAxis->GetValue());
  glv->x_axis.matrix_cell = (int)cellXAxis->GetValue();

  // if setting a col for 1st time, we automatically turn on (since it would be ro)
  GraphColView* tcol = (GraphColView*)lelZAxis->GetValue();
  if (tcol && !glv->z_axis.GetColPtr())
    oncZAxis->setChecked(true);
  glv->z_axis.on = oncZAxis->isChecked();
  pdtZAxis->GetValue_(&(glv->z_axis.fixed_range));
  glv->z_axis.row_num = rncZAxis->isChecked();
  glv->z_axis.SetColPtr(tcol);
  glv->z_axis.matrix_cell = (int)cellZAxis->GetValue();

  int pltsz = MIN(max_plots, glv->plots.size);

  for(int i=0;i<pltsz; i++) {
    tcol = (GraphColView*)lelYAxis[i]->GetValue();
    if (tcol && !glv->plots[i]->GetColPtr())
      oncYAxis[i]->setChecked(true);
    glv->plots[i]->on = oncYAxis[i]->isChecked();
    pdtYAxis[i]->GetValue_(&(glv->plots[i]->fixed_range)); // this can change, so update
    glv->plots[i]->alt_y = chkYAltY[i]->isChecked();
    glv->plots[i]->SetColPtr(tcol);
    glv->plots[i]->matrix_cell = (int)cellYAxis[i]->GetValue();

    tcol = (GraphColView*)lelErr[i]->GetValue();
    if (tcol && !glv->errbars[i]->GetColPtr())
      oncErr[i]->setChecked(true);
    glv->errbars[i]->on = oncErr[i]->isChecked();
    glv->errbars[i]->SetColPtr(tcol);
  }

  glv->err_spacing = (int)fldErrSpacing->GetValue();
  glv->label_font_size = (float)fldLabelSz->GetValue();
  glv->axis_font_size = (float)fldAxisSz->GetValue();
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

