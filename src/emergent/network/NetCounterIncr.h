// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef NetCounterIncr_h
#define NetCounterIncr_h 1

// parent includes:
#include <NetCounterBase>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetCounterIncr);

class E_API NetCounterIncr: public NetCounterBase { 
  // increment a network counter: program keeps a local version of the counter, and updates both this and the network's copy
INHERITED(NetCounterBase)
public:
  String	GetDisplayName() const override;
  String	GetToolbarName() const override { return "net ctr inc"; }
  bool    CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool    CvtFmCode(const String& code) override;

  PROGEL_SIMPLE_BASEFUNS(NetCounterIncr);
protected:
  bool		GenCssBody_impl(Program* prog) override;

private:
  void	Initialize() { };
  void	Destroy() { };
};

#endif // NetCounterIncr_h
