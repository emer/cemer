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

#include "iProgramToolBar.h"
#include <ProgramToolBar>
#include <IViewerWidget>
#include <ViewColor_List>
#include <iColor>
#include <ProgEl>
#include <iBaseClipWidgetAction>
#include <iToolBoxDockViewer>
#include <ForLoop>
#include <MethodCall>
#include <ToolBoxRegistrar>

#include <taMisc>
#include <tabMisc>
#include <taiMisc>
#include <taRootBase>

taTypeDef_Of(DataTable);
taTypeDef_Of(ControlPanel);
taTypeDef_Of(Program);
taTypeDef_Of(Function);

taTypeDef_Of(ForLoop);
taTypeDef_Of(ForeachLoop);
taTypeDef_Of(DoLoop);
taTypeDef_Of(WhileLoop);

taTypeDef_Of(If);
taTypeDef_Of(Else);
taTypeDef_Of(ElseIf);
taTypeDef_Of(IfContinue);
taTypeDef_Of(IfBreak);
taTypeDef_Of(IfReturn);
taTypeDef_Of(IfGuiPrompt);
taTypeDef_Of(Switch);
taTypeDef_Of(StopStepPoint);
taTypeDef_Of(Comment);
taTypeDef_Of(BlankLineEl);

taTypeDef_Of(AssignExpr);
taTypeDef_Of(VarIncr);
taTypeDef_Of(MemberAssign);
taTypeDef_Of(MethodCall);
taTypeDef_Of(MemberMethodCall);
taTypeDef_Of(FunctionCall);
taTypeDef_Of(ProgramCall);
taTypeDef_Of(ProgramCallFun);

taTypeDef_Of(StaticMethodCall);
taTypeDef_Of(MathCall);
taTypeDef_Of(RandomCall);
taTypeDef_Of(MiscCall);
taTypeDef_Of(PrintExpr);
taTypeDef_Of(PrintVar);

taTypeDef_Of(DataProcCall);
taTypeDef_Of(DataAnalCall);
taTypeDef_Of(DataGenCall);

taTypeDef_Of(DataLoop);
taTypeDef_Of(ResetDataRows);
taTypeDef_Of(AddNewDataRow);
taTypeDef_Of(DoneWritingDataRow);
taTypeDef_Of(DataVarProg);
taTypeDef_Of(DataVarRead);
taTypeDef_Of(DataVarWrite);
taTypeDef_Of(DataVarProgMatrix);

taTypeDef_Of(DataSortProg);
taTypeDef_Of(DataGroupProg);
taTypeDef_Of(DataSelectRowsProg);
taTypeDef_Of(DataSelectColsProg);
taTypeDef_Of(DataJoinProg);

taTypeDef_Of(DataCalcLoop);
taTypeDef_Of(DataCalcAddDestRow);
taTypeDef_Of(DataCalcSetDestRow);
taTypeDef_Of(DataCalcSetSrcRow);
taTypeDef_Of(DataCalcCopyCommonCols);


IViewerWidget* ProgramToolBar::ConstrWidget_impl(QWidget* gui_parent) {
  return new iProgramToolBar(this, gui_parent); // usually parented later
}

void iProgramToolBar::Constr_post() {
//  iMainWindowViewer* win = viewerWindow(); //cache
//TODO: add the appropriate global actions
//  int icon_sz = (int)(1.1f * (float)taiM_->label_height(taiMisc::sizMedium));
  int icon_sz = taiM_->label_height(taiMisc::sizSmall);
  this->setIconSize(QSize(icon_sz, icon_sz));
}

static void ptbp_deco_widget(QWidget* widg, taBase* obj) {
  if(!widg) return;
  String dec_key = obj->GetTypeDecoKey(); // nil if none
  if(dec_key.nonempty()) {
    ViewColor* vc = taMisc::view_colors->FindName(dec_key);
    iColor colr;
    if(vc) {
      if(vc->use_fg)
        colr = vc->fg_color.color();
      else if(vc->use_bg)
        colr = vc->bg_color.color();
      QPalette pal;
      pal.setColor(QPalette::ButtonText, colr);
      widg->setPalette(pal);
      widg->setFont(taiM->buttonFont(taiMisc::sizMedium));

    }
  }
}

//static void ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td) {
void iProgramToolBar::ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td) {
  taOBase* obj = (taOBase*)tabMisc::root->GetTemplateInstance(td);
  if(td == &TA_ForLoop) {
    ((ForLoop*)obj)->init.expr = "_toolbox_tmp_"; // flag for auto-updating of for loop var
  }
  iBaseClipWidgetAction* act = new iBaseClipWidgetAction(obj->GetToolbarName(), obj);
  QWidget* widg = tb->AddClipToolWidget(sec, act);
  ptbp_deco_widget(widg, obj);
}

void ProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("New"); //note: need to keep it short
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataTable);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ControlPanel);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Program);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Function);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgVar);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DynEnum);

  sec = tb->AssertSection("Control"); //note: need to keep it short
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ForLoop);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ForeachLoop);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DoLoop);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_WhileLoop);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_If);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Else);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ElseIf);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_IfContinue);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_IfBreak);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_IfReturn);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_IfGuiPrompt);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Switch);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_StopStepPoint);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Comment);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_BlankLineEl);

  ////////////////////////////////////////////////////////////////////////////
  //            Var/Fun
  sec = tb->AssertSection("Assign/Call");
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_AssignExpr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_VarIncr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MemberAssign);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MemberMethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgramCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_FunctionCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgramCallFun);

    ////////////////////////////////////////////////////////////////////////////
  //            Print/Misc
  sec = tb->AssertSection("Functions");
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_PrintExpr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_PrintVar);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_StaticMethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MathCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_RandomCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MiscCall);

  ////////////////////////////////////////////////////////////////////////////
  //            Data processing
  sec = tb->AssertSection("Data R/W"); //note: need to keep it short
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataLoop);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ResetDataRows);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_AddNewDataRow);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DoneWritingDataRow);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataVarProg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataVarRead);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataVarWrite);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataVarProgMatrix);

  ////////////////////////////////////////////////////////////////////////////
  //            Data processing
  sec = tb->AssertSection("Data Proc"); //note: need to keep it short
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataSortProg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataGroupProg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataSelectRowsProg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataSelectColsProg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataJoinProg);
  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataCalcLoop);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataCalcAddDestRow);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataCalcSetDestRow);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataCalcSetSrcRow);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataCalcCopyCommonCols);
  
  ////////////////////////////////////////////////////////////////////////////
  //            Data Procs
  sec = tb->AssertSection("Data Procs"); //note: need to keep it short
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataProcCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataAnalCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataGenCall);
}

ToolBoxRegistrar ptb(ProgramToolBoxProc);
