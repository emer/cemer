// Copyright 2017-2018, Regents of the University of Colorado,
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

// these are basic defines for separate State-based computational code
// allow same codebase to be compiled in different contexts
// including providing stubs for TA compatibility without TA dependencies

// see State_cpp.h, State_main.h, and State_cuda.h for includes that use this

// everything here is robust to redefinition and can be included multiple times
// to define appropriate context for what follows

// these macros are used to suffix classes -- double-nesting required due to cpp
#ifndef PASTETWOITEMSTOGETHER
#define PASTETWOITEMSTOGETHER(c,s) c ## s
#define CLASS_SUFFIXED(c,s) PASTETWOITEMSTOGETHER(c,s)
#define STRINGIFYITEM(c) #c
#define CLASS_SUFFIX_STR(c) STRINGIFYITEM(c)
#endif

// key define -- use this for all classes that can be used in different targets
#ifdef STATE_CLASS
#undef STATE_CLASS
#undef STATE_CLASS_CPP
#undef STATE_SUFFIX_STR
#endif

#define STATE_CLASS(c)       CLASS_SUFFIXED(c,STATE_SUFFIX)
#define STATE_SUFFIX_STR     CLASS_SUFFIX_STR(STATE_SUFFIX)

// default major types

#ifdef NETWORK_STATE
#undef NETWORK_STATE
#undef BP_NETWORK_STATE
#undef LEABRA_NETWORK_STATE
#endif
#ifdef LAYER_STATE
#undef LAYER_STATE
#undef LEABRA_LAYER_STATE
#undef LEABRA_LAYER
#endif
#ifdef PRJN_STATE
#undef PRJN_STATE
#undef LEABRA_PRJN_STATE
#endif
#ifdef UNGP_STATE
#undef UNGP_STATE
#undef LEABRA_UNGP_STATE
#endif
#ifdef UNIT_STATE
#undef UNIT_STATE
#undef BP_UNIT_STATE
#undef LEABRA_UNIT_STATE
#endif
#ifdef CON_STATE
#undef CON_STATE
#undef BP_CON_STATE
#undef LEABRA_CON_STATE
#endif

#ifdef LAYER_SPEC
#undef LAYER_SPEC
#undef LAYER_SPEC_CPP
#undef LEABRA_LAYER_SPEC
#undef LEABRA_LAYER_SPEC_CPP
#endif
#ifdef PRJN_SPEC
#undef PRJN_SPEC
#undef PRJN_SPEC_CPP
#endif
#ifdef UNIT_SPEC
#undef UNIT_SPEC
#undef UNIT_SPEC_CPP
#undef BP_UNIT_SPEC
#undef BP_UNIT_SPEC_CPP
#undef LEABRA_UNIT_SPEC
#undef LEABRA_UNIT_SPEC_CPP
#endif
#ifdef CON_SPEC
#undef CON_SPEC
#undef CON_SPEC_CPP
#undef BP_CON_SPEC
#undef BP_CON_SPEC_CPP
#undef LEABRA_CON_SPEC
#undef LEABRA_CON_SPEC_CPP
#endif

#define LAYER_SPEC STATE_CLASS(LayerSpec)
#define PRJN_SPEC STATE_CLASS(ProjectionSpec)
#define UNIT_SPEC STATE_CLASS(UnitSpec)
#define CON_SPEC STATE_CLASS(ConSpec)

#define BP_UNIT_SPEC STATE_CLASS(BpUnitSpec)
#define BP_CON_SPEC STATE_CLASS(BpConSpec)

#define LEABRA_LAYER_SPEC STATE_CLASS(LeabraLayerSpec)
#define LEABRA_UNIT_SPEC STATE_CLASS(LeabraUnitSpec)
#define LEABRA_CON_SPEC STATE_CLASS(LeabraConSpec)

// other defines to support TA idioms outside of TA

#ifdef INHERITED
#undef INHERITED
#endif

#ifdef __MAKETA__
# define INHERITED(c)
#else
#ifdef STATE_MAIN  
# define INHERITED(c) typedef c inherited;
#else
# define INHERITED(c) typedef STATE_CLASS(c) inherited;
#endif
#endif

// TA swap-out code to support TA in MAIN code and not otherwise
// NOTE: all State-code does NOT manage lifetime / alloc and pointers -- all of that
// is done outside of that codebase -- no Destructors, non-std InitLinks, etc..

#ifdef STATE_DECO_KEY
#undef STATE_DECO_KEY
#endif
#ifdef STATE_TA_STD_CODE
#undef STATE_TA_STD_CODE
#endif
#ifdef STATE_TA_STD_CODE_SPEC
#undef STATE_TA_STD_CODE_SPEC
#endif
#ifdef STATE_UAE
#undef STATE_UAE
#endif

#ifdef STATE_MAIN
///////////////////////////////////////////////
// TA code

#define STATE_CLASS_CPP(c)   CLASS_SUFFIXED(c,_cpp)

// main uses _cpp state!
#define NETWORK_STATE NetworkState_cpp
#define LAYER_STATE LayerState_cpp
#define PRJN_STATE PrjnState_cpp
#define UNGP_STATE UnGpState_cpp
#define UNIT_STATE UnitState_cpp
#define CON_STATE ConState_cpp

#define BP_NETWORK_STATE BpNetworkState_cpp
#define BP_UNIT_STATE BpUnitState_cpp
#define BP_CON_STATE BpConState_cpp

#define LEABRA_NETWORK_STATE LeabraNetworkState_cpp
#define LEABRA_LAYER_STATE LeabraLayerState_cpp
#define LEABRA_LAYER LeabraLayer
#define LEABRA_PRJN_STATE LeabraPrjnState_cpp
#define LEABRA_UNGP_STATE LeabraUnGpState_cpp
#define LEABRA_UNIT_STATE LeabraUnitState_cpp
#define LEABRA_CON_STATE LeabraConState_cpp

// use these in State code when it should be the CPP version for main and otherwise the basic type
#define LAYER_SPEC_CPP LayerSpec_cpp
#define PRJN_SPEC_CPP ProjectionSpec_cpp
#define UNIT_SPEC_CPP UnitSpec_cpp
#define CON_SPEC_CPP ConSpec_cpp

#define BP_UNIT_SPEC_CPP BpUnitSpec_cpp
#define BP_CON_SPEC_CPP BpConSpec_cpp

#define LEABRA_LAYER_SPEC_CPP LeabraLayerSpec_cpp
#define LEABRA_UNIT_SPEC_CPP LeabraUnitSpec_cpp
#define LEABRA_CON_SPEC_CPP LeabraConSpec_cpp

#define STATE_DECO_KEY(c)                                       \
  String        GetTypeDecoKey() const override { return c; }

#define STATE_TA_STD_CODE(c) \
  TA_SIMPLE_BASEFUNS(c);   \
private: \
  void Destroy() { }

#define STATE_TA_STD_CODE_SPEC(c) \
  TA_SIMPLE_BASEFUNS(c);   \
protected: \
  SPEC_DEFAULTS; \
private: \
  void Destroy() { }

#define STATE_UAE(c) \
  protected: \
  void UpdateAfterEdit_impl() override { inherited::UpdateAfterEdit_impl(); \
    c }

#else
///////////////////////////////////////////////
// all non-TA code

#define STATE_CLASS_CPP(c)       CLASS_SUFFIXED(c,STATE_SUFFIX)

#define NETWORK_STATE STATE_CLASS(NetworkState)
#define LAYER_STATE STATE_CLASS(LayerState)
#define PRJN_STATE STATE_CLASS(PrjnState)
#define UNGP_STATE STATE_CLASS(UnGpState)
#define UNIT_STATE STATE_CLASS(UnitState)
#define CON_STATE STATE_CLASS(ConState)

#define BP_NETWORK_STATE STATE_CLASS(BpNetworkState)
#define BP_UNIT_STATE STATE_CLASS(BpUnitState)
#define BP_CON_STATE STATE_CLASS(BpConState)

#define LEABRA_NETWORK_STATE STATE_CLASS(LeabraNetworkState)
#define LEABRA_LAYER_STATE STATE_CLASS(LeabraLayerState)
#define LEABRA_LAYER STATE_CLASS(LeabraLayerState)
#define LEABRA_PRJN_STATE STATE_CLASS(LeabraPrjnState)
#define LEABRA_UNGP_STATE STATE_CLASS(LeabraUnGpState)
#define LEABRA_UNIT_STATE STATE_CLASS(LeabraUnitState)
#define LEABRA_CON_STATE STATE_CLASS(LeabraConState)

#define LAYER_SPEC_CPP LAYER_SPEC
#define PRJN_SPEC_CPP PRJN_SPEC
#define UNIT_SPEC_CPP UNIT_SPEC
#define CON_SPEC_CPP CON_SPEC

#define BP_UNIT_SPEC_CPP BP_UNIT_SPEC
#define BP_CON_SPEC_CPP BP_CON_SPEC

#define LEABRA_LAYER_SPEC_CPP LEABRA_LAYER_SPEC
#define LEABRA_UNIT_SPEC_CPP LEABRA_UNIT_SPEC
#define LEABRA_CON_SPEC_CPP LEABRA_CON_SPEC

#define STATE_DECO_KEY(c) \
  String        GetTypeDecoKey() const { return c; }

// just the constructor
#define STATE_TA_STD_CODE(c) STATE_CLASS(c)() { Initialize(); }

#define STATE_TA_STD_CODE_SPEC(c) STATE_CLASS(c)() { Initialize(); }

#define STATE_UAE(c) \
  public: \
  void UpdateAfterEdit_impl() { c }

#endif // STATE_MAIN


////////////////////////////////////////////////////////////////
//      Misc other global defines etc

#ifdef CUDA_COMPILE
typedef unsigned int con_mem_idx; // connection memory index type -- this should be int64_t but not clear if cuda supports it
#else
typedef long long con_mem_idx; // connection memory index type
#endif

// connection state iteration
#ifndef CON_STATE_LOOP
#define CON_STATE_LOOP(cg, expr) \
  const int sz = cg->size; \
  for(int i=0; i<sz; i++) \
    expr
#endif

// unit group iterator -- sets gpidx to either -1 for whole-layer unit group, or 0..n_ungps for unit groups
#ifndef UNIT_GP_ITR
#define UNIT_GP_ITR(lay, expr) \
  if(lay->HasUnitGroups()) {\
    for(int gpidx=0; gpidx < lay->n_ungps; gpidx++) {\
      expr \
    }\
  }\
  else {\
    int gpidx = -1;\
    expr \
  }
#endif


#ifndef StateIntArrayFuns_h
#define StateIntArrayFuns_h 1

///////////////////////////////////////////////////////////////
//   State int array functions for replicating basic int_Array functionality

INLINE void IntArrayPermute(int* array, int size, int thr_no=-1) {
  int i, nv;
  int tmp;
  for(i=0; i<size; i++) {
#if defined(STATE_MAIN) || defined(STATE_CPP)
    nv = (int) ((MTRnd::GenRandInt32(thr_no) % (size - i)) + i); // get someone from the future
#else
    nv = (int) ((rand() % (size - i)) + i);
#endif
    // swap:
    tmp = array[i];
    array[i] = array[nv];
    array[nv] = tmp;
  }
}

INLINE void IntArrayFillSeq(int* array, int size, int start=0, int inc=1) {
  for(int i=0; i<size; i++) {
    array[i] = start + inc * i;
  }
}

INLINE void IntArrayInitVals(int* array, int size, int val=0) {
  for(int i=0; i<size; i++) {
    array[i] = val;
  }
}

INLINE void IntArraySeqPermute(int* array, int size, int thr_no=-1) {
  IntArrayFillSeq(array, size);
  IntArrayPermute(array, size, thr_no);
}

INLINE void IntArraySort(int* array, int size, bool descending=false) {
  if(size <= 1) return;
  int lt_compval = -1;          // comparison return value for less-than
  if(descending)
    lt_compval = 1;
  // lets do a heap sort since it requires no secondary storage
  int n = size;
  int l,j,ir,i;
  int tmp;

  l = (n >> 1)+1;
  ir = n;
  for(;;){
    if(l>1) {
      tmp = array[--l -1]; // tmp = ra[--l]
    }
    else {
      tmp = array[ir-1]; // tmp = ra[ir]
      array[ir-1] = array[0]; // ra[ir] = ra[1]
      if(--ir == 1) {
        array[0] = tmp; // ra[1]=tmp
        return;
      }
    }
    i=l;
    j=l << 1;
    if(descending) {
      while(j<= ir) {
        if(j<ir && (array[j-1] > array[j])) j++;
        if(tmp > array[j-1]) { // tmp > ra[j]
          array[i-1] = array[j-1]; // ra[i]=ra[j];
          j += (i=j);
        }
        else j = ir+1;
      }
    }
    else {
      while(j<= ir) {
        if(j<ir && (array[j-1] < array[j])) j++;
        if(tmp < array[j-1]) { // tmp < ra[j]
          array[i-1] = array[j-1]; // ra[i]=ra[j];
          j += (i=j);
        }
        else j = ir+1;
      }
    }
    array[i-1] = tmp; // ra[i] = tmp;
  }
}

#endif // StateIntArrayFuns_h
