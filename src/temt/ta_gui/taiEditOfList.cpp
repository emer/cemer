// Copyright 2013-2018, Regents of the University of Colorado,
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

#include "taiEditOfList.h"
#include <taList_impl>
#include <taiEditorOfListCompact>
#include <taiEditorOfList>



int taiEditOfList::BidForEdit(TypeDef* td) {
  if (td->InheritsFrom(TA_taList_impl))
    return taiEdit::BidForEdit(td)+1;
  return 0;
}

taiEditorOfClass* taiEditOfList::CreateDataHost(void* base, bool readonly, bool modal) {
  // compact is either specified explicitly,
  // or we must use it if the base_type of the list requires inline
  bool use_compact = false;
  if (!typ->HasOption("NO_CHILDREN_INLINE")) {
    use_compact = typ->HasOption("CHILDREN_INLINE");
    if (!use_compact && base) { // try checking base type
      taList_impl* lst = (taList_impl*)base;
      use_compact = lst->el_typ->it->requiresInline();
    }
  }
  if (use_compact)
    return new taiEditorOfListCompact(base, typ, readonly, modal);
  else
    return new taiEditorOfList(base, typ, readonly, modal);
}
