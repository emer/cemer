// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

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
  STATE_CLASS(MatrixActSpec)         matrix;         // #CONDSHOW_ON_matrix_patch:MATRIX parameters for Matrix activation dynamics
  bool                  deep_mod_zero;  // #CONDSHOW_ON_matrix_patch:PATCH for modulation coming from the BLA via deep_mod_net -- when this modulation signal is below zero, does it have the ability to zero out the patch activations?  i.e., is the modulation required to enable patch firing?
  
  INIMPL void  Init_UnitState(UNIT_STATE* uv, NETWORK_STATE* net, int thr_no) override;

  INIMPL virtual GateType      MatrixGateType(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no);
  // get type of gating that given unit participates in
  
  INLINE void   Compute_ApplyInhib
  (LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no, LEABRA_LAYER_STATE* lay, float ival) override {
    inherited::Compute_ApplyInhib(u, net, thr_no, lay, ival);

    // note shunting previously applied
    if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
      GateType gt = MatrixGateType(u, net, thr_no);
      if(gt == OUT) {
        u->gc_i += matrix.out_ach_inhib * (1.0f - u->ach);
      }
    }
  }
  // note: called in compute_act -- applies ach inhibition of output

  INLINE virtual void  Compute_PatchShunt(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    // note: recv this in prior Act_Post from  patch unit spec, apply in Act
    if(u->shunt > 0.0f) {         // todo: could be more quantitative here..
      u->da_p *= matrix.patch_shunt;
      if(matrix.shunt_ach) {
        u->ach *= matrix.patch_shunt;
      }
    }
  }
  // compute patch shunting of da and ach from shunt variable received in prior cycle Act_Post stage -- updated in compute_act prior to new acts
  
  INLINE virtual void  SaveGatingThal(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) {
    if(u->thal_gate > 0.0f) {
      u->thal_cnt = u->thal;
      u->act_g = GetRecAct(u);       // todo: experiment with learning based on this!
    }
  }
  // save gating value into thal_cnt and gated activation into act_g when thal_gate indicates gating -- note 1 trial delayed from actual gating -- updated in compute_act *prior* to computing new act, so it reflects actual gating cycle activation

  INLINE void  SaveGatingAct(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net, int thr_no) override { };
  
  INLINE void  Compute_Act_Rate(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    // note: critical for this to come BEFORE updating new act!
    if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
      Compute_PatchShunt(u, net, thr_no);
      SaveGatingThal(u, net, thr_no);
    }
    // note: ApplyInhib called here:
    inherited::Compute_Act_Rate(u, net, thr_no);
  }
    
  INLINE void   Compute_Act_Spike(LEABRA_UNIT_STATE* u, LEABRA_NETWORK_STATE* net, int thr_no) override {
    // note: critical for this to come BEFORE updating new act!
    if(dorsal_ventral == DORSAL && matrix_patch == MATRIX) {
      Compute_PatchShunt(u, net, thr_no);
      SaveGatingThal(u, net, thr_no);
    }
    inherited::Compute_Act_Spike(u, net, thr_no);
  }
    
  INIMPL void  Compute_DeepMod(LEABRA_UNIT_STATE* uv, LEABRA_NETWORK_STATE* net, int thr_no) override;

  INLINE void Initialize_core() {
    dar = D1R;    matrix_patch = MATRIX;    dorsal_ventral = DORSAL;
    valence = APPETITIVE;
    deep_mod_zero = true;
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_MSNUnitSpec; }

