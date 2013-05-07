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

#ifndef ActrAction_h
#define ActrAction_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <ActrBuffer>
#include <ActrChunk>
#include <ProgVar>
#include <Program>

// declare all other types mentioned but not required to include:
class ActrModel; //
class ActrProceduralModule; //
class ActrProduction; //


eTypeDef_Of(ActrAction);

class E_API ActrAction : public taOBase {
  // ##NO_TOKENS ##INSTANCE ##EDIT_INLINE ##CAT_ActR one action that is performed when an Act-R production fires
INHERITED(taOBase)
public:
  enum ActType {
    UPDATE,              // =buffer -- update buffer state of ActR chunk in a buffer with chunk info
    REQUEST,             // +buffer -- request info from buffer (and associated module) according to chunk info
    CLEAR,               // -buffer -- clear chunk from buffer
    OUTPUT,              // print out the val value follwed by contents of chunk if chunk type is set
    STOP,                // stop execution of the model
    PROG_VAR,            // set value of a program variable
    PROG_RUN,            // run a Program (not for official ActR models)
  };

  ActType       action;         // what type of action to perform
  ActrBufferRef buffer;         // what buffer to operate on
  ActrChunk     chunk;          // #SHOW_TREE chunk information for the action
  ProgVarRef    prog_var;       // #CONDSHOW_ON_action:PROG_VAR program variable to set to val
  String        val;            // #CONDSHOW_ON_action:PROG_VAR,OUTPUT value to print out or to set program variable to
  ProgramRef    program;        // #CONDSHOW_ON_action:PROG_RUN program to run -- does not set any arg variables prior to running -- just calls Run on it -- use prior actions to set program variables


  virtual void     SetVariables(ActrProduction& prod, ActrChunk* ck);
  // #CAT_ActR set any variables from current vars in production 

  virtual bool     DoAction(ActrProduction& prod,
                            ActrProceduralModule* proc_mod, ActrModel* model);
  // #CAT_ActR perform the action as specified

  override String  GetTypeDecoKey() const { return "Function"; }
  override String  GetDisplayName() const;
  override String& Print(String& strm, int indent = 0) const;
  override String  GetDesc() const;

  TA_SIMPLE_BASEFUNS(ActrAction);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrAction); // ActrActionRef

#endif // ActrAction_h
