// Co2018ght 2013-2017, Regents of the University of Colorado,
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
#include <taiMisc>

#include <QComboBox>
#include <QBoxLayout>

TA_BASEFUNS_CTORS_DEFN(ProgElChoiceDlg);


int ProgElChoiceDlg::GetLocalGlobalChoice(String& var_nm, int& local_global_choice,
                                          ProgVar::VarType& var_type_choice, bool type_is_guess,
                                          LocalGlobalOption option, bool make_new_instr) {
  String  row;  // reuse for each widget
  String  chs_str;
  bool    showInstruction = var_nm.nonempty();  // if we don't know the var name ask for it
  bool    show_type_chooser = (var_type_choice == ProgVar::T_UnDef);
  bool    show_local_global_chooser = (local_global_choice == 2); // not local or global so show the chooser
  
  if (!show_type_chooser && type_is_guess) {
    // in this case show type chooser but select the guess type
    show_type_chooser = true;
  }

  dlg.win_title = "Create Variable - Local or Global";
  dlg.width = taiMisc::resizeByMainFont(300);
  dlg.height = taiMisc::resizeByMainFont(125);

  String widget("main");
  String vbox("mainv");
  dlg.AddWidget(widget);
  dlg.AddVBoxLayout(vbox, "", widget);

  if (showInstruction) {
    row = "instrRow";
    dlg.AddHBoxLayout(row, vbox);
    if(make_new_instr) {
      chs_str = "Create \'" + var_nm + "\' as Local or Global?;";
    }
    else {
      chs_str = "Program Variable   \'" + var_nm + "\'   Not Found;";
    }
    String labelStr = "label=" + chs_str;
    dlg.AddLabel("Instructions", widget, row, labelStr);
  }
  else {
    row = "var_name";
    dlg.AddHBoxLayout(row, vbox);
    dlg.AddLabel("var_name_label", widget, row, "label=New Var Name:;");
    dlg.AddStringField(&var_nm, "var_name", widget, row, "tooltip=Enter a name for the new variable;");
  }

  dlg.AddSpace(40, "mainv");

  row = "local_global";
  dlg.AddHBoxLayout(row, vbox);
  dlg.AddLabel("local or global", widget, row, "label=Create Variable As:;");
  String_Array choices;
  switch (option) {
  case LOCAL:
    choices.FmDelimString("Local");
    break;
  case GLOBAL:
    choices.FmDelimString("Global");
    break;
  default:
    choices.FmDelimString("Local Global Ignore");
    break;
  }

  QComboBox* combo_local_global = new QComboBox;
  QComboBox* combo_var_type = new QComboBox;

  if (show_local_global_chooser) {
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

  if (show_type_chooser) {  // var type not chosen - give user the options
    String_Array var_types;
    var_types.FmDelimString("Int Real String Bool Object* Enum DynEnum UnDef");
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
      combo_var_type->setCurrentIndex(var_type_choice);
    }
  }
  
  int result = dlg.PostDialog(true); // true is modal
  if(show_local_global_chooser) {
    local_global_choice = combo_local_global->currentIndex();
  }
  if (show_type_chooser) {
    var_type_choice = (ProgVar::VarType)combo_var_type->currentIndex();
  }

  return result;
}
