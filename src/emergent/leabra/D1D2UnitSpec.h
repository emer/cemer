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

#ifndef D1D2UnitSpec_h
#define D1D2UnitSpec_h 1

// parent includes:
#include <LeabraUnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(D1D2UnitSpec);

class E_API D1D2UnitSpec : public LeabraUnitSpec {
  // base class for basal ganglia and amygdala neurons that express dopamine D1 vs. D2 receptors and thus learn differentially from dopamine -- this also supports a multiplicative up-state dynamic using the deep* variables, and sent to other neurons via a SendDeepNormConSpec
INHERITED(LeabraUnitSpec)
public:
  enum DAReceptor {             // type of dopamine receptor expressed
    D1R,                        // Primarily expresses Dopamine D1 Receptors -- dopamine is excitatory and bursts of dopamine lead to increases in synaptic weight, while dips lead to decreases -- direct pathway in dorsal striatum
    D2R,                        // Primarily expresses Dopamine D2 Receptors -- dopamine is inhibitory and bursts of dopamine lead to decreases in synaptic weight, while dips lead to increases -- indirect pathway in dorsal striatum
  };

  enum Valence {                // appetitive vs. aversive valence US-coding (VENTRAL only)
    APPETITIVE,                 // has an appetitive (positive valence) US coding
    AVERSIVE,                   // has an aversive (negative valence) US coding
  };

  // note subclasses will want to include these enums in their own way, so we don't do
  // it here..
  // DAReceptor            dar;            // type of dopamine receptor: D1 vs. D2


  /*
  void Compute_DeepRaw(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Activation update the deep_raw activations -- assumes checks have already been done
  void Send_DeepRawNetin(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep send deep5b netinputs through SendDeepRawConSpec connections
  void Send_DeepRawNetin_Post(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep send context netinputs through SendDeepRawConSpec connections -- post processing rollup

  void Compute_DeepRawNorm(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep compute deep_raw_norm values from deep_raw values
  void Compute_DeepNorm(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep compute deep_norm values from deep_raw and deep_ctxt values
  void Compute_DeepMod(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep compute deep_mod values from deep_norm
    
  void Send_DeepNormNetin(LeabraUnitVars* uv, LeabraNetwork* net,
                                   int thr_no) override;
  // #CAT_Deep send deep_norm netinputs through SendDeepNormConSpec connections
  void Send_DeepNormNetin_Post(LeabraUnitVars* uv, LeabraNetwork* net,
                                        int thr_no) override;
  // #CAT_Deep send deep_norm netinputs through SendDeepNormConSpec connections -- post processing rollup
  
  void Compute_DeepStateUpdt(LeabraUnitVars* uv, LeabraNetwork* net, int thr_no) override;
  // #CAT_Deep state update for deep leabra -- typically at start of new alpha trial -- copy deep_mod from deep_norm, deep_ctxt from deep_ctxt_net

  */

  TA_SIMPLE_BASEFUNS(D1D2UnitSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // D1D2UnitSpec_h
