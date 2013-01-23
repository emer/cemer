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
#include <IDataViewWidget>
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
#include <taRootBase>

TypeDef_Of(ForLoop);
TypeDef_Of(ForeachLoop);
TypeDef_Of(DoLoop);
TypeDef_Of(WhileLoop);

TypeDef_Of(IfElse);
TypeDef_Of(IfContinue);
TypeDef_Of(IfBreak);
TypeDef_Of(IfReturn);
TypeDef_Of(IfGuiPrompt);
TypeDef_Of(Switch);

TypeDef_Of(CodeBlock);
TypeDef_Of(UserScript);
TypeDef_Of(StopStepPoint);

TypeDef_Of(LocalVars);

TypeDef_Of(AssignExpr);
TypeDef_Of(VarIncr);
TypeDef_Of(MemberAssign);
TypeDef_Of(MethodCall);
TypeDef_Of(MemberMethodCall);
TypeDef_Of(CssExpr);

TypeDef_Of(Function);
TypeDef_Of(FunctionCall);
TypeDef_Of(ReturnExpr);

TypeDef_Of(ProgramCall);
TypeDef_Of(ProgramCallVar);
TypeDef_Of(OtherProgramVar);

TypeDef_Of(PrintExpr);
TypeDef_Of(PrintVar);
TypeDef_Of(Comment);

TypeDef_Of(ProgVarFmArg);
TypeDef_Of(MemberFmArg);
TypeDef_Of(DataColsFmArgs);
TypeDef_Of(SelectEditsFmArgs);
TypeDef_Of(RegisterArgs);

TypeDef_Of(StaticMethodCall);
TypeDef_Of(MathCall);
TypeDef_Of(RandomCall);
TypeDef_Of(MiscCall);

TypeDef_Of(DataProcCall);
TypeDef_Of(DataAnalCall);
TypeDef_Of(DataGenCall);
TypeDef_Of(ImageProcCall);

TypeDef_Of(DataLoop);
TypeDef_Of(ResetDataRows);
TypeDef_Of(AddNewDataRow);
TypeDef_Of(DoneWritingDataRow);
TypeDef_Of(DataVarProg);
TypeDef_Of(DataVarProgMatrix);

TypeDef_Of(DataSortProg);
TypeDef_Of(DataGroupProg);
TypeDef_Of(DataSelectRowsProg);
TypeDef_Of(DataSelectColsProg);
TypeDef_Of(DataJoinProg);

TypeDef_Of(DataCalcLoop);
TypeDef_Of(DataCalcAddDestRow);
TypeDef_Of(DataCalcSetDestRow);
TypeDef_Of(DataCalcSetSrcRow);
TypeDef_Of(DataCalcCopyCommonCols);


IDataViewWidget* ProgramToolBar::ConstrWidget_impl(QWidget* gui_parent) {
  return new iProgramToolBar(this, gui_parent); // usually parented later
}

void iProgramToolBar::Constr_post() {
//  iMainWindowViewer* win = viewerWindow(); //cache

//TODO: add the appropriate global actions
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
//       pal.setColor(QPalette::Button, Qt::white);
      pal.setColor(QPalette::ButtonText, colr);
      widg->setPalette(pal);
//       widg->setAutoFillBackground(true);
    }
  }
}

static void ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td) {
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

  ptbp_add_widget(tb, sec, &TA_ForLoop);
  ptbp_add_widget(tb, sec, &TA_ForeachLoop);
  ptbp_add_widget(tb, sec, &TA_DoLoop);
  ptbp_add_widget(tb, sec, &TA_WhileLoop);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_IfElse);
  ptbp_add_widget(tb, sec, &TA_IfContinue);
  ptbp_add_widget(tb, sec, &TA_IfBreak);
  ptbp_add_widget(tb, sec, &TA_IfReturn);
  ptbp_add_widget(tb, sec, &TA_IfGuiPrompt);
  ptbp_add_widget(tb, sec, &TA_Switch);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_CodeBlock);
  ptbp_add_widget(tb, sec, &TA_UserScript);
  ptbp_add_widget(tb, sec, &TA_StopStepPoint);

  ////////////////////////////////////////////////////////////////////////////
  //            Var/Fun
  sec = tb->AssertSection("Var/Fun");
  QWidget* widg = tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var",
                       tabMisc::root->GetTemplateInstance(&TA_ProgVar)));
  ptbp_deco_widget(widg, tabMisc::root->GetTemplateInstance(&TA_ProgVar));
  ptbp_add_widget(tb, sec, &TA_LocalVars);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_AssignExpr);
  ptbp_add_widget(tb, sec, &TA_VarIncr);
  ptbp_add_widget(tb, sec, &TA_MemberAssign);
  ptbp_add_widget(tb, sec, &TA_MethodCall);
  ptbp_add_widget(tb, sec, &TA_MemberMethodCall);
  ptbp_add_widget(tb, sec, &TA_CssExpr);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_Function);
  ptbp_add_widget(tb, sec, &TA_FunctionCall);
  ptbp_add_widget(tb, sec, &TA_ReturnExpr);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_ProgramCall);
  ptbp_add_widget(tb, sec, &TA_ProgramCallVar);
  ptbp_add_widget(tb, sec, &TA_OtherProgramVar);

  ////////////////////////////////////////////////////////////////////////////
  //            Print/Misc
  sec = tb->AssertSection("Print/Args..");
  ptbp_add_widget(tb, sec, &TA_PrintExpr);
  ptbp_add_widget(tb, sec, &TA_PrintVar);
  ptbp_add_widget(tb, sec, &TA_Comment);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_ProgVarFmArg);
  ptbp_add_widget(tb, sec, &TA_MemberFmArg);
  ptbp_add_widget(tb, sec, &TA_DataColsFmArgs);
  ptbp_add_widget(tb, sec, &TA_SelectEditsFmArgs);
  ptbp_add_widget(tb, sec, &TA_RegisterArgs);

  ////////////////////////////////////////////////////////////////////////////
  //            Misc Fun
  sec = tb->AssertSection("Misc Fun");
  ptbp_add_widget(tb, sec, &TA_StaticMethodCall);
  ptbp_add_widget(tb, sec, &TA_MathCall);
  ptbp_add_widget(tb, sec, &TA_RandomCall);
  ptbp_add_widget(tb, sec, &TA_MiscCall);
  // add other spec meth calls here..

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_DataProcCall);
  ptbp_add_widget(tb, sec, &TA_DataAnalCall);
  ptbp_add_widget(tb, sec, &TA_DataGenCall);
  ptbp_add_widget(tb, sec, &TA_ImageProcCall);

  ////////////////////////////////////////////////////////////////////////////
  //            Data processing
  sec = tb->AssertSection("Data"); //note: need to keep it short
  ptbp_add_widget(tb, sec, &TA_DataLoop);
  ptbp_add_widget(tb, sec, &TA_ResetDataRows);
  ptbp_add_widget(tb, sec, &TA_AddNewDataRow);
  ptbp_add_widget(tb, sec, &TA_DoneWritingDataRow);
  ptbp_add_widget(tb, sec, &TA_DataVarProg);
  ptbp_add_widget(tb, sec, &TA_DataVarProgMatrix);

  ////////////////////////////////////////////////////////////////////////////
  //            Data processing
  sec = tb->AssertSection("Data Proc"); //note: need to keep it short
  ptbp_add_widget(tb, sec, &TA_DataSortProg);
  ptbp_add_widget(tb, sec, &TA_DataGroupProg);
  ptbp_add_widget(tb, sec, &TA_DataSelectRowsProg);
  ptbp_add_widget(tb, sec, &TA_DataSelectColsProg);
  ptbp_add_widget(tb, sec, &TA_DataJoinProg);
  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_DataCalcLoop);
  ptbp_add_widget(tb, sec, &TA_DataCalcAddDestRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcSetDestRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcSetSrcRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcCopyCommonCols);
}

ToolBoxRegistrar ptb(ProgramToolBoxProc);
