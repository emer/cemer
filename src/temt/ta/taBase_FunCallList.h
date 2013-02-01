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

#ifndef taBase_FunCallList_h
#define taBase_FunCallList_h 1

// parent includes:
#include <FunCallItem>
#include <taPtrList>
#include <IMultiSigLinkClient>

// member includes:

// declare all other types mentioned but not required to include:
class taPtrList_impl; // 
class TypeDef; // 
class taiSigLink; // 
class taSigLink; // 
class taBase; // 


TypeDef_Of(taBase_FunCallList);

class TA_API taBase_FunCallList: public taPtrList<FunCallItem>
#ifndef __MAKETA__
, public IMultiSigLinkClient
#endif
{
  // #INSTANCE function call list manager
INHERITED(taPtrList<FunCallItem>)
public:

  bool  AddBaseFun(taBase* obj, const String& fun_name); // add base + function -- no check for unique on base_funs

  taBase_FunCallList() {}
  ~taBase_FunCallList() {Reset();}

public: // ITypedObject interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_taBase_FunCallList;}

public: // ISigLinkClient interface
  override void         SigLinkDestroying(taSigLink* dl);
  override void         SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {}

protected:
  override void El_Done_(void* it); // unref link

private:
  void Initialize();
  void Destroy();
};

#endif // taBase_FunCallList_h
