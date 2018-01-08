// Copyright 2013-2017, Regents of the University of Colorado,
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

#include "DataOpBaseSpec.h"

#include <taLeafItr>
#include <DataTable>
#include <DataCol>
#include <Completions>

TA_BASEFUNS_CTORS_DEFN(DataOpBaseSpec);

void DataOpBaseSpec::Initialize() {
}

void DataOpBaseSpec::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  ops.CheckConfig(quiet, rval);
}

String DataOpBaseSpec::GetArgForCompletion(const String& method, const String& arg) {
  return "dt";
}

void DataOpBaseSpec::GetArgCompletionList(const String& method, const String& arg, taBase* arg_obj, const String& cur_txt, Completions& completions) {
  if (arg_obj) {
    if (arg_obj->InheritsFrom(&TA_DataTable)) {
      DataTable* table = (DataTable*)arg_obj;
      FOREACH_ELEM_IN_LIST(DataCol, col, table->data) {
        completions.object_completions.Link(col);      }
    }
  }
}

