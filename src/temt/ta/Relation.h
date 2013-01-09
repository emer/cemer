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

#ifndef Relation_h
#define Relation_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ProgVar>

// declare all other types mentioned but not required to include:


class TA_API Relation : public taNBase {
  // #STEM_BASE ##NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP ##CAT_Math counting criteria params
  INHERITED(taNBase)
public:
  enum Relations {
    EQUAL,              // #LABEL_=
    NOTEQUAL,           // #LABEL_!=
    LESSTHAN,           // #LABEL_<
    GREATERTHAN,        // #LABEL_>
    LESSTHANOREQUAL,    // #LABEL_<=
    GREATERTHANOREQUAL  // #LABEL_>=
  };

  Relations     rel;            // #LABEL_ relationship to evaluate
  double        val;            // #LABEL_ comparison value
  bool          use_var;        // if true, use a program variable to specify the relation value
  ProgVarRef    var;            // #CONDSHOW_ON_use_var:true variable that contains the comparison value (only used if this is embedded in a DataSelectRowsProg program element) -- variable must be a top-level (.args or .vars) variable and not a local one

  bool          CacheVar(Relation& tmp_rel);
  // copy rel and cache the variable value in new Relation object (tmp_rel), or copy val -- optimizes repeated actions using same relation object so they don't have to keep getting the variable

  bool          Evaluate(double cmp) const;

  void  Initialize();
  void  Destroy()               { };
  TA_SIMPLE_BASEFUNS(Relation);
};

#endif // Relation_h
