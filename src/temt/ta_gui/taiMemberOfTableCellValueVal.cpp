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

#include "taiMemberOfTableCellValueVal.h"
#include <taiWidgetField>
#include <DataTableCell>
#include <DataTable>
#include <DataCol>

int taiMemberOfTableCellValueVal::BidForMember(MemberDef* md, TypeDef* td){
  if(td->InheritsFrom(&TA_DataTableCell) && (md->name == "value"))
    return taiMember::BidForMember(md,td)+10;
  return 0;
}

taiWidget* taiMemberOfTableCellValueVal::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
                                                       QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  taiWidgetField* rval = new taiWidgetField(&TA_taString, host_, par, gui_parent_, flags_);
  return rval;
}

void taiMemberOfTableCellValueVal::GetImage_impl(taiWidget* dat, const void* base) {
  DataTableCell* dtc = (DataTableCell*)base;
  taiWidgetField* field_rep = (taiWidgetField*)dat;
  if(!field_rep) return;
  field_rep->GetImage(dtc->value_column->GetValAsString(dtc->view_row));
}

void taiMemberOfTableCellValueVal::GetMbrValue_impl(taiWidget* dat, void* base) {
  DataTableCell* dtc = (DataTableCell*)base;
  taiWidgetField* field_rep = (taiWidgetField*)dat;
  if(!field_rep) return;
  dtc->value_column->SetValAsString(field_rep->GetValue(), dtc->view_row);
}
