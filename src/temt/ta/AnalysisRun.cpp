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

#include "AnalysisRun.h"

#include <DataTable>
#include <taGuiDialog>
#include <taDataAnal>
#include <taDataAnalParams>
#include <taMisc>
#include <DynEnum>
#include <String_Array>

#include <QComboBox>
#include <QBoxLayout>

TA_BASEFUNS_CTORS_DEFN(AnalysisRun);

bool AnalysisRun::Init(AnalysisType type, DataTable* src_table, const String& src_col_name, DataTable* result_table) {
  analysis_type = type;

  if (src_table == NULL)
    return false;
  if (RequiresResultsTable(type) && result_table == NULL)
    return false;
  params.Init(src_table, src_col_name, result_table);
  return true;
}

bool AnalysisRun::RequiresResultsTable(AnalysisType type) {
  if (type == REGRESS_LINEAR) {
    return false;
  }
  else {
    return true;
  }
}

bool AnalysisRun::Run() {
  bool rval = false;

  if (analysis_type == CLUSTER) {
    bool paramsSet = CollectParamsCluster(params);
    if (paramsSet) {
      rval = taDataAnal::Cluster(params.result_data_table, params.view, params.src_data_table, params.data_column_name,
          params.name_column_name, params.distance_metric, params.norm, params.tolerance);
    }
  }
  else if (analysis_type == PCA_2d) {
    bool paramsSet = CollectParamsPCA_2d(params);
    if (paramsSet) {
      rval = taDataAnal::PCA2dPrjn(params.result_data_table, params.view, params.src_data_table, params.data_column_name,
          params.name_column_name, params.x_axis_component, params.y_axis_component);
    }
  }
  else if (analysis_type == PCA_EIGEN) {
    bool paramsSet = CollectParamsPCA_Eigen(params);
    if (paramsSet) {
      rval = taDataAnal::PCAEigenTable(params.result_data_table, params.view, params.src_data_table, params.data_column_name);
    }
  }
  else if (analysis_type == DISTANCE_MATRIX) {
    bool paramsSet = CollectParamsDistanceMatrix(params);
    if (paramsSet) {
      rval = taDataAnal::DistMatrixTable(params.result_data_table, params.view, params.src_data_table, params.data_column_name,
          params.name_column_name, params.distance_metric, params.norm, params.tolerance, params.include_scalars);
    }
  }
  else if (analysis_type == REGRESS_LINEAR) {
    delete params.result_data_table;  // output added as column to src table
    bool paramsSet = CollectParamsLinearRegress(params);
    if (paramsSet) {
      String equation = taDataAnal::RegressLinear(params.src_data_table, params.name_column_name, params.data_column_name, params.view); // reusing "view" bool for "render line"
      rval = true;
    }
  }

  return rval;
}

// TODO - add popup to choose frame for graph

bool AnalysisRun::CollectParamsCluster(taDataAnalParams& params) {
  bool rval = false;

  taGuiDialog dlg;
  taBase::Ref(dlg);   // no need to UnRef - will be deleted at end of method

  dlg.win_title = "Cluster Analysis";
  dlg.width = 300;
  dlg.height = 400;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row("");
  int space = 0;

  row = "input_table";  // no spaces!
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_table_str = "Source Data Table: " + params.src_data_table->name;
  src_table_str = "label=" + src_table_str + ";";
  dlg.AddLabel("input_table_label", widget, row, src_table_str);

  row = "input_data_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_column_str = "Source Input Column: " + params.data_column_name;
  src_column_str = "label=" + src_column_str + ";";
  dlg.AddLabel("input_data_column_label", widget, row, src_column_str);

  QComboBox* combo_name_column = new QComboBox;
  {
    row = "input_name_column";
    dlg.AddSpace(space, vbox);
    dlg.AddHBoxLayout(row, vbox);
    dlg.AddLabel("input_name_column_label", widget, row, "label=Source Name Column:;");
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    for (int idx = 0; idx < params.src_data_table->data.size; ++idx) {
      String item = params.src_data_table->data[idx]->name;
      if (item != params.data_column_name) {
        combo_name_column->addItem(item);
      }
    }
    hbox->addWidget(combo_name_column);
    int idx_combo = combo_name_column->findText("Name");
    if (idx_combo >= 0) {
      combo_name_column->setCurrentIndex(idx_combo);
    }
  }

  // TODO Add a real separator to taGuiDialog AddSeparator method

  row = "separator_1";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_1_label", widget, row, "label=-------------------------------;");

  row = "results_table";
  String result_table_name = params.src_data_table->name + "_cluster";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("result_table_label", widget, row, "label=Results Table:;");
  dlg.AddStringField(&result_table_name, "results_table", widget, row, "tooltip=Enter a name for the results table that will be generated;");

  row = "distance_metric";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("distance_metric_label", widget, row, "label=Distance Metric:;");

  // TODO - figure out how to use DynEnum
  // ALERT KLUDGE  - hand coding enum values into array
  //  enum DistMetric {             // generalized distance metrics
  //      SUM_SQUARES,                // sum of squares:  sum[(x-y)^2]
  //      EUCLIDIAN,                  // Euclidian distance (sqrt of sum of squares)
  //      HAMMING,                    // Hamming distance: sum[abs(x-y)]
  //      COVAR,                      // covariance: sum[(x-<x>)(y-<y>)]
  //      CORREL,                     // correlation: sum[(x-<x>)(y-<y>)] / sqrt(sum[x^2 y^2])
  //      INNER_PROD,                 // inner product: sum[x y]
  //      CROSS_ENTROPY               // cross entropy: sum[x ln(x/y) + (1-x)ln((1-x)/(1-y))]
  //    };
  String_Array dist_metrics;
  dist_metrics.FmDelimString("SUM_SQUARES EUCLIDIAN HAMMING"); // only 3 for cluster

  QComboBox* combo_dist_metric = new QComboBox;
  {
    taGuiLayout *hboxEmer = dlg.FindLayout(row);  // Get the hbox for this row so we can add our combobox to it.
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    for (int idx = 0; idx < dist_metrics.size; ++idx) {
      combo_dist_metric->addItem(dist_metrics[idx]);
    }
    hbox->addWidget(combo_dist_metric);
    int idx_combo = combo_dist_metric->findText("EUCLIDIAN");
    if (idx_combo >= 0) {
      combo_dist_metric->setCurrentIndex(idx_combo);//
    }
  }

  row = "norm";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("norm", widget, row, "label=Normalize the data?:;");
  dlg.AddBoolCheckbox(&params.norm, "norm", widget, row,
      "tooltip=Do you want the data normalized, default is yes.;");
  dlg.AddStretch(row);

  row = "tolerance";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("tolerance", widget, row, "label=Tolerance Value:;");
  dlg.AddFloatField(&params.tolerance, "tolerance", widget, row,
      "tooltip=Float value for the tolerance, 0.0 is the default.;");
  dlg.AddStretch(row);

  row = "separator_2";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_2_label", widget, row, "label=-------------------------------;");

  row = "view";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("view_label", widget, row, "label=Draw Graph;");
  dlg.AddBoolCheckbox(&params.view, "view", widget, row,
      "tooltip=Show the analysis results in a new graph panel?;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);

  params.name_column_name = combo_name_column->itemText(combo_name_column->currentIndex());
  params.result_data_table->name = result_table_name;
  params.distance_metric = (taMath::DistMetric)combo_dist_metric->currentIndex();

  if (drval != 0)
    rval = true;

  return rval;
}

bool AnalysisRun::CollectParamsPCA_2d(taDataAnalParams& params) {
  bool rval = false;

  taGuiDialog dlg;
  taBase::Ref(dlg);

  dlg.win_title = "PCA 2D";
  dlg.width = 300;
  dlg.height = 400;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row("");
  int space = 0;

  row = "input_table";  // no spaces!
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_table_str = "Source Data Table: " + params.src_data_table->name;
  src_table_str = "label=" + src_table_str + ";";
  dlg.AddLabel("input_table_label", widget, row, src_table_str);

  row = "input_data_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_column_str = "Source Input Column: " + params.data_column_name;
  src_column_str = "label=" + src_column_str + ";";
  dlg.AddLabel("input_data_column_label", widget, row, src_column_str);

  row = "input_name_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("input_name_column_label", widget, row, "label=Source Name Column:;");
  // Get the hbox for this row so we can add our combobox to it.
  taGuiLayout *hboxEmer = dlg.FindLayout(row);
  if (!hboxEmer) {
    return false;
  }
  QBoxLayout *hbox = hboxEmer->layout;
  if (!hbox) {
    return false;
  }
  QComboBox* combo_name_column = new QComboBox;
  for (int idx = 0; idx < params.src_data_table->data.size; ++idx) {
    String item = params.src_data_table->data[idx]->name;
    if (item != params.data_column_name) {
      combo_name_column->addItem(item);
    }
  }
  hbox->addWidget(combo_name_column);
  int idx_combo = combo_name_column->findText("Name");
  if (idx_combo >= 0) {
    combo_name_column->setCurrentIndex(idx_combo);
  }

  // TODO Add a real separator to taGuiDialog AddSeparator method

  row = "separator_1";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_1_label", widget, row, "label=-------------------------------;");

  row = "results_table";
  String result_table_name = params.src_data_table->name + "_PCA2D";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("result_table_label", widget, row, "label=Results Table:;");
  dlg.AddStringField(&result_table_name, "results_table", widget, row, "tooltip=Enter a name for the results table that will be generated;");

  row = "x_axis_component";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("x_axis_component", widget, row, "label=X Axis Component:;");
  dlg.AddIntField(&params.x_axis_component, "x_axis", widget, row,
      "tooltip=Enter the component number to plot on the X axis. The first component '0' accounts for the most variance.;");
  dlg.AddStretch(row);

  row = "y_axis_component";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("y_axis_component", widget, row, "label=Y Axis Component:;");
  dlg.AddIntField(&params.y_axis_component, "y_axis", widget, row,
      "tooltip=Enter the component number to plot on the Y axis. The second component '1' accounts for the second most variance.;");
  dlg.AddStretch(row);

   row = "separator_2";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_2_label", widget, row, "label=-------------------------------;");

  row = "view";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("view_label", widget, row, "label=Draw Graph;");
  dlg.AddBoolCheckbox(&params.view, "view", widget, row,
      "tooltip=Show the analysis results in a new graph panel?;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);

  params.name_column_name = combo_name_column->itemText(combo_name_column->currentIndex());
  params.result_data_table->name = result_table_name;

  if (drval != 0)
    rval = true;

  return rval;
}

bool AnalysisRun::CollectParamsPCA_Eigen(taDataAnalParams& params) {
  bool rval = false;

  taGuiDialog dlg;
  taBase::Ref(dlg);

  dlg.win_title = "PCA Eigen";
  dlg.width = 300;
  dlg.height = 300;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row("");
  int space = 0;

  row = "input_table";  // no spaces!
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_table_str = "Source Data Table: " + params.src_data_table->name;
  src_table_str = "label=" + src_table_str + ";";
  dlg.AddLabel("input_table_label", widget, row, src_table_str);

  row = "input_data_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_column_str = "Source Input Column: " + params.data_column_name;
  src_column_str = "label=" + src_column_str + ";";
  dlg.AddLabel("input_data_column_label", widget, row, src_column_str);

  // TODO Add a real separator to taGuiDialog AddSeparator method

  row = "separator_1";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_1_label", widget, row, "label=-------------------------------;");

  row = "results_table";
  String result_table_name = params.src_data_table->name + "_PCA_Eigen";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("result_table_label", widget, row, "label=Results Table:;");
  dlg.AddStringField(&result_table_name, "results_table", widget, row, "tooltip=Enter a name for the results table that will be generated;");

   row = "separator_2";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_2_label", widget, row, "label=-------------------------------;");

  row = "view";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("view_label", widget, row, "label=Draw Graph;");
  dlg.AddBoolCheckbox(&params.view, "view", widget, row,
      "tooltip=Show the analysis results in a new graph panel?;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);

  params.result_data_table->name = result_table_name;

  if (drval != 0)
    rval = true;

  return rval;
}

bool AnalysisRun::CollectParamsDistanceMatrix(taDataAnalParams& params) {
  bool rval = false;

  taGuiDialog dlg;
  taBase::Ref(dlg);

  dlg.win_title = "Distance Matrix";
  dlg.width = 300;
  dlg.height = 400;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row("");
  int space = 0;

  row = "input_table";  // no spaces!
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_table_str = "Source Data Table: " + params.src_data_table->name;
  src_table_str = "label=" + src_table_str + ";";
  dlg.AddLabel("input_table_label", widget, row, src_table_str);

  row = "input_data_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_column_str = "Source Input Column: " + params.data_column_name;
  src_column_str = "label=" + src_column_str + ";";
  dlg.AddLabel("input_data_column_label", widget, row, src_column_str);

  QComboBox* combo_name_column = new QComboBox;
  {
    row = "input_name_column";
    dlg.AddSpace(space, vbox);
    dlg.AddHBoxLayout(row, vbox);
    dlg.AddLabel("input_name_column_label", widget, row, "label=Source Name Column:;");
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    // provide null option as it is better to leave out the name column
    // for large matrices
    combo_name_column->addItem("NULL");
    for (int idx = 0; idx < params.src_data_table->data.size; ++idx) {
      String item = params.src_data_table->data[idx]->name;
      if (item != params.data_column_name) {
        combo_name_column->addItem(item);
      }
    }
    hbox->addWidget(combo_name_column);
    int idx_combo = combo_name_column->findText("Name");
    if (idx_combo >= 0) {
      combo_name_column->setCurrentIndex(idx_combo);
    }
  }

  // TODO Add a real separator to taGuiDialog AddSeparator method

  row = "separator_1";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_1_label", widget, row, "label=-------------------------------;");

  row = "results_table";
  String result_table_name = params.src_data_table->name + "_DistanceMatrix";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("result_table_label", widget, row, "label=Results Table:;");
  dlg.AddStringField(&result_table_name, "results_table", widget, row, "tooltip=Enter a name for the results table that will be generated;");

  row = "distance_metric";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("distance_metric_label", widget, row, "label=Distance Metric:;");

  String_Array dist_metrics;
  dist_metrics.FmDelimString("SUM_SQUARES EUCLIDIAN HAMMING COVAR CORREL INNER_PROD CROSS_ENTROPY");

  QComboBox* combo_dist_metric = new QComboBox;
  {
    taGuiLayout *hboxEmer = dlg.FindLayout(row);  // Get the hbox for this row so we can add our combobox to it.
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    for (int idx = 0; idx < dist_metrics.size; ++idx) {
      combo_dist_metric->addItem(dist_metrics[idx]);
    }
    hbox->addWidget(combo_dist_metric);
    int idx_combo = combo_dist_metric->findText("EUCLIDIAN");
    if (idx_combo >= 0) {
      combo_dist_metric->setCurrentIndex(idx_combo);//
    }
  }

  row = "norm";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("norm", widget, row, "label=Normalize the data?:;");
  dlg.AddBoolCheckbox(&params.norm, "norm", widget, row,
      "tooltip=Do you want the data normalized, default is yes.;");
  dlg.AddStretch(row);

  row = "tolerance";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("tolerance", widget, row, "label=Tolerance Value:;");
  dlg.AddFloatField(&params.tolerance, "tolerance", widget, row,
      "tooltip=Float value for the tolerance, 0.0 is the default.;");
  dlg.AddStretch(row);

  row = "scalars";
    dlg.AddSpace(space, vbox);
    dlg.AddHBoxLayout(row, vbox);
    dlg.AddLabel("scalars", widget, row, "label=Include Scalars?:;");
    dlg.AddBoolCheckbox(&params.include_scalars, "scalars", widget, row,
        "tooltip=Do you want to include scalar values, default is no.;");
    dlg.AddStretch(row);

  row = "separator_2";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_2_label", widget, row, "label=-------------------------------;");

  row = "view";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("view_label", widget, row, "label=Draw Graph;");
  dlg.AddBoolCheckbox(&params.view, "view", widget, row,
      "tooltip=Show the analysis results in a new graph panel?;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);

  params.name_column_name = combo_name_column->itemText(combo_name_column->currentIndex());
  if (params.name_column_name == "NULL") // replace literal NULL with an empty string
    params.name_column_name = "";
  params.result_data_table->name = result_table_name;
  params.distance_metric = (taMath::DistMetric)combo_dist_metric->currentIndex();

  if (drval != 0)
    rval = true;

  return rval;
}

bool AnalysisRun::CollectParamsLinearRegress(taDataAnalParams& params) {
  bool rval = false;

  taGuiDialog dlg;
  taBase::Ref(dlg);

  dlg.win_title = "Linear Regression";
  dlg.width = 300;
  dlg.height = 300;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  String row("");
  int space = 0;

  row = "input_table";  // no spaces!
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_table_str = "Source Data Table: " + params.src_data_table->name;
  src_table_str = "label=" + src_table_str + ";";
  dlg.AddLabel("input_table_label", widget, row, src_table_str);

  row = "X_var_data_column";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  String src_column_str = "Variable X: " + params.data_column_name;
  src_column_str = "label=" + src_column_str + ";";
  dlg.AddLabel("X_var_data_column_label", widget, row, src_column_str);

  QComboBox* combo_Y_var_column = new QComboBox;
  {
    row = "Y_var_data_column";
    dlg.AddSpace(space, vbox);
    dlg.AddHBoxLayout(row, vbox);
    dlg.AddLabel("Y_var_data_column_label", widget, row, "label= Variable Y: ;");
    // Get the hbox for this row so we can add our combobox to it.
    taGuiLayout *hboxEmer = dlg.FindLayout(row);
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    for (int idx = 0; idx < params.src_data_table->data.size; ++idx) {
      String item = params.src_data_table->data[idx]->name;
      if (item != params.data_column_name) {
        combo_Y_var_column->addItem(item);
      }
    }
    hbox->addWidget(combo_Y_var_column);
    int idx_combo = combo_Y_var_column->findText("Name");
    if (idx_combo >= 0) {
      combo_Y_var_column->setCurrentIndex(idx_combo);
    }
  }

  row = "separator_1";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("separator_1_label", widget, row, "label=-------------------------------;");

  row = "view";
  dlg.AddSpace(space, vbox);
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("view_label", widget, row, "label=Render Line?;");
  dlg.AddBoolCheckbox(&params.view, "view", widget, row,
      "tooltip=If render_line is true, a column called regress_line' is created and the function is generated into it as data;");

  bool modal = true;
  int drval = dlg.PostDialog(modal);

  params.name_column_name = combo_Y_var_column->itemText(combo_Y_var_column->currentIndex());

  if (drval != 0)
    rval = true;

  return rval;
}
