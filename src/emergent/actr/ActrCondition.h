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

#ifndef ActrCondition_h
#define ActrCondition_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ActrChunk>

// declare all other types mentioned but not required to include:
class ActrProceduralModule; //
class ActrModel; //

eTypeDef_Of(ActrCondition);

class E_API ActrCondition : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR one condition that must be met to fire an Act-R production
INHERITED(taOBase)
public:
  enum CondSource {
    BUFFER_EQ,                  // match against an ActR chunk in a buffer (for regular ActR model) 
    BUFFER_QUERY,               // check state of buffer or module 
    PROG_VAR,                   // match against value of a program variable (for use in controlling system) -- must be a global (args or vars) variable, not a local var
    NET_UNIT,                   // match against unit activation in a network
    NET_LAYER,                  // match against layer-level state variable in a network
    OBJ_MEMBER,                 // match against a member in an arbitrary object
  };

  enum Relations {
    EQUAL,              // #LABEL_=
    NOTEQUAL,           // #LABEL_!=
    LESSTHAN,           // #LABEL_<
    GREATERTHAN,        // #LABEL_>
    LESSTHANOREQUAL,    // #LABEL_<=
    GREATERTHANOREQUAL  // #LABEL_>=
  };

  CondSource    cond_src;      // what is the source of the data to match against
  TypeDef*      src_type;      // #CONDSHOW_ON_cond_src:OBJ_MEMBER #NO_NULL #TYPE_taBase type of object with source data to match against
  taBaseRef     src;           // #TYPE_ON_src_type the source object to obtain data to match against (e.g., buffer, etc)
  String        unit_name;     // #CONDSHOW_ON_cond_src:NET_UNIT name of unit within layer to obtain unit value from -- can only access named units
  Relations     rel;           // relationship between source value and comparison value -- only = and != are valid for BUFFER source
  String        cmp_val;       // #CONDSHOW_OFF_cond_src:BUFFER_EQ comparison value
  ActrChunk     cmp_chunk;     // #CONDSHOW_ON_cond_src:BUFFER_EQ comparison chunk -- fill in chunk type and all chunk values that should match.  use =name for variable copying

  // todo: config checking, matching etc

  virtual bool  Matches();
  // #CAT_ActR does this condition match?
  virtual String WhyNot();
  // #BUTTON #USE_RVAL #CAT_ActR report why this condition does not match

  virtual void  SendBufferReads(ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR send BUFFER-READ-ACTION events for all BUFFER_EQ cases

  override String 	GetTypeDecoKey() const { return "ProgCtrl"; }

  TA_SIMPLE_BASEFUNS(ActrCondition);
protected:
  void  UpdateAfterEdit_impl();

private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

#endif // ActrCondition_h
