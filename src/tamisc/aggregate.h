/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef aggregate_h
#define aggregate_h

#include "ta_base.h"
#include "tamisc_TA_type.h"

class CountParam : public taBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE counting criteria params
public:
  enum Relation {
    EQUAL,		// #LABEL_=
    NOTEQUAL,		// #LABEL_!=
    LESSTHAN,		// #LABEL_<
    GREATERTHAN,	// #LABEL_>
    LESSTHANOREQUAL,	// #LABEL_<=
    GREATERTHANOREQUAL 	// #LABEL_>=
  };

  Relation	rel;		// #LABEL_ relation of statistic to target value
  float		val;		// #LABEL_ target or comparison value

  bool 		Evaluate(float cmp) const;

  void  Initialize();
  void 	Destroy()		{ };
  void	Copy_(const CountParam& cp);
  COPY_FUNS(CountParam, taBase);
  TA_BASEFUNS(CountParam);
};

class Aggregate : public taOBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER #INLINE Basic aggregation operations
public:
  enum Operator {		// Aggregate Operators
    DEFAULT=-1,			// select the default specified for the object
    LAST=0,			// The most recent value
    SUM,			// Summation
    PROD,			// Product
    MIN,			// Minimum
    MAX,			// Maximum
    AVG,			// Average (mean)
    COPY,			// Copy (keep each individual value)
    COUNT 			// Count of the number times count relation was true
  };

  Operator      op;		// how to aggregate over the network
  bool		no0;		// #DEF_false don't aggregate when the value is zero
  CountParam	count;		// #CONDEDIT_ON_op:COUNT parameters for the COUNT aggregation
  int		n_updt;		// #READ_ONLY number of times agg updated (for AVG)

  void 	AggLAST(float& to, float fm) const	{ to = fm; } // at this level..
  void 	AggSUM(float& to, float fm) const	{ to += fm; }
  void 	AggPROD(float& to, float fm) const	{ to *= fm; }
  void 	AggMIN(float& to, float fm) const	{ to = (fm < to ) ? fm :  to; }
  void 	AggMAX(float& to, float fm) const	{ to = (fm > to ) ?  fm :  to; }
  void 	AggAVG(float& to, float fm) const	{ to = (to * (float)n_updt + fm) / (float)(n_updt + 1); }
  void 	AggCOPY(float& to, float fm) const	{ to = fm; } // not defined at this level..
  void 	AggCOUNT(float& to, float fm) const	{ if(count.Evaluate(fm)) to += 1.0; }

  virtual void	ComputeAgg(float& to, float fm);
  // compute aggregation into `to' based on current op from `fm'
  virtual bool	ComputeAggNoUpdt(float& to, float fm);
  // compute aggregation but don't update the n_updt counter (for lists) (return false if fm = 0, else true)
  void		IncUpdt()		{ n_updt++; } // increment the update counter

  virtual const char* GetAggName() const;  // get string representation of aggregation opr
  virtual String AppendAggName(const char* nm) const;
  // append aggregation name to given name
  virtual String PrependAggName(const char* nm) const;
  // prepend aggregation name to given name

  virtual void	Init();		// initialize agg variables
  virtual float	InitAggVal() const;
  // returns the initial aggregation value based on op (0,1,or FLT_MAX)

  void 	Initialize();
  void 	Destroy();
  void	InitLinks();
  void	Copy_(const Aggregate& cp);
  COPY_FUNS(Aggregate, taOBase);
  TA_BASEFUNS(Aggregate);
};

class SimpleMathSpec : public taBase {
  // #INLINE #NO_UPDATE_AFTER ##NO_TOKENS params for std kinds of simple math operators
public:
  enum MathOpr {
    NONE,			// no function
    THRESH,			// threshold: if val >= arg then hi, else lo
    ABS,			// take absolute-value
    SQUARE,			// square (raise to 2nd power)
    SQRT,			// square root
    LOG,			// natural log
    LOG10,			// log base 10
    EXP,			// exponential (e^val)
    ADD,			// add arg value
    SUB,			// subtract arg value
    MUL,			// multiply by arg value
    POWER,			// raise to the power of arg
    DIV, 			// divide by arg value
    GTEQ,			// make all values greater than or equal to arg
    LTEQ,			// make all values less than or equal to arg
    GTLTEQ			// make all values greater than lw and less than hi
  };

  MathOpr 	opr;		// what math operator to use
  float		arg;		// #CONDEDIT_ON_opr:THRESH,ADD,SUB,MUL,POWER,DIV,GTEQ,LTEQ argument for ops (threshold add/sub/mul/div arg)
  float		lw;		// #CONDEDIT_ON_opr:THRESH,GTLTEQ the value to assign values below threshold
  float		hi;		// #CONDEDIT_ON_opr:THRESH,GTLTEQ the value to assign values above threshold


  float		Evaluate(float val) const; // evaluate math operator on given value

  void 	Initialize();
  void	Destroy()	{ };
  void	Copy_(const SimpleMathSpec& cp);
  COPY_FUNS(SimpleMathSpec, taBase);
  TA_BASEFUNS(SimpleMathSpec);
};

#endif // aggregate_h
