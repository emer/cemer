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

#ifndef ActrDeclarativeModule_h
#define ActrDeclarativeModule_h 1

// parent includes:
#include <ActrModule>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(ActrActParams);

class E_API ActrActParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR declarative memory activation parameters
INHERITED(taOBase)
public:
  bool          learn;         // :bll != nil in ACT-R -- use base-level activation learning -- if not, then base activation is const init val
  float         decay;         // #CONDSHOW_ON_learn #DEF_0.5 :bll in ACT-R -- value of the decay paramter in base level learning equation
  float         inst_noise;    // :ans in ACT-R -- extra instantaneous noise variance to provide to activations (0 default)
  float         perm_noise;    // :pas in ACT-R -- permanent noise for chunks -- added only once, when chunk is added to DM (0 default)
  float         init;          // #DEF_0 :blc in ACT-R -- default initial base level activation value if not learning
  int           n_finst;       // #DEF_4 :declarative-num-finsts in ACT-R -- number of 'fingers of instantiation' for declarative memory system
  float         finst_span;    // #DEF_3 :declarative-finst-span in ACT-R -- how long a 'fingers of instantiation' persists for declarative memory system

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrActParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};


eTypeDef_Of(ActrRetrievalParams);

class E_API ActrRetrievalParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR declarative memory activation parameters
INHERITED(taOBase)
public:
  float         thresh;     // #DEF_0 :rt in ACT-R -- retrieval threshold -- minimum chunk activation to be able to be retrieved
  float         time_pow;   // #DEF_1 :le in ACT-R -- retrieval time latency exponent (power)
  float         time_gain;  // #DEF_1 :lf in ACT-R -- multiplier on retrieval time latencies

  inline float  GetRt(float act) {
    float rt = time_gain * std::exp(-time_pow * act);
    return rt;
  }
  // get the retrieval time for a given activation level


  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrRetrievalParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrPartialParams);

class E_API ActrPartialParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR declarative memory activation parameters
INHERITED(taOBase)
public:
  bool          on;             // is partial matching enabled?  
  float         mismatch_p;    // #CONDSHOW_ON_on :mp in ACT-R -- mismatch penalty -- this is the penalty value P for mismatches
  float         max_diff;      // #CONDSHOW_ON_on #DEF_-1 :md in ACT-R -- maximum difference between two chunks -- provides the default similarity (distance) between chunks
  float         max_sim;       // #CONDSHOW_ON_on #DEF_0 :ms in ACT-R -- maximum similarity between two chunks, equal to the similarity of a chunk and itself

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrPartialParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

eTypeDef_Of(ActrAssocParams);

class E_API ActrAssocParams : public taOBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_ActR declarative memory associative strength parameters
INHERITED(taOBase)
public:
  bool          on;          // is spreading activation through associative strengths enabled?  
  float         max_str;     // #CONDSHOW_ON_on :mas in ACT-R -- maximum associative strength parameter
  bool          neg_ok;      // #CONDSHOW_ON_on :nsji in ACT-R -- allow negative assoc strength values allowed

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrAssocParams);
private:
  void	Initialize();
  void	Destroy()	{ };
};

// todo: add blending parameters


eTypeDef_Of(ActrDeclarativeModule);

class E_API ActrDeclarativeModule : public ActrModule {
  // declarative memory module
INHERITED(ActrModule)
public:
  enum TraceLevel {
    NO_TRACE,                   // don't add special trace for declarative memory -- just shows the one that was selected in the end
    TRACE_ALL,                  // :act high in ACT-R -- log the activation computation in full detail
    TRACE_MATCH,                // :act low in ACT-R -- log only the activation details for the matching chunks 
  };

  enum Recency {
    NO_RECENCY,                 // no recency constraint on retrieval
    RECENT,                     // only select among recently retrieved items (i.e., in the finst list)
    NOT_RECENT,                 // only select among non-recently retrieved items (i.e., not in finst list)
  };

  TraceLevel            trace_level; // how much detail to provide on the activation and retrieval process
  ActrActParams         act;         // base-level activation parameters -- only when subsymbolic computation is enabled in global params
  ActrRetrievalParams   ret;         // retrieval parameters -- only when subsymbolic computation is enabled in global params
  ActrPartialParams     partial;  // partial matching parameters -- when enabled, supports partial matches to be retrieved in proportion to closeness of match
  ActrAssocParams       assoc;       // associative strength parameters -- when enabled, associative strengths between chunks contribute to overall activation

  ActrChunk_List        init_chunks; // initial chunks to start pre-loaded into declarative memory
  ActrChunk_List        active; // #NO_EXPAND_ALL the current chunks active in declarative memory
  ActrChunk_List        finsts; // #HIDDEN #NO_SAVE list of current declarative fingers of instantiation chunks
  ActrChunk_List        eligible; // #HIDDEN #NO_SAVE list of all matching chunks eligible for retrieval
  ActrChunk_List        tmp_match; // #HIDDEN #NO_SAVE temporary matching list
  ActrChunkRef          retrieved;  // #HIDDEN #NO_SAVE final chunk retrieved on last retrieval

  virtual void  AddInitChunks();
  // #CAT_ActR add all the initial chunks to active list in memory (add_dm equivalent)
  virtual void  ComputeAct();
  // #CAT_ActR compute activation for all active chunks -- calls each of following functions
    virtual void  ComputeBaseAct();
    // #CAT_ActR compute base-level activation for all active chunks
    virtual void  ComputeSpreadAct();
    // #CAT_ActR compute spreading activation for all active chunks 
    virtual void  ComputePartialAct();
    // #CAT_ActR compute partial matching activation for all active chunks 
    virtual void  ComputeTotalAct();
    // #CAT_ActR compute overall total activation for all active chunks

  virtual bool  FindMatchingRetrieval(ActrChunk* ck, Recency recency = NO_RECENCY);
  // #CAT_ActR find all chunks that match given chunk, put in eligible list
  virtual void  ChooseFromEligible();
  // #CAT_ActR choose from among multiple eligible chunks for retrieval


  virtual void  RemoveOldFinsts();
  // #CAT_ActR remove any old finsts (older than finst_span) -- done prior to matching
  virtual void  UpdateFinsts();
  // #CAT_ActR update finsts to reflect last retrieved item

  virtual void  RetrievalRequest(ActrEvent& event);
  // #CAT_ActR process a retrieval request from given chunk probe
  virtual void  RetrievedChunk(ActrEvent& event);
  // #CAT_ActR after proper amount of time has passed, we register the retrieval
  virtual void  RetrievalFailure(ActrEvent& event);
  // #CAT_ActR after proper amount of time has passed, we register the retrieval failure

  virtual bool  FindMatchingBasic(ActrChunk* ck, ActrChunk_List& lst,
                                  ActrChunk_List& matches);
  // #CAT_ActR basic utility function for finding memory matches of chunks on given list, storing in matches list -- returns true if any matches found
  virtual bool  AddChunk(ActrChunk* ck, bool merge = true);
  // #CAT_ActR add a new chunk to declarative memory active buffer, if merge is true, chunk is merged with any existing ones, and base activation is increased


  void  InitModule() CPP11_OVERRIDE;
  void  ProcessEvent(ActrEvent& event) CPP11_OVERRIDE;
  bool  SetParam(const String& param_nm, Variant par1, Variant par2) CPP11_OVERRIDE;
  void  Init() CPP11_OVERRIDE;

  TA_SIMPLE_BASEFUNS(ActrDeclarativeModule);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ActrDeclarativeModule_h
