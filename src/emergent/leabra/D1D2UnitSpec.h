// this is included directly in LeabraExtraUnitSpecs_cpp / _cuda
// {

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

  INLINE void Initialize_core() {
    deep.mod_thr = 0.1f;         // default is .1
  }
  // #IGNORE

  INLINE int  GetStateSpecType() const override
  { return LEABRA_NETWORK_STATE::T_D1D2UnitSpec; }

