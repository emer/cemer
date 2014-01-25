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

#ifndef ClampDaLayerSpec_h
#define ClampDaLayerSpec_h 1

// parent includes:
#include <LeabraLayerSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ClampDaLayerSpec);

class E_API ClampDaLayerSpec : public LeabraLayerSpec {
  // a dopamine layer that you can just clamp to any value and it will send it to other layer's dav values
INHERITED(LeabraLayerSpec)
public:
  enum  SendDaMode {            // when to send da values to other layers
    CYCLE,                      // send every cycle
    PLUS_START,                 // send at start of plus phase
    PLUS_END,                   // send at end of plus phase
  };

  SendDaMode    send_da;        // when to send da values

  virtual void  Send_Da(LeabraLayer* lay, LeabraNetwork* net);
  // send the da value to sending projections: every cycle

  void  Compute_CycleStats(LeabraLayer* lay, LeabraNetwork* net) override;
  void	PostSettle(LeabraLayer* lay, LeabraNetwork* net) override;
  void  Compute_HardClamp(LeabraLayer* lay, LeabraNetwork* net) override;

  TA_SIMPLE_BASEFUNS(ClampDaLayerSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init() { };
};

#endif // ClampDaLayerSpec_h
