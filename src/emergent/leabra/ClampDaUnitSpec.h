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

#ifndef ClampDaUnitSpec_h
#define ClampDaUnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ClampDaUnitSpec);

class E_API ClampDaUnitSpec : public LeabraUnitSpec {
  // a dopamine unit that you can just clamp to any value and it will send it to other layer's dav values
INHERITED(LeabraUnitSpec)
public:
  enum  SendDaMode {            // when to send da values to other layers
    CYCLE,                      // send every cycle
    PLUS_START,                 // start sending at start of plus phase
    PLUS_END,                   // send only at the end of plus phase
  };

  SendDaMode    send_da;        // when to send da values

  virtual void  Send_Da(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // send the da value to sending projections

  void	Compute_Act_Rate(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;
  void	Compute_Act_Spike(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  void	Quarter_Final(LeabraUnitVars* u, LeabraNetwork* net, int thr_no) override;

  TA_SIMPLE_BASEFUNS(ClampDaUnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void	Defaults_init() { };
};

#endif // ClampDaUnitSpec_h
