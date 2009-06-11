#include "audioproc.h"

#include "ta_math.h"
#include "ta_project.h"

#include <math.h>

/* Block Processing Description
  
  Buffer Data
    Buffers usually have the following addressing scheme:
      d0:val -- often 0; used when more than one value is output per channel
      d1:chan -- for all stages processed by frequency, these are the channels
      d2:field -- for mono, always 0; for stereo, 0=L, 1=R [TODO: verify]
        note that for the early raw input, d0=1;d1=1 (no vals/chans) so field
        is essentially the least sig buffer index
      d3:item -- the index in the current buffer; buff length will depend
          on the type of block, how much data is being chunked at once, etc.
      d4:stage -- buffers that keep more than one block, ex. for historical
        integration, etc.
      

  Sample Freq
    A "sample freq" is the sampling frequency associated with a data buffer
    or stream. The sample freq is independent of the number of samples that
    may be processed in a batch, either from necessity or convenience.

  Inputs and Outputs
    A block can have one or more input blocks, and be the source to one or
    more client blocks. The standard block provides a concrete 
    implementation for a single input block, which is the most common case.
    
    Some blocks would not have an input block, such as a signal generator
    or stream source (ex file reader, device wrapper, etc.).
    
    Some blocks would not have any output, and thus no clients; an example
    would be a monitoring block. A clock could still have output but no
    clients, if the "client" is higher-level code reading the data, ex.
    a pdp program reading the output and applying it to a network layer.
    
  Output Buffer(s)
    A block that provides output has one or more output buffers. 
    Each output buffer has the following attributes:
      geometry -- (see Buffer Data above)
      sample freq -- the sampling rate associated with the buffer
      duration -- this will be implied from the d0/fs values of 1 frame
      data -- the actual data itself
      stages -- often 1, indicates the number of frames
    RESTRICTION: the output buffers for a block are fixed, and referenced
      by ordinal (def 0); most have only 1 buff
    
  Input Source(s)
    A block can have one or more input sources (usually only 1).
    An input source is basically a *subscription* to the data output
    of the source block.
    Each Source Spec has the following attributes:
      src block -- the src block
      src buffer -- specific output buffer of the src block
    
      
  Output Client(s) (sinks)
    A block can have one or more blocks that have subscribed to it.
    There is a reciprocal relationship between an Input Source and
    an Output Client.
    The client information is maintained in each output buffer (source)
    that the block provides.

  Subscription Rules
    A block can only subscribe to a specific output block once.
    An output can have any number of subscribers.
    A subscriber may subscribe to more than one block output of a block.
      
  Input Buffer(s)
    A block can specify an input buffer for an input source.
    An input buffer has the following attributes:
      geometry --  (see Buffer Data above)
        based on the buffering needs; when all vals have been written
        then the block can process the input
      data -- the actual data itself 
      sample freq -- the sampling rate associated with the buffer;
        this will be set based on the sampling rate of the src output
      duration -- this will be implied from the d0/fs values (1 frame)
      stages -- often 1, the number of frames
      stage -- current frame being written
      items -- number of items per frame
      fields -- 1 for mono, 2 for stereo
      chans -- number of channels, typically 1, but, ex. many for a gammatone filterbank
      vals -- number of items per channel
      val -- the number of items written to the frame so far;
        when it reaches data.size, the buff is full, and the block
        can process it
    
    The input buffer would only be used when the input source provides
    less data per its output frames than we require -- an example would
    be where a block does a 4ms frame, but has input that comes
    at the sample freq (ex, a signal gen block). 
  
  Data and Processing, General
    A generic block could have several inputs, and several outputs. It is
    conceivable that both inputs and outputs could also have a mixture of
    sampling rates and frame sizes.
    
  Initialization/ConfigCheck
    The user first needs to make sure the blocks are wired up properly.
    
    Initialize proceeds from source blocks, through all target blocks.
    The ConfigCheck follows the same path.
    A block initializes in this order:
      1. its invariant parameters (that don't depend on in/out)
      2. its input buffers
      3. its output buffers
      4. calls the subscribers to its it output buffers
    
  Processing Goals
    The entire processing edifice must have a goal or goals, which is
    typically to deliver a certain kind of data at a certain rate to
    a higher-level consuming process. This may involve delivery of
    a single data deliverable at a constant rate, or could involve
    delivering several data items, each at its own rate.
    
    Some kind of time-frame must be established, that the outer 
    controller will use in invoking the processing stream. This will,
    almost of necessity, be the Greatest Common Divisor of the
    various data packets, as measured in sample times. For example,
    a delivery at 4ms and 12ms rates, the GCD is 4ms, so that must be
    the "frame rate". It is conceivable (though unlikely) that the 
    GCD could be as little as 1 sample.
    
  Outer Processing Loop
    EOD - end of data (ex. from a file)
  
    int n = GCD of required packets, in samples
    do {
      input block:: Next(n) // do the next n samples of input
      if (n mod packet1 == 0) deliver packet1
      if (n mod packet2 == 0) deliver packet2
      etc.
    } while (!EOD)
    
    There are two kinds of processing calls: asynchronous data-driven,
      and synchronous time-driven.
      
  Time-driven Processing
    Input blocks are typically time-driven. An input block gets 
    calls from the outer controller. 
    
    ProcNext_Samples(int n) -- this causes a block to do n cycles of its data
      process, such as generating a tone, or reading samples from
      an input source (file, hardware device, etc.) -- it will invoke
      the data output processes, as applicable for its output settings
    ProcNext_Duration(float m) -- same as above, but in terms of ms, that
      will be turned into n samples based on the block's fs -- this
      enables the outer controller to be independent of input sampling
      rate
  
  Data-driven Processing
    
    Processing is always driven by the source, such as a signal generator
    or a stream reader of some kind.
    
    Processing is by output buffer, since each output buffer
    can have an independent frame rate.
    A block "wants" to produce as much data as it can at a time. But it
    is limited by the following:
      1. it never produces more than one stage worth of data in one call
      2. it is limited by how much new data is available from its input
        stages
    When a block fills the stage of an output buffer, it then calls
      every Block subscribed to this block, and passes a reference
      to the buffer. 
      
    For most blocks that have only one input, when they get a notice 
      from their source block then will (optionally) perform their
      input step, then if they have sufficient data, do their output
      step.
    
    For blocks that would require data from several other blocks, they
    need to keep a state of how many of the blocks have been received,
    and when all the required input has been received, then trigger
    the data production process, and triggering of its clients.
        
    A block either needs input buffers or not.
    A block that uses input buffers, does the following:
      * when a new stage is available from the source, append that
        stage to the input buffer
        
    A block that is not wired to any other blocks will be ignored.
    
    These are the routines that deal with processing:
    
    Most Blocks:
      AcceptInput(src_block, sb_output)
        -- a  block calls every subscriber to itself when
          one of its buffer stages has been filled --
          it passes refs to itself and the buffer, to the sink
        -- a single-src callee (most) will then invoke its
          own processing
        -- a multi-src callee will determine what input src
          has called, and optionally copy to an input;
          it will then advance a state mechanism, and if
          all required inputs have been received, will invoke
          its own processing, and clear the state
          
    Input Blocks
      These blocks do not receive data from other blocks.
      
        
        
      Next() -- this is for sources, who by def have no input;
        this routine can return a STOP signal, meaning that 
        something along the chain requested stop; ex. out of
        data, etc.
      
    
    The controller does the following:
      call Next on the source block
        -- this will cause the block to generate frame output --
           every output that has a stage ready, will have
           stage==0
      call Next on the next block in sequence, this block will have
      inputs, and will typically do the following:
        foreach (src block)
      
    If a block has not completed any output frames, then it returns from Next.
    A block produces all the 
    If a block HAS completed an output block, then it calls Next on every
      client of that output frame.
      
    There is an assumption of "sense":
      * it starts with an input somewhere
      * the input block completes;
      * it then invokes the following blocks in order
      * if there is parallelism, it occurs by putting all
        the parallel blocks in order, before any following blocks
        
    
      
  
*/

//////////////////////////////////
//  SampleFreq			//
//////////////////////////////////

void SampleFreq::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if ((fs_val != SF_CUSTOM) && (fs_val != SF_AUTO)) {
    // the enum value IS the rate!
    fs_act = fs_val;
  }
}

void SampleFreq::Set(SampleFreqVal val, float act) {
  if ((val != SF_CUSTOM) && (val != SF_AUTO)) {
    // the enum value IS the rate!
    act = val;
  }
  fs_val = val;
  fs_act = act;
}

//////////////////////////////////
//  Level			//
//////////////////////////////////

float Level::ActualToLevel(float act_level, Units units) {
  float rval;
  switch (units) {
  case UN_PERCENT: rval = act_level * 100.0f; break;
  case UN_SCALE: rval = act_level; break;
  case UN_DBI:
    rval = log10(act_level) * 10.0f;
    break;
  case UN_DBP:
    rval = log10(act_level) * 20.0f;
    break;
  }
  return rval;
}

float Level::LevelToActual(float level, Units units) {
  float rval;
  switch (units) {
  case UN_PERCENT: rval = level / 100.0f; break;
  case UN_SCALE: rval = level; break;
  case UN_DBI:
    rval = powf(10.0f, (level / 10.0f));
    break;
  case UN_DBP:
    rval = powf(10.0f, (level / 20.0f));
    break;
  }
  return rval;
}

float Level::Convert(float level, Units units, Units new_units) {
  return ActualToLevel(LevelToActual(level, units), new_units);
}

Level::Level(Units init_units, float init_act_level) 
{
  level = ActualToLevel(init_act_level, init_units);
  units = init_units;
  act_level = init_act_level;
}

void Level::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  act_level = LevelToActual(level, units);
}

void Level::Set(float new_level, Units new_units) {
  level = new_level;
  units = new_units;
  act_level = LevelToActual(new_level, new_units);
  DataChanged(DCR_ITEM_UPDATED);
}

void Level::Update(float in_level, Units in_units) {
  act_level = LevelToActual(in_level, in_units);
  level = ActualToLevel(act_level, units);
  DataChanged(DCR_ITEM_UPDATED);
}

//////////////////////////////////
//  Phase			//
//////////////////////////////////

void Phase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  switch (units) {
  case UN_DEGREES: act_phase = (float)((phase / 360.0) * (2 * M_PI)); break;
  case UN_RADIANS: act_phase = phase; break;
  }
}


//////////////////////////////////
//  Duration			//
//////////////////////////////////

int Duration::StatCompare(const Duration& a, const Duration& b, float fs) {
  // try to avoid math:
  if (a.units == b.units) {
    if (a.duration < b.duration) return -1;
    else if (a.duration > b.duration) return 1;
    else return 0;
  }
  // use samples, to avoid dividing, and make result directly
  int ia = a.GetDurationSamples(fs);
  int ib = b.GetDurationSamples(fs);
  return ia- ib;
}

double Duration::StatGetDecayDt(double duration, Units units, float fs)
{
  if (units == UN_CONT) 
    return 1.0;
  double samples = 0.0f;
  if (units == UN_TIME_MS) 
    duration /= 1000;
  if (units != UN_SAMPLES) {
    // requires a valid sample rate
    if (fs <= 0) 
      return 0.0f;
    samples = fs * duration;
  }
  //note: dt is 1 - tc
  return 1.0 - exp(-1.0/samples);
}

double Duration::StatGetDurationTime(double duration, Units units, float fs) {
  if (units == UN_CONT) 
    return 0; 
  else if (units == UN_TIME_S)
    return duration;
  else if (units == UN_TIME_MS) 
    return duration / 1000;
    
  // UN_SAMPLES requires a valid sample rate
  if (fs <= 0) 
    return 0.0f;
  if (units == UN_SAMPLES) 
    return duration / fs;
  return 0.0f; // compiler food: should never be reached
}

double Duration::StatGetDurationSamples(double duration, Units units, float fs) {
  if (units == UN_CONT) 
    return 0; 
  else if (units == UN_SAMPLES) 
    return duration;
    
  if (units == UN_TIME_S)
    return (duration * fs);
  else //if (units == UN_TIME_MS) 
    return ((duration / 1000) * fs);
}

void Duration::Initialize() {
  duration = 1.0; 
  units = UN_SAMPLES;
}

void Duration::AddTime(double dur, float fs) {
  switch (units) {
  case UN_TIME_S: duration += dur; break;
  case UN_TIME_MS: duration += (dur * 1000); break;
  case UN_SAMPLES: 
    duration += StatGetDurationSamples(dur, UN_TIME_S, fs); 
    break;
  default: break;
  }
}

void Duration::AddSamples(double samp, float fs) {
  switch (units) {
  case UN_TIME_S: 
    duration += StatGetDurationTime(samp, UN_SAMPLES, fs); 
    break;
  case UN_TIME_MS:
    duration += (StatGetDurationTime(samp, UN_SAMPLES, fs) * 1000); 
    break;
  case UN_SAMPLES: duration += samp;
    break;
  default: break;
  }
}

//////////////////////////////////
//  DataBuffer			//
//////////////////////////////////

void DataBuffer::Initialize() {
  fr_dur.Set(1, Duration::UN_SAMPLES); // common default
  min_stages = 1;
  fields_ro = false;
  stages = min_stages;
  stage = 0;
  items = 1;
  item = 0;
  fields = 1;
  field = 0;
  chans = 1;
  vals = 1;
  item_cnt = 0;
  enabled = true;
}

void DataBuffer::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //TODO: should we invalidate stuff???
  if (min_stages < 1) {
    min_stages = 1;
  }
  if (stages < min_stages) {
    stages = min_stages;
  }
  const int min_fields = 1; // TEMP, maybe need a full field
  if (fields < min_fields) {
    fields = min_fields;
  }
}

bool DataBuffer::NextIndex() {
  bool rval = false;
  if (++item >= items) {
    rval = true;
    item = 0;
    NextStage();
  }
  ++item_cnt;
  return rval;
}

void DataBuffer::NextStage() {
  if (++stage >= stages) stage = 0;
}

void DataBuffer::CheckThisConfig_impl(bool quiet, bool& ok) {
  inherited::CheckThisConfig_impl(quiet, ok);
}


void DataBuffer::InitConfig(bool check, bool quiet, bool& ok) {
  int t_items = fr_dur.GetDurationSamples(fs);
  if (!enabled || (t_items == 0)) {
    // not in use -- reclaim space
    mat.SetGeom(0, 0);
    return; 
  }
  // enforce minimums, esp. when those were set by some other prog
  if (!check) {
    if (stages < min_stages) stages = min_stages;
  }
  
  if (CheckError(((vals < 1) || (chans < 1) || (fields < 1) ||
    (stages < 1)), quiet, ok,
    "DataBuffer::InitConfig: vals/chans/fields/stages must be >= 1")) return;
    
  if (check) return;
    
  items = t_items;
  mat.SetGeom(5, vals, chans, fields, items, stages);
  mat.Clear(); // for subsequent calls, clears previous data
  stage = 0;
  item = 0;
  field = 0;
  item_cnt = 0;
}
//note: these are sort of obsolete.. just use the std dims
int DataBuffer::GetChanDim() const {
  return SignalProcBlock::CHAN_DIM;
}

int DataBuffer::GetChans() const {
  int rval = mat.dim(GetChanDim());
  return rval;
} 
  
bool DataBuffer::GetBaseGeomOfStageAbs(int abs_stage, MatrixGeom& geom) {
  if (TestError(((abs_stage < 0) || (abs_stage >= mat.geom.SafeEl(SignalProcBlock::STAGE_DIM))), "GetBaseGeomOfStageAbs",
    "abs_stage (", String(abs_stage), ") is out of bounds or data unexpectedly does not have the outer dim ")) return false;
  // set same dims as mat, and all 0 (base) except stage dim
  geom.SetGeom(5, 0, 0, 0, 0, abs_stage); // val chan field item stage
  return true;
}

int DataBuffer::GetRelStage(int stage, int offs) const {
  int whr = stage + offs;
  if (whr < 0) whr += stages;
  else if (whr >= stages) whr -= stages;
  if (TestError(((whr < 0) || (whr >= mat.geom.SafeEl(SignalProcBlock::STAGE_DIM))),
    "GetRelStage",
    "offs is out of bounds or data unexpectedly does not have the outer dim (eff_stage=" , String(whr), ")")) return 0;
  return whr;
}

bool DataBuffer::GetBaseGeomOfStageRel(int rel_stage, MatrixGeom& geom) {
  int whr = stage + rel_stage;
  if (whr < 0) whr += stages;
  else if (whr >= stages) whr -= stages;
  if (TestError(((whr < 0) || (whr >= mat.geom.SafeEl(SignalProcBlock::STAGE_DIM))),
    "GetBaseGeomOfStageRel",
    "rel_stage is out of bounds or data unexpectedly does not have the outer dim (eff_stage=" , String(whr), ")")) return false;
  // set same dims as mat, and all 0 (base) except stage dim
  geom.SetGeom(5, 0, 0, 0, 0, whr); // val chan field item stage
  return true;
}

int DataBuffer::prevStage() const {
  int rval = stage - 1; // 1 less than next
  if (rval < 0) rval = stages - 1;
  return rval;
}

//////////////////////////////////
//  SignalProcItem		//
//////////////////////////////////

void SignalProcItem::InitConfig_Int(bool check, bool quiet, bool& ok)
{
  if (off()) return;
  InitConfig_impl(check, quiet, ok);
  if (!check) DataChanged(DCR_ITEM_UPDATED); // yuh think???
}

void SignalProcItem::InitConfig_impl(bool check, bool quiet, bool& ok) {
  InitThisConfig_impl(check, quiet, ok);
  // don't even descend until we are fixed!
  if (!check && !ok) return;
  InitThisConfigDataIn_impl(check, quiet, ok);
  if (!check && !ok) return;
  InitChildConfig_impl(check, quiet, ok);
  if (!check && !ok) return;
  InitThisConfigDataOut_impl(check, quiet, ok);
}

void SignalProcItem::InitChildConfig_impl(bool check, bool quiet, bool& ok) {
  taList_impl* chld = children_();
  if (chld && chld->el_base->DerivesFrom(&TA_SignalProcItem)) {
    for (int i = 0; i < chld->size; ++i) {
      // don't move on to init next child, if prev step didn't succeed
      if (!check && !ok) return;
      SignalProcItem* itm = (SignalProcItem*)chld->FastEl_(i);
      if (!itm) continue; // normally shouldn't happen
      InitChildItemConfig_impl(itm, check, quiet, ok);
    }
  }
}

void SignalProcItem::InitChildItemConfig_impl(SignalProcItem* itm, 
  bool check, bool quiet, bool& ok) 
{
  itm->InitConfig_Int(check, quiet, ok);
}



//////////////////////////////////
//  SourceBlockSpec		//
//////////////////////////////////

void SourceBlockSpec::Initialize() {
  taBase::Own(src_block, this);
  buff_index = 0;
}

void SourceBlockSpec::CutLinks() { 
  src_block = NULL; // so notifies go out
  CutLinks_taAuto(&TA_SourceBlockSpec); 
  inherited::CutLinks();
}

DataBuffer* SourceBlockSpec::GetBuffer() const {
  DataBuffer* rval = NULL;
  if (src_block.ptr())
    rval = src_block->outBuff(buff_index);
  return rval;
}

void SourceBlockSpec::SmartRef_DataRefChanging(taSmartRef* ref, 
    taBase* obj, bool setting) 
{
  SignalProcBlock* own = GET_MY_OWNER(SignalProcBlock);
  if (!own) return; // huh?
  if ((ref == &src_block) && obj) {
    SignalProcBlock* blk = dynamic_cast<SignalProcBlock*>(obj);
    if (blk)
      blk->AddRemoveClientBlock(own, setting);
    else 
     taMisc::Warning("SourceBlockSpec::SmartRef_DataRefChanging",
     "Unexpected null obj or not a SignalProcBlock");
  }
}


//////////////////////////////////
//  SignalProcBlock		//
//////////////////////////////////

void SignalProcBlock::Initialize() {
  flags = BF_0;
  num_clients = 0;
  clients.setOwner(this);
}

void SignalProcBlock::AddRemoveClientBlock(SignalProcBlock* blk, 
    bool adding)
{
  if (!blk) return;
  // we do a unique add, when adding
  if (adding) {
    clients.AddUnique(blk);
    goto exit;
  }
  // when removing, we only remove when no more input guys on that block use us
  // blk will already have been removed from the ref
  for (int i = 0; i < blk->srcBlockCount(); ++i) {
    SourceBlockSpec* sbs = blk->srcBlock(i); // note: can easily be NULL
    if (sbs && (sbs->src_block.ptr() == this)) return; //still has a ref
  }
  // no more refs
  clients.RemoveEl(blk);
exit:
  num_clients = clients.size;
  DataChanged(DCR_ITEM_UPDATED);
}

void SignalProcBlock::DataDestroying_Ref(taBase_RefList* src, taBase* ta) {
// only needed for guys that don't go through the api
  num_clients = clients.size;
  DataChanged(DCR_ITEM_UPDATED);
}

void SignalProcBlock::DataChanged_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2) {
}

#define DQR(x) ((x) ? DQR_SAVE : DQR_NO_SAVE)

double SignalProcBlock::GetElapsedTime_impl() {
  InputBlockBase* ib = GetInputBlock();
  if (ib) return ib->GetElapsedTime();
  else return 0.0;
}

InputBlockBase* SignalProcBlock::GetInputBlock(int idx) {
  InputBlockBase* rval = NULL;
  // iterate our src blocks, delegating to them, until found or fail
  for (int i = 0; (!rval && (i < srcBlockCount())); ++i) {
    SourceBlockSpec* sbs = srcBlock(i); // note: can easily be NULL
    if (sbs && (bool)sbs->src_block) rval = sbs->src_block->GetInputBlock();
  }
  return rval;
}

SignalProcBlock* SignalProcBlock::GetUpstreamBlock(TypeDef* typ) {
  if (!typ) return NULL;
  // try our immediate src blocks first
  for (int i = 0; (i < srcBlockCount()); ++i) {
    SourceBlockSpec* sbs = srcBlock(i); // note: can easily be NULL
    if (!(sbs && (bool)sbs->src_block)) continue;
    // tentatively grab, pending correct type...
    SignalProcBlock* rval = sbs->src_block;
    if (rval->InheritsFrom(typ)) return rval;
  }
  // iterate our src blocks, delegating to them, until found or fail
  for (int i = 0; (i < srcBlockCount()); ++i) {
    SourceBlockSpec* sbs = srcBlock(i); // note: can easily be NULL
    if (!(sbs && (bool)sbs->src_block)) continue;
    // tentatively grab, pending correct type...
    SignalProcBlock* rval = sbs->src_block;
    // delegate to that guy, and return if he succeeded
    rval = rval->GetUpstreamBlock(typ);
    if (rval) return rval;
  }
  return NULL;
}

/*TEMP SignalProcSet* SignalProcBlock::GetParentSet() const {
  SignalProcSet* rval = GET_MY_OWNER(SignalProcSet);
  return rval;
}*/

bool SignalProcBlock::InitConfig() {
  bool ok = true;
  InitConfig_Int(false, false/*true*/, ok); // does DataChanged
  return ok;
}

void SignalProcBlock::InitConfig_Gui() {
  bool ok = InitConfig();
  if (!ok) {
    taMisc::Error("The block could not be initialized -- run CheckConfig for detailed errors.");
  }
}

void SignalProcBlock::InitConfig_impl(bool check, bool quiet, bool& ok) {
  if (in_init_config) return; // must be a feedback loop somewhere
  ++in_init_config;
  inherited::InitConfig_impl(check, quiet, ok);
  if (ok) {
    InitClientsConfig_impl(check, quiet, ok);
  }
  --in_init_config;;
}

void SignalProcBlock::InitClientsConfig_impl(bool check, bool quiet, bool& ok) {
  for (int i = 0; (ok && (i < clients.size)); ++i) {
    SignalProcBlock* snk_blk = static_cast<SignalProcBlock*>(clients.FastEl(i));
    if (!snk_blk) continue;
    snk_blk->InitConfig_Int(check, quiet, ok);
  }
}

void SignalProcBlock::InitThisConfigDataIn_impl(bool check,
  bool quiet, bool& ok) 
{ 
  for (int i = 0; (ok && (i < inBuffCount())); ++i) {
    DataBuffer* buff = inBuff(i);
    if (buff) buff->InitConfig(check, quiet, ok);
  }
}

void SignalProcBlock::InitThisConfigDataOut_impl(bool check,
  bool quiet, bool& ok) 
{ 
  for (int i = 0; (ok && (i < outBuffCount())); ++i) {
    DataBuffer* buff = outBuff(i);
    if (buff) buff->InitConfig(check, quiet, ok);
  }
}

void SignalProcBlock::DataReady(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  //note: src_buff can be NULL for composite blocks, like InputBlockSet
  // "off" means off, 
  if (flags & BF_OFF) return;
  
  // make sure we actually are interested, then dispatch
  for (int i = 0; ((ps == PS_OK) && (i < srcBlockCount())); ++i) {
    SourceBlockSpec* sbs = srcBlock(i);
    if (!sbs || (sbs->buff_index != buff_index)) continue;
    AcceptData_impl(src_blk, src_buff, buff_index, stage, ps);
    //note: could theoretically be more than one
  }
}

void SignalProcBlock::NotifyClientsBuffStageFull(DataBuffer* buff, int index, ProcStatus& ps) {
  for (int i = 0; ((ps == PS_OK) && (i < clients.size)); ++i) {
    SignalProcBlock* snk_blk = static_cast<SignalProcBlock*>(clients.FastEl(i));
    if (!snk_blk) continue;
    int stage = (buff) ? buff->prevStage() : -1;
    snk_blk->DataReady(this, buff, index, stage, ps);
  }
}


//////////////////////////////////
//  InputBlockBase		//
//////////////////////////////////

FloatTDCoord InputBlockBase::no_pos;

void InputBlockBase::Initialize() {
  use_fs = true; // descendents can override
  fs.Set(SampleFreq::SF_22050); // usual default
  max_dur.Set(0, Duration::UN_CONT); // no limit, but derived may set other default
  elapsed = 0;
  samples = 0;
  max_dur_samp = 0;
}

void InputBlockBase::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  if (check) return;
  elapsed = 0;
  samples = 0;
  // note: we'll check for CONT and just ignore this if so
  max_dur_samp = max_dur.GetDurationSamples(fs);
}

double InputBlockBase::GetElapsedTime_impl() {
  return Duration::StatGetDurationTime(samples, Duration::UN_SAMPLES, fs);
}

SignalProcBlock::ProcStatus InputBlockBase::ProcNext_Duration(float s) {
   // round to nearest integer, prevents missing an update
  return ProcNext_Samples(Duration::StatGetDurationSamples(
    s, Duration::UN_TIME_S, fs) + 0.5);
}

SignalProcBlock::ProcStatus InputBlockBase::ProcNext_Samples(int n) {
  ProcStatus ps = PS_OK;
  // do the elapsed time calcs first, to see if we need to chop n
  // note that our calcs are safe from roundoff errors, because
  // we use the same test when checking for stop
  if (max_dur.units != Duration::UN_CONT) {
    // note that our exit test is >= so we only chop if we have too much
    if ((samples + n) > max_dur_samp) {
      // trim n appropriately -- always add one more sample, to guard from roundoff
      // otherwise we'd never stop!
      int tn = (int)(max_dur_samp - samples) + 1;
      // but don't ask for more!
      if (tn < n) n = tn;
    }
  }
  
  ProcNext_Samples_impl(n, ps);
  // samples better have been updated!
  // update the elasped time counter
  elapsed = Duration::StatGetDurationTime(samples, Duration::UN_SAMPLES, fs);
  
  // don't change an error to STOP,even if we timed out
  if (ps == PS_OK) {
    if ((max_dur.units != Duration::UN_CONT) && 
     (samples >= max_dur_samp)) 
        ps = PS_STOP;
  }
  DataChanged(DCR_ITEM_UPDATED_ND);
  return ps;
}



//////////////////////////////////
//  InputBlock			//
//////////////////////////////////

void InputBlock::Initialize() {
  pos.y = 0.2332381f; //defaults to unity gain
}


//////////////////////////////////
//  InputBlockSet		//
//////////////////////////////////

void InputBlockSet::Initialize() {
  blocks.SetBaseType(&TA_InputBlockBase);
  stages = 1;
}

void InputBlockSet::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // note: we assume we will be connected to a ListenerBlock, and thus should
  // have enough stages to feed it -- we check, using the defaults for head/sound
  // if stages seems to low, then bump it
  int min_stages = ListenerBlock::CalcMinInputStages((float)fs);
  if (stages < min_stages) {
    stages = min_stages;
    taMisc::Warning("InputBlockSet::UpdateAfterEdit(): increased output stages to meet assumed minimum for a ListenerBlock -- this should be harmless if not needed");
  }
}


void InputBlockSet::InitChildItemConfig_impl(SignalProcItem* itm, 
    bool check, bool quiet, bool& ok)
{
  // before calling its init, we force its fs to be ours, and also turn off option
  InputBlockBase* blk = dynamic_cast<InputBlockBase*>(itm);
  if (blk) { // should always be...
    // slave to our fs
    blk->use_fs = false; // user should not set this
    blk->fs = fs;
    if (check) goto cont;
    // all buffering needs to be the same, based on our own buffer
    for (int b = 0; b < blk->outBuffCount(); ++b) {
      DataBuffer* buf = blk->outBuff(b);
      buf->stages = stages;
    }
  }
cont:
  inherited::InitChildItemConfig_impl(itm, check, quiet, ok);
}

DataBuffer* InputBlockSet::outBuff(int idx) {
  SignalProcBlock* blk = blocks.SafeEl(idx);
  if (blk) return blk->outBuff(1);
  return NULL;
}

void InputBlockSet::ProcNext_Samples_impl(int n, ProcStatus& ps) {
  //note: usually n is 1 anyway, but we make sure
  PROC_NEXT_LOOP(ps,i) {
    ProcStatus l_ps = PS_OK; // local status -- only stop after all or on error 
    for (int b = 0; ((b < blocks.size) && (l_ps != PS_ERROR)); ++b) {
      InputBlockBase* blk = dynamic_cast<InputBlockBase*>(blocks.FastEl(b));
      if (!blk) continue; // for robustness, but always should be right type
      if (blk->off()) continue;
      l_ps = blk->ProcNext_Samples(1);
      // if Stop or Error, then save that
      if (l_ps != PS_OK)
        ps = l_ps;
    }
    // note that this notify doesn't provide a buffer
    NotifyClientsBuffStageFull(NULL, 0, ps);
  }
}


//////////////////////////////////
//  ListenerBlock		//
//////////////////////////////////

void ListenerBlock::Initialize() {
  speed_sound = 344.0f; // 344 m/s
  head_radius = .09f; // 9 cm average head
  pos_ear_L.x = -head_radius;
  pos_ear_R.x = head_radius;
  norm_by_n = false;
  output_mode = OM_Stereo;
  dist_min = 0.03f;
  dist_atten = 16.0f;
  dyn_gain = 1.0f;
}

void ListenerBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (TestError((speed_sound <= 0.0f), "UpdateAfterEdit",
    "speed_sound must be > 0 -- it has been reset to default"))
  {
    speed_sound = 344.0f;
  }
  if (TestError((head_radius <= 0.0f), "UpdateAfterEdit",
    "head_radius must be > 0 -- it has been reset to default"))
  {
    head_radius = .09f;
  }
  if (TestError((dist_min <= 0.0f), "UpdateAfterEdit",
    "dist_min must be > 0 -- it has been reset to default"))
  {
    dist_min = .03f;
  }
  if (TestError((dist_atten <= 0.0f), "UpdateAfterEdit",
    "dist_atten must be > 0 -- it has been reset to default"))
  {
    dist_atten = 16.0f;
  }
}

int ListenerBlock::CalcMinInputStages(float fs, float head_radius,
  float speed_sound)
{
  return ((int)taMath_float::ceil(((head_radius * 2) / speed_sound)
    * fs)) + 1;
}


void ListenerBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  SignalProcBlock* src_blk = in_block.src_block;
  if (!src_blk) return;
  
  // really only works with InputBlock or InputBlockSet
  InputBlockSet* src_ib = dynamic_cast<InputBlockSet*>(src_blk);
  if (CheckError((!src_ib), quiet, ok,
    "ListenerBlock requires an in_block of type InputBlockSet"))
    return;
  
  // make sure source has enough stages for our ear sep
  // note: need to add 1 because stage0 is zero delay
  int min_stages = CalcMinInputStages((float)src_ib->fs, head_radius, speed_sound);
    
  if (CheckError((src_ib->stages < min_stages), quiet, ok,
    "InputBlockSet for ListenerBlock will require ", String(min_stages),
    " stages at current fs, speed_sound, and head_radius -- this value has been updated, rerun InitConfig"))
  {
    src_ib->stages = min_stages;
    src_ib->DataChanged(DCR_ITEM_UPDATED);
    return;
  }
  
  // note: we assume 1 val, 1 chan 
  
  if (!ok) return;
  
  if (check) return;
  
  UpdateAfterEdit_Audio3D();
  
  out_buff.fs = src_ib->fs;
  out_buff.fr_dur.Set(1, Duration::UN_SAMPLES);
  out_buff.fields = (output_mode == OM_Stereo) ? 2 : 1;
  out_buff.chans = 1;
  out_buff.vals = 1;
}

void ListenerBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* /*src_buff*/, int/*buff_index*/,
    int /*stage=-1*/, ProcStatus& ps)
{
  // NOTE: src_buff will be NULL for InputBlockSet!
  // we assume an InputBlock or InputBlockSet is feeding us
  InputBlockBase* src_ib = dynamic_cast<InputBlockBase*>(src_blk);
  if (!src_ib) return;
  // clear the accumulation buff
  double accum_buff[2];
  accum_buff[0] = accum_buff[1] = 0.0;
//NOTE: supposed to only be one item per notification  
  // process every InputBlock 
  for (int b = 0; b < src_blk->inputBlockCount(); ++b) {
    InputBlockBase* blk = src_blk->GetInputBlock(b);
    if (!blk) continue;
    if (blk->off()) continue;
    // note: InputBlockSet gets one buffer per contained block in following
    DataBuffer* buff = blk->outBuff(0);
    if (!buff) continue;
    AcceptData_OneInput(blk, buff, b, accum_buff);
  }
  
  if (output_mode == OM_Mono) {
    // note: dyn_gain already had the /2 factor baked in
    out_buff.mat.FastEl(0, 0, OFFS_MONO, 0, out_buff.stage) =
      (float)((accum_buff[OFFS_L] + accum_buff[OFFS_R]) * dyn_gain);
  } else {
    out_buff.mat.FastEl(0, 0, OFFS_L, 0, out_buff.stage) =
      (float)(accum_buff[OFFS_L] * dyn_gain);
    out_buff.mat.FastEl(0, 0, OFFS_R, 0, out_buff.stage) =
      (float)(accum_buff[OFFS_R] * dyn_gain);
  }
  if (out_buff.NextIndex()) {
    NotifyClientsBuffStageFull(&out_buff, 0, ps);
  }
  
} 

void ListenerBlock::AcceptData_OneInput(InputBlockBase* blk, 
    DataBuffer* buff, int idx, double* accum_buff/*[2]*/)
{
  MatrixGeom geom;
  // sample number -- note: one will always be 0 (closest)
  int delay_L = (int)params.FastEl(PI_DELAY_L, idx);
  int delay_R = (int)params.FastEl(PI_DELAY_R, idx);
  // select the delayed value, and then multiply by the ATTEN factor
  //note: inputs are always mono, we assume one chan, item
  // note: we have to subtract one from delay, because buff.stage has already been ++
  buff->GetBaseGeomOfStageRel(delay_L - 1, geom);
  accum_buff[OFFS_L] += (buff->mat.SafeElN(geom) *
    params.FastEl(PI_ATTEN_L, idx));
    
  buff->GetBaseGeomOfStageRel(delay_R - 1, geom);
  accum_buff[OFFS_R] += (buff->mat.SafeElN(geom) *
    params.FastEl(PI_ATTEN_R, idx));
}

void ListenerBlock::SetPos(const FloatTDCoord& head_pos, const Phase& azim) {
  pos = head_pos;
  FloatTDCoord* pos_ear = this->pos_ear(); // cache
  // calculcate absolute position of the ears
  for (int i = 0; i < 2; ++i) {
    FloatTDCoord t;
    // add offset for ear pos
    float f = ((i*2) - 1) * head_radius;
    // rotate by azimuth
    t.x = f * cos(azim);
    t.y = (-f) * sin(azim); 
    // TODO: rotate by tilt
   
    // apply translation
    pos_ear[i] = t + pos;
  }
  UpdateAfterEdit_Audio3D();
  DataChanged(DCR_ITEM_UPDATED);
}

void ListenerBlock::UpdateAfterEdit_Audio3D() {
  InputBlockSet* src_blk = dynamic_cast<InputBlockSet*>(in_block.src_block.ptr());
  if (!src_blk) return;
  
  // create room for the cached params -- make a row enabled or not
  int n_in = src_blk->inputBlockCount();
  if (n_in == 0) return;
  params.SetGeom(2, PI_COUNT, n_in);
  for (int b = 0; b < n_in; ++b) {
    InputBlockBase* blk = src_blk->GetInputBlock(b);
    if (!blk) continue; // huh?
    // distance from src to ears
    const FloatTDCoord& pos_src = blk->GetPos();
    
    // calc the distance to each ear
    // prevent infinity in gain calc
    float dist_L = taMath_float::max(pos_ear_L.Dist(pos_src), dist_min);
    float dist_R = taMath_float::max(pos_ear_R.Dist(pos_src), dist_min);
    params.FastEl(PI_DIST_L, b) = dist_L;
    params.FastEl(PI_DIST_R, b) = dist_R;
    
    // TODO: calc the azim -- used to calc the head shadow atten
    float azim = 0.0f;
    params.FastEl(PI_AZIM, b) = azim;
    
    // phase will depend on the relative dist from src to the two ears
    // the one w/ shortest distance will be the reference
    // for delay -- 0 is closest, +n is farthest ear
    int delay[2];
    int con_i = OFFS_L;
    float dist_dif = fabs(dist_L - dist_R);
    if (dist_L < dist_R) {
      delay[OFFS_L] = 0;
      con_i = OFFS_R;
    } else {
      delay[OFFS_R] = 0;
    }
    float samps = (dist_dif / speed_sound) * out_buff.fs;
    // sanity check: needs to be <= head width, in samples (ie stages)
    if (TestError((samps > src_blk->stages) , "UpdateAfterEdit_Audio3D",
      "L to R head delay for source ", String(b), " caused delay (in samples): ",
      String(samps), " to exceed stages -- value is floored to stages"))
    {
      samps = src_blk->stages;
    }
    delay[con_i] = -((int)taMath_float::round(samps));
    params.FastEl(PI_DELAY_L, b) = delay[OFFS_L];
    params.FastEl(PI_DELAY_R, b) = delay[OFFS_R];
    
    // TODO: also calc head shadow atten component (based on azim)
    float atten_L = 1.0f / (dist_atten * dist_L * dist_L);
    float atten_R = 1.0f / (dist_atten * dist_R * dist_R);
    params.FastEl(PI_ATTEN_L, b) = atten_L;
    params.FastEl(PI_ATTEN_R, b) = atten_R;
  }
  // gain normalization, if enabled
  if (norm_by_n) dyn_gain = 1.0f / n_in;
  else dyn_gain = 1.0f; 
  if (output_mode == OM_Mono)
    dyn_gain *= 0.5f;
} 

//////////////////////////////////
//  OutputBlock			//
//////////////////////////////////

void OutputBlock::Initialize() {
}


//////////////////////////////////
//  StdBlock			//
//////////////////////////////////

void StdBlock::Initialize() {
}

//////////////////////////////////
//  MonBlockColSpec		//
//////////////////////////////////
/*
void MonBlockColSpec::Initialize() {
  val_type = DataCol::VT_FLOAT;
}
*/

//////////////////////////////////
//  SignalMonBlock		//
//////////////////////////////////

void SignalMonBlock::Initialize() {
  last_x = 0;
  mon_flags = (MonFlags)(MF_0 | MF_RESET_ON_INIT);
  mon_type = MT_VAL;
  x_units = Duration::UN_SAMPLES;
  trial = 0;
  item_col = "item";
  item_dims = 2; // most common case, per below
  item_dim0 = DIM_VAL;
  item_dim1 = DIM_CHAN;
  item_dim2 = item_dim3 = item_dim4 = item_dim5 = DIM_OFF;
}

void SignalMonBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  item_dims = 0;
  while (itemDim_(item_dims) != DIM_OFF)
    ++item_dims;
}

SignalMonBlock::SrcDim SignalMonBlock::itemDim_(int idx) const {
  switch (idx) {
  case 0: return item_dim0;
  case 1: return item_dim1;
  case 2: return item_dim2;
  case 3: return item_dim3;
  case 4: return item_dim4;
  case 5: return item_dim5;
  default: return DIM_OFF;
  }
  return DIM_OFF;
}
  
void SignalMonBlock::InitThisConfigDataOut_impl(bool check, 
  bool quiet, bool& ok) 
{
  inherited::InitThisConfigDataOut_impl(check, quiet, ok);
  
  // make sure dest_map makes sense
  if (mon_type == MT_ITEM) {
    CheckError(((item_dims <= 0)), quiet, ok,
    "item_dims must be >= 1");
  }
  
  if (check) return; // nothing else to do on check
  
  //note: no mon_data is not an error, we just do nothing
  DataBuffer* db = in_block.GetBuffer();
  if (!db) return;
  float_Matrix* mat = &db->mat;
  if (!mat || !mon_data.ptr()) return;
  
  if (mon_flags & MF_RESET_ON_INIT)
    mon_data->ResetData();
  last_x = 0;
  mon_data->StructUpdate(true);
  Init_Common(check, quiet, ok);
  if (ok) switch (mon_type) {
  case MT_VAL:
    Init_MT_VAL(check, quiet, ok);
    break;
  case MT_ITEM:
    Init_MT_ITEM(check, quiet, ok);
    break;
  }
  mon_data->StructUpdate(false);
}

void SignalMonBlock::Init_Common(bool check, bool quiet, bool& ok) 
{
//  DataBuffer* db = in_block.GetBuffer();
//  float_Matrix* mat = &db->mat;
  
  DataCol* col = NULL;
  if (mon_flags & MF_USE_TRIAL) {
    col = mon_data->FindMakeCol("trial", VT_INT);
  }
  col = mon_data->FindMakeCol("X", VT_FLOAT);
  if (isForPlot()) 
    col->SetUserData("X_AXIS", true);
}

void SignalMonBlock::Init_MT_VAL(bool check, bool quiet, bool& ok) 
{
  DataBuffer* db = in_block.GetBuffer();
  float_Matrix* mat = &db->mat;
  
  DataCol* col = NULL;
  // fields
  int i = 0;
  for (int f = 0; f < mat->dim(FIELD_DIM); ++f) {
    // vals -- note that ordinals for plotting are 1-based
    for (int val = 0; val < mat->dim(VAL_DIM); ++val) {
      String col_nm = "Y"; // only add num if more than 1 val or field
      if (mat->dim(VAL_DIM) > 1)
        col_nm += String(val+1);
      if (mat->dim(FIELD_DIM) > 1)
        col_nm += (f==0) ? "L" : "R";
      col = mon_data->FindMakeCol(col_nm, VT_FLOAT);
      if (isForPlot()) 
        col->SetUserData("PLOT_" + String(i+1), true);
      ++i;
    }
  }
  if (mat->dim(CHAN_DIM) > 1) {
    col = mon_data->FindMakeCol("Z", VT_FLOAT);
    if (isForPlot()) 
      col->SetUserData("Z_AXIS", true);
  }
/*  const int fields = mat->dim(FIELD_DIM);
  if (fields > 1) {
    col = mon_data->FindMakeCol("field", VT_FLOAT);
  } */
}

void SignalMonBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  if (!src_buff) return; // ex. we can't mon InputBlockSet
  float_Matrix* mat = &src_buff->mat;
  if (!mat || !mon_data.ptr()) return;
  
  // insure at least one dim, and has some data
  if ((mat->dims() < 1) || (mat->dim(0) == 0)) {return;}
  
  switch (mon_type) {
  case MT_VAL:
    AcceptData_MT_VAL(src_buff, mat, stage, ps);
    break;
  case MT_ITEM:
    AcceptData_MT_ITEM(src_buff, mat, stage, ps);
    break;
  }
  // src_buff.item_cnt will be equivalent to the next cumulative item index to write
  // therefore, regardless how many items get written per call, it will be the
  // index of the first (or only) item written in the next batch
  last_x = src_buff->item_cnt;
} 

double SignalMonBlock::GetX(const SampleFreq& fs) {
  double x_val = 0;
  switch (x_units) {
  case Duration::UN_TIME_MS:
    x_val = Duration::StatGetDurationTime(last_x, Duration::UN_SAMPLES, fs)
      * 1000;
    break;
  case Duration::UN_TIME_S:
    x_val = Duration::StatGetDurationTime(last_x, Duration::UN_SAMPLES, fs);
    break;
  case Duration::UN_CONT: // no sense, so just use x
  case Duration::UN_SAMPLES:
    x_val = last_x;
    break;
  }
  return x_val;
}

void SignalMonBlock::AcceptData_MT_VAL(DataBuffer* src_buff, float_Matrix* mat,
  int stage, ProcStatus& ps)
{
  DataCol* col_trial = NULL;
  if (mon_flags & MF_USE_TRIAL) {
    col_trial = mon_data->FindMakeCol("trial", VT_INT);
  }
  DataCol* col_z = NULL;
  DataCol* col_x = mon_data->FindMakeCol("X", VT_FLOAT);
  //note: had to use malloc because msvc won't allow runtime dim'ed arrays
  DataCol** col_y = (DataCol**)malloc(mat->dim(VAL_DIM) * mat->dim(FIELD_DIM) * sizeof(DataCol*));
//  DataCol* col_field = NULL;
  
  int i = 0;
  for (int f = 0; f < mat->dim(FIELD_DIM); ++f) {
    for (int val = 0; val < mat->dim(VAL_DIM); ++val) {
      String col_nm = "Y"; // only add num if more than 1
      if (mat->dim(VAL_DIM) > 1)
        col_nm += String(val+1);
      if (mat->dim(FIELD_DIM) > 1)
        col_nm += (f==0) ? "L" : "R";
      col_y[i] = mon_data->FindMakeCol(col_nm, VT_FLOAT);
      ++i;
    }
  }
  if (mat->dim(CHAN_DIM) > 1) {
    col_z = mon_data->FindMakeCol("Z", VT_FLOAT);
  }
  const int fields = mat->dim(FIELD_DIM);
/*  if (fields > 1) {
    col_field = mon_data->FindMakeCol("field", VT_FLOAT);
  }*/
  
  // we create one row per buffer item block
  mon_data->DataUpdate(true);
  if (!(mon_flags & MF_ACCUM)) {
    mon_data->ResetData(); 
  }
  for (int i = 0; ((ps == PS_OK) && (i < mat->dim(ITEM_DIM))); ++i) {
    for (int chan = 0; chan < mat->dim(CHAN_DIM); ++chan) {
      if (!mon_data->AddRows(1)) {ps = PS_ERROR; break;}
      if (col_trial) col_trial->SetValAsInt(trial, -1);
      col_x->SetValAsDouble(GetX(src_buff->fs), -1);
      int c = 0;
      for (int f = 0; f < fields; ++f)
      for (int val = 0; val < mat->dim(VAL_DIM); ++val) {
        col_y[c]->SetValAsFloat(mat->SafeElAsFloat(val, chan, f, i, stage), -1);
        ++c;
      }
      if (col_z) col_z->SetValAsFloat(chan, -1);
//      if (col_field) col_field->SetValAsFloat(f, -1);
    }
    ++last_x;
  }
  mon_data->DataUpdate(false);
  free(col_y);
} 

void SignalMonBlock::Init_MT_ITEM(bool check, bool quiet, bool& ok) 
{
  DataBuffer* db = in_block.GetBuffer();
  float_Matrix* mat = &db->mat;
  // create dims according the spec 
  MatrixGeom cell_geom;
  cell_geom.SetSize(item_dims);
  for (int d = 0; d < item_dims; d++) {
    int dest_sz = 1; // default for dummy dims
    int sd = itemDim(d);
    if (sd == DIM_OFF) break; // shouldn't happen!
    if (sd != DIM_DUMMY) {
      //BUG WARNING: assumes mat has been initialized -- may need
      // to manually do a case and grab specs from ex. db->vals etc.
      dest_sz = mat->dim(sd);
    }
    cell_geom.Set(d, dest_sz); 
  }
 
  DataCol* col = NULL;
  const int fields = mat->dim(FIELD_DIM);
  int col_idx;
  if ((fields > 1) && (mon_flags & MF_SPLIT_FIELDS)) {
    col = mon_data->FindMakeColMatrixN(item_col + "_L", VT_FLOAT, cell_geom, col_idx);
    col = mon_data->FindMakeColMatrixN(item_col + "_R", VT_FLOAT, cell_geom, col_idx);
  } else {
    col = mon_data->FindMakeColMatrixN(item_col, VT_FLOAT, cell_geom, col_idx);
  }
}

void SignalMonBlock::AcceptData_MT_ITEM(DataBuffer* src_buff, float_Matrix* mat,
  int stage, ProcStatus& ps)
{
  DataCol* col_trial = NULL;
  if (mon_flags & MF_USE_TRIAL) {
    col_trial = mon_data->FindMakeCol("trial", VT_INT);
  }
  DataCol* col_x = mon_data->FindMakeCol("X", VT_FLOAT);
  
  // note: eff_fields is only 2 when stereo AND split, else 1
  const int eff_fields = ((mat->dim(FIELD_DIM) > 1) && (mon_flags & MF_SPLIT_FIELDS)) ? 2 : 1;
  DataCol* col_y[2];
  if (eff_fields == 2) {
    col_y[0] = mon_data->FindColName(item_col + "_L", true);
    col_y[1] = mon_data->FindColName(item_col + "_R", true);
  } else {
    col_y[0] = mon_data->FindColName(item_col, true);
  }
  
  mon_data->DataUpdate(true);
  float_MatrixPtr dst_slice;
  
  // we only need one row, and just use last if exists in non-accum
  bool need_row = ((mon_flags & MF_ACCUM) || (mon_data->rows == 0));
  if (need_row && !mon_data->AddRows(1)) {
    ps = PS_ERROR; 
    goto err_exit;
  }
  
  if (col_trial) col_trial->SetValAsInt(trial, -1);
  col_x->SetValAsDouble(GetX(src_buff->fs), -1);
  
  // copy the data -- either 1 or 2 field cols
  // NOTE: we currently only support all ASC except Stage which is DESC
  // also, Stage is relative, not absolute, so we have to test that as a special case
  for (int y_col_field = 0; y_col_field < eff_fields; ++y_col_field) {
    dst_slice = dynamic_cast<float_Matrix*>(
      col_y[y_col_field]->GetValAsMatrix(-1));
    if (!(bool)dst_slice) continue; // could happen if user made diff col type
    const int dst_cnt = dst_slice->count();
    
    // src indexing is somewhat convoluted...
    // start by getting base of latest stage -- always our starting point
    MatrixGeom src_idx;
    MatrixGeom ovf_idx; // detect dim over/underflow -- curr only used for Stage
    src_buff->GetBaseGeomOfStageRel(-1, src_idx);
    ovf_idx = src_idx;
    src_idx.FastEl(FIELD_DIM) = y_col_field;
    
    // we iterate # times needed for data elements,
    // bumping the src index each time, according to the spec
    // dest data can just be accessed flat/sequentially
    for (int dst_idx = 0; dst_idx < dst_cnt; ++dst_idx) {
      float val = mat->SafeElN(src_idx);
      dst_slice->Set_Flat(val, dst_idx);
  
      // now, bump src index, working our way from innermost dim to outermost
      // note that stage is not really a special case, since it just wraps
      // like all the rest, even though it may have started <> 0
      for (int src_dim_idx = 0; src_dim_idx < item_dims; ++src_dim_idx) {
        int src_dim;
        bool desc;
        GetItemDimDesc(src_dim_idx, src_dim, desc);
        if (src_dim == DIM_DUMMY) continue; // idx already 0 and stays that way
        int& dim_val = src_idx.FastEl(src_dim); // manip/access in place
        // dec or inc this iter dim, wrapping around -- overflow test follows
        if (desc) {
          if (--dim_val < 0)
            dim_val = mat->dim(src_dim) - 1;
        } else { // ASC (typical case)
          if (++dim_val >= mat->dim(src_dim))
            dim_val = 0;
        }
        // if didn't overflow, then that's all we need to do this data val iter
        if (dim_val != ovf_idx[src_dim])
          break;
      }
    }
    dst_slice = NULL; // unref now, so we don't interfere with expanding
  }
  
err_exit:  
  mon_data->DataUpdate(false);
} 




//////////////////////////////////
//  SignalProcSet		//
//////////////////////////////////

void SignalProcSet::Initialize() {
//  out_buff.fr_dur.Set(0, Duration::UN_SAMPLES); 
}
/*
void SignalProcSet::ProcNextFrame_impl(ProcStatus& ps) {
  for (int i = 0; i < blocks.size; ++i) {
    // only continue while previous op said ok
    if (ps != PS_OK);
    SignalProcBlock* blk = blocks.FastEl(i);
    if (blk->flags & BF_OFF) continue;
    ps = blk->ProcNextFrame();
  }
}
*/


//////////////////////////////////
//  SequentialProcSet		//
//////////////////////////////////

void SequentialProcSet::Initialize() {
}

int SequentialProcSet::outBuffCount() const {
  return (blocks.size == 0) ? 0 : blocks.Peek()->outBuffCount();
}

DataBuffer* SequentialProcSet::outBuff(int idx) {
  return (blocks.size == 0) ? NULL : blocks.Peek()->outBuff(idx);
}



//////////////////////////////////
//  StimChan			//
//////////////////////////////////

TAPtr StimChan::SetOwner(TAPtr own) {
  inherited::SetOwner(own);
  chan_flags = CF_0;
  stim_gen = GET_MY_OWNER(StimGen); 
  return own;
}


//////////////////////////////////
//  ToneChan			//
//////////////////////////////////

void ToneChan::Initialize() {
  wave_type = WT_SINE;
  freq_mode = FM_ABSOLUTE;
  freq = 1000.0f;
  act_freq = freq;
  phase_per_samp = 0.0f;
  cur_phase = 0.0f;
}

void ToneChan::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // as a convenience, if user chooses Harmonic, set a logical value
  if ((freq_mode == FM_HARMONIC) && (freq == 1000.0f)) {
    freq = index + 1; // assume 2nd guy will be 2nd harmonic, etc.
  }
  bool ok = true; // ignored
  // force the config -- lets us dynamically change things while running
  InitThisConfig_impl(false, true, ok);
}

float ToneChan::GetNext() {
  float unit_val; // value in a -1:1 range, scaled later
  switch (wave_type) {
  case WT_SINE: 
    unit_val = sinf(cur_phase); 
    break;
  case WT_COSINE: 
    unit_val = cosf(cur_phase); 
    break;
  case WT_SQUARE: 
    unit_val = (cur_phase < M_PI) ? 1.0f : -1.0f;
    break;
  case WT_TRIANGLE: {
    // easier if we shift phase by 2PI for first 1/4 of waveform
    float cp_norm = cur_phase / M_PI; // .5::2.5 normalize of cur_phase
    if (cp_norm < 0.5f) cp_norm += 2.0f;
    unit_val = (cp_norm < 1.5f) ? 2.0f * (1.0f - cp_norm) : 2.0f * (cp_norm - 2.0f);
    } break;
  case WT_SAWTOOTH: {
    float cp_norm = cur_phase / M_PI; // 0::2
    unit_val = (cp_norm <= 1.0f) ? cp_norm : cp_norm - 2.0f;
    } break;
  }

  // update phase, keeping it within 0 <= ph < 2*pi
  cur_phase += phase_per_samp;
  if (cur_phase >= (2 * M_PI))
    cur_phase -= (2 * M_PI);
    
  return unit_val * level;
}

void ToneChan::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
//NOTE: if check=false we assume InitConfig ran successfully on parent
  if (!stim_gen) return; // instance, not a real guy
  
  // find the first tone guy (could be us!) -- we may need it
  ToneChan* tch1 = NULL;
  for (int i = 0; i < stim_gen->chans.size; ++i) {
    StimChan* sc = stim_gen->chans.FastEl(i);
    if (sc->GetTypeDef()->DerivesFrom(&TA_ToneChan)) {
      tch1 = (ToneChan*)sc;
      break;
    }
  }
  
  if (this == tch1) {
    // harmonic mode not allowed for first tone guy
    CheckError((freq_mode == FM_HARMONIC), quiet, ok,
      "Harmonic mode is not allowed on first ToneProc --"
      " please set to Absolute and adjust freq as applicable"
    );
  }
  // set the actual frequency, which depends on mode
  act_freq = 0.0f; // catch, in case something isn't handled
  switch (freq_mode) {
  case FM_ABSOLUTE:
    act_freq = freq;
    break;
  case FM_HARMONIC: {
    if (tch1) { // should exist, otherwise we'd be the guy, and switched
      act_freq = tch1->act_freq * freq;
    }
    } break;
  }
  
  // set the rads per sample -- only 0 for DC (which is not really supported)
  if (!check)
    phase_per_samp = (act_freq * (2 * M_PI)) / (stim_gen->fs);
  
  // initialize phase
  cur_phase = starting_phase;
}


//////////////////////////////////
//  NoiseChan			//
//////////////////////////////////

 // scale factor, we calc once, to get 0.0-2.0 range
const double NoiseChan::rand_fact = 1 / (((double)RAND_MAX) * 0.5);  

void NoiseChan::Initialize() {
  noise_type = NT_WHITE;
}


float NoiseChan::GetNext() {
  float unit_val; // value in a -1:1 range, scaled later
  switch (noise_type) {
  case NT_WHITE: 
    unit_val = (float)((rand() * rand_fact) - 1.0); 
    break;
  }
  
  return unit_val * level;
}


//////////////////////////////////
//  StimGen			//
//////////////////////////////////

void StimGen::Initialize() {
  use_fs = true;
  max_dur.Set(1.0, Duration::UN_TIME_S); // default override
}

void StimGen::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  if (check) return;
  
  out_buff.fs = fs;
}

void StimGen::AddChan(TypeDef* chan_type, int num) {
  if (num < 1) return;
  chans.New(num, chan_type);
}

void StimGen::ProcNext_Samples_impl(int n, ProcStatus& ps)
{
  const int buff_index = 0;//clarity
  PROC_NEXT_LOOP(ps,i) {
    double dat = 0.0;
    int cnt = 0;
    for (int i = 0; i < chans.size; ++i) {
      StimChan* sc = chans.FastEl(i);
      if (sc->chan_flags & StimChan::CF_OFF) continue;
      dat += sc->GetNext();
      ++cnt;
    }
    if (cnt > 0) {
      dat /= (double)cnt;
    }
    // note: 1 val, 1 chan, 1 field
    dat *= (float)gain;
    out_buff.mat.Set((float)dat, 0, 0, 0, out_buff.item, out_buff.stage);
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, buff_index, ps);
    }
  }
} 


//////////////////////////////////
//  SourceBlock			//
//////////////////////////////////

void SourceBlock::Initialize() {
}


//////////////////////////////////
//  RampGen			//
//////////////////////////////////

void RampGen::Initialize() {
  ramp_dir = RD_ON;
  ramp_type = RT_DEF;
  ramp_onset.Set(0, Duration::UN_TIME_MS);
  ramp_dur.Set(0, Duration::UN_TIME_MS);
  ramp_state = RS_PRE;
}

void RampGen::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  
  // we only support inputs 1 at a time, for simplicity
  if (CheckError((src_buff->mat.dim(ITEM_DIM) > 1), quiet, ok,
    "RampGen only supports 1-item buffers (not block buffers)"))
    return;
    
  if (check) return;
  out_buff.fs = src_buff->fs;
  // just set the dims on the zero buffer -- mats are inited to zero
  zero_buff.mat.SetGeomN(src_buff->mat.geom);
  zero_buff.fs = src_buff->fs;
  
  ramp_state = RS_PRE;
  cnt_samp = 0;
  // onset and dur, in samples
  ramp_onset_samp = ramp_onset.GetDurationSamples(src_buff->fs);
  ramp_dur_samp = ramp_dur.GetDurationSamples(src_buff->fs);
  UpdateState(); // note: could set to POST if all are 0
  
}

void RampGen::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
 // determine what case: 0, 1, or ramp (case 0,1,2)
  int c;
  switch (ramp_state) {
  case RS_PRE: c = (ramp_dir == RD_ON) ? 0 : 1; break;
  case RS_RAMP: c = 2; break;
  case RS_POST: c = (ramp_dir == RD_ON) ? 1 : 0; break;
  }
  DataBuffer* eff_buff = NULL; // the buff we will forward
  switch (c) {
  case 0:
    // just pass the zero buff on instead
    eff_buff = &zero_buff;
    break;
  case 1:
    // just pass the src buff on instead (no overhead!)
    eff_buff = src_buff;
    break;
  case 2: {
    eff_buff = &out_buff;
    int out_stage = out_buff.stage;
    float_Matrix* in_mat = &src_buff->mat;
    float_Matrix* out_mat = &out_buff.mat;
    float gain = GetRampGain();
    //NOTE: we only allowed buff_size of 1
    ACCEPT_DATA_LOOP_VAL(ps,in_mat,i,f,chan,val) {
      float dat = in_mat->SafeElAsFloat(val, chan, f, i, stage);
      out_mat->Set(dat * gain, val, chan, f, i, out_stage); 
    }
    out_buff.NextIndex(); // assumed to cause NextStage
    } break;
  default: break; // compiler food
  }
  
  // update counters etc. before we notify
  cnt_samp += 1;
  UpdateState();
  
  NotifyClientsBuffStageFull(eff_buff, buff_index, ps);
} 

float RampGen::GetRampGain() {
// where we are in the ramp, in samples, offset by 1/2 sample for balance
  double ramp_cnt_samp = (cnt_samp - ramp_onset_samp) + 0.5; 
  // note: we are never called if the ramp_dur is 0, so ok to divide
  double rval;
  switch (ramp_type) {
  case RT_TRIANGULAR:
    rval = (float)(ramp_cnt_samp / ramp_dur_samp);
    break;
  case RT_RAISED_COSINE:
    // from Lyons, "Understanding Signal Processing", p. 77
    rval = 0.5 - (0.5 * cos(M_PI * (ramp_cnt_samp / ramp_dur_samp)));
    break;
  case RT_HAMMING:
    // from Lyons p. 77
    rval = 0.54 - (0.46 * cos(M_PI * (ramp_cnt_samp / ramp_dur_samp)));
    break;
  }
  if (ramp_dir == RD_OFF)
    rval = 1.0 - rval;
  return rval;
}

void RampGen::UpdateState() {
  if (cnt_samp >= ramp_onset_samp) {
    if ((cnt_samp >= ramp_onset_samp + ramp_dur_samp)) {
      ramp_state = RS_POST;
    } else {
      ramp_state = RS_RAMP;
    }
  } else {
    ramp_state = RS_PRE;
  }
}


///////////////////////////////////////////
//	DoG Filter

void DoG1dFilterSpec::Initialize() {
  filter_type = FT_DOG;
  half_width = 4;
  filter_size = half_width * 2 + 1;
  on_sigma_norm = 1.0f;
  off_sigma_norm = 2.0f;
//   on_filter.SetGeom(2, filter_size, filter_size);
//   off_filter.SetGeom(2, filter_size, filter_size);
//   net_filter.SetGeom(2, filter_size, filter_size);
}

void DoG1dFilterSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  filter_size = half_width * 2 + 1;
  RenderFilter();
}

float DoG1dFilterSpec::FilterPoint(int x, float val) {
  return val * net_filter.FastEl(x+half_width);
}

void DoG1dFilterSpec::RenderFilter() 
{
  on_filter.SetGeom(1, filter_size);
  off_filter.SetGeom(1, filter_size);
  net_filter.SetGeom(1, filter_size);
  
  RenderFilter_impl(); 
  

  for (int i=0; i < on_filter.size; i++) {
    float net = on_filter.FastEl_Flat(i) - off_filter.FastEl_Flat(i);
    net_filter.FastEl_Flat(i) = net;
  }
  
/*  // now, shift the curve so it is entirely +ve, and then norm again
  float v_min = net_flt.SafeEl_Flat(0);
  for (int i=1; i<filter_size; i++) {
    float val = net_flt.FastEl_Flat(i);
    if (val > v_min) continue;
    v_min = val;
  }
  for (int i=0; i<filter_size; i++) {
    net_flt.FastEl_Flat(i) -= v_min;
  }
  taMath_float::vec_norm_sum(&net_flt); // make sure sums to 1.0
*/  
  // even though the components were norm'ed to 1, the resultant
  // +ve and =ve components of the gain are usually low, which
  // means that when we separate +/- and stream separately, we
  // have much different gain than not doging, so we scale
  // to make each side have unity gain
  float pos_sum = 0.0f;
  for (int i=0; i<filter_size; i++) {
    float val = net_filter.FastEl_Flat(i);
    if (val <= 0.0f) continue;
    pos_sum += val;
  }
  if (pos_sum == 0.0f) return;
  for (int i=0; i<filter_size; i++) {
    float& val = net_filter.FastEl_Flat(i);
    val /= pos_sum;
  }
}

void DoG1dFilterSpec::RenderFilter_impl() 
{
  switch (filter_type) {
  case FT_DOG: RenderFilter_DoG_impl(); break;
  case FT_SIEVE: RenderFilter_Sieve_impl(); break;
  }
}
 
void DoG1dFilterSpec::RenderFilter_DoG_impl() 
{
  // for convenience, 1 and 2 are considered "typical" values for half=4
  float	on_sigma = (on_sigma_norm * half_width) * 0.25f;	
  float	off_sigma = (off_sigma_norm * half_width) * 0.25f;
  for (int x=-half_width; x<=half_width; x++) {
    float dist = x;
    float ong = 0.0f;
    float offg = 0.0f;
    // only set values inside of filter radius
    ong = taMath_float::gauss_den_sig(dist, on_sigma);
    offg = taMath_float::gauss_den_sig(dist, off_sigma);
    on_filter.Set(ong, x+half_width);
    off_filter.Set(offg, x+half_width);
  }

  taMath_float::vec_norm_sum(&on_filter); // make sure sums to 1.0
  taMath_float::vec_norm_sum(&off_filter); // make sure sums to 1.0
} 

void DoG1dFilterSpec::RenderFilter_Sieve_impl() 
{
}
void DoG1dFilterSpec::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GraphFilter", true);
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
//  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);

  xda->SetUserData("X_AXIS", true);
//  zda->SetUserData("Z_AXIS", true);
  valda->SetUserData("PLOT_1", true);

//  float_Matrix* mat = &net_filter;
  int x;
    for(x=-half_width; x<=half_width; x++) {
      float val = FilterPoint(x, 1.0f);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(x, -1);
 //     zda->SetValAsFloat(z, -1);
      valda->SetValAsFloat(val, -1);
    }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();
}

void DoG1dFilterSpec::GridFilter(DataTable* graph_data, bool reset) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GridFilter", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

  float maxv = taMath_float::vec_max(&on_filter, idx);

  graph_data->SetUserData("N_ROWS", 3);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 2.0f);

  for(int i=0;i<3;i++) {
    float_Matrix* mat;
    graph_data->AddBlankRow();
    if(i==0) {
      nmda->SetValAsString("On", -1);
      mat = &on_filter;
    }
    else if(i==1) {
      nmda->SetValAsString("Off", -1);
      mat = &off_filter;
    }
    else {
      nmda->SetValAsString("Net", -1);
      mat = &net_filter;
    }
    matda->SetValAsMatrix(mat, -1);
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

//////////////////////////////////
//  LogLinearBlock			//
//////////////////////////////////
/*obs

void LogLinearBlock::Initialize() {
  // note: following would be for only one channel, spanning ~96 dB
  zero_level = -48;
  width = 48;
}

void LogLinearBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (width < 10) width = 10;
}

void LogLinearBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  
  if (CheckError(((zero_level < -120) || (zero_level >= 0)), quiet, ok,
    "zero_level should be in range: -120:0")) return;
  if (CheckError(((width <= 10) || (width > 120)), quiet, ok,
    "width should be in range: 10:120")) return;
    
  if (check) return;
  out_buff.fs = src_buff->fs;
  out_buff.fields = src_buff->fields;
  out_buff.chans = src_buff->chans;
  out_buff.vals = src_buff->vals;
}

void LogLinearBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_LL(in_mat, stage);
}

SignalProcBlock::ProcStatus LogLinearBlock::AcceptData_LL(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  float_Matrix* out_mat = &out_buff.mat;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM);
  const int in_vals = in_mat->dim(VAL_DIM);
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) {
    for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) 
    for (int chan = 0; ((ps == PS_OK) && (chan < in_chans)); ++chan) 
    for (int val = 0; ((ps == PS_OK) && (val < in_vals)); ++val) 
    {
      float dat = in_mat->SafeEl(val, chan, f, i, stage);
      dat = CalcValue(dat * in_gain);
      out_mat->Set(dat, val, chan, f, out_buff.item, out_buff.stage);
    }
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, 0, ps);
    }
  }
  return ps;
}

float LogLinearBlock::CalcValue(float in) {
  if (in < 0) return 0; // only defined for non-neg values
  // transform to dB -- sh/be ~ -96 < in_db <= 0
  float in_db = 10 * log10(in); // note: the ref is 1, but ok if exceeded
  // in_db will vary:  -120 << in_db ~=< 0 (but it can go a bit higher)
  // translate so that cl is at 0, and normalize
//AN  double rval = (in_db - cl) * norm; 
  double rval = ((in_db - zero_level) / width); 
  return rval;
}
*/

//////////////////////////////////
//  AGCBlock			//
//////////////////////////////////

/*
  This block provides automatic gain control. The purpose is to normalize
  the sound input so that subsequent stages can use normalized values for
  things like level limiting, compression, and so on. (The block applies
  the same gain to both fields when operating in stereo.)
 
  The block can be operated in two modes:
    online: the values are continuously updated according to the control
      paramters
    offline: an online training period is used to gather statistics and
      determine optimum consensus parameters, which are then applied
 
 The block measures the following parameters of the input (for each field):
   peak: the highest (rms) value encountered "recently"
   avg: the average value of the signal, with given time constant
 
 There are two distinct transforms that are applied to the signal:
   
  gain: this adjusts the overall level of the signal so that the peak
    channel(s) corresponds to an output of about 1.0
    
  
  
  Note that these functions are partially under top-down control in 
  the vertebrate auditory system, so future work may include "soft" input
  able to augment this automatic control. The amount of control applied 
  can itself be monitored, to provide input into the net.
  
  For stereo feeds, the gain control applied to both fields is the same, so
  the gain channel outputs are always mono.
  
References:
  http://en.wikipedia.org/wiki/Audio_level_compression

*/

void AGCBlock::Initialize() {
  agc_flags = AGC_ON;
  agc_type = AGC_AVG;
  update_rate = 10.0f;
  gain_units = Level::UN_DBI;
  prev_gain_units = gain_units;
  output_level = 0;
  init_gain = 0;
  cur_gain = 0;
  gain_thresh = -50;
  gain_limits.Set(-10, 20);
  in_tc.Set(200, Duration::UN_TIME_MS);
  agc_tc_attack.Set(50, Duration::UN_TIME_MS);
  agc_tc_decay.Set(500, Duration::UN_TIME_MS);

  in_peak = 0;
  in_avg = 0;
  out_size = 0;
  frame_size = 0;
  in_idx = 0;
  UpdateDerived();
}

void AGCBlock::UpdateDerived() {
  output_level_abs = Level::LevelToActual(output_level, gain_units);
  cur_gain_abs = Level::LevelToActual(cur_gain, gain_units);
  // dt's are: exp(-(tc)) where tc is in samples
  const float fs_update = 1000 / update_rate;
  in_dt = in_tc.GetDecayDt(fs_update);
  agc_dt_attack = agc_tc_attack.GetDecayDt(fs_update);
  agc_dt_decay = agc_tc_decay.GetDecayDt(fs_update);
}
 
void AGCBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // if user changes gain_units, rescale others
  if (!taMisc::is_loading && (gain_units != prev_gain_units)) {
    output_level = Level::Convert(output_level, prev_gain_units, gain_units);
    init_gain = Level::Convert(init_gain, prev_gain_units, gain_units);
    cur_gain = Level::Convert(cur_gain, prev_gain_units, gain_units);
    gain_thresh = Level::Convert(gain_thresh, prev_gain_units, gain_units);
    gain_limits.Set(
      Level::Convert(gain_limits.min, prev_gain_units, gain_units), 
      Level::Convert(gain_limits.max, prev_gain_units, gain_units));
  }
  // enforce limits on init_gain
  init_gain = gain_limits.Clip(init_gain);
  UpdateDerived();
  prev_gain_units = gain_units;
}

void AGCBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  
//  if (CheckError((dyn_range_out <= 0), quiet, ok,
//    "dyn_range_out must be > 0")) return;
    
  if (check) return;
  
  out_size = (int)Duration::StatGetDurationSamples(update_rate, Duration::UN_TIME_MS, src_buff->fs);
  if (CheckError((out_size == 0), quiet, ok,
		 "AGCBlock out_rate must give out_size > 0"))
    return;
  frame_size = out_size * 2;
  in_idx = 0;
  // init the cur_gain
  cur_gain = init_gain;
  UpdateDerived(); // mostly for tcs
  
  accum.SetGeom(2, 2, src_buff->fields);
  accum.Clear(); // for reconfigs
  // init peak to lowest value, and avg to neutral based on current params
  in_peak = output_level_abs *  Level::LevelToActual(gain_thresh, gain_units);
  in_avg =  output_level_abs / cur_gain_abs;
  
  out_buff.fs = src_buff->fs;
  out_buff.fields = src_buff->fields;
  out_buff.chans = src_buff->chans;
  out_buff.vals = src_buff->vals;
  
  if (out_buff_params.enabled) {
    out_buff_params.fs.SetCustom(1000.0f/update_rate);
    out_buff_params.fr_dur.Set(1, Duration::UN_SAMPLES); // we just output one item at a time
    out_buff_params.fields = 1; // same for both fields
    out_buff_params.chans = 1;//src_buff->chans;
    out_buff_params.vals = 2; // 0=gain, in gain.units
  }
  
  out_buff_energy.enabled = (agc_flags & AGC_ENERGY);
  if (out_buff_energy.enabled) {
    out_buff_energy.fs.SetCustom(1000.0f/update_rate);
    out_buff_energy.fr_dur.Set(1, Duration::UN_SAMPLES); // we just output one item at a time
    out_buff_energy.fields = 1; // same for both fields
    out_buff_energy.chans = 1;//src_buff->chans;
    out_buff_energy.vals = 2; // 0=gain, in gain.units
  }
}

void AGCBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_AGC(in_mat, stage);
}

SignalProcBlock::ProcStatus AGCBlock::AcceptData_AGC(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  float_Matrix* out_mat = &out_buff.mat;
    float_Matrix* out_mat_gain = &out_buff_params.mat;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM);
  const int in_vals = in_mat->dim(VAL_DIM);
  
  // gather stats on each items, then do it
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) {
 
    
    if (agc_flags & (AGC_ON | AGC_ENERGY)) {
      // accumulate the current dat^2 for each field, for each frame
      for (int f = 0; f < in_fields; ++f)
      for (int chan = 0; chan < in_chans; ++chan) 
      for (int v = 0; v < in_vals; ++v) {
	double dat = in_mat->FastEl(v, chan, f, i, stage);
	dat *= dat;
	for (int eo = 0; eo < 2; ++eo) {
	  accum.FastEl(eo, f) += dat;
	}
      } // field

      // we update AGC and output params every 1/2 frame size (typ 10 ms)
      if (++in_idx >= frame_size)
	in_idx = 0;
      if ((in_idx % out_size) == 0) {
	const int N = (frame_size * in_vals * in_chans);
	int eo = (in_idx == 0) ? 0 : 1; // even/odd is totally arbitray
	// compute each field energy, and accum for total energy (applicable for stereo)
        double tot_energy = 0;
	if (in_fields == 1) {
          double& dat = accum.FastEl(eo, 0);
          // compute field energy
          dat = sqrt(dat / N);
          tot_energy = dat;
	} else {
          for (int f = 0; f < in_fields; ++f) {
            double& dat = accum.FastEl(eo, f);
            tot_energy += dat; 
            // compute field energy
            dat = sqrt(dat / N);
          }
          tot_energy = sqrt(tot_energy / (N * in_fields));
	}
        if (agc_flags & AGC_ON) {
	  UpdateAGC(eo);
	}
	
	// output secondary channels
	if (out_buff_params.enabled) {
	  float dat = cur_gain;
	  out_mat_gain->Set(dat, 0, 0, 0, out_buff_params.item, out_buff_params.stage);
	  if (out_buff_params.NextIndex()) {
	    NotifyClientsBuffStageFull(&out_buff_params, 1, ps);
	  }
	}
	if (out_buff_energy.enabled) {
	  out_mat_gain->Set(tot_energy, 0, 0, 0, out_buff_energy.item, out_buff_energy.stage);
	  if (out_buff_energy.NextIndex()) {
	    NotifyClientsBuffStageFull(&out_buff_energy, 2, ps);
	  }
	}
	
      }
    } // AGC_ON	etc.
        
    // now output all data, applying current gain
    for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) 
    for (int chan = 0; ((ps == PS_OK) && (chan < in_chans)); ++chan) 
    for (int val = 0; ((ps == PS_OK) && (val < in_vals)); ++val) 
    {
      float dat = in_mat->SafeEl(val, chan, f, i, stage);
      if (!(agc_flags & AGC_BYPASS))
        dat *= cur_gain_abs;
      out_mat->Set(dat, val, chan, f, out_buff.item, out_buff.stage);
    }
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, 0, ps);
    }
  }
  return ps;
}

void AGCBlock::UpdateAGC(int eo) {
  // NOTE: the caller has calculated the avg energy for the frame/field in accum(eo, *)
  // find highest value -- that's always what we use
  double dat = accum.FastEl(eo, 0);
  for (int f = 1; f < accum.dim(1); ++f)
    dat = max(dat, accum.FastEl(eo, f));
  
  //TODO: need to decay peak somehow!
  // update peak
  if (dat > in_peak) in_peak = dat;
  
  float gain_floor = in_peak * Level::LevelToActual(gain_thresh, gain_units);
  // if the input is too low, do nothing
  if (dat < gain_floor) return;
  if (dat < 1e-6) return; // hard limit
  
  //TODO: we really need to do this by field, not in aggregate
  // integrate avg input level
  in_avg = ((1 - in_dt) * in_avg) + (in_dt * dat);
  float source; 
  switch (agc_type) {
  case AGC_AVG:
    source = in_avg; break;
  case AGC_PEAK:
    source = in_peak; break;
  }; // no default, must handle all cases
  
  // target for gain is for peak to be 1
  Level targ_gain(gain_units, (output_level_abs / source)); // init to units/act level
  // clip it *before* doing delta, integration etc. -- we clip in gain_units space
  targ_gain.Set(gain_limits.Clip(targ_gain.level));
  double delta_gain = targ_gain.act_level - cur_gain_abs;
  // note: (1 - dt) * cur + (dt * new) -> cur + (delta * dt)
  if (delta_gain > 0.0) {
    cur_gain_abs += (delta_gain * agc_dt_attack);
  } else {
    cur_gain_abs += (delta_gain * agc_dt_decay);
  }
  //calculate and limit new gain in requested units
  cur_gain = Level::ActualToLevel(cur_gain_abs, gain_units);
  if (agc_flags & AGC_UPDATE_INIT) {
    init_gain = cur_gain;
  }
  if (taMisc::use_gui) 
    DataChanged(DCR_ITEM_UPDATED);
}

