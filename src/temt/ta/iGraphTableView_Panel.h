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

#ifndef iGraphTableView_Panel_h
#define iGraphTableView_Panel_h 1

// parent includes:
#include <iDataTableView_Panel>

// member includes:

// declare all other types mentioned but not required to include:
class taiField; // #IGNORE
class taiIncrField; // #IGNORE
class ColorScaleBar; // #IGNORE
class taiListElsButton; // #IGNORE
class taiPolyData; // #IGNORE
class iCheckBox; // #IGNORE
class taiComboBox; //
class GraphTableView; //

class TA_API iGraphTableView_Panel: public iDataTableView_Panel {
  Q_OBJECT
INHERITED(iDataTableView_Panel)
public:
  static const int      max_plots = 8; // maximum number of y axis data plots (fixed by plot_x guys below)
  // IMPORTANT: above copied from GraphTableView -- must be same

  QHBoxLayout*            layTopCtrls;
  QCheckBox*                chkDisplay;
  QCheckBox*                chkManip;
  QLabel*                   lblGraphType;
  taiComboBox*              cmbGraphType;
  QLabel*                   lblPlotStyle;
  taiComboBox*              cmbPlotStyle;

  QPushButton*              butRefresh;

  QHBoxLayout*            layVals;
  QLabel*                   lblRows;
  taiIncrField*             fldRows; // number of rows to display
  QLabel*                   lblLineWidth;
  taiField*                 fldLineWidth;
  QLabel*                   lblPointSpacing;
  taiField*                 fldPointSpacing;
  QLabel*                   lblLabelSpacing;
  taiField*                 fldLabelSpacing;
  QCheckBox*                chkNegDraw;
  QCheckBox*                chkNegDrawZ;

  QLabel*                   lblWidth;
  taiField*                 fldWidth; // width of the display (height is always 1.0)
  QLabel*                   lblDepth;
  taiField*                 fldDepth; // depth of the display (height is always 1.0)

  QHBoxLayout*            layXAxis;
  QLabel*                   lblXAxis;
  taiListElsButton*         lelXAxis; // list element chooser
  QCheckBox*                rncXAxis; // row number checkbox
  taiPolyData*              pdtXAxis; // fixed_range polydata (inline)

  QHBoxLayout*            layZAxis;
  iCheckBox*                oncZAxis; // on checkbox
  QLabel*                   lblZAxis;
  taiListElsButton*         lelZAxis; // list element chooser
  QCheckBox*                rncZAxis; // row number checkbox
  taiPolyData*              pdtZAxis; // fixed_range polydata (inline)

  QHBoxLayout*            layYAxis[max_plots];
  iCheckBox*                oncYAxis[max_plots];
  QLabel*                   lblYAxis[max_plots];
  taiListElsButton*         lelYAxis[max_plots]; // list element chooser
  taiPolyData*              pdtYAxis[max_plots]; // fixed_range polydata (inline)
  QCheckBox*                chkYAltY[max_plots];

  QHBoxLayout*            layErr[2]; // two rows
  QLabel*                   lblErr[max_plots];
  taiListElsButton*         lelErr[max_plots];
  iCheckBox*                oncErr[max_plots]; // on checkbox

  QLabel*                   lblErrSpacing;
  taiField*                 fldErrSpacing;

  QHBoxLayout*            layCAxis;
  QLabel*                   lblColorMode;
  taiComboBox*              cmbColorMode;
  QLabel*                   lblCAxis;
  taiListElsButton*         lelCAxis; // list element chooser
  QLabel*                   lblThresh;
  taiField*                 fldThresh;

  QHBoxLayout*            layColorScale;
  ColorScaleBar*            cbar;             // colorbar
  QPushButton*              butSetColor;

  QHBoxLayout*            layRAxis;
  QLabel*                   lblRAxis;
  taiListElsButton*         lelRAxis; // list element chooser
  taiPolyData*              pdtRAxis; // fixed_range polydata (inline)

  override String       panel_type() const; // this string is on the subpanel button for this panel
  GraphTableView*       glv() {return (GraphTableView*)m_dv;}

  iGraphTableView_Panel(GraphTableView* glv);
  ~iGraphTableView_Panel();

protected:
  override void         InitPanel_impl(); // called on structural changes
  override void         UpdatePanel_impl(); // called on structural changes
  override void         GetValue_impl();
  override void         CopyFrom_impl();

public: // IDataLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iGraphTableView_Panel;}

protected slots:
  void          butRefresh_pressed();
  void          butClear_pressed();
  void          butSetColor_pressed();

};

#endif // iGraphTableView_Panel_h
