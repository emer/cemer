// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef SimpleMathSpec_h
#define SimpleMathSpec_h 1

// parent includes:
#include <taINBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(SimpleMathSpec);

class TA_API SimpleMathSpec : public taINBase {
  // #STEM_BASE ##INLINE ##INLINE_DUMP #NO_UPDATE_AFTER ##CAT_Math params for std kinds of simple math operators applied to an input value 'val'
  INHERITED(taINBase)
public:
  enum MathOpr {
    NONE,                       // no function
    THRESH,                     // threshold: if val >= arg then hi, else lo
    ABS,                        // take absolute-value of val
    SQUARE,                     // square val (raise to 2nd power)
    SQRT,                       // square root of val
    LOG,                        // natural log of val
    LOG10,                      // log base 10 ov val
    EXP,                        // exponential (e^val)
    ADD,                        // add val + arg
    SUB,                        // subtract val - arg
    MUL,                        // multiply val * arg
    DIV,                        // divide val / arg
    POWER,                      // raise val to the power of arg: val^arg
    MAX,                        // maximum of value and arg: MAX(val,arg)
    MIN,                        // minimum of value and arg: MIN(val,arg)
    MINMAX,                     // minimum of value and hi, and maximum of value and lw (enforce val between lw-hi range)
    REPLACE,                    // replace value arg value with lw value
  };

  MathOpr       opr;            // what math operator to use
  double        arg;            // #CONDSHOW_ON_opr:THRESH,ADD,SUB,MUL,POWER,DIV,MIN,MAX,REPLACE argument for ops (threshold add/sub/mul/div,power,max,min arg,replace)
  double        lw;             // #CONDSHOW_ON_opr:THRESH,MINMAX,REPLACE the value to assign values below threshold for THRESH, or the low range for MINMAX, or value to replace with for REPLACE
  double        hi;             // #CONDSHOW_ON_opr:THRESH,MINMAX the value to assign values above threshold for THRESH, or the high range for MINMAX

  double        Evaluate(double val) const; // evaluate math operator on given value
  Variant&      EvaluateVar(Variant& val) const; // #IGNORE evaluate math operator on given value

  void  Initialize();
  void  Destroy()       { };
  TA_SIMPLE_BASEFUNS(SimpleMathSpec);
};

#endif // SimpleMathSpec_h
