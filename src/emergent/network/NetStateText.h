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

#ifndef NetStateText_h
#define NetStateText_h 1

// parent includes:
#include "network_def.h"
#include <taNBase>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Network;
class String_Array;

eTypeDef_Of(NetViewStateItem);

class E_API NetViewStateItem : public taNBase {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Display misc parameters for the network display
  INHERITED(taNBase)
public:
  NetViewStateItem(String var_name, bool is_net_member, bool do_display = false, int width = 8);
  // taNBase name member is used for the variable name
  bool              net_member; // is item a network member - alternative is monitor variable
  bool              display;    // render the item?
  int               width;      // width in chars of value portion of field
  bool              found;      // set to true when rebuilding list - if not set it is stale and should be removed
  
  TA_SIMPLE_BASEFUNS(NetViewStateItem);
private:
  void  Initialize();
  void  Destroy()               { };
};

eTypeDef_Of(NetViewStateItem_List);

class E_API NetViewStateItem_List : public taList<NetViewStateItem> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##NO_EXPAND List of NetViewStateItem objects
  INHERITED(taList<NetViewStateItem>)
public:
  
  TA_BASEFUNS_NOCOPY(NetViewStateItem_List);
private:
  void Initialize()  { SetBaseType(&TA_NetViewStateItem); };
  void Destroy()     { };
};

eTypeDef_Of(NetStateText);

class E_API NetStateText : public taNBase {
  // controls a list of network variables for display in a view class
INHERITED(taNBase)
public:
  NetViewStateItem_List   state_items;    // #NO_COPY #READ_ONLY all standard net state items (i.e. marked VIEW) plua any in network owned monitor - maintains order, width, display flag
  
  virtual void            GetItems(Network* net);
  // these are the Network vars marked #VIEW (e.g. cycle, trial_name, ...) plus any vars being monitored by the network
  NetViewStateItem*       GetItem(int index) { return state_items.SafeEl(index); }
  // return the item with index or -1 if not found
  virtual void            MoveItem(int from_index, int to_index);
  // update list of state vars - item has been moved
  virtual void            ShowItem(const String& name, bool show);  // show or hide item
  // item show/hide state has changed
  virtual int             GetItemDisplayWidth(const String& name);
  // return the width in chars of the value portion of the state item
  virtual void            SetItemDisplayWidth(const String& name, int width);
  // set the width in chars of the value portion of the state item

  TA_SIMPLE_BASEFUNS(NetStateText);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // NetStateText_h
