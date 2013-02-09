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

#ifndef Aggregate_h
#define Aggregate_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <Relation>

// declare all other types mentioned but not required to include:


taTypeDef_Of(Aggregate);

class TA_API Aggregate : public taNBase {
  // #STEM_BASE ##NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP ##CAT_Math Basic aggregation operations
  INHERITED(taNBase)
public:
  enum Operator {               // Aggregate Operators
    GROUP,                      // group by this field
    FIRST,                      // first item
    LAST,                       // last item
    FIND_FIRST,                 // find the first item that fits rel relationship
    FIND_LAST,                  // find the last item that fits rel relationship
    MIN,                        // Minimum
    MAX,                        // Maximum
    ABS_MIN,                    // Minimum of absolute values
    ABS_MAX,                    // Maximum of absolute values
    SUM,                        // Summation
    PROD,                       // Product
    MEAN,                       // Mean of values
    VAR,                        // Variance
    SS,                 // Sum of squares around the mean
    STDEV,                      // Standard deviation
    SEM,                        // Standard error of the mean (always uses the unbiased estimate of the variance)
    N,                          // the number of data items in the vector
    COUNT,                      // Count of the number times count relation was true
    MEDIAN,                     // middle item in sorted list of values
    MODE,                       // most frequent item (note: requires sorting)
    QUANTILE,                   // value at ordinal position within a sorted list given by rel.val parameter (normalized 0-1 position within sorted list, e.g., .5 = median, .25 = first quartile, etc) -- rel relation is not used
    NONE,                       // no aggregation operation
  };

#ifdef __MAKETA__
  String        name;           // #HIDDEN_INLINE name of the object
#endif

  Operator      op;             // how to aggregate over the network
  Relation      rel;            // #CONDSHOW_ON_op:COUNT,FIND_FIRST,FIND_LAST,QUANTILE,VAR,STDEV parameters for the COUNT, FIND_xxx, and QUANTILE operators

  virtual String GetAggName() const;  // get string representation of aggregation opr
  virtual ValType MinValType() const; // minimum value type that aggregation operator can operate on (VT_INT = any kind of numeric data, VT_STRING = even non-numeric is ok (GROUP, FIRST, LAST)
  virtual ValType MinReturnType() const; // minimum return type of the aggregation operator (VT_FLOAT = floating point type (double, float), VT_INT = integer type, VT_STRING = could even be a non-numeric type (GROUP, FIRST, LAST)

  void  Initialize();
  void  Destroy();
  TA_SIMPLE_BASEFUNS(Aggregate);
};

#endif // Aggregate_h
