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

#ifndef SigLinkSignal_h
#define SigLinkSignal_h 1

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

/* What happens on various Signals

  SLS_ITEM_UPDATED
    taiEditorOfClass: does a GetImage

  NOTE: you can use the test (sls <= SLS_ITEM_UPDATED_ND) to test for both versions

  NOTE: for lists and groups, all the SLSs relating to item add/remove/reorder
    are sequential, and can be tested via ?

  PLEASE keep numbers updated as some debuggers do not record the enum symbols..
*/

enum SigLinkSignal { /* reason why SigEmit being called, as well as defining ops (also used by taBase and other classes) -- some operations will emit multiple SigEmit calls */
  SLS_ITEM_UPDATED = 0, // 0 after user edits (or load) ex. taBase::UpdateAfterEdit call; ops not used
  SLS_ITEM_UPDATED_ND,  // 1 same as IU, but doesn't invoke Dirty (to avoid circular dirtying)
  SLS_CHILD_ITEM_UPDATED = 3, // 3 op1=item; can optionally be invoked by an owned object (usually a member, usually not list/group items) -- owner can ignore this, or do something with it

  SLS_ARY_SIZE_CHANGED, // 4 this is the only notify we send from arrays

  SLS_LIST_INIT = 10,   // 10
  SLS_LIST_ITEM_UPDATE, // 11 op1=item
  SLS_LIST_ITEM_INSERT, // 12 op1=item, op2=item_after, null=at beginning
  SLS_LIST_ITEM_REMOVE, // 13 op1=item -- note, item not DisOwned yet, but has been removed from list
  SLS_LIST_ITEM_MOVED,  // 14 op1=item, op2=item_after, null=at beginning
  SLS_LIST_ITEMS_SWAP,  // 15 op1=item1, op2=item2
  SLS_LIST_SORTED,      // 16 after sorting; ops not used

  SLS_GROUP_UPDATE = 21,// 21 op1=group, typically called for group name change
  SLS_GROUP_INSERT,     // 22 op1=group, op2=group_after, null=at beginning
  SLS_GROUP_REMOVE,     // 23 op1=group -- note, item not DisOwned yet, but has been removed from list
  SLS_GROUP_MOVED,      // 24 op1=group, op2=group_after, null=at beginning
  SLS_GROUPS_SWAP,      // 25 op1=group1, op2=group2
  SLS_GROUPS_SORTED,    // 26 op1=group1, op2=group2

  SLS_GROUP_ITEM_UPDATE = 31, // 31 op1=item
  SLS_GROUP_ITEM_INSERT, // 32 op1=item, op2=item_after, null=at beginning
  SLS_GROUP_ITEM_REMOVE, // 33 op1=item -- note, item not DisOwned yet, but has been removed from list
  SLS_GROUP_ITEM_MOVED,  // 34 op1=item, op2=item_after, null=at beginning
  SLS_GROUP_ITEMS_SWAP,  // 35 op1=item1, op2=item2
  SLS_GROUP_LIST_SORTED, // 36 after sorting; ops not used

  SLS_UPDATE_VIEWS = 40, // 40 no ops; sent for UpdateAllViews
  SLS_REBUILD_VIEWS,     // 41 no ops; sent to DataViews for RebuildAllViews

  SLS_STRUCT_UPDATE_BEGIN = 50, // 50 for some updating, like doing Layer->Build, better for gui to just do one
  SLS_STRUCT_UPDATE_END,  // 51 update operation at the end of everything
  SLS_STRUCT_UPDATE_ALL,  // 52 special case for rebuilding entire gui display (e.g., tree view browser) associated with an item when changes have occurred that have not been tracked (e.g., during the load process, where changes are blocked)
  SLS_DATA_UPDATE_BEGIN = 60, // 60 for some data changes, like various log updates, better for gui to just do one
  SLS_DATA_UPDATE_END,  // 61 update operation at the end of everything

  SLS_ITEM_DELETING = 70,  // 70 NOTE: not used in standard SigEmit calls, but may be used by forwarders, ex. taDataMonitor

  SLS_USER_DATA_UPDATED = 80, // called when we create user data, or when user data is updated -- helps gui snooping so it doesn't do detailed UserData checks on every ITEM_UPDATED or such

  SLS_RESOLVE_NOW = 90, // called ex by SomeMethod() buttons to request all gui clients of the guy to apply their outstanding changes

  SLS_LIST_MIN          = SLS_LIST_INIT,
  SLS_LIST_MAX          = SLS_LIST_SORTED,
  SLS_GROUP_MIN         = SLS_GROUP_UPDATE,
  SLS_GROUP_MAX         = SLS_GROUPS_SWAP,
  SLS_LIST_ITEM_MIN     = SLS_LIST_ITEM_UPDATE,
  SLS_LIST_ITEM_MAX     = SLS_LIST_SORTED,
  SLS_LIST_ORDER_MIN    = SLS_LIST_ITEM_INSERT, // anything related to item ordering
  SLS_LIST_ORDER_MAX    = SLS_LIST_SORTED,
  SLS_GROUP_ITEM_MIN    = SLS_GROUP_ITEM_UPDATE,
  SLS_GROUP_ITEM_MAX    = SLS_GROUP_LIST_SORTED,
  SLS_LIST_ITEM_TO_GROUP_ITEM_MIN = SLS_LIST_ITEM_UPDATE, // for checking to translate for item->item
  SLS_LIST_ITEM_TO_GROUP_ITEM_MAX = SLS_LIST_SORTED, // for checking to translate for item->item
  SLS_LIST_ITEM_TO_GROUP_MIN = SLS_LIST_ITEM_UPDATE, // for checking to translate for item->item
  SLS_LIST_ITEM_TO_GROUP_MAX = SLS_LIST_SORTED, // for checking to translate for item->item
  SLS_XXX_UPDATE_MIN = SLS_STRUCT_UPDATE_BEGIN, // for forwarding these guys, ex. Matrix
  SLS_XXX_UPDATE_MAX = SLS_DATA_UPDATE_END, // for forwarding these guys, ex. Matrix
};

/* SigLinkSignal Notes

   GROUP_xx notifications are simply the LIST_ITEM notifications of the gp list,
   passed on to the owning Group as GROUP_ ops
   (by adding the SLS_List_Group_Offset to the op code)
*/

#define SLS_ListItem_GroupItem_Offset   (SLS_GROUP_ITEM_MIN - SLS_LIST_ITEM_TO_GROUP_ITEM_MIN)
#define SLS_ListItem_Group_Offset       (SLS_GROUP_MIN - SLS_LIST_ITEM_TO_GROUP_MIN)

#endif // SigLinkSignal_h
