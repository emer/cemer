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

#ifndef DataChangedReason_h
#define DataChangedReason_h 1

// GROUP_ITEM_ events are all the leaf LIST events of a group, sent to the root group

/*
  Batch Updating

  Batch updating is used in pairs around complex/compound updating operations, in order
  to defer client notification until the end of the operation. The implementation should
  count the BEGIN/END calls. In addition, a child notification can be omitted when a
  parent object is also inside a corresponding batch operations whose notification will
  trigger the child operation anyways.

  There are two kinds of batch updates: Structural (str), and parametric (data).
  Structural updates will or might cause some kind of major structural or configural change
  to the data item, such that views of the item will probably be reset then rebuilt.
  Parameteric updates involve only changes to the values of existing data items, and
  typically don't involve any structural changes, such as addition or deletion of items, etc.
  It is assumed that the type of update done by a Parameteric update is implied in the more
  comprehensive Structural update.

  Here are the rules that should be applied regarding notifications and state:

  Parent State  Child State     effect. Child State     Child signal end of child batch?
  none          str             str                     yes, str
  none          data            data                    yes, data
  str           str             str                     no
  data          str             str                     yes, str
  str           data            str                     no
  data          data            data                    no

  The type of update being performed only needs to be communicated at the beginning, since
  an end operation can use the current state and parent state to determine the new effective
  state and whether an update should be triggered.

*/

/* What happens on various DCRs

  DCR_ITEM_UPDATED
    taiEditDataHost: does a GetImage

  NOTE: you can use the test (dcr <= DCR_ITEM_UPDATED_ND) to test for both versions

  NOTE: for lists and groups, all the DCRs relating to item add/remove/reorder
    are sequential, and can be tested via ?

  PLEASE keep numbers updated as some debuggers do not record the enum symbols..
*/

enum DataChangedReason { /* reason why DataChanged being called, as well as defining ops (also used by taBase and other classes) -- some data change operations will emit multiple DataChanged calls */
  DCR_ITEM_UPDATED = 0, // 0 after user edits (or load) ex. taBase::UpdateAfterEdit call; ops not used
  DCR_ITEM_UPDATED_ND,  // 1 same as IU, but doesn't invoke Dirty (to avoid circular dirtying)
  DCR_CHILD_ITEM_UPDATED = 3, // 3 op1=item; can optionally be invoked by an owned object (usually a member, usually not list/group items) -- owner can ignore this, or do something with it

  DCR_ARY_SIZE_CHANGED, // 4 this is the only notify we send from arrays

  DCR_LIST_INIT = 10,   // 10
  DCR_LIST_ITEM_UPDATE, // 11 op1=item
  DCR_LIST_ITEM_INSERT, // 12 op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEM_REMOVE, // 13 op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_LIST_ITEM_MOVED,  // 14 op1=item, op2=item_after, null=at beginning
  DCR_LIST_ITEMS_SWAP,  // 15 op1=item1, op2=item2
  DCR_LIST_SORTED,      // 16 after sorting; ops not used

  DCR_GROUP_UPDATE = 21,// 21 op1=group, typically called for group name change
  DCR_GROUP_INSERT,     // 22 op1=group, op2=group_after, null=at beginning
  DCR_GROUP_REMOVE,     // 23 op1=group -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_MOVED,      // 24 op1=group, op2=group_after, null=at beginning
  DCR_GROUPS_SWAP,      // 25 op1=group1, op2=group2
  DCR_GROUPS_SORTED,    // 26 op1=group1, op2=group2

  DCR_GROUP_ITEM_UPDATE = 31, // 31 op1=item
  DCR_GROUP_ITEM_INSERT, // 32 op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEM_REMOVE, // 33 op1=item -- note, item not DisOwned yet, but has been removed from list
  DCR_GROUP_ITEM_MOVED,  // 34 op1=item, op2=item_after, null=at beginning
  DCR_GROUP_ITEMS_SWAP,  // 35 op1=item1, op2=item2
  DCR_GROUP_LIST_SORTED, // 36 after sorting; ops not used

  DCR_UPDATE_VIEWS = 40, // 40 no ops; sent for UpdateAllViews
  DCR_REBUILD_VIEWS,     // 41 no ops; sent to DataViews for RebuildAllViews

  DCR_STRUCT_UPDATE_BEGIN = 50, // 50 for some updating, like doing Layer->Build, better for gui to just do one
  DCR_STRUCT_UPDATE_END,  // 51 update operation at the end of everything
  DCR_STRUCT_UPDATE_ALL,  // 52 special case for rebuilding entire gui display (e.g., tree view browser) associated with an item when changes have occurred that have not been tracked (e.g., during the load process, where changes are blocked)
  DCR_DATA_UPDATE_BEGIN = 60, // 60 for some data changes, like various log updates, better for gui to just do one
  DCR_DATA_UPDATE_END,  // 61 update operation at the end of everything

  DCR_ITEM_DELETING = 70,  // 70 NOTE: not used in standard DataChanged calls, but may be used by forwarders, ex. taDataMonitor

  DCR_USER_DATA_UPDATED = 80, // called when we create user data, or when user data is updated -- helps gui snooping so it doesn't do detailed UserData checks on every ITEM_UPDATED or such

  DCR_RESOLVE_NOW = 90, // called ex by SomeMethod() buttons to request all gui clients of the guy to apply their outstanding changes

#ifndef __MAKETA__
  DCR_LIST_MIN          = DCR_LIST_INIT,
  DCR_LIST_MAX          = DCR_LIST_SORTED,
  DCR_GROUP_MIN         = DCR_GROUP_UPDATE,
  DCR_GROUP_MAX         = DCR_GROUPS_SWAP,
  DCR_LIST_ITEM_MIN     = DCR_LIST_ITEM_UPDATE,
  DCR_LIST_ITEM_MAX     = DCR_LIST_SORTED,
  DCR_LIST_ORDER_MIN    = DCR_LIST_ITEM_INSERT, // anything related to item ordering
  DCR_LIST_ORDER_MAX    = DCR_LIST_SORTED,
  DCR_GROUP_ITEM_MIN    = DCR_GROUP_ITEM_UPDATE,
  DCR_GROUP_ITEM_MAX    = DCR_GROUP_LIST_SORTED,
  DCR_LIST_ITEM_TO_GROUP_ITEM_MIN = DCR_LIST_ITEM_UPDATE, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_ITEM_MAX = DCR_LIST_SORTED, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_MIN = DCR_LIST_ITEM_UPDATE, // for checking to translate for item->item
  DCR_LIST_ITEM_TO_GROUP_MAX = DCR_LIST_SORTED, // for checking to translate for item->item
  DCR_XXX_UPDATE_MIN = DCR_STRUCT_UPDATE_BEGIN, // for forwarding these guys, ex. Matrix
  DCR_XXX_UPDATE_MAX = DCR_DATA_UPDATE_END, // for forwarding these guys, ex. Matrix
#endif
};

/* DataChangedReason Notes

   GROUP_xx notifications are simply the LIST_ITEM notifications of the gp list,
   passed on to the owning Group as GROUP_ ops
   (by adding the DCR_List_Group_Offset to the op code)
*/

#define DCR_ListItem_GroupItem_Offset   (DCR_GROUP_ITEM_MIN - DCR_LIST_ITEM_TO_GROUP_ITEM_MIN)
#define DCR_ListItem_Group_Offset       (DCR_GROUP_MIN - DCR_LIST_ITEM_TO_GROUP_MIN)

#endif // DataChangedReason_h
