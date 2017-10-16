// Copyright 2017, Regents of the University of Colorado,
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

// TA-like base classes for State implementation code
// included in appropriate specific context-setting code: State_core.h, cuda.h

#pragma maketa_file_is_target NetworkState


// these are all the base State classes

class STATE_CLASS(NetworkState); //
class STATE_CLASS(LayerState); //
class STATE_CLASS(PrjnState); //
class STATE_CLASS(UnGpState); //
class STATE_CLASS(UnitState); //
class STATE_CLASS(ConState); // 

// and corresponding spec core classes that contain all the algorithms

class STATE_CLASS(LayerSpec); //
class STATE_CLASS(UnitSpec); //
class STATE_CLASS(ConSpec); //

class STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class for core
public:
};

class STATE_CLASS(taOBase) : public STATE_CLASS(taBase) {
  // ##NO_TOKENS ##INLINE base class with owner for core
public:
};

class STATE_CLASS(taNBase) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS ##INLINE base class with owner and name for core
public:
};

class STATE_CLASS(BaseSpec) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all State versions of specs
public:
};

class STATE_CLASS(SpecMemberBase) : public STATE_CLASS(taNBase) {
  // ##NO_TOKENS ##INLINE base class for all core versions of spec members
public:
};


class STATE_CLASS(Average) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math encapsulates the sum and count values for computing an average value in an incremental fashion over time
public:
  float         avg;    // the computed average value that was last computed, as sum / n -- this may not reflect current sum, n values -- just depends on when GetAvg was computed -- see UpdtAvg versions of increment methods
  float         sum;    // #DMEM_AGG_SUM overall sum of values accumulated since last reset
  int           n;      // #DMEM_AGG_SUM the number of values accumulated since last reset


  INLINE void   ResetSum()
  { sum = 0.0f; n = 0; }
  // #CAT_Average reset the sum and n accumulation variables
  INLINE void   ResetAvg()
  { ResetSum(); avg = 0.0f; }
  // #CAT_Average reset the sum and n accumulation variables, and the computed average value

  INLINE float  GetAvg()
  { if(n > 0) avg = sum / (float)n; return avg; }
  // #CAT_Average compute the average as sum / n, update the avg member to store this value, and return it

  INLINE float  GetAvg_Reset()
  { float rval = GetAvg(); ResetSum(); return rval; }
  // #CAT_Average compute the average as sum / n, update the avg member to store this value, and return it -- also reset sum, n counters for next time

  INLINE void   Increment(float val)
  { sum += val; n++; }
  // #CAT_Average increment the sum by given value, and n by one count
  INLINE float  IncrementAvg(float val)
  { Increment(val); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by one count, and return new average value (updates avg field)

  INLINE void   IncrementN(float val, int n_new)
  { sum += val; n += n_new; }
  // #CAT_Average increment the sum by given value, and n by given count -- for a batch update of multiple items
  INLINE float  IncrementNAvg(float val, int n_new)
  { IncrementN(val, n_new); return GetAvg(); }
  // #CAT_Average increment the sum by given value, and n by given count, and return new average value (updates avg field)

  STATE_CLASS(Average)()      { ResetAvg(); }
};

 

class STATE_CLASS(MinMax) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##CAT_Math minimum-maximum values
  INHERITED(taOBase)
public:
  float         min;    // minimum value
  float         max;    // maximum value

  INLINE bool  RangeTest(float val) const { return ((val > min) && (val < max)); }
  // test whether value is within the range (but not equal to max or min)
  INLINE bool  RangeTestEq(float val) const { return ((val >= min) && (val <= max)); }
  // test whether value is within the range (or equal)

  INLINE bool  operator < (const float val) const   { return (val < min); }
  INLINE bool  operator <= (const float val) const  { return (val <= min); }
  INLINE bool  operator > (const float val) const   { return (val > max); }
  INLINE bool  operator >= (const float val) const  { return (val >= max); }

  INLINE void  Init(float it)  { min = max = it; }  // initializes the max and min to this value
  INLINE void  Set(float mn, float mx) { min = mn; max = mx; }  // set values

  INLINE float  Range() const  { return (max - min); }
  INLINE float  Scale() const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }
  // scale is the inverse of range
  INLINE float  MidPoint() const { return 0.5f * (min + max); }
  // returns the range between the min and the max

  INLINE void  UpdateRange(STATE_CLASS(MinMax)& it)
  { min = fminf(it.min, min); max = fmaxf(it.max, max); }

  INLINE void  UpdateRange(float it)
  { min = fminf(it, min); max = fmaxf(it, max); }  // updates the range

  INLINE void  MaxLT(float it)  { max = fminf(it, max); }
  // max less than (or equal)

  INLINE void  MinGT(float it)  { min = fmaxf(it, min); }
  // min greater than (or equal)

  INLINE void  WithinRange(STATE_CLASS(MinMax)& it)         // put my range within given one
  { min = fmaxf(it.min, min); max = fminf(it.max, max); }
  INLINE void  WithinRange(float min_, float max_) // #IGNORE put my range within given one
  { min = fmaxf(min_, min); max = fminf(max_, max); }
  INLINE void  SymRange() // symmetrize my range around zero, with max abs value of current min, max
  { float mxabs = fmaxf(fabsf(min), fabsf(max)); min = -mxabs; max = mxabs; }

  INLINE float Normalize(float val) const { return (val - min) * Scale(); }
  // normalize given value to 0-1 range given current in max

  INLINE float Project(float val) const   { return min + (val * Range()); }
  // project a normalized value into the current min-max range

  INLINE float Clip(float val) const
  { val = fminf(max,val); val = fmaxf(min,val); return val; }
  // clip given value within current range

  STATE_CLASS(MinMax)() { Initialize(); }
  
private:
  void  Initialize()            { min = max = 0.0f; }
  void  Copy_(const STATE_CLASS(MinMax)& cp) { min = cp.min; max = cp.max; }
};


class STATE_CLASS(MinMaxRange) : public STATE_CLASS(MinMax) {
  // ##UAE_IN_PROGRAM min-max values plus scale and range
public:
  float         range;          // #HIDDEN distance between min and max
  float         scale;          // #HIDDEN scale (1.0 / range)

  INLINE float Normalize(float val) const { return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max

  INLINE float Project(float val) const   { return min + (val * range); }
  // project a normalized value into the current min-max range

  STATE_CLASS(MinMaxRange)() { Initialize(); }
  
private:
  void  Initialize()            { range = scale = 0.0f; }
  void  Copy_(const STATE_CLASS(MinMaxRange)& cp) { range = cp.range; scale = cp.scale; }
};


class STATE_CLASS(PRerrVals) : public STATE_CLASS(taOBase) {
  // ##NO_TOKENS #INLINE #NO_UPDATE_AFTER ##CAT_Network contains precision and recall error values
public:
  float         true_pos;       // #DMEM_AGG_SUM true positive values -- e.g., how many active targets were correctly activated by the network (actually a sum over graded activations -- if(targ > act) act else targ)
  float         false_pos;      // #DMEM_AGG_SUM false positive values -- e.g., how many inactive targets were incorrectly activated by the network (actually a sum over graded activations -- if(act > targ) act - targ)
  float         false_neg;      // #DMEM_AGG_SUM false negative values -- e.g., how many active targets were incorrectly not activated by the network (actually a sum over graded activations -- if(targ > act) targ - act)
  float         true_neg;      // #DMEM_AGG_SUM true negative values -- e.g., to what extent was the unit correctly off
  float         precision;      // precision = true_pos / (true_pos + false_pos) -- how many of the positive responses were true positives -- i.e., of the responses the network made, how many were correct -- does not include any of the false negatives, so the network could be under-responding (see recall)
  float         recall;         // recall = true_pos / (true_pos + false_neg) -- how many true positive responses were there relative to the total number of positive targets -- if it did not respond to a number of cases where it should have, then recall will be low
  float         specificity;    // specificity = true_neg / (false_pos + true_neg) -- proportion of true negatives correctly identified as such
  float         fallout;       // fallout = fp / (fp + tn) -- fallout - false positive rate
  float         fdr;           // false discovery rate = fp / (fp + tp)
  float         fmeasure;       // fmeasure = 2 * precision * recall / (precision + recall) -- harmonic mean of precision and recall -- is 1 when network is performing optimally
  float         mcc;            // mcc = ((tp*tn) - (fp*fn)) / sqrt((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)) -- Matthews Correlation Coefficient. balanced statistic that reflects all of the kinds of errors the network could make. ranges from -1 to 1. this is the best overall value to look at for summary performance

  INLINE void   InitVals() { true_pos = false_pos = false_neg = true_neg = specificity = mcc = precision = recall = fallout = fdr = fmeasure = 0.0f; }
  // initialize all values to 0

  INLINE void   IncrVals(const STATE_CLASS(PRerrVals)& vls)
  { true_pos += vls.true_pos; false_pos += vls.false_pos; false_neg += vls.false_neg; true_neg += vls.true_neg; }
  // increment values from another set of values

  INLINE void   ComputePR() {
    precision = recall = specificity = fdr = fallout = fmeasure = mcc = 0.0f;
    float tp = true_pos; float fp = false_pos; float tn = true_neg; float fn = false_neg;

    precision   = tp > 0 && fp > 0  ? tp/(tp+fp):0;
    recall      = tp > 0 && fn > 0  ? tp/(tp+fn):0;
    specificity = fp > 0 && tn > 0  ? tn/(fp+tn):0;
    fdr         = tp > 0 && fp > 0  ? fp/(fp+tp):0;
    fallout     = fp > 0 && tn > 0  ? fp/(fp+tn):0;
    fmeasure    = tp > 0 && fp > 0 && fn > 0 ? 2*tp/(2*tp+fp+fn):0;
    mcc         = tp > 0 && fp > 0 && tn > 0 && fn > 0 ?
      (tp*tn-fp*fn)/sqrtf((tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)):0;
  }
  // compute the precision, recall, and fmeasure values based on current raw stats values

  STATE_CLASS(PRerrVals)() { InitVals(); }
};


#ifndef STATE_CUDA

// TODO: need to make MTRnd independent of TA, and eliminate taMath_double depend

#include <MTRnd>
#include <taMath_double>

class STATE_CLASS(Random) : public STATE_CLASS(taNBase) {
  // #STEM_BASE #NO_UPDATE_AFTER ##INLINE ##CAT_Math ##NO_TOKENS ##STATIC_COMPLETION Random Number Generation
INHERITED(taNBase)
public:
  enum Type {
    UNIFORM,                    // uniform with var = range on either side of the mean
    BINOMIAL,                   // binomial with var = p, par = n
    POISSON,                    // poisson with lambda = var
    GAMMA,                      // gamma with var scaling parameter and par = k stages
    GAUSSIAN,                   // normal with var
    BETA,                       // beta with var = a and par = b shape parameters
    NONE,                       // just the mean
  };

  Type          type;           // type of random variable to generate
  double        mean;           // mean of random distribution
  double        var;            // #CONDSHOW_OFF_type:NONE 'varibility' parameter for the random numbers (gauss = standard deviation, not variance; uniform = half-range)
  double        par;            // #CONDSHOW_ON_type:GAMMA,BINOMIAL,BETA extra parameter for distribution (depends on each one)

  INLINE double Gen(int thr_no = -1) const {
    if(var == 0.0f) return mean;
    switch(type) {
    case NONE:
      return mean;
    case UNIFORM:
      return UniformMeanRange(mean, var, thr_no);
    case BINOMIAL:
      return mean + Binom((int)par, var, thr_no);
    case POISSON:
      return mean + Poisson(var, thr_no);
    case GAMMA:
      return mean + Gamma(var, (int)par, thr_no);
    case GAUSSIAN:
      return mean + Gauss(var, thr_no);
    case BETA:
      return mean + Beta(var, par, thr_no);
    }
    return 0.0f;
  }
  // generate a random variable according to current parameters -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  
  ////////////////////////////////////////////////////////////////////////
  // various handy static random number generation functions:

  static int    IntZeroN(int n, int thr_no = -1)
  { if(n > 0) return (int)(MTRnd::GenRandInt32(thr_no) % (uint)n); return 0; }
  // #CAT_Int uniform random integer in the range between 0 and n, exclusive of n: [0,n) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static int    IntMinMax(int min, int max, int thr_no = -1)
  { return min + IntZeroN(max - min, thr_no); }
  // #CAT_Int uniform random integer in range between min and max, exclusive of max: [min,max) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static int    IntMeanRange(int mean, int range, int thr_no = -1)
  { return mean + (IntZeroN(2 * range + 1, thr_no) - range); }
  // #CAT_Int uniform random integer with given range on either side of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double ZeroOne(int thr_no = -1)
  { return MTRnd::GenRandRes53(thr_no); }
  // #CAT_Float uniform random number between zero and one (inclusive of 1 due to rounding!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double UniformMinMax(double min, double max, int thr_no = -1)
  { return min + (max - min) * ZeroOne(thr_no); }
  // #CAT_Float uniform random number between min and max values inclusive (Do not use for generating integers - will not include max!) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double UniformMeanRange(double mean, double range, int thr_no = -1)
  { return mean + range * 2.0 * (ZeroOne(thr_no) - 0.5); }
  // #CAT_Float uniform random number with given range on either size of the mean: [mean - range, mean + range] -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static double Binom(int n, double p, int thr_no = -1)
  { return taMath_double::binom_dev(n,p,thr_no); }
  // #CAT_Float binomial with n trials (par) each of probability p (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Poisson(double l, int thr_no = -1)
  { return taMath_double::poisson_dev(l,thr_no); }
  // #CAT_Float poisson with parameter l (var) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gamma(double var, double j, int thr_no = -1)
  { return taMath_double::gamma_dev(j, var, thr_no); }
  // #CAT_Float gamma with given variance, number of exponential stages (par) -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Gauss(double stdev, int thr_no = -1)
  { return stdev * taMath_double::gauss_dev(thr_no); }
  // #CAT_Float gaussian (normal) random number with given standard deviation -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)
  static double Beta(double a, double b, int thr_no = -1)
  { return taMath_double::beta_dev(a, b, thr_no); }
  // #CAT_Float beta random number with two shape parameters a > 0 and b > 0 -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  static bool   BoolProb(double p, int thr_no = -1)
  { return (ZeroOne(thr_no) < p); }
  // #CAT_Bool boolean true/false with given probability -- (0 <= thr_no < 100) specifies thread or dmem proc number for parallel safe random sequences (-1 = taMisc::dmem_proc for auto-safe dmem)

  STATE_CLASS(Random)() { Initialize(); }
private:
  void Initialize() {
    type = UNIFORM;
    mean = 0.0f;
    var = 1.0f;
    par = 1.0f;
  }

  void Copy_(const STATE_CLASS(Random)& cp){
    type = cp.type;
    mean = cp.mean;
    var = cp.var;
    par = cp.par;
  }
};

#endif // STATE_CUDA
