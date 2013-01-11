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

#include "taOBase.h"

#include <taDataLink>
#include <UserDataItem_List>

void taOBase::Destroy() {
  CutLinks();
}

void taOBase::CutLinks() {
  if (m_data_link) {
    m_data_link->DataDestroying(); // link NULLs our pointer
    delete m_data_link; // NULLS the ref
  }
  owner = NULL;
  if (user_data_) {
    delete user_data_;
    user_data_ = NULL;
  }
  inherited::CutLinks();
}

void taOBase::Copy_(const taOBase& cp) {
  if (user_data_) {
    user_data_->Reset(); // note: we just leave an empty list if no cp.userdata
    if (cp.user_data_)
      user_data_->Copy(*cp.user_data_);
  } else if (cp.user_data_ && (cp.user_data_->size > 0)) {
    GetUserDataList(true)->Copy(*cp.user_data_);
  }
}

UserDataItem_List* taOBase::GetUserDataList(bool force) const {
  if (!user_data_ && force) {
    user_data_ = new UserDataItem_List;
    taOBase* ths =  const_cast<taOBase*>(this); // note: harmless const casts
    taBase::Own(user_data_, ths);
    user_data_->el_typ = &TA_UserDataItem; // set default type to create
    ths->DataChanged(DCR_USER_DATA_UPDATED);
  }
  return user_data_;
}

