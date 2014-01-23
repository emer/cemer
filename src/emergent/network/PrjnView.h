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

#ifndef PrjnView_h
#define PrjnView_h 1

// parent includes:
#include <nvhDataView>

// member includes:

// declare all other types mentioned but not required to include:
class Projection; //
class T3PrjnNode; //
class BaseSpec; //

eTypeDef_Of(PrjnView);

class E_API PrjnView: public nvhDataView {
INHERITED(nvhDataView)
friend class NetView;
public:
  Projection*           prjn() const {return (Projection*)data();}
  T3PrjnNode*           node_so() const {return (T3PrjnNode*)inherited::node_so();}

  virtual void          SetHighlightSpec(BaseSpec* spec);

  T3_DATAVIEWFUNS(PrjnView, nvhDataView)
protected:
  void         DoHighlightColor(bool apply) override;
  void         Render_pre() override; // #IGNORE
  void         Render_impl() override; // #IGNORE
  void         Reset_impl() override; // #IGNORE
private:
  NOCOPY(PrjnView)
  void                  Initialize();
  void                  Destroy();
};

#endif // PrjnView_h
