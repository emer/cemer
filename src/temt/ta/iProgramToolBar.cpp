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

taTypeDef_Of(CodeBlock);
taTypeDef_Of(UserScript);
taTypeDef_Of(StopStepPoint);

taTypeDef_Of(LocalVars);

taTypeDef_Of(AssignExpr);
taTypeDef_Of(VarIncr);
taTypeDef_Of(MemberAssign);
taTypeDef_Of(MethodCall);
taTypeDef_Of(MemberMethodCall);
taTypeDef_Of(CssExpr);

taTypeDef_Of(Function);
taTypeDef_Of(FunctionCall);
taTypeDef_Of(ReturnExpr);

taTypeDef_Of(ProgramCall);
taTypeDef_Of(ProgramCallVar);
taTypeDef_Of(OtherProgramVar);

taTypeDef_Of(PrintExpr);
taTypeDef_Of(PrintVar);
taTypeDef_Of(Comment);

taTypeDef_Of(ProgVarFmArg);
taTypeDef_Of(MemberFmArg);
taTypeDef_Of(DataColsFmArgs);
taTypeDef_Of(SelectEditsFmArgs);
taTypeDef_Of(RegisterArgs);

taTypeDef_Of(StaticMethodCall);
taTypeDef_Of(MathCall);
taTypeDef_Of(RandomCall);
taTypeDef_Of(MiscCall);

taTypeDef_Of(DataProcCall);
taTypeDef_Of(DataAnalCall);
taTypeDef_Of(DataGenCall);
taTypeDef_Of(ImageProcCall);

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
  int icon_sz = taiM_->label_height(taiMisc::sizMedium);
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
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  if(td == &TA_ForLoop) {
    ((ForLoop*)obj)->init.expr = "_toolbox_tmp_"; // flag for auto-updating of for loop var
  }
  iBaseClipWidgetAction* act = new iBaseClipWidgetAction(obj->GetToolbarName(), obj);
  QWidget* widg = tb->AddClipToolWidget(sec, act);
  ptbp_deco_widget(widg, obj);
}

void ProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Ctrl"); //note: need to keep it short

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

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_CodeBlock);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_UserScript);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_StopStepPoint);

  ////////////////////////////////////////////////////////////////////////////
  //            Var/Fun
  sec = tb->AssertSection("Var/Fun");
  QWidget* widg = tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var",
                       tabMisc::root->GetTemplateInstance(&TA_ProgVar)));
  ptbp_deco_widget(widg, tabMisc::root->GetTemplateInstance(&TA_ProgVar));
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_LocalVars);

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_AssignExpr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_VarIncr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MemberAssign);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MemberMethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_CssExpr);

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Function);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_FunctionCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ReturnExpr);

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgramCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgramCallVar);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_OtherProgramVar);

  ////////////////////////////////////////////////////////////////////////////
  //            Print/Misc
  sec = tb->AssertSection("Print/Args..");
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_PrintExpr);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_PrintVar);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_Comment);

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ProgVarFmArg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MemberFmArg);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataColsFmArgs);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_SelectEditsFmArgs);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_RegisterArgs);

  ////////////////////////////////////////////////////////////////////////////
  //            Misc Fun
  sec = tb->AssertSection("Misc Fun");
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_StaticMethodCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MathCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_RandomCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_MiscCall);
  // add other spec meth calls here..

  tb->AddSeparator(sec);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataProcCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataAnalCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_DataGenCall);
  iProgramToolBar::ptbp_add_widget(tb, sec, &TA_ImageProcCall);

  ////////////////////////////////////////////////////////////////////////////
  //            Data processing
  sec = tb->AssertSection("Data"); //note: need to keep it short
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
}

ToolBoxRegistrar ptb(ProgramToolBoxProc);
