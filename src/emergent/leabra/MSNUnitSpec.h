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

#ifndef MSNUnitSpec_h
#define MSNUnitSpec_h 1

// parent includes:
#include <D1D2UnitSpec>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(MatrixActSpec);

class E_API MatrixActSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for matrix MSN unit activation
INHERITED(SpecMemberBase)
public:
  float         mnt_ach_inhib;  // how much does the PRESENCE of ACh from the TAN units drive extra inhibition to maintenance-gating Matrix units -- gi += mnt_ach_inhib * ach -- provides a bias for maint gating on non-reward trials
  float         out_ach_inhib;  // how much does the LACK of ACh from the TAN units drive extra inhibition to output-gating Matrix units -- gi += out_ach_inhib * (1-ach) -- provides a bias for output gating on reward trials
  
  String       GetTypeDecoKey() const override { return "UnitSpec"; }

  TA_SIMPLE_BASEFUNS(MatrixActSpec);
protected:
  SPEC_DEFAULTS;
private:
  void  Initialize();
  void  Destroy()       { };
  void  Defaults_init();
};


eTypeDef_Of(MSNUnitSpec);

class E_API MSNUnitSpec : public D1D2UnitSpec {
  // Medium Spiny Neuron, which is principal cell of the striatum -- determines the dopamine receptor type and patch / matrix specialization of the neuron, and overall anatomical location (dorsal / ventral), which are then used by the MSNConSpec and other areas in determining learning and other dynamics
INHERITED(D1D2UnitSpec)
public:
  enum MatrixPatch {            // matrix (matrisome) vs. patch (striosome) structure
    MATRIX,                     // Matrix-type units, which in dorsal project to GPe/Gpi and are primarily responsible for gating events, and in ventral are responsive to CS onsets 
    PATCH,                      // Patch-type units, which in dorsal may shunt dopamine signals, and in ventral are responsible for blocking transient dopamine bursts via shunting and dipping
  };

  enum DorsalVentral {          // dorsal vs. ventral striatum
    DORSAL,                     // dorsal striatum -- projects to GPi/e and drives gating of PFC and modulation of MSN dopamine
    VENTRAL,                    // ventral striatum -- projects to VTA, LHB, ventral pallidum -- drives updating of OFC, ACC and modulation of VTA dopamine
  };

  enum GateType {               // type of gating that Matrix unit engages in
    MAINT,                      // maintenance gating
    OUT,                        // output gating
  };
  
  DAReceptor            dar;            // type of dopamine receptor: D1 vs. D2 -- also determines direct vs. indirect pathway in dorsal striatum
  MatrixPatch           matrix_patch;   // matrix vs. patch specialization
  DorsalVentral         dorsal_ventral; // dorsal vs. ventral specialization
  Valence               valence;        // #CONDSHOW_ON_dorsal_ventral:VENTRAL US valence coding of the ventral neurons
  MatrixActSpec         matrix;         // #CONDSHOW_ON_matrix_patch:MATRIX parameters for Matrix activation dynamics
  
  virtual GateType      MatrixGateType(LeabraUnitVars* u, LeabraNetwork* net, int thr_no);
  // get type of gating that given unit participates in
  
  void Compute_ApplyInhib
    (LeabraUnitVars* uv, LeabraNetwork* net, int thr_no, LeabraLayerSpec* lspec,
     LeabraInhib* thr, float ival) override;

  TA_SIMPLE_BASEFUNS(MSNUnitSpec);
protected:
  SPEC_DEFAULTS;
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy()     { };
  void  Defaults_init();
};

#endif // MSNUnitSpec_h
