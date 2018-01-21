// Copyright 2018, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
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

#include "NetStateText.h"

#include <Network>
#include <MemberDef>

TA_BASEFUNS_CTORS_DEFN(NetStateText);
TA_BASEFUNS_CTORS_DEFN(NetViewStateItem);
TA_BASEFUNS_CTORS_DEFN(NetViewStateItem_List);

void NetViewStateItem::Initialize() {
}

NetViewStateItem::NetViewStateItem(String var_name, bool is_net_member, bool do_display, int the_width) {
  name = var_name;
  net_member = is_net_member;
  display = do_display;
  width = the_width;
  found = true;
}

void NetStateText::MoveItem(int from_index, int to_index) {
  state_items.MoveIdx(from_index, to_index);
}

void NetStateText::GetItems(Network* net) {
  if(!net) return;
    
  // clear found flag - later remove any items not found
  for (int i=0; i<state_items.size; i++) {
    GetItem(i)->found = false;
  }
  
  // get the network members
  TypeDef* td = net->GetTypeDef();
  for(int i=0; i < td->members.size ; i++) {
    MemberDef* md = td->members[i];
    if(!md->HasOption("VIEW")) continue;
    
    bool add = true;
    NetViewStateItem* existing_item = state_items.FindName(md->name);
    if (existing_item) {
      add = false;
      existing_item->found = true;
    }
    if (add) {
      NetViewStateItem* item = new NetViewStateItem(md->name, true, false, 8);
      String name = item->name;
      // default display items
      if (name == "batch" || name == "epoch" || name == "trial" || name == "quarter"
          || name == "cycle" || name == "sse" || name == "trial_name" || name == "output_name") {
        item->display = true;
      }
      String def_width_str = md->OptionAfter("VIEW_WIDTH_");
      if (def_width_str.nonempty()) {
        int def_width = (int)def_width_str;
        if (def_width > 0) {
          item->width = def_width;
        }
      }
      state_items.Add(item);
    }
  }
  
  // Now the Network monitor items
  DataTable* monitor_data = &net->mon_data;
  if (monitor_data) {
    for (int i=0; i<monitor_data->data.size; i++) {
      String mon_item_name = monitor_data->data.SafeEl(i)->GetName();
      bool add = true;
      NetViewStateItem* existing_item = state_items.FindName(mon_item_name);
      if (existing_item) {
        add = false;
        existing_item->found = true;
      }
      if (add) {
        NetViewStateItem* item = new NetViewStateItem(mon_item_name, false, true, 8);
        state_items.Add(item);
      }
    }
  }
  
  // remove not found
  for (int i=state_items.size -1; i>=0; i--) {
    if (!GetItem(i)->found) {
      state_items.RemoveIdx(i);
    }
  }
}

void NetStateText::ShowItem(const String& name, bool show) {
  NetViewStateItem* item = state_items.FindName(name);
  if (item) {
    item->display = show;
  }
}

int NetStateText::GetItemDisplayWidth(const String& name) {
  NetViewStateItem* item = state_items.FindName(name);
  if (item) {
    return item->width;
  }
  return -1;
}

void NetStateText::SetItemDisplayWidth(const String& name, int width) {
  NetViewStateItem* item = state_items.FindName(name);
  if (item) {
    item->width = width;
  }
}

