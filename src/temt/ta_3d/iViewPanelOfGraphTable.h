// Copyright 2017, Regents of the University of Colorado,
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

#ifndef iViewPanelOfGraphTable_h
#define iViewPanelOfGraphTable_h 1

// parent includes:
#include <iViewPanelOfDataTable>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetField; //
class taiWidgetFieldIncr; //
class iColorScaleBar; //
class taiWidgetListElChooser; //
class taiWidgetPoly; //
class iCheckBox; //
class taiWidgetComboBox; //
class GraphTableView; //
class QHBoxLayout; //
class QCheckBox; //
class QPushButton; //
class QLabel; //
class iStripeWidget; //
class iFormLayout; //
class iLabel; //


taTypeDef_Of(iViewPanelOfGraphTable);

class TA_API iViewPanelOfGraphTable: public iViewPanelOfDataTable {
  Q_OBJECT
  INHERITED(iViewPanelOfDataTable)
public:
  static const int          axis_chooser_width;
  static const int          axis_label_width;
  static const int          margin_l_r;  // left right contents margin
  static const int          margin_t_b;  // top bottom contents margin
  
  QHBoxLayout*              layTopCtrls;
  QCheckBox*                chkDisplay;
  QCheckBox*                chkManip;
  QLabel*                   lblGraphType;
  taiWidgetComboBox*        cmbGraphType;
  QLabel*                   lblPlotStyle;
  taiWidgetComboBox*        cmbPlotStyle;
  QCheckBox*                chkNegDraw;
  QCheckBox*                chkNegDrawZ;
  QPushButton*              butRefresh;
  
  QHBoxLayout*            layVals;
  QLabel*                   lblRows;
  taiWidgetFieldIncr*       fldRows; // number of rows to display
  QLabel*                   lblLineWidth;
  taiWidgetField*           fldLineWidth;
  QLabel*                   lblPointSpacing;
  taiWidgetField*           fldPointSpacing;
  QLabel*                   lblLabelSpacing;
  taiWidgetField*           fldLabelSpacing;
  QLabel*                   lblWidth;
  taiWidgetField*           fldWidth; // width of the display (height is always 1.0)
  QLabel*                   lblDepth;
  taiWidgetField*           fldDepth; // depth of the display (height is always 1.0)
  QLabel*                   lblNPlots;
  taiWidgetField*           fldNPlots;
  
  QHBoxLayout*            layAxisLabelChks;
  QLabel*                   lblAxisLabelChks;
  QCheckBox*                chkXAxisLabel; // show or hide X axis label
  QCheckBox*                chkYAxisLabel; // show or hide Y axis label
  QCheckBox*                chkAltYAxisLabel; // show or hide Y alt axis label
  QCheckBox*                chkZAxisLabel; // show or hide Z axis label
  QCheckBox*                chkLinesSolid; // solid lines
  
  QHBoxLayout*            layXAxis;
  QLabel*                   lblXAxis;
  taiWidgetListElChooser*   lelXAxis; // list element chooser
  QCheckBox*                rncXAxis; // row number checkbox
  taiWidgetPoly*            pdtXAxis; // fixed_range polydata (inline)
  QLabel*                   lblcellXAxis;
  taiWidgetFieldIncr*       cellXAxis; // matrix cell
  iCheckBox*                labonXAxis; // labels on checkbox
  taiWidgetListElChooser*   labXAxis; // labels list element chooser
  iCheckBox*                flipXAxis;
  QPushButton*              butLinePropsXAxis;
  
  QHBoxLayout*            layZAxis;
  iCheckBox*                oncZAxis; // on checkbox
  QLabel*                   lblZAxis;
  taiWidgetListElChooser*   lelZAxis; // list element chooser
  QCheckBox*                rncZAxis; // row number checkbox
  taiWidgetPoly*            pdtZAxis; // fixed_range polydata (inline)
  QLabel*                   lblcellZAxis;
  taiWidgetFieldIncr*       cellZAxis; // matrix cell
  iCheckBox*                labonZAxis; // labels on checkbox
  taiWidgetListElChooser*   labZAxis; // labels list element chooser
  iCheckBox*                flipZAxis;
  QPushButton*              butLinePropsZAxis;
  
  iStripeWidget*            plotsWidg; // plot holding widget
  iFormLayout*              layPlots; // all the plots
  int                       cur_built_plots; // number of plots that are currently built
  int                       row_height;
  
  QHBoxLayout**            layYAxis;
  iCheckBox**                oncYAxis;
  iLabel**                   lblYAxis;
  taiWidgetListElChooser**   lelYAxis; // list element chooser
  taiWidgetPoly**            pdtYAxis; // fixed_range polydata (inline)
  QCheckBox**                chkYAltY;
  QLabel**                   lblcellYAxis;
  taiWidgetFieldIncr**       cellYAxis; // matrix cell
  taiWidgetListElChooser**   lelErr;
  iCheckBox**                oncErr; // on checkbox
  iCheckBox**                flipYAxis;
  QPushButton**              butLineProps;

  QHBoxLayout*            layCAxis;
  QLabel*                   lblColorMode;
  taiWidgetComboBox*        cmbColorMode;
  QLabel*                   lblCAxis;
  taiWidgetListElChooser*   lelCAxis; // list element chooser
  QLabel*                   lblThresh;
  taiWidgetField*           fldThresh;
  QLabel*                   lblErrSpacing;
  taiWidgetField*           fldErrSpacing;
  QLabel*                   lblLabelSz;
  taiWidgetField*           fldLabelSz;
  QLabel*                   lblPointSz;
  taiWidgetField*           fldPointSz;
  QLabel*                   lblAxisSz;
  taiWidgetField*           fldAxisSz;
  QLabel*                   lblXlblRot;
  taiWidgetField*           fldXlblRot;
  
  QHBoxLayout*            layRAxis;
  QLabel*                   lblRAxis;
  taiWidgetListElChooser*   lelRAxis; // list element chooser
  taiWidgetPoly*            pdtRAxis; // fixed_range polydata (inline)
  QLabel*                   lblBarSpace;
  taiWidgetField*           fldBarSpace;
  QLabel*                   lblBarDepth;
  taiWidgetField*           fldBarDepth;
  
  QHBoxLayout*            layColorScale;
  iColorScaleBar*           cbar;             // colorbar
  QPushButton*              butSetColor;
  
  String                  panel_type() const override; // this string is on the subpanel button for this panel
  GraphTableView*         glv() {return (GraphTableView*)m_dv;}
  
  int                     last_plot_index; // this is the plot index of the most recent plot variable selected
  void*                   GetAlternateSelection() override;

  
  iViewPanelOfGraphTable(GraphTableView* glv);
  ~iViewPanelOfGraphTable();
  
protected:
  void              InitPanel_impl() override; // called on structural changes
  void              UpdatePanel_impl() override; // called on structural changes
  void              GetValue_impl() override;
  void              CopyFrom_impl() override;
  virtual bool      BuildPlots();
  
public: // ISigLinkClient interface
  void*             This() override {return (void*)this;}
  TypeDef*          GetTypeDef() const override {return &TA_iViewPanelOfGraphTable;}

  protected slots:
  void              butRefresh_pressed();
  void              butClear_pressed();
  void              butSetColor_pressed();
  void              butSetLineStyle(int plot_num);
  void              butSetLineStyleXAxis();
  void              butSetLineStyleZAxis();
  void              InsertPlotBefore(int plot_index);
  void              InsertPlotAfter(int plot_index);
  void              DeletePlot(int plot_index);
  void              MovePlotBefore(int old_index);
  void              ChooseVarPressed(int index);
  
#ifndef __MAKETA__
  void              label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e); // note, it MUST have this name
#endif
};

#endif // iViewPanelOfGraphTable_h
