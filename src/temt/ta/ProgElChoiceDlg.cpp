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

#include "ProgElChoiceDlg.h"

#include <taGuiDialog>
#include <Program>
#include <ProgEl>
#include <ProgVar>
#include <String_Array>
#include <DynEnum>

#include <QComboBox>
#include <QBoxLayout>


int ProgElChoiceDlg::GetLocalGlobalChoice(Program* prg, String& var_nm, int& local_global_choice, ProgVar::VarType& var_type_choice) {
  String row;  // reuse for each widget

  String chs_str = "Program Variable   \'" + var_nm + "\'   Not Found;";
  dlg.win_title = "Create Variable - Local or Global";
  dlg.width = 300;
  dlg.height = 125;

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  row = "instrRow";
  dlg.AddHBoxLayout(row, vbox);
  String labelStr = "label=" + chs_str;
  dlg.AddLabel("Instructions", widget, row, labelStr);

  dlg.AddSpace(40, "mainv");

  row = "local_global";
  dlg.AddHBoxLayout(row, vbox);
//  dlg.AddLabel("local or global", widget, row, "label=Create a local or global variable;");
  String_Array choices;
  choices.FmDelimString("Create As Global, Create As Local", ",");

  QComboBox* combo_local_global = new QComboBox;
  {
    taGuiLayout *hboxEmer = dlg.FindLayout(row);  // Get the hbox for this row so we can add our combobox to it.
    if (!hboxEmer) {
      return false;
    }
    QBoxLayout *hbox = hboxEmer->layout;
    if (!hbox) {
      return false;
    }
    for (int idx = 0; idx < choices.size; ++idx) {
      combo_local_global->addItem(choices[idx]);
    }
    hbox->addWidget(combo_local_global);
      combo_local_global->setCurrentIndex(0);//
  }

  //  row = "local_global";
  //   dlg.AddHBoxLayout(row, vbox);
 //  dlg.AddLabel("local or global", widget, row, "label=Create a local or global variable;");
   String_Array var_types;
   var_types.FmDelimString("Int Real String Bool Object* Enum DynEnum UnDef");

   QComboBox* combo_var_type = new QComboBox;
   {
     taGuiLayout *hboxEmer = dlg.FindLayout(row);  // Get the hbox for this row so we can add our combobox to it.
     if (!hboxEmer) {
       return false;
     }
     QBoxLayout *hbox = hboxEmer->layout;
     if (!hbox) {
       return false;
     }
     for (int idx = 0; idx < var_types.size; ++idx) {
       combo_var_type->addItem(var_types[idx]);
     }
     hbox->addWidget(combo_var_type);
     int idx = combo_var_type->findText("UnDef");
     if (idx >= 0) {
       combo_var_type->setCurrentIndex(idx);  // default is UnDef
     }
   }
  int result = dlg.PostDialog(true); // true is modal
  local_global_choice = combo_local_global->currentIndex();
  var_type_choice = (ProgVar::VarType)combo_var_type->currentIndex();

  return result;
}
