// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

// v3_compat_qtso.cpp -- version 3 gui compatability objects, only for converting

#include "v3_compat_qtso.h"

#include "ta_qt.h"


//////////////////////////////////
//     ProcessDialog		//
//////////////////////////////////

void ProcessDialog::Ok() {
/*obs  Process* prc = (Process*)cur_base;
  if(prc->ctrl_panel.ctrl_panel == this) {
    prc->ctrl_panel.ctrl_panel = NULL;
    prc->ctrl_panel.active = false;
  }
  if(prc->InheritsFrom(&TA_SchedProcess)) {
    SchedProcess* sp = (SchedProcess*)prc;
    if(sp->im_run_proc)
      sp->Stop(); 
  }*/
  taiEditDataHost::Ok();
}

void ProcessDialog::Cancel() {
  Process* prc = (Process*)cur_base;
/*obs  if(prc->ctrl_panel.ctrl_panel == this) {
    prc->ctrl_panel.ctrl_panel = NULL;
    prc->ctrl_panel.active = false;
  } */
  if(prc->InheritsFrom(&TA_SchedProcess)) {
    SchedProcess* sp = (SchedProcess*)prc;
/*obs    if(sp->im_run_proc)
      sp->Stop(); */
  }
  taiEditDataHost::Cancel();
}

ProcessDialog::~ProcessDialog() {
//obs  CloseWindow();
}

//////////////////////////////////
//     taiProcess		//
//////////////////////////////////

int taiProcess::BidForEdit(TypeDef* td){
  if(td->InheritsFrom(&TA_Process))
    return taiEdit::BidForEdit(td) +1;
  return 0;
}

taiEditDataHost* taiProcess::CreateDataHost(void* base, bool readonly) {
  return new ProcessDialog(base, typ, readonly);
}

int taiProcess::Edit(void* base, bool readonly, const iColor* bgclr) {
  ProcessDialog* dlg = (ProcessDialog*)taiMisc::FindEdit(base, typ);
  if ((dlg == NULL) || dlg->CtrlPanel()) {
    dlg = (ProcessDialog*)CreateDataHost(base, readonly);
    dlg->Constr("", "", bgclr);
//    dlg->cancel_only = readonly;
    return dlg->Edit(false);
  }
  if(!dlg->modal) {
    dlg->Iconify(false);
    dlg->Raise();
  }
  return 2;
}

void taiProcess::Initialize()	{
  run_ie = NULL;
}

void taiProcess::Destroy() {
  if(run_ie) delete run_ie;
  run_ie = NULL;
}

Process_RunDlg::~Process_RunDlg() {
//obs  CloseWindow();
}

bool Process_RunDlg::ShowMember(MemberDef* md) {
  if((ProcessDialog::ShowMember(md)) &&
     (md->type->InheritsFrom(&TA_Counter) || md->HasOption("CONTROL_PANEL")))
    return true;
  return false;
}


//////////////////////////////////
//     taiProcessRunBox		//
//////////////////////////////////

taiEditDataHost* taiProcessRunBox::CreateDataHost(void* base, bool readonly) {
  return new Process_RunDlg(base, typ, readonly);
}


