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

#ifndef RecvCons_List_h
#define RecvCons_List_h 1

// parent includes:
#include <taList>
#include <RecvCons>

// member includes:

// declare all other types mentioned but not required to include:
class Projection; //
class Layer; //

TypeDef_Of(RecvCons_List);

class EMERGENT_API RecvCons_List: public taList<RecvCons> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of receiving connections, one per projection
INHERITED(taList<RecvCons>)
public:
  virtual RecvCons*     NewPrjn(Projection* prjn);
  // #CAT_Structure create a new recv cons from given projection, with given ownership (own_cons)

  virtual int           FindPrjnIdx(Projection* prjn) const;
  // #CAT_Structure find index of recv cons associated with given projection
  virtual RecvCons*     FindPrjn(Projection* prjn) const;
  // #CAT_Structure find recv cons associated with given projection

  virtual int           FindFromIdx(Layer* from) const;
  // #CAT_Structure find index of recv cons that receives from given layer
  virtual RecvCons*     FindFrom(Layer* from) const;
  // #CAT_Structure find recv cons that receives from given layer

  virtual int           FindFromNameIdx(const String& fm_nm) const;
  // #CAT_Structure find index of recv cons that receives from given layer named fm_nm
  virtual RecvCons*     FindFromName(const String& fm_nm) const;
  // #CAT_Structure find recv cons that receives from given layer named fm_nm

  virtual int           FindTypeFromIdx(TypeDef* prjn_typ, Layer* from) const;
  // #CAT_Structure find index of recv cons that recvs prjn of given type from layer
  virtual RecvCons*     FindTypeFrom(TypeDef* prjn_typ, Layer* from) const;
  // #CAT_Structure find recv cons that recvs prjn of given type from layer

  virtual bool          RemovePrjn(Projection* prjn);
  // #CAT_Structure remove recv cons associated with given projection
  virtual bool          RemoveFrom(Layer* from);
  // #MENU #CAT_Structure remove recv cons that receives from given layer

  override String       GetTypeDecoKey() const { return "Connection"; }

  NOCOPY(RecvCons_List)
  TA_BASEFUNS(RecvCons_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // RecvCons_List_h
