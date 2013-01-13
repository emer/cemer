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

#include "VPUnref.h"
#include <MemberDef>
#include <taSmartRef>
#include <taBase>
#include <VPUList>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>



VPUnref::VPUnref(void* base_, taBase* par, const String& p, MemberDef* md) {
  base = base_; parent = par; path = p; memb_def = md;
  name = String((intptr_t)base);
}

taBase* VPUnref::Resolve() {
  MemberDef* md;
  taBase* bs = tabMisc::root->FindFromPath(path, md);
  if(!bs)
    return NULL;
  if(md) {
    if(md->type->ptr == 1) {
      bs = *((taBase**)bs);
      if(!bs)
	return NULL;
    }
    else if(md->type->ptr != 0) {
      taMisc::Warning("ptr count != 0 in path:", path);
      return NULL;
    }
  }

  if (memb_def && memb_def->type->InheritsFrom(&TA_taSmartRef)) {
    taSmartRef& ref = *((taSmartRef*)base);
    ref = bs;
  } else {// assume it is taBase_ptr or (binary-compat) taBasePtr
    if((memb_def != NULL) && memb_def->HasOption("OWN_POINTER")) {
      if(parent == NULL)
        taMisc::Warning("NULL parent for owned pointer:",path);
      else
        taBase::OwnPointer((taBase**)base, bs, parent);
    } else 
      taBase::SetPointer((taBase**)base, bs);
  }

  if(taMisc::verbose_load >= taMisc::MESSAGES)
    taMisc::Warning("<== Resolved Reference:",path);
  if(parent != NULL)
    parent->UpdateAfterEdit();
  return bs;
}

void VPUList::Resolve() {
  if(size <= 0)
    return;
  int i=0;
  do {
    if(FastEl(i)->Resolve() != NULL)
      RemoveIdx(i);		// take off the list if resolved!
    else {
      VPUnref* vp = (VPUnref*)FastEl(i);
      String par_path;
      if(vp->parent != NULL)
	par_path = vp->parent->GetPathNames();
      taMisc::Warning("Could not resolve following path:",vp->path,
		    "in object:",par_path);
      i++;
    }
  } while(i < size);
}

void VPUList::AddVPU(void* b, taBase* par, const String& p, MemberDef* md) {
  AddUniqNameOld(new VPUnref(b,par,p,md));
  if(taMisc::verbose_load >= taMisc::MESSAGES)
    taMisc::Warning("==> Unresolved Reference:",p);
}

