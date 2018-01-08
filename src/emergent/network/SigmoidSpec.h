// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef SigmoidSpec_h
#define SigmoidSpec_h 1

// parent includes:
#include "network_def.h"
#include <taOBase>

// member includes:
#ifndef __MAKETA__
#include <math.h>
#endif

// declare all other types mentioned but not required to include:

eTypeDef_Of(SigmoidSpec);

class E_API SigmoidSpec : public taOBase {
// ##NO_TOKENS ##INLINE ##CAT_Math Specifies a Sigmoid 1 / [1 + exp(-(x - off) * gain)]
INHERITED(taOBase) //
public:
#ifndef __MAKETA__
  static const float SIGMOID_MAX_VAL; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET; // #READ_ONLY #HIDDEN maximium net input value
#else
  static const float SIGMOID_MAX_VAL = 0.999999f; // #READ_ONLY #HIDDEN max eval value
  static const float SIGMOID_MIN_VAL = 0.000001f; // #READ_ONLY #HIDDEN min eval value
  static const float SIGMOID_MAX_NET = 13.81551f; // #READ_ONLY #HIDDEN maximium net input value
#endif
  float         off;            // offset for .5 point
  float         gain;           // gain

  bool          gain_eq_1;     // #READ_ONLY #NO_SAVE true if gain == 1 -- for optimizations
  
  static inline float  Clip(float y)
  { y = fmaxf(y,SIGMOID_MIN_VAL); y = fminf(y,SIGMOID_MAX_VAL); return y; }
  static inline float  ClipNet(float x)
  { x = fmaxf(x,-SIGMOID_MAX_NET); x = fminf(x,SIGMOID_MAX_NET); return x; }
  inline float  Eval(float x)
  { return Clip(1.0f / (1.0f + expf(-ClipNet((x - off) * gain)))); }
  inline float  Deriv(float x)          { x = Clip(x); return x * (1.0f - x) * gain; }
  inline float  Inverse(float y)        { y=y+off; return logf(y / (1.0f - y)) / gain; }

  SIMPLE_COPY(SigmoidSpec);
  TA_BASEFUNS(SigmoidSpec);
protected:
  void	UpdateAfterEdit_impl() override;
private:
  void  Initialize()            { off = 0.0f; gain = 1.0f; gain_eq_1 = true; }
  void  Destroy()               { };
};

#endif // SigmoidSpec_h
