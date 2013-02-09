// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef SendCons_List_h
#define SendCons_List_h 1

// parent includes:
#include <taList>
#include <SendCons>

// member includes:
class Projection; //
class Layer; //

// declare all other types mentioned but not required to include:

eTypeDef_Of(SendCons_List);

class E_API SendCons_List: public taList<SendCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of sending connections, one per projection
INHERITED(taList<SendCons>)
public:
  // projection-related functions for operations on sub-groups of the group
  virtual SendCons*     NewPrjn(Projection* prjn);
  // #CAT_Structure create a new sub_group from given projection, with given ownership (own_cons)

  virtual int           FindPrjnIdx(Projection* prjn) const;
  // #CAT_Structure find index of send cons associated with given projection
  virtual SendCons*     FindPrjn(Projection* prjn) const;
  // #CAT_Structure find send cons associated with given projection

  virtual int           FindToIdx(Layer* to) const;
  // #CAT_Structure find index of send cons that sends to given layer
  virtual SendCons*     FindTo(Layer* to) const;
  // #CAT_Structure find send cons that sends to given layer

  virtual int           FindToNameIdx(const String& to_nm) const;
  // #CAT_Structure find index of send cons that sends to given layer named to_nm
  virtual SendCons*     FindToName(const String& to_nm) const;
  // #CAT_Structure find send cons that sends to given layer named to_nm

  virtual int           FindTypeToIdx(TypeDef* prjn_typ, Layer* to) const;
  // #CAT_Structure find index of send cons that sends prjn of given type to layer
  virtual SendCons*     FindTypeTo(TypeDef* prjn_typ, Layer* to) const;
  // #CAT_Structure find send cons that sends prjn of given type to layer

  virtual bool          RemovePrjn(Projection* prjn);
  // #CAT_Structure remove sending connections associated with given projection
  virtual bool          RemoveTo(Layer* to);
  // #MENU #CAT_Structure remove sending connections to given layer

  override String       GetTypeDecoKey() const { return "Connection"; }

  TA_BASEFUNS_NOCOPY(SendCons_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // SendCons_List_h
