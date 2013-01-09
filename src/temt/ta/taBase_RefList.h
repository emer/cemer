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

#ifndef taBase_RefList_h
#define taBase_RefList_h 1

// parent includes:
#include <taBase_PtrList>

// member includes:

// declare all other types mentioned but not required to include:
class IRefListClient; //

// todo: is the following used??   why #IGNORE?

class TA_API taBase_RefList: public taPtrList<taBase>,
   virtual public IMultiDataLinkClient { // #IGNORE a primitive taBase list type, that uses SmartRef semantics to manage the items -- note: this list does NOT manage ownership/lifetimes
public:
  void                  setOwner(IRefListClient* own_);

  taBase_RefList(IRefListClient* own_ = NULL) {Initialize(); setOwner(own_);}
  ~taBase_RefList();

  int UpdatePointers_NewPar(taBase* old_par, taBase* new_par);

public: // IDataLinkClient i/f
  void*                 This() {return this;}  // #IGNORE
  override TypeDef*     GetTypeDef() const {return &TA_taBase_RefList;} // #IGNORE
protected: // we actually protect these
  override void         DataLinkDestroying(taDataLink* dl); // #IGNORE
  override void         DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
     // #IGNORE

protected:
  IRefListClient*       m_own; // optional owner

  String        El_GetName_(void* it) const { return ((taBase*)it)->GetName(); }
  TypeDef*      El_GetType_(void* it) const {return ((taBase*)it)->GetTypeDef();}
  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
private:
  void Initialize();
};

#endif // taBase_RefList_h
