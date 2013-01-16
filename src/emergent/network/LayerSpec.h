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

#ifndef LayerSpec_h
#define LayerSpec_h 1

// parent includes:
#include <BaseSpec>
#include <SpecPtr>

// member includes:

// declare all other types mentioned but not required to include:
class Layer; //

TypeDef_Of(LayerSpec);

class EMERGENT_API LayerSpec : public BaseSpec {
  // generic layer specification
INHERITED(BaseSpec)
public:
  virtual bool          CheckConfig_Layer(Layer* lay, bool quiet = false)
    {return true;} // #CAT_ObjectMgmt This is ONLY for spec-specific stuff; the layer still does all its default checking (incl child checking)

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  void  InitLinks();
  void  CutLinks();
  TA_BASEFUNS_NOCOPY(LayerSpec); //
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { CutLinks(); }
  void  Defaults_init()         { };
};

#endif // LayerSpec_h
