#include "gammatone.h"
 
#include "ta_project.h"
#include "ta_math.h"

#include <math.h>

//////////////////////////////////
//  GammatoneChan		//
//////////////////////////////////

const KeyString GammatoneChan::key_on("on");
const KeyString GammatoneChan::key_cf("cf");
const KeyString GammatoneChan::key_erb("erb");
const KeyString GammatoneChan::key_gain("gain");

void GammatoneChan::Initialize() {
  on = true; // assumed
  cf = 0;
  tpt = 0;
  gain = 0;
  erb = 0;
  non_lin = GammatoneBlock::NL_NONE;
}

String GammatoneChan::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_on) return String(GetEnabled());
  else if (key == key_cf) return String(cf);
  else if (key == key_erb) return  String(erb);
  else if (key == key_gain) return  String(gain);
  else return inherited::GetColText(key, itm_idx);
}

bool GammatoneChan::ChanFreqOk(float cf, float ear_q, float min_bw,
   float fs)
{
  double erb = min_bw + (cf / ear_q);
  // if 2 bandwidths (2*.5*erb) above cf exceeds nyquist, then shut us off
  bool rval = (cf + erb) < (fs / 2);
  return rval;
}

void GammatoneChan::InitChan(float cf_, float ear_q, float min_bw,
   float fs, int non_lin_, float gain_eq)
{
  cf = cf_;
  erb = min_bw + (cf / ear_q);
  // if 2 bandwidths (2*.5*erb) above cf exceeds nyquist, then shut us off
  on = ChanFreqOk(cf_, ear_q, min_bw, fs);
  if (!on) return; // nothing else will be used...
  
  non_lin = non_lin_;
  tpt = (2 * M_PI) / fs;
  double tptbw = tpt * erb * 1.019; //Moore and Glasberg (1983)
  

  // init filter coefficiants 
  double a = exp(-tptbw);
  a1 = 4.0*a; 
  a2 = -6.0*a*a; 
  a3 = 4.0*a*a*a; 
  a4 = -a*a*a*a; 
  a5 = 4.0*a*a;
  
//  gain = (tptbw*tptbw*tptbw*tptbw)/3; ning
  //gain = pow(tptbw,4) / 4.66; // jim
  gain=(2.0*(1-a1-a2-a3-a4)/(1+a1+a5)) * gain_eq; // jim 2.0
  
  coscf = cos(tpt*cf);
  sincf = sin(tpt*cf);
  cs = 1; sn = 0;
  
  
  // init buffer
  p0r=p1r=p2r=p3r=p4r=0;
  p0i=p1i=p2i=p3i=p4i=0;
  
  DataChanged(DCR_ITEM_UPDATED);
}
  
void GammatoneChan::DoFilter(int n, int in_stride, const float* x,
  int out_stride, float* bm, float* env, float* instf)
{
  double u0r, u0i, u1r, u1i;
  double oldcs, oldsn;

  //====================================================================================
  // complex z=x+j*y, exp(z) = exp(x)*(cos(y)+j*sin(y)) = exp(x)*cos(x)+j*exp(x)*sin(y).
  // z = -j * tpt*i*cf, exp(z) = cos(tpt*i*cf) - j * sin(tpt*i*cf)
  //====================================================================================
  
  for (int i=0; i<n; i++) {      
    p0r = cs*(*x) + a1*p1r + a2*p2r + a3*p3r + a4*p4r;
    p0i = sn*(*x) + a1*p1i + a2*p2i + a3*p3i + a4*p4i;

    u0r = p0r + a1*p1r + a5*p2r;
    u0i = p0i + a1*p1i + a5*p2i;
 
    p4r = p3r; p3r = p2r; p2r = p1r; p1r = p0r;
    p4i = p3i; p3i = p2i; p2i = p1i; p1i = p0i;
  
   //==========================================
   // Basilar membrane displacement
   //==========================================
    
    double tbm = (u0r*cs+u0i*sn) * gain;
    //non-linearity
    switch (non_lin) {
    //case NL_NONE:
    case GammatoneBlock::NL_HALF_WAVE:
      if (tbm < 0) tbm = 0;
      break;
    case GammatoneBlock::NL_FULL_WAVE:
      if (tbm < 0) tbm = -tbm;
      break;
    case GammatoneBlock::NL_SQUARE:
      tbm = tbm * tbm;
      break;
    default: break; // compiler food
    }
    if (bm) {
      *bm = (float)tbm;
      bm += out_stride;
    }
    
    if (env || instf) {
      //==========================================
      // Instantaneous envelope 
      //==========================================
        
      double instp = u0r*u0r+u0i*u0i;
      if (env) {
        *env = sqrt(instp) * gain;
        env += out_stride;
      }

      //==========================================
      // Instantaneous freqency
      //==========================================
      
      if (instf) {
        u1r = p1i + a1*p2i + a5*p3i;
        u1i = p1r + a1*p2r + a5*p3r;
      
        if (instp < 1.0e-20) {
          *instf = cf;
        }
        else {
          *instf = cf + (u1r*u0i - u0r*u1i)/(tpt*instp);
          if (*instf < 10 || *instf > 10000) {
            *instf = cf;
          }
        }
        instf += out_stride;
      }
    } // env

   //=========================================
   // sin(A+B) = sin(A)*cos(B) + cos(A)*sin(B)
   // cos(A+B) = cos(A)*cos(B) + sin(A)*sin(B)
   // cs = cos(tpt*i*cf); sn = -sin(tpt*i*cf);
   //=========================================
    
    cs = (oldcs=cs)*coscf + (oldsn=sn)*sincf;
    sn = oldsn*coscf - oldcs*sincf;
    
    // update in ptr
    x += in_stride;
  }
}
  


//////////////////////////////////
//  GammatoneChan_List		//
//////////////////////////////////

String GammatoneChan_List::GetColHeading(const KeyString& key) const {
  if (key == GammatoneChan::key_on) return "On";
  else if (key == GammatoneChan::key_cf) return "CF";
  else if (key == GammatoneChan::key_erb) return "ERB";
  else if (key == GammatoneChan::key_gain) return "Gain";
  else return inherited::GetColHeading(key);
}

const KeyString GammatoneChan_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return GammatoneChan::key_on;
  case 1: return GammatoneChan::key_cf;
  case 2: return GammatoneChan::key_erb;
  case 3: return GammatoneChan::key_gain;
  default: return _nilKeyString;
  }
}

int GammatoneChan_List::NumListCols() const {
  return 4;
}

//////////////////////////////////
//  GammatoneBlock		//
//////////////////////////////////

void GammatoneBlock::Initialize() {
  chan_spacing = CS_0;
  ear_q = 9.26449f;
  min_bw = 24.7f;
  cf_lo = 50.0f;
  cf_hi = 9800.0f;
  n_chans = 32;
  out_vals = OV_SIG;
  num_out_vals = 1;
  non_lin = NL_HALF_WAVE;
}

void GammatoneBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // note: because of the codes used, there is always at least one val
  num_out_vals = 0;
  if (out_vals & OV_SIG) num_out_vals++;
  if (out_vals & OV_ENV) num_out_vals++;
  if (out_vals & OV_FREQ) num_out_vals++;
  
  // some non-linearity 
  switch (non_lin) {
  case NL_HALF_WAVE:
    // chops out half the signal, so, duh...
    auto_gain.Set(2, Level::UN_SCALE);
    break;
  case NL_SQUARE:
    //NOTE: the required gain has not been determined!!!
    auto_gain.Set(1, Level::UN_SCALE);
    break;
  // all these guys are gain-neutral
  case NL_NONE:
  case NL_FULL_WAVE:
  default: 
    auto_gain.Set(1, Level::UN_SCALE);
    break;
  }
}

void GammatoneBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  
  if (CheckError((src_buff->chans > 1), quiet, ok,
    "GammatoneBlock only supports single channel input"))
    return;
    
  if (CheckError((src_buff->vals > 1), quiet, ok,
    "GammatoneBlock only supports single val input"))
    return;
    
  // warn about the gain for square nonlin
  if ((non_lin == NL_SQUARE) && check && !quiet) {
    taMisc::Warning("GammatoneBlock: use of Squaring non-linearity does not apply any gain correction");
  }
    
  // warn about nyquist violations -- some upper chans will be disabled
  bool on_hi = GammatoneChan::ChanFreqOk(cf_hi, ear_q, min_bw, src_buff->fs.fs_act);
  if ((!on_hi) && check && !quiet) {
    taMisc::Warning("GammatoneBlock: some hi freq chans will be disabled to prevent aliasing");
  }
  
  if (check) return;
  
  // just init all chans, whether enabled or not
  int buff_bit = 1;
  for (int obi = 0; obi <= 2; ++obi, buff_bit<<=1) {
    DataBuffer* ob = outBuff(obi);
    ob->enabled = (out_vals & buff_bit);
    if (!ob->enabled) continue; // will have mat set to zero 
    ob->fs = src_buff->fs;
    ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
    ob->fields = src_buff->fields;
    ob->chans = n_chans;
    ob->vals = 1;
  }
  
}

void GammatoneBlock::InitChildConfig_impl(bool check, bool quiet, bool& ok) {
//TODO: when sharpening, correct the gain of the edge channels to compensate
// for the loss caused by not integrating the full DoG filter width

  chans.SetSize(n_chans);
  for (int i = 0; i < chans.size; ++i) {
    // don't move on to next child, if prev step didn't succeed
    if (!check && !ok) return;
    GammatoneChan* gc = chans.FastEl(i);
    if (!check) {
      // center frequency
      float cf;
      switch (chan_spacing) {
      case CS_MooreGlassberg: {
        if (i < (chans.size - 1))
          cf = -(ear_q * min_bw) + exp((n_chans-i - 1) *
          (-log(cf_hi + ear_q*min_bw) + log(cf_lo + ear_q*min_bw))/(n_chans-1)) *
          (cf_hi + ear_q*min_bw);
        else 
          cf = cf_hi;
      } break;
        if (i < (chans.size - 1))
          cf = -(ear_q * min_bw) + exp((n_chans-i - 1) *
          (-log(cf_hi + ear_q*min_bw) + log(cf_lo + ear_q*min_bw))/(n_chans-1)) *
          (cf_hi + ear_q*min_bw);
        else 
          cf = cf_hi;
      case CS_LogLinear: {
        if (i == 0)
          cf = cf_lo;
        else if (i < (chans.size - 1))
          cf = exp((n_chans-i - 1) *
          (-log(cf_hi) + log(cf_lo))/(n_chans-1)) * (cf_hi);
        else 
          cf = cf_hi;
      } break; 
      default: break;// compiler food -- handle all cases above
      }
      gc->InitChan(cf, ear_q, min_bw, out_buff.fs, non_lin, auto_gain);
    }
  }
  // do the default, which calls all the items, prob does nothing
  inherited::InitChildConfig_impl(check, quiet, ok);
}

void GammatoneBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_GT(in_mat, stage);
}

SignalProcBlock::ProcStatus GammatoneBlock::AcceptData_GT(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM); // only 1 in allowed!!!
  const int in_vals = in_mat->dim(VAL_DIM); // only 1 in allowed!!!
  const int in_chan = 0; // only 1 in allowed
  const int in_val = 0; // only 1 in allowed
  
  // in_stride: the num items between each x in a channel
  const int in_stride = in_vals * in_chans * in_fields;
  // out_stride: the num items between each y in an output channel
  const int out_stride =  n_chans * in_fields;
  
  // note: we only support 1 chan, 1 val input, so don't iterate those
  for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) {
    for (int g_ch = 0; g_ch < n_chans; ++g_ch) {
      GammatoneChan* sc = chans.FastEl(g_ch);
      if (!sc->on) continue; // will just leave 0;s everywhere
      float& dat = in_mat->FastEl(in_val, in_chan, f, 0, stage);
      // we only pass buffer addresses for values actually used
      // instf
      float* bm = NULL;
      float* env = NULL;
      float* instf = NULL;
      if (out_vals & OV_SIG) {
        bm = &(out_buff.mat.FastEl(0, g_ch, f, 0, out_buff.stage));
      }
      if (out_vals & OV_ENV) {
        env =  &(out_buff_env.mat.FastEl(0, g_ch, f, 0, out_buff_env.stage));
      }
      if (out_vals & OV_FREQ) {
        instf =  &(out_buff_freq.mat.FastEl(0, g_ch, f, 0, out_buff_freq.stage));;
      }
      
      sc->DoFilter(in_items, in_stride, &dat, out_stride, bm,
          env, instf);
    }
  } // field
  
  // advance stage pointer, and notify clients
  for (int obi = 0; ((ps != PS_ERROR) && (obi <= 2)); ++obi) {
    DataBuffer* ob = outBuff(obi);
    if (!ob->enabled) continue; // will have mat set to zero 
    ob->NextStage();
    NotifyClientsBuffStageFull(ob, obi, ps);
  }
  
  return ps;
}

/* old sharpening code from above routine
  // do sharpening (if enabled) before notifying anyone
  // note that there are small gain errors at the edges, but the lowest
  // and highest freq channels are typically low information anyway
  // note: "in" and "out" in below refer to out_buff and out_buff1 resp.
  if (sharpen) {
    const int in_stage = out_buff.stage;
    const int out_stage = out_buff1.stage;
    float_Matrix& out_mat1 = out_buff1.mat;
    for (int f = 0; f < in_fields; ++f)
    for (int v = 0; v < num_out_vals; ++v)
    for (int out_ch = 0; out_ch < n_chans; ++out_ch)
    {
      float out_val = 0.0f;
      for (int offs = -dog.filter_width; offs <= dog.filter_width; ++offs) {
        int in_ch = out_ch + offs;
        // check for under/overflow (edges)
        if ((in_ch < 0) || (in_ch >= n_chans)) continue;
        float val = out_mat.FastEl(v, in_ch, f, 0, in_stage);
        out_val += dog.FilterPoint(offs, val);
      }
      out_mat1.FastEl(v, out_ch, f, 0, out_stage) = out_val;
    }
  }

*/
void GammatoneBlock::GraphFilter(DataTable* graph_data,
  bool log_freq, GammatoneBlock::OutVals response) 
{
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GammatoneFilter");
    graph_data->ClearDataFlag(DataTable::SAVE_ROWS); // don't save by default!
    newguy = true;
  }
  
  // we will set params to make nice output
  
  const int n = 1024; 
  // need vars here so we can goto
  bool ok = true;
  int idx = -1;
  ProcStatus ps = PS_OK;
  int anali = 0;
  
  // to plot the filter, we make a copy of ourself and plot the impulse response
  GammatoneBlock* gb = new GammatoneBlock;
  float_Matrix* in_mat = new float_Matrix;
  
  gb->InitLinks();
  gb->Copy(*this);
  gb->non_lin = NL_NONE; // no rect for filter response
  if ((response != OV_SIG) && (response != OV_ENV) && (response != OV_FREQ))
    response = OV_SIG;
  gb->out_vals = response;

  if (!ok) goto exit;
  // need to configure, and check it is valid, ex. fs set, etc.
  
  if (!gb->InitConfig()) goto exit;
  
{ int buff_bit = 1;
  // note: we only analyze one channel -- whatever is highest user selected
  for (int obi = 0; ((ps != PS_ERROR) && (obi <= 2)); ++obi, buff_bit<<=1) {
    DataBuffer* ob = gb->outBuff(obi);
    if (!ob->enabled) continue; // will have mat set to zero 
    anali = obi;
    ob->fr_dur.Set(n, Duration::UN_SAMPLES);
    ob->InitConfig(false, false, ok);
    if (!ok) goto exit;
  }
}  
  in_mat->SetGeom(5, 1,1,1,n,1); //val,ch,f,it,st
  in_mat->Set(1.0f, 0); // the impulse!
  
  // ok, run one round
  ps = gb->AcceptData_GT(in_mat);
  if (CheckError((ps != PS_OK), false, ok,
    "ProcNextFrame did not complete ok")) goto exit;
  // NOTE: we only analyze one of the outputs
{     
  DataBuffer* ob = outBuff(anali);
  // do an fft on it -- have to invert the data
  float_Matrix fft_in(2, n, gb->n_chans);
  for (int chan = 0; chan < gb->n_chans; ++chan)
  for (int i = 0; i < n; ++i) {
    fft_in.FastEl(i, chan) = ob->mat.SafeEl(0, chan, 0, i);
  }
  float_Matrix fft;
  // get the real (power) only
  ok = taMath_float::fft_real_transform(&fft, &fft_in,
    true, false);
  if (CheckError((!ok), false, ok,
    "fft did not complete ok")) goto exit;
{
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  valda->SetUserData("PLOT_1", true);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  zda->SetUserData("Z_AXIS", true);
  graph_data->StructUpdate(false);
  
  float_Matrix* mat = &fft;
  //NOTE: find the peak value of all the filters, for 0db point
  float peak = mat->SafeEl(0);
  for (int i = 1; i < mat->size; ++i) {
    float val = mat->FastEl_Flat(i);
    if (val > peak) peak = val;
  }
  taMisc::Info("Gammatone filter peak (for normalization) was:", String(peak));
  float fs = ob->fs;
  graph_data->DataUpdate(true);
  graph_data->AllocRows(mat->dim(1) * ((mat->dim(0) / 2)));
  for (int ch = 0; ch < mat->dim(1); ++ch) {
    // note: we omit DC, and the aliased freq's above 1/2 fs
    for (int i = 1; i < (mat->dim(0) / 2); ++i) {
      float freq = i * (fs / mat->dim(0));//gb->chans.SafeEl(i);
      if (log_freq) freq = log10(freq);
      float val = mat->FastEl(i, ch);
      float val_db;
      if (peak <= 0) val_db = -120; // shouldn't really happen
      else val_db = 10 * log10(val/peak);
      if (val_db < -120) val_db = -120; // floor for the graph
      graph_data->AddBlankRow();
      zda->SetValAsFloat(ch, -1);
      xda->SetValAsFloat(freq, -1);
      valda->SetValAsFloat(val_db, -1);
    } 
  }
  graph_data->SetUserData("VIEW_ROWS", graph_data->rows);
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}}
exit:
  if (in_mat) delete in_mat;
  if (gb) delete gb;
}


void GammatoneBlock::MakeStdFilters(float cf_lo, float cf_hi, int n_chans) {
  this->cf_lo = cf_lo;
  this->cf_hi = cf_hi;
  this->n_chans = n_chans;
  chan_spacing = CS_LogLinear;
  ear_q = 9.26449f;
  min_bw = 24.7f;
  UpdateAfterEdit();
}


//////////////////////////////////
//  TemporalDeltaBlock		//
//////////////////////////////////

/*
  The gain for the TWB will be the sum of the weights
*/
/*
void TemporalDeltaBlock::Initialize() {
  w.Set(-30.0f, Level::UN_DBI);
  tpl = 5.5f;
  tsl = 26.0f;
  tpu = 2.5f;
  tsu = 12.0f;
  l_dur = 24.0f;
  u_dur = 8.0f;
  out_rate = 4.0f;
  l_flt_wd = 0;
  u_flt_wd = 0;
}

void TemporalDeltaBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (out_rate <= 0) out_rate = 4.0;
}

void TemporalDeltaBlock::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(GetTypeDef());
  out_buffs.SetSize(2);
}

void TemporalDeltaBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
  
  // our frame rate must be > 0 (or fs calc will fault!)
  if (CheckError((l_dur < 0) || (u_dur < 0) ||
    ((l_dur + u_dur) <= 0), quiet, ok,
    "l_dur/u_dur must be >= 0 and must add to > 0"))
    return;
    
  if (CheckError((out_rate <= 0), quiet, ok,
    "out_rate must be > 0"))
    return;
    
  if (!ok) return;
  
  if (!check) {
    // each output value is one stage by definition, but we need to keep several on the go
    // since we are convolving the temporal filter as we get inputs
    out_buff.fs.SetCustom(1 / (out_rate / 1000));
    out_buff.fr_dur.Set(1, Duration::UN_SAMPLES);
    out_buff.stages = Duration::StatGetDurationSamples((l_dur + u_dur),
      Duration::UN_TIME_MS, out_buff.fs);
    out_buff.fields = src_buff->fields;
    out_buff.chans = src_buff->chans;
    out_buff.vals = src_buff->vals;
    // output duration is set in terms of the input sample rate
  }
  // filter
  CheckMakeFilter(src_buff->fs, check, quiet, ok);
}

void TemporalDeltaBlock::InitThisConfigDataOut_impl(bool check,
  bool quiet, bool& ok)
{
  // clear out old data
  if (!check) out_buff.mat.Reset();
  inherited::InitThisConfigDataOut_impl(check, quiet, ok);
  if (!ok) return;
  if (check) return;
  
  
  // init the conv indexes, as if we've already been processing
  conv_idx.SetGeom(1, out_buff.stages);
  for (int i = 0; i < conv_idx.size; ++i) {
    // spread the conv idx's evenly over the buff items
    // because we apply in asc order, we need to preload in desc order
    // i.e. next guy out will be one most through the seq
    int idx = ((int) ((conv_idx.size - i) * ((float)filter.size) / (float)out_buff.stages)) - 1;
    conv_idx.Set(idx, i);
  }
}

void TemporalDeltaBlock::CheckMakeFilter(const SampleFreq& fs_in,
  bool check, bool quiet, bool& ok)
{
  //figure out # samples we'll need in our filter, based on filter width
  float tot_dur = l_dur + u_dur;
  
  l_flt_wd = Duration::StatGetDurationSamples(
    l_dur, Duration::UN_TIME_MS, fs_in);
  u_flt_wd = Duration::StatGetDurationSamples(
    u_dur, Duration::UN_TIME_MS, fs_in);
  if (CheckError(((l_flt_wd + u_flt_wd) <= 0), quiet, ok,
    "filter width is 0 -- check sample rate and filter widths"))
    return;
    
  if (CheckError(((w > 1) || (w < 0)), quiet, ok,
    "w must be a weighting factor from 0 to 1"))
    return;
  if (CheckError(((tpl <= 0) || (tsl <= 0) || (tpu <= 0) || (tsu <= 0)), quiet, ok,
    "tpl/tsl/tpu/tsu values must be > 0"))
    return;
    
  if (check) return;
  
  filter.SetGeom(1, l_flt_wd + u_flt_wd);
  
  double filt_gain = 0; // gain of the filter -- we normalize by this
  // make filter -- note: we define lower[0] as t = 0
  // lower -- time goes -ve
  // note: the formula uses -t, but easier for us to calc
  for (int i = 0; i < l_flt_wd; ++i) {
    float nt = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    float val = (1-w) * exp(nt/tpl) + (w * exp(nt/tsl));
    filt_gain += val;
    filter.Set_Flat(val, i);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    float nt = -( (((float) (i + 1)) / u_flt_wd) * u_dur);
    float val = (1-w) * exp(nt/tpu) + (w * exp(nt/tsu));
    filter.Set_Flat(val, l_flt_wd + i);
    filt_gain += val;
  } 
  // normalize
  filt_gain = 1 / filt_gain; // more efficient to multiply by this
//TEMP
taMisc::Info("Normalizing TemporalDeltaBlock by ", String(filt_gain));
  for (int i = 0; i < filter.size; ++i) {
    filter.FastEl(i) *= filt_gain;
  } 
}

void TemporalDeltaBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  const int fields = in_mat->dim(FIELD_DIM);
  const int chans = in_mat->dim(CHAN_DIM); 
  const int vals = in_mat->dim(VAL_DIM);
  float_Matrix* out_mat = &out_buff.mat;
  // note: critical this invariant is true:
  bool ok = true;
  // we do one input item at a time, convolving with all our out guys
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) {
  
    // iterate all our outputs, convolving in, and outputting when appropriate
    for (int ci = 0; ci < conv_idx.size; ++ci) {
      int& cidx = conv_idx.FastEl(ci); // we test/inc/set
      float filt_val = filter.SafeEl(cidx);
      // std accept loop:
      for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f)
      for (int chan = 0; ((ps == PS_OK) && (chan < chans)); ++chan) 
      for (int val = 0; ((ps == PS_OK) && (val < vals)); ++val)
      {
          float dat = in_mat->SafeElAsFloat(val, chan, f, i, stage);
          float& item = out_mat->FastEl(val, chan, f, 0, ci);
          item += (dat * filt_val);
      }
      // now, dec the conv_indx, and if < 0, means need to output and reset 
      if (++cidx >= filter.size) {
        cidx = 0;
        out_buff.stage = ci;
        // send output
        out_buff.NextIndex(); // note: always rolls over, so we don't test
        NotifyClientsBuffStageFull(&out_buff, 0, ps);
        
        // clear out the line
        for (int f = 0; f < fields; ++f) 
        for (int chan = 0; chan < chans; ++chan) 
        for (int val = 0; val < vals; ++val)
        {
          out_mat->Set(0, val, chan, f, 0, ci);
        }
      }
    }
  }
}


void TemporalDeltaBlock::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_TemporalFilter", true);
    newguy = true;
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  valda->SetUserData("PLOT_1", true);
  graph_data->StructUpdate(false);
  
  graph_data->DataUpdate(true);
  float_Matrix* mat = &filter;
  float t, val;
  for (int i = 0; i < l_flt_wd; ++i) {
    float t = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    val = mat->SafeEl(i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    float t = ( (((float) (i + 1)) / u_flt_wd) * u_dur);
    val = mat->SafeEl(l_flt_wd + i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  }
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}
*/

//////////////////////////////////
//  TemporalWindowBlock		//
//////////////////////////////////

/*
  The gain for the TWB will be the sum of the weights
*/

void TemporalWindowBlock::Initialize() {
  w.Set(-30.0f, Level::UN_DBI);
  tpl = 5.5f;
  tsl = 26.0f;
  tpu = 2.5f;
  tsu = 12.0f;
  l_dur = 24.0f;
  u_dur = 8.0f;
  out_rate = 4.0f;
  l_flt_wd = 0;
  u_flt_wd = 0;
}

void TemporalWindowBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (out_rate <= 0) out_rate = 4.0;
}

void TemporalWindowBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
  
  // our frame rate must be > 0 (or fs calc will fault!)
  if (CheckError((l_dur < 0) || (u_dur < 0) ||
    ((l_dur + u_dur) <= 0), quiet, ok,
    "l_dur/u_dur must be >= 0 and must add to > 0"))
    return;
    
  if (CheckError((out_rate <= 0), quiet, ok,
    "out_rate must be > 0"))
    return;
    
  if (!ok) return;
  
  if (!check) {
    // each output value is one stage by definition, but we need to keep several on the go
    // since we are convolving the temporal filter as we get inputs
    out_buff.fs.SetCustom(1 / (out_rate / 1000));
    out_buff.fr_dur.Set(1, Duration::UN_SAMPLES);
    out_buff.stages = Duration::StatGetDurationSamples((l_dur + u_dur),
      Duration::UN_TIME_MS, out_buff.fs);
    out_buff.fields = src_buff->fields;
    out_buff.chans = src_buff->chans;
    out_buff.vals = src_buff->vals;
    // output duration is set in terms of the input sample rate
  }
  // filter
  CheckMakeFilter(src_buff->fs, check, quiet, ok);
}

void TemporalWindowBlock::InitThisConfigDataOut_impl(bool check,
  bool quiet, bool& ok)
{
  // clear out old data
  if (!check) out_buff.mat.Reset();
  inherited::InitThisConfigDataOut_impl(check, quiet, ok);
  if (!ok) return;
  if (check) return;
  
  
  // init the conv indexes, as if we've already been processing
  conv_idx.SetGeom(1, out_buff.stages);
  for (int i = 0; i < conv_idx.size; ++i) {
    // spread the conv idx's evenly over the buff items
    // because we apply in asc order, we need to preload in desc order
    // i.e. next guy out will be one most through the seq
    int idx = ((int) ((conv_idx.size - i) * ((float)filter.size) / (float)out_buff.stages)) - 1;
    conv_idx.Set(idx, i);
  }
}

void TemporalWindowBlock::CheckMakeFilter(const SampleFreq& fs_in,
  bool check, bool quiet, bool& ok)
{
  //figure out # samples we'll need in our filter, based on filter width
  float tot_dur = l_dur + u_dur;
  
  l_flt_wd = Duration::StatGetDurationSamples(
    l_dur, Duration::UN_TIME_MS, fs_in);
  u_flt_wd = Duration::StatGetDurationSamples(
    u_dur, Duration::UN_TIME_MS, fs_in);
  if (CheckError(((l_flt_wd + u_flt_wd) <= 0), quiet, ok,
    "filter width is 0 -- check sample rate and filter widths"))
    return;
    
  if (CheckError(((w > 1) || (w < 0)), quiet, ok,
    "w must be a weighting factor from 0 to 1"))
    return;
  if (CheckError(((tpl <= 0) || (tsl <= 0) || (tpu <= 0) || (tsu <= 0)), quiet, ok,
    "tpl/tsl/tpu/tsu values must be > 0"))
    return;
    
  if (check) return;
  
  filter.SetGeom(1, l_flt_wd + u_flt_wd);
  
  double filt_gain = 0; // gain of the filter -- we normalize by this
  // make filter -- note: we define lower[0] as t = 0
  // lower -- time goes -ve
  // note: the formula uses -t, but easier for us to calc
  for (int i = 0; i < l_flt_wd; ++i) {
    float nt = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    float val = (1-w) * exp(nt/tpl) + (w * exp(nt/tsl));
    filt_gain += val;
    filter.Set_Flat(val, i);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    float nt = -( (((float) (i + 1)) / u_flt_wd) * u_dur);
    float val = (1-w) * exp(nt/tpu) + (w * exp(nt/tsu));
    filter.Set_Flat(val, l_flt_wd + i);
    filt_gain += val;
  } 
  // normalize
  filt_gain = 1 / filt_gain; // more efficient to multiply by this
//TEMP
taMisc::Info("Normalizing TemporalWindowBlock by ", String(filt_gain));
  for (int i = 0; i < filter.size; ++i) {
    filter.FastEl(i) *= filt_gain;
  } 
}

void TemporalWindowBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  const int fields = in_mat->dim(FIELD_DIM);
  const int chans = in_mat->dim(CHAN_DIM); 
  const int vals = in_mat->dim(VAL_DIM);
  float_Matrix* out_mat = &out_buff.mat;
  // note: critical this invariant is true:
  bool ok = true;
  // we do one input item at a time, convolving with all our out guys
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) {
  
    // iterate all our outputs, convolving in, and outputting when appropriate
    for (int ci = 0; ci < conv_idx.size; ++ci) {
      int& cidx = conv_idx.FastEl(ci); // we test/inc/set
      float filt_val = filter.SafeEl(cidx);
      // std accept loop:
      for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f)
      for (int chan = 0; ((ps == PS_OK) && (chan < chans)); ++chan) 
      for (int val = 0; ((ps == PS_OK) && (val < vals)); ++val)
      {
          float dat = in_mat->SafeElAsFloat(val, chan, f, i, stage);
          float& item = out_mat->FastEl(val, chan, f, 0, ci);
          item += (dat * filt_val);
      }
      // now, dec the conv_indx, and if < 0, means need to output and reset 
      if (++cidx >= filter.size) {
        cidx = 0;
        out_buff.stage = ci;
        // send output
        out_buff.NextIndex(); // note: always rolls over, so we don't test
        NotifyClientsBuffStageFull(&out_buff, 0, ps);
        
        // clear out the line
        for (int f = 0; f < fields; ++f) 
        for (int chan = 0; chan < chans; ++chan) 
        for (int val = 0; val < vals; ++val)
        {
          out_mat->Set(0, val, chan, f, 0, ci);
        }
      }
    }
  }
}


void TemporalWindowBlock::GraphFilter(DataTable* graph_data) {
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_TemporalFilter", true);
    newguy = true;
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  valda->SetUserData("PLOT_1", true);
  graph_data->StructUpdate(false);
  
  graph_data->DataUpdate(true);
  float_Matrix* mat = &filter;
  float t, val;
  for (int i = 0; i < l_flt_wd; ++i) {
    float t = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    val = mat->SafeEl(i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    float t = ( (((float) (i + 1)) / u_flt_wd) * u_dur);
    val = mat->SafeEl(l_flt_wd + i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  }
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}


//////////////////////////////////
//  IIDBlock			//
//////////////////////////////////

void IIDBlock::Initialize() {
  chan.max = -1;
}

/*void IIDBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}*/

void IIDBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
  
  if (CheckError((src_buff->fields > 2), quiet, ok,
    "IIDBlock: requires mono (dummy mode) or stereo"))
    return;
  
  // won't produce output unless stereo
  if ((src_buff->fields == 1) && check && !quiet) {
    taMisc::Warning("IIDBlock: requires stereo (fields==2) to produce output");
  }
  
  if (!ok) return;
  
  if (check) return;
  
  // determine eff chan
  chan_eff = chan;
  if (chan_eff.min < 0) 
    chan_eff.min = 0;
  if (chan_eff.min >= src_buff->chans) 
    chan_eff.min = src_buff->chans - 1;
  if (chan_eff.max < 0) 
    chan_eff.max = src_buff->chans - 1;
  if (chan_eff.max < chan_eff.min)
    chan_eff.max = chan_eff.min;
  
  int n_chans = chan_eff.Count();
  
  if (CheckError((n_chans == 0), quiet, ok,
    "IIDBlock -- no effective channels to process!"))
    return;
  
  for (int obi = 0; obi <= 1; ++obi) {
    DataBuffer* ob = outBuff(obi);
    ob->fs = src_buff->fs;
    ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
    ob->fields = src_buff->fields; // sb 2
    ob->chans = n_chans;
    ob->vals = src_buff->vals;
  }
}

void IIDBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_IID(in_mat, stage);
}

SignalProcBlock::ProcStatus IIDBlock::AcceptData_IID(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM); // needs to be 2 for meaningful output!
  const int in_chans = in_mat->dim(CHAN_DIM); // typically many, but we only use range
  const int in_vals = in_mat->dim(VAL_DIM); 
  
  const int in_val = 0; // only 1 in allowed
  // we use this hack to do dummy processing when input is mono (r = l)
  // note that this hack is merely a convenience so stuff still works, but is
  // not usually used in practice, so don't worry that we repeat saves etc. below
  const int field_r = (in_fields == 2) ? 1 : 0;
  
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) 
//  for (int f = 0; ((ps == PS_OK) && (f < in_mat->dim(SignalProcBlock::FIELD_DIM))); ++f) 
  for (int v = 0; ((ps == PS_OK) && (v < in_mat->dim(SignalProcBlock::VAL_DIM))); ++v)
  {
    int out_ch = 0;
    for (int in_ch = chan_eff.min;
      ((ps == PS_OK) && (in_ch <= chan_eff.max)); ++in_ch, ++out_ch)
    {
      float dat_l = in_mat->SafeElAsFloat(v, in_ch, 0, i, stage);
      float dat_r =  in_mat->SafeElAsFloat(v, in_ch, field_r, i, stage);
      float lmr = dat_l - dat_r;
      float pl_l, mi_l, pl_r, mi_r;
      if (lmr >= 0.0f) {
        pl_l = lmr; mi_l = 0.0f;  pl_r = 0.0f; mi_r = lmr;
      } else {
        pl_l = 0.0f; mi_l = -lmr;  pl_r = -lmr; mi_r = 0.0f;
      }
      // left output
      out_buff_pl.mat.FastEl(v, out_ch, 0, i, out_buff_pl.stage) = pl_l;
      out_buff_mi.mat.FastEl(v, out_ch, 0, i, out_buff_mi.stage) = mi_l;
      // right output
      // note: all vals are 0 anyway if mono, so just rewrite for that case
      out_buff_pl.mat.FastEl(v, out_ch, field_r, i, out_buff_pl.stage) = pl_r;
      out_buff_mi.mat.FastEl(v, out_ch, field_r, i, out_buff_mi.stage) = mi_r;
    }
  }
  return ps;  
}


//////////////////////////////////
//  ITDBlock			//
//////////////////////////////////

void ITDBlock::Initialize() {
  chan.max = -1;
}

/*void IIDBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}*/


//////////////////////////////////
//  ANBlock			//
//////////////////////////////////

void ANBlock::Initialize() {
}

void ANBlock::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ANBlock);
  if (!taMisc::is_loading) {
    val_list.New(1);
  }
}

void ANBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  
  if (CheckError((val_list.size < 1), quiet, ok,
    "You must have at least one output value"))
    return;
    
  if (CheckError((src_buff->vals > 1), quiet, ok,
    "ANBLock only supports single val input"))
    return;
    
    
  if (check) return;
  out_buff.fs = src_buff->fs;
  out_buff.fields = src_buff->fields;
  out_buff.chans = src_buff->chans;
  out_buff.vals = val_list.size;
}

void ANBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_AN(in_mat, stage);
}

SignalProcBlock::ProcStatus ANBlock::AcceptData_AN(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  float_Matrix* out_mat = &out_buff.mat;
  const int out_vals = out_buff.vals;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM);
  const int in_vals = in_mat->dim(VAL_DIM);
  const int in_val = 0;
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) {
    for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) 
    for (int chan = 0; ((ps == PS_OK) && (chan < in_chans)); ++chan) 
    {
      float dat = in_mat->SafeEl(in_val, chan, f, i, stage);
      dat *= in_gain; 
      for (int out_val = 0; out_val < out_vals; ++out_val) {
        ANVal* av = val_list.FastEl(out_val);
        float res = av->CalcValue(dat);
        out_mat->Set(res, out_val, chan, f, out_buff.item, out_buff.stage);
      }
    }
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, 0, ps);
      //TODO: also write to output table if we have one
    }
  }
  return ps;
}

void ANBlock::MakeVals(ANBlock::ANValType val_type, int n_vals,
    float cl_min, float cl_max)
{
  bool ok = true;
  if (CheckError(!((n_vals >= 1) &&
    (cl_min < cl_max)), false, ok, 
    "Params must be: (n_vals >= 1) && "
      "(cl_min < cl_max))")) return;
      
  // just do it analytically
  float l_ovl = 6.0f;
  float cl_step = (cl_max - cl_min) / (n_vals - 1);
  float l_wid = cl_step - (l_ovl / 2);
  // width is double for GAUSS
  if (val_type == ANBlock::AN_GAUSS)
    l_wid *= 2.0f;
   
  val_list.SetSize(n_vals);
  float cl = cl_min;
  for (int i = 0; i < n_vals; ++i, cl += cl_step) {
    ANVal* val = val_list.FastEl(i);
    val->SetParams(val_type, cl, l_wid);
  }
  /*nuke  float cl = l_max - (l_wid / 2);
  do { // make at least one
    ANVal* val = (ANVal*)val_list.New(1);
    val->SetParams(val_type, cl, l_wid);
    cl -= (l_wid - (l_ovl / 2));
  } while ((cl - l_wid) >= l_min);*/
}

void ANBlock::GraphFilter(DataTable* graph_data, float l_min) {
  if (l_min < -120) l_min = -120;
  else if (l_min > -48) l_min = -48;
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ANFilter", true);
    newguy = true;
  }
  
  // we will set params to make nice output
  
  const double l_inc = 0.1f;
  const int n = (int)(-l_min / l_inc) + 1;
  // need vars here so we can goto
  bool ok = true;
  int idx = -1;
  ProcStatus ps = PS_OK;
  Level l;
  
  // to plot the filter, we make a copy of ourself and plot the impulse response
  ANBlock* an = new ANBlock;
  float_Matrix* in_mat = new float_Matrix;
  
  an->InitLinks();
  an->Copy(*this);
  an->out_buff.fr_dur.Set(n, Duration::UN_SAMPLES);
  an->out_buff.stages = 1;
  an->out_buff.chans = 1;
  an->out_buff.vals = val_list.size;
  // need to configure, and check it is valid, ex. fs set, etc.
  if (!an->InitConfig()) goto exit;
  
  in_mat->SetGeom(5, 1,1,1,1,1); 
  
  
  // ok, run one round
  for (int i = 0; i < n; ++i) {
    l.Set(l_min + (i * l_inc), Level::UN_DBI);
    in_mat->Set(1.0f * l, 0); // the unitary input!
    ps = an->AcceptData_AN(in_mat);
    if (CheckError((ps != PS_OK), false, ok,
      "AcceptData_AN did not complete ok")) goto exit;
  }
{
{
  float_Matrix* mat = &an->out_buff.mat;
  const int vals = mat->dim(VAL_DIM);
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* yda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  yda->SetUserData("PLOT_1", true);
  DataCol* zda = NULL;
  if (vals > 1) {
    zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
    zda->SetUserData("Z_AXIS", true);
  }
  graph_data->StructUpdate(false);
  
  graph_data->DataUpdate(true);
  graph_data->AllocRows(mat->size);
  // do each val as a separate sequence
  const int ch = 0;
  const int fld = 0;
  const int stage = 0;
  for (int val = 0; val < vals; ++val) {
    for (int i = 0; i < n; ++i) {
      float l = l_min + (i * l_inc);
      float dat = mat->SafeEl(val, ch, fld, i, stage);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(l, -1);
      yda->SetValAsFloat(dat, -1);
      if (zda) zda->SetValAsFloat(val, -1);
    } 
  }
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}}
exit:
  if (in_mat) delete in_mat;
  if (an) delete an;
}

//////////////////////////////////
//  ANVal			//
//////////////////////////////////

void ANVal::Initialize() {
  val_type = ANBlock::AN_EXP; // legacy default
  // note: following would be for only one channel, spanning ~96 dB
  cl = -48;
  width = 80;
  f = .3f; // determined empirically - gives ~ .5-.95 points for +-10dB
}

void ANVal::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateParams();
}

void ANVal::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  if (CheckError(((cl < -120) || (cl >= 0)), quiet, ok,
    "cl should be in range: -120:0")) return;
  if (CheckError(((width <= 0) || (width > 120)), quiet, ok,
    "width should be in range: 0:120")) return;
    
    
  if (check) return;
}

float ANVal::CalcValue(float in) {
  if (in < 0) return 0; // only defined for non-neg values
  // transform to dB -- sh/be ~ -96 < in_db <= 0
  float in_db = 10 * log10(in); // note: the ref is 1, but ok if exceeded
  // translate so that cf is at 0, and normalize
  double rval = (in_db - cl) * norm; 
  switch (val_type) {
  case ANBlock::AN_EXP: {  
    // do the exponential
    rval = 1 / (1 + exp(-(rval * f)));
    } break;
  case ANBlock::AN_SIG: {
    //TODO:
    } break;
  case ANBlock::AN_GAUSS: {  
    // do the guassian
    rval = exp(-((rval * rval)/2)) * f;
    } break;
  //no default -- must handle all, so let compiler warn
  }
  return rval;
}

void ANVal::SetParams(ANBlock::ANValType val_type_,
    float cl_, float width_)
{
  val_type = val_type_;
  cl = cl_;
  width = width_;
  UpdateParams();
}

void ANVal::UpdateParams()
{
  switch (val_type) {
  case ANBlock::AN_EXP: {  
    norm = (20 / width);
    f = .3f; // determined empirically - gives ~ .5-.95 points for +-10dB
    } break;
  case ANBlock::AN_SIG: {
    //TODO:
    } break;
  case ANBlock::AN_GAUSS: {  
    // a 90% confidence interval is 1.64485 sd's, so we normalize accordingly
    // divide by 2 because 2-tailed
    norm = 1.64485 / (width / 2);
    f = 1.0f;1 / sqrt(2 * M_PI);
    } break;
  //no default -- must handle all, so let compiler warn
  }
}

