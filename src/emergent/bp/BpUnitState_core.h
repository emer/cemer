// this contains core shared code, and is included directly in BpUnitState _cpp.h, _cuda.h
//{
  float         bias_pdw;       // #VIEW_HOT previous bias weight change
  float 	err; 		// #VIEW_HOT error value -- this is E for target units, not dEdA
  float 	dEdA;		// #VIEW_HOT derivative of error wrt activation
  float 	dEdNet;		// #VIEW_HOT derivative of error wrt net input
  float         misc1;          // miscellaneous computational value -- used for exp(netin) in SOFTMAX case, and to hold the index of the most active unit among input connections for MAX_POOL (cast to int)

