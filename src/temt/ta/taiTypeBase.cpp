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

#include "taiTypeBase.h"
#include <iColor>


const iColor taiTypeBase::def_color;

taiTypeBase::taiTypeBase()
  : typ(0)
  , bid(0)
  , next_lower_bidder(0)
  , no_setpointer(false)
{
}

taiTypeBase::taiTypeBase(TypeDef* typ_)
  : typ(typ_)
  , bid(0)
  , next_lower_bidder(0)
  , no_setpointer(false)
{
}

taiTypeBase::~taiTypeBase() {
  delete next_lower_bidder;
  next_lower_bidder = 0;
}

// Insert this object into a list of taiTypeBase objects bidding
// for some type.  The list is sorted in order of the objects' bids.
void taiTypeBase::InsertThisIntoBidList_impl(taiTypeBase *&pHead) {
  // Walk the linked list, looking for the appropriate place to insert
  // this object.  This list is sorted in descending order of bids, so
  // keep walking until our bid exceeds (or equals) one in the list.
  taiTypeBase** ppCurr = &pHead;
  while (*ppCurr && this->bid < (*ppCurr)->bid) {
    // Move on to the next object in the list.
    ppCurr = &(*ppCurr)->next_lower_bidder;
  }

  // If this object already has subtypes linked to it, then unref them.
  // This shouldn't be needed, since typically this function is called
  // after an object is first created and its next_lower_bidder is null.
  taRefN::SafeUnRefDone(next_lower_bidder);

  // Link the current item under us, since our bid is higher.
  this->next_lower_bidder = *ppCurr;

  // Bump our reference count and insert us into the list.
  // TODO: it would be better if the taRefN() ctor set the
  // initial reference count to 1.
  taRefN::Ref(this);
  *ppCurr = this;
}
