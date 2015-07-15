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

#ifndef ProgObjList_h
#define ProgObjList_h 1

// parent includes:
#include <taBase_List>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; // 

// Note: object operations per se don't affect Program::stale, but
// they will indirectly to the extent that adding/removing them
// causes a corresponding var to get created

taTypeDef_Of(ProgObjList);

class TA_API ProgObjList: public taBase_List {
  // ##CAT_Program A list of program objects (just a taBase list with proper update actions to update variables associated with objects)
INHERITED(taBase_List)
public:

  virtual DataTable* NewDataTable(int n_tables = 1);
  // #BUTTON make new DataTable object(s) for local use in Program
  virtual DataTable* OneNewTable();
  // #EXPERT goofy callback for ChooseNew callback

  void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;
  virtual void  GetVarsForObjs();
  // automatically create variables for objects in parent program

  virtual void  StructUpdateEls(bool begin);
  // runs StructUpdate(begin) on all the elements in the list

  String       GetTypeDecoKey() const override { return "ProgVar"; }

  TA_BASEFUNS_NOCOPY(ProgObjList);
protected:
  void*         El_Own_(void* it); // give anon objs a name

private:
  void Initialize() { SetBaseType(&TA_taNBase); } // need name for var, owner to stream!
  void Destroy() { }
};

#endif // ProgObjList_h
