// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_engine.h"

#include "ta_project.h"

////////////////////////////
//  taEngineInst  	  //
////////////////////////////

void taEngineInst::Initialize() {
  use_log = false; // set by Engine
  taBase::Own(m_engine, this);
  taBase::Own(log_table, this);
}

void taEngineInst::Destroy() {
  log_table = NULL;
  m_engine = NULL;
}

void taEngineInst::InitLinks() {
  inherited::InitLinks();
  taBase::Own(&tasks, this);
}

void taEngineInst::CutLinks() {
  tasks.CutLinks();
  inherited::CutLinks();
}

void taEngineInst::AssertLogTable() {
  if (!use_log || (bool)log_table) return;
  taProject* proj = GET_MY_OWNER(taProject);
  if (proj) {
    DataTable* dt = proj->GetNewAnalysisDataTable(m_engine->GetName());
    dt->Reset();
    log_table = dt;
  }
}

void taEngineInst::setTaskCount(int val) {
  if (val == tasks.size) return;
  else tasks.SetSize(val);
}

void taEngineInst::WriteLogRecord() {
  if (!(use_log && (bool)log_table)) return;
  if ((max_rows >= 0) && (log_table->rows >= max_rows)) return; 
  log_table->AddBlankRow();
  WriteLogRecord_impl();
}

////////////////////////////
//      taEngine  	  //
////////////////////////////

void taEngine::Initialize() {
#ifdef DEBUG
  use_log = true;
#else
  use_log = false;
#endif
  max_rows = 2000;
}

void taEngine::Destroy() {
}

taEngineInst* taEngine::MakeEngineInst_impl() const {
  taEngineInst* rval = NewEngineInst_impl();
  rval->m_engine = const_cast<taEngine*>(this);
  rval->use_log = use_log;
  rval->max_rows = max_rows;
  return rval;
}
